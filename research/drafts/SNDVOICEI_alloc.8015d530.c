// The voice allocator (salloc.c). SNDVOICEI_alloc reserves count voices in
// [first, last): free voices first (oldest start time), then by stealing
// voices of a lower priority (oldest first among equals). The reserved list
// is sorted, busy voices stopped, and the voices linked into a group whose
// first voice holds the new handle and the member list. SNDVOICEI_free
// releases a voice, taking the rest of its sample group into account, and
// SNDVOICEI_get maps a handle to its voice index. sndgs's voice table (128-byte
// records) is accessed through inferred offsets.
extern "C" char sndgs[];
extern "C" int SNDstop(int handle);

short sndvoicereserved[4];

/* inferred: voice record view */
typedef struct SNDVOICE {
    int handle;              // 0x00
    short members[15];       // 0x04: group members, held by the group's first voice
    unsigned char group;     // 0x22
    unsigned char lead;      // 0x23
    short master;            // 0x24: the group's first voice
    unsigned char priority;  // 0x26
    unsigned char pad27;
    unsigned int starttime;  // 0x28
    unsigned char unknown2c[0x5d - 0x2c];
    signed char active;      // 0x5d
    unsigned char unknown5e[0x80 - 0x5e];
} SNDVOICE;

#define VOICES (*(SNDVOICE**)(sndgs + 468))

/* inferred: is the voice already in the reserved list */
static inline int isreserved(short* list, int count, int voice) {
    int i;

    for (i = 0; i < count; i++) {
        if (list[i] == voice)
            return 1;
    }
    return 0;
}

static inline int stealvoice(int first, int last, short* list, int reserved, int lim) {
    int best;
    int bestpriority;
    unsigned int besttime;
    int voice;
    SNDVOICE* v;

    best = -1;
    bestpriority = lim;
    besttime = best;
    v = &VOICES[(unsigned)first];
    for (voice = first; voice < last; v++, voice++) {
        if (!isreserved(list, reserved, voice)) {
            if (v->priority < 101) {
                if (v->priority < bestpriority) {
                    besttime = v->starttime;
                    bestpriority = v->priority;
                    best = voice;
                } else if (v->priority == bestpriority) {
                    if (v->starttime < besttime) {
                        besttime = v->starttime;
                        best = voice;
                    }
                }
            }
        }
    }
    return best;
}

int SNDVOICEI_alloc(int count, int priority, int* handle, int first, int last) {
    int result = -9;
    static int lasthandle = 0;
    int i;
    int j;
    short* list;
    int reserved = 0;
    short* out;

    for (i = 0; i < count; i++)
        sndvoicereserved[i] = -1;
    lasthandle += 256;
    if (lasthandle < 0)
        lasthandle = 0;
    out = list = sndvoicereserved;

    for (i = 0; i < count; i++) {
        SNDVOICE* v;
        int best;
        unsigned int besttime;
        int voice;
        best = -1;
        besttime = -1;
        v = &VOICES[first];
        for (voice = first; voice < last; v++, voice++) {
            if (!v->active && !isreserved(list, reserved, voice)) {
                if (v->starttime < besttime) {
                    besttime = v->starttime;
                    best = voice;
                }
            }
        }
        if (best >= 0) {
            *out++ = best;
            reserved++;
        }
    }

    out = &sndvoicereserved[reserved];
    for (i = reserved; i < count; i++) {
        int lim = priority;
        if (!*(unsigned char*)(sndgs + 60))
            lim = priority - 1;
        int found = stealvoice(first, last, list, reserved, lim);
        if (found >= 0) {
            reserved++;
            *out++ = found;
            if (reserved >= count)
                break;
        }
    }

    if (reserved == count) {
        do {
            result = 1;
            for (i = 0; i < count - 1; i++) {
                if (list[i] > list[i + 1]) {
                    short t = list[i];
                    list[i] = list[i + 1];
                    list[i + 1] = t;
                    result = 0;
                }
            }
        } while (!result);

        *handle = lasthandle | sndvoicereserved[0];
        result = sndvoicereserved[0];
        for (i = 0; i < reserved; i++) {
            SNDVOICE* v = &VOICES[*list];
            int h = v->handle;
            if (v->active) {
                if (h < 0)
                    h = VOICES[v->master].handle;
                SNDstop(h);
            }
            v->active = 1;
            v->starttime = *(unsigned int*)(sndgs + 372);
            v->priority = priority;
            list++;
        }

        VOICES[sndvoicereserved[0]].handle = *handle;
        VOICES[sndvoicereserved[0]].members[0] = sndvoicereserved[0];
        VOICES[sndvoicereserved[0]].master = -1;
        for (j = 1; j < reserved; j++) {
            VOICES[sndvoicereserved[0]].members[j] = sndvoicereserved[j];
            VOICES[sndvoicereserved[j]].handle = -1;
            VOICES[sndvoicereserved[j]].master = sndvoicereserved[0];
        }
    }
    return result;
}

/* inferred: marks a voice free */
static inline void releasevoice(SNDVOICE* v) {
    v->active = 0;
    v->group = 0;
    v->lead = 0;
    v->starttime = *(unsigned int*)(sndgs + 372);
}

void SNDVOICEI_free(int voice) {
    SNDVOICE* voices = VOICES;
    SNDVOICE* const v = &voices[voice];
    SNDVOICE* p;
    int group;
    int i;
    int n = 0;
    int lead = -1;


    group = v->group;
    if (v->group) {
        p = voices;
        for (i = 0; i < *(short*)(sndgs + 368); p++, i++) {
            if (p->group == group && p->handle >= 0 && p->active) {
                n++;
                if (p->lead)
                    lead = i;
            }
        }
        if (n == 1) {
            releasevoice(v);
            return;
        }
        p = &voices[lead];
        if (p->active == 2 && voice != lead && n == 2) {
            releasevoice(v);
            VOICES[lead].active = 0;
            VOICES[lead].group = 0;
            VOICES[lead].lead = 0;
            VOICES[lead].starttime = *(unsigned int*)(sndgs + 372);
            return;
        }
        if (p->active == 1 && voice == lead) {
            p->active = 2;
            return;
        }
        releasevoice(v);
    } else {
        releasevoice(v);
    }
}

int SNDVOICEI_get(int handle) {
    int voice;
    char* entry;

    if (handle < 0)
        return -8;
    voice = handle & 0xff;
    if (voice >= *(short*)(sndgs + 368))
        return -8;
    entry = *(char**)(sndgs + 468) + voice * 128;
    if (!*(signed char*)(entry + 93) || *(int*)entry != handle)
        voice = -8;
    return voice;
}
