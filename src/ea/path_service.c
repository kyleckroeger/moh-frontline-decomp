/* PATHI_servicetimer and PATHI_servicetask (path.c, 0x8010d084): the timer
   and task services of the music path player. Unless the player is paused,
   uninitialised or busy, each locks it, records the service kind in
   bankservice (0x42 timer, 0x20 task), reads the clock, refreshes the track
   states, and for every active project finds the leading track (the
   unpaused playing track with the highest latency, its latency and current
   beat) before servicing the project's two passes; it then marks every track
   pending and unlocks. The functions and globals are named by their symbols;
   the state, track and node layouts and the inlines (state selection as in
   path.c, the playing test, the leader search) are inferred. The rest of the
   file is in other units. */
struct PATHNODEVIEW {
    unsigned char unknown0[2];
    unsigned short track : 5;
    unsigned short beat : 6;
    unsigned short unknown2 : 5;
};

struct PATHTRACKVIEW {
    unsigned char unknown00[2];
    signed char paused;
    unsigned char unknown03[13];
    unsigned int latency;
    unsigned char unknown14[8];
    PATHNODEVIEW* current;
    unsigned char unknown20[2];
    short node;
};

struct PATHSTATEVIEW {
    int leader;
    unsigned int latency;
    int beat;
    unsigned char unknown00c[20];
    unsigned int id;
    void* active;
    unsigned char unknown028[288];
    PATHTRACKVIEW* tracks[24];
};

extern signed char inited;
extern signed char paused;
extern unsigned int pfactive;
extern PATHSTATEVIEW* pfstate;
extern PATHSTATEVIEW pfstates[4];
extern int milliseconds;
extern unsigned char bankservice;

extern "C" int PATHX_milliseconds(void);
extern "C" void PATHX_statusall(int);
void PATHI_serviceproject(int, int);

static inline int selectstate(int index) {
    if (!pfactive)
        pfactive = -1;
    if (index >= 0 && index < 4 && pfstates[index].active) {
        pfstate = &pfstates[index];
        return pfactive = pfstates[index].id;
    } else if (index != -1) {
        pfactive = 0;
    }
    return pfactive;
}

static inline bool isplaying(PATHTRACKVIEW* track) {
    bool playing = false;

    if (track->node >= 0 && track->current)
        playing = true;
    return playing;
}

static inline void findleader(void) {
    int j;
    PATHTRACKVIEW* track;
    pfstate->latency = 0;
    pfstate->leader = -1;
    pfstate->beat = 0;
    for (j = 0; j < 24; j++) {
        track = pfstate->tracks[j];
        if (track && !track->paused && isplaying(track) && pfstate->latency < track->latency) {
            pfstate->latency = track->latency;
            pfstate->leader = j;
            pfstate->beat = track->current->beat;
        }
    }
}

void PATHI_servicetimer() {
    unsigned char i;
    int j;
    PATHTRACKVIEW* track;

    if (!paused && inited && !pfactive && selectstate(-1)) {
    bankservice = 0x42;
    milliseconds = PATHX_milliseconds();
    PATHX_statusall(0);
    for (i = 0; i < 4; i++) {
        int project = i;

        if (pfstates[i].active && selectstate(project)) {
            findleader();
            PATHI_serviceproject(project, 1);
            PATHI_serviceproject(project, 0);
        }
    }
    PATHX_statusall(1);
    pfactive = 0;
    pfstate = 0;
    }
}

void PATHI_servicetask() {
    unsigned char i;
    int j;
    int project;
    PATHTRACKVIEW* track;

    if (!paused && inited && !pfactive && selectstate(-1)) {
    bankservice = 0x20;
    milliseconds = PATHX_milliseconds();
    PATHX_statusall(0);
    for (i = 0; i < 4; i++) {
        project = i;
        if (pfstates[i].active && selectstate(project)) {
            findleader();
            PATHI_serviceproject(project, 1);
            PATHI_serviceproject(project, 0);
        }
    }
    PATHX_statusall(1);
    pfactive = 0;
    pfstate = 0;
    }
}
