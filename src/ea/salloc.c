// Fragment of the voice allocator: SNDVOICEI_free releases a voice, taking
// the rest of its sample group into account (freeing the playing lead voice
// only marks it finished; a finished lead goes with its last other member),
// and SNDVOICEI_get maps a sound handle to its voice index (the
// handle's low byte) and checks that the voice is active and still owns the
// handle. SNDVOICEI_alloc before them is not reconstructed. sndgs's voice
// table (128-byte records) is accessed through inferred offsets.
extern "C" char sndgs[];

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

static inline void releasevoice(SNDVOICE* v) {
    v->active = 0;
    v->group = 0;
    v->lead = 0;
    v->starttime = *(unsigned int*)(sndgs + 372);
}

void SNDVOICEI_free(int voice) {
    SNDVOICE* v = &VOICES[voice];
    int group = v->group;
    int n = 0;
    int lead = -1;
    int i;
    SNDVOICE* p;

    if (v->group) {
        p = VOICES;
        for (i = 0; i < *(short*)(sndgs + 368); i++) {
            SNDVOICE* q = &p[i];
            if (q->group == group && q->handle >= 0 && q->active) {
                n++;
                if (q->lead)
                    lead = i;
            }
        }
        if (n == 1) {
            releasevoice(&VOICES[voice]);
            return;
        }
        p = &VOICES[lead];
        if (p->active == 2 && voice != lead && n == 2) {
            releasevoice(&VOICES[voice]);
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
        releasevoice(&VOICES[voice]);
        return;
    }
    releasevoice(&VOICES[voice]);
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
