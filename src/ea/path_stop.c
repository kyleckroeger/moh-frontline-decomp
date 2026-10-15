/* A second fragment of the music path player's path.c (src/ea/path.c holds
   PATH_control onward): PATH_volume clamps the volume to 0..100, keeps it as
   the global volume scale for the all-tracks handle (-1), and for each masked
   track stores it and, while the track plays, sets the track's base volume
   scaled by it; PATH_stop stops every track of the matching player
   states and marks them stopped (node -1); PATH_pause records a global pause
   for the all-tracks handle (-1) and pauses the masked tracks; PATH_destroy
   destroys the first masked track, deactivates its state when that song has
   no tracks left and ends the service when no song has any; PATH_jump queues a
   jump event (node -node-1, the type from the third argument, forced to 64 when
   no track leads) for each masked track whose node belongs to it. The file
   name is this project's, not the original's: the original file record is
   path.c and PATH_event (between PATH_jump and PATH_control) is not
   reconstructed, so the two parts are separate units. All views, field names
   and the inline helpers (state selection, track count) are inferred from the
   code; pfstates and the functions are named by their symbols. */
struct PATHNODEVIEW {
    unsigned char unknown0[2];
    unsigned short track : 5;
    unsigned short beat : 6;
    unsigned short unknown2 : 5;
};

struct PATHMAPVIEW {
    unsigned char unknown00[14];
    unsigned char beats;
    unsigned char events;
    unsigned char unknown10[2];
    unsigned short nodes;
};

struct PATHEVENTVIEW {
    int node : 16;
    unsigned int type : 7;
    unsigned int unknown2a : 2;
    unsigned int unknown2b : 1;
    unsigned int unknown2c : 1;
    unsigned int track : 5;
    int unknown4;
    int unknown8;
    unsigned int mask;
};

struct PATHRECORDVIEW {
    int unknown0 : 8;
    unsigned int flags : 8;
    unsigned int unknown2 : 16;
};

struct PATHTRACKVIEW {
    unsigned char unknown00;
    unsigned char control;
    signed char paused;
    signed char volume;
    unsigned char unknown04[6];
    signed char playing;
    unsigned char unknown0b[5];
    unsigned int latency;
    unsigned char unknown14[8];
    PATHNODEVIEW* current;
    signed char basevolume;
    unsigned char unknown21;
    short node;
    unsigned char unknown24[24];
    int handle;
};

struct PATHSTATEVIEW {
    int leader;
    unsigned int latency;
    int beat;
    unsigned char unknown00c[20];
    unsigned int id;
    PATHMAPVIEW* active;
    short* nodemap;
    unsigned char unknown02c[4];
    unsigned char* recordmap;
    PATHRECORDVIEW* records;
    unsigned char unknown038[16];
    PATHEVENTVIEW events[16];
    PATHTRACKVIEW* tracks[24];
};

extern signed char inited;
extern signed char volscale;
extern signed char paused;
extern unsigned int pfactive;
extern PATHSTATEVIEW* pfstate;
extern PATHSTATEVIEW pfstates[4];

extern "C" void PATHX_volume(int, int);
extern "C" void PATHX_stop(int);
extern "C" void PATHX_pause(int, int);
extern "C" void PATHX_destroytrack(int);
extern "C" void PATHX_endservice(void);
void PATHI_sortprojects(void);

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

static inline int counttracks(unsigned int songs) {
    int i;
    int j;
    int count = 0;
    for (i = 0; i < 4; i++) {
        if ((songs & (0x01000000 << i)) && pfstates[i].active) {
            for (j = 0; j < 24; j++) {
                if (pfstates[i].tracks[j])
                    count++;
            }
        }
    }
    return count;
}

extern "C" void PATH_volume(unsigned int handle, signed char volume) {
    unsigned int project;
    unsigned int song;
    int scale;
    int i;
    int j;
    if (!inited || pfactive || !selectstate(-1))
        return;
    if (volume > 100)
        volume = 100;
    if (volume < 0)
        volume = 0;
    if (handle == 0xFFFFFFFF)
        volscale = volume;
    project = handle & 0xF0000000;
    song = handle & 0x0F000000;
    scale = volume;
    for (i = 0; i < 4; i++) {
        if (pfstates[i].active && (project & pfstates[i].id) && (song & pfstates[i].id)) {
            if (selectstate(i)) {
                for (j = 0; j < 24; j++) {
                    PATHTRACKVIEW* track = pfstate->tracks[j];
                    if (track && (handle & (1 << j))) {
                        track->volume = volume;
                        if (track->playing < 0)
                            PATHX_volume(track->handle, track->basevolume * scale / 100);
                    }
                }
            }
        }
    }
    pfactive = 0;
    pfstate = 0;
}

extern "C" void PATH_stop(unsigned int handle) {
    unsigned int project;
    unsigned int song;
    int i;
    int j;
    if (!inited || pfactive || !selectstate(-1))
        return;
    project = handle & 0xF0000000;
    song = handle & 0x0F000000;
    for (i = 0; i < 4; i++) {
        if (pfstates[i].active && (project & pfstates[i].id) && (song & pfstates[i].id)) {
            if (selectstate(i)) {
                for (j = 0; j < 24; j++) {
                    PATHTRACKVIEW* track = pfstate->tracks[j];
                    if (track) {
                        PATHX_stop(track->handle);
                        track->node = -1;
                    }
                }
            }
        }
    }
    pfactive = 0;
    pfstate = 0;
}

extern "C" void PATH_pause(unsigned int handle, int pause) {
    unsigned int project;
    unsigned int song;
    int i;
    int j;
    if (!inited || pfactive || !selectstate(-1))
        return;
    if (handle == 0xFFFFFFFF)
        paused = pause;
    project = handle & 0xF0000000;
    song = handle & 0x0F000000;
    for (i = 0; i < 4; i++) {
        if (pfstates[i].active && (project & pfstates[i].id) && (song & pfstates[i].id)) {
            if (selectstate((unsigned char)i)) {
                for (j = 0; j < 24; j++) {
                    PATHTRACKVIEW* track = pfstate->tracks[j];
                    if (track && (handle & (1 << j))) {
                        PATHX_pause(track->handle, pause);
                        track->paused = pause;
                    }
                }
            }
        }
    }
    pfactive = 0;
    pfstate = 0;
}

extern "C" PATHTRACKVIEW* PATH_destroy(unsigned int handle) {
    unsigned int project;
    unsigned int song;
    int i;
    int j;
    int count;
    PATHTRACKVIEW* track = 0;
    if (!inited || pfactive || !selectstate(-1))
        return 0;
    project = handle & 0xF0000000;
    song = handle & 0x0F000000;
    for (i = 0; i < 4; i++) {
        if (pfstates[i].active && (project & pfstates[i].id) && (song & pfstates[i].id)) {
            if (selectstate((unsigned char)i)) {
                for (j = 0; j < 24; j++) {
                    track = pfstate->tracks[j];
                    if (track && (handle & (1 << j))) {
                        int h = track->handle;
                        pfstate->tracks[j] = 0;
                        PATHX_destroytrack(h);
                        if (!counttracks(0x01000000 << i)) {
                            pfstate->active = 0;
                            PATHI_sortprojects();
                        }
                        goto done;
                    }
                }
            }
        }
    }
done:
    count = counttracks(0x0F000000);
    if (inited && !count)
        PATHX_endservice();
    pfactive = 0;
    pfstate = 0;
    return track;
}

extern "C" int PATH_jump(unsigned int handle, int node, signed char type) {
    PATHEVENTVIEW event;
    PATHNODEVIEW* entry;
    unsigned int i;
    int j;
    unsigned int bit;
    if (!inited || pfactive || !selectstate(-1))
        return -18;
    for (i = 0; i < 4; i++) {
        if (pfstates[i].active && ((handle & 0xF0000000) & pfstates[i].id) && ((handle & 0x0F000000) & pfstates[i].id)) {
            if (selectstate((unsigned char)i)) {
                if (pfstate->leader < 0)
                    type = 64;
                if (node < 0 || node > pfstate->active->nodes)
                    goto done;
                for (j = 0; j < 24; j++) {
                    bit = 1 << j;
                    if (handle & bit) {
                        entry = &((PATHNODEVIEW*)pfstate->active)[pfstate->nodemap[node]];
                        if (!entry)
                            goto done;
                        if ((1 << entry->track) != bit)
                            goto done;
                        event.unknown2a = 1;
                        event.node = -node - 1;
                        event.unknown2b = 0;
                        event.mask = ~bit;
                        event.track = j;
                        event.unknown4 = 0;
                        event.unknown8 = 0;
                        switch (type) {
                        case 0:
                            event.type = 0;
                            break;
                        case 16:
                            event.type = 16;
                            break;
                        case 32:
                            event.type = 32;
                            break;
                        default:
                            event.type = 64;
                            break;
                        }
                        pfstate->events[0] = event;
                        pfstate->events[1].mask = 0;
                    }
                }
            }
        }
    }
done:
    pfactive = 0;
    pfstate = 0;
    return 0;
}
