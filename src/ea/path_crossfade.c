/* PATHI_crossfadetracks (path.c, 0x8010df10): crossfades from a project's
   track to its partner track (the same track bit with the project nibble
   flipped, found among the four player states). The outgoing track takes the
   partner's volume byte and fades to the partner's volume (as a percentage
   of 1.27), the partner fades to silence; each fade is timed by the node's
   fade-out or fade-in field (from the outgoing track's node) when set, else
   it is handed to PATHX_ifade at the project's fade time, immediately when
   more than one project plays. The functions and globals are named by their
   symbols; the state, track and node layouts and the inlines (track lookup as
   in path.c, the fade, the partner's fade-in through PATHX_ifade, whose
   locals give the original's shared zero) are inferred. The rest of the file is in other units. */
struct PATHFINDEVENT;

struct PATHNODEVIEW {
    unsigned char unknown0[8];
    unsigned short unknown8 : 6;
    unsigned short fadeout : 5;
    unsigned short fadein : 5;
};

struct PATHTRACKVIEW {
    unsigned char unknown00[3];
    signed char volume;
    int time;
    signed char start;
    signed char fade;
    signed char target;
    unsigned char unknown0b[17];
    PATHNODEVIEW* current;
    signed char level;
    unsigned char unknown21[23];
    int unknown38;
    int handle;
};

struct PATHSTATEVIEW {
    unsigned char unknown000[20];
    int fadetime;
    unsigned int playing;
    unsigned char unknown01c[4];
    unsigned int id;
    void* active;
    unsigned char unknown028[288];
    PATHTRACKVIEW* tracks[24];
};

extern PATHSTATEVIEW* pfstate;
extern PATHSTATEVIEW pfstates[4];
extern int milliseconds;

extern "C" int PATHX_getvolume(int);
int PATHX_ifade(int, int, int, char);

static inline PATHTRACKVIEW* findtrack(unsigned int handle) {
    unsigned int i;
    int j;
    unsigned int id;
    unsigned int project;
    unsigned int song;
    unsigned int mask;
    project = handle & 0xF0000000;
    song = handle & 0x0F000000;
    mask = handle & 0x00FFFFFF;
    for (i = 0; i < 4; i++) {
        id = pfstates[i].id;
        if (pfstates[i].active && (id & project) && (id & song)) {
            for (j = 0; j < 24; j++) {
                if ((1 << j) & mask)
                    return pfstates[i].tracks[j];
            }
        }
    }
    return 0;
}

static inline void fadetrack(PATHTRACKVIEW* track, int fade, int target, int time) {
    if (fade) {
        track->time = milliseconds;
        track->fade = fade;
        track->start = (int)((float)PATHX_getvolume(track->handle) / 1.27f);
        track->target = target;
        track->level = (int)(1.27f * (float)target);
    } else {
        char immediate = 0;
        track->target = target;
        track->level = (int)(1.27f * (float)target);
        int volume = track->level * track->volume / 100;
        if (pfstate->playing > 1) {
            track->fade = 0;
            immediate = 1;
        }
        PATHX_ifade(track->handle, volume, time, immediate);
    }
}

static inline void fadein(PATHTRACKVIEW* to, int time) {
    int target2 = 0;
    char immediate = 0;
    to->target = target2;
    to->level = (int)(1.27f * (float)target2);
    int volume = to->level * to->volume / 100;
    if (pfstate->playing > 1) {
        to->fade = 0;
        immediate = 1;
    }
    PATHX_ifade(to->handle, volume, time, immediate);
}

void PATHI_crossfadetracks(int, unsigned int index, PATHFINDEVENT*) {
    PATHTRACKVIEW* from = pfstate->tracks[index];
    PATHTRACKVIEW* to;
    int time;
    int target;

    from->unknown38 = 0;
    time = pfstate->fadetime;
    to = findtrack((pfstate->id + (1 << index)) ^ 0xF0000000);
    if (!to)
        return;
    PATHX_getvolume(from->handle);
    target = (float)PATHX_getvolume(to->handle) / 1.27;
    from->volume = to->volume;
    fadetrack(from, from->current->fadeout, target, time);
    if (to->current->fadein) {
        int fade = from->current->fadein;
        to->time = milliseconds;
        to->fade = fade;
        to->start = (int)((float)PATHX_getvolume(to->handle) / 1.27f);
        int target2 = 0;
        to->target = target2;
        to->level = (int)(1.27f * (float)target2);
    } else {
        fadein(to, time);
    }
}
