/* A fragment of EA's music path player (path.c, 0x8010d58c): PATHI_serviceproject
   services one project's tracks per pass (the leader track on pass 1, the
   others on pass 0). For each track on the current service (timer or task)
   it updates fades, starts the pending event when its time falls inside the
   period, releases holds, aligns the event time to the track's queued time
   (synchronising held events across all four path states) and queues the
   next node or stops a silent track. Afterwards an unscheduled event gets the
   latest queued time of all tracks, and a finished event settles the track
   volumes and is shifted out of the 16-entry event queue.
   Matching notes: event word 0 is a bitfield run (gives the original's
   word-wise struct copy in the shift); helpers are inlines with their own
   locals (shared constant registers); the direction is read into a short
   local in eventtime(); the time difference uses 'if (diff < 0) diff = -diff'. */
/* inferred: a queued path event (16 bytes) */
struct PATHFINDEVENT {
    signed int node : 16;
    unsigned int type : 7;
    unsigned int unknown2_0 : 1;
    unsigned int unknown3_7 : 1;
    unsigned int hold : 1;
    unsigned int unknown3_5 : 1;
    unsigned int track : 5;
    unsigned int time;
    unsigned int last;
    unsigned int mask;
};

struct PATHFINDNODE {
    short type;
    unsigned char unknown02[4];
    unsigned char bars : 4;
    unsigned char beats : 4;
};

struct PATHTRACK {
    unsigned char unknown00;
    signed char layer;
    signed char paused;
    signed char volume;
    unsigned int fadetime;
    signed char fadestart;
    signed char fade;
    signed char target;
    unsigned char unknown0b;
    unsigned int unknown0c : 4;
    signed int direction : 5;
    unsigned int unknown0c_9 : 5;
    unsigned int queuesize : 5;
    unsigned int queueindex : 5;
    unsigned int unknown0c_24 : 8;
    int latency;
    unsigned int service;
    unsigned char unknown18[4];
    PATHFINDNODE* current;
    signed char basevolume;
    unsigned char flags;
    short node;
    unsigned char unknown24[8];
    int length;
    int beatlength;
    unsigned int start;
    unsigned int hold;
    int handle;
};

struct PATHSTATEVIEW {
    int leader;
    unsigned char unknown004[8];
    unsigned int taskperiod;
    unsigned int timerperiod;
    unsigned char unknown014[16];
    int* nodes;
    short* nodeindex;
    unsigned char unknown02c[28];
    PATHFINDEVENT events[16];
    PATHTRACK* tracks[24];
};

extern PATHSTATEVIEW* pfstate;
extern PATHSTATEVIEW pfstates[4];
extern unsigned int milliseconds;
extern char bankservice;

extern "C" int PATHX_readyfornewrequest(int);
extern "C" int PATHX_timeremaining(int, int);
extern "C" int PATHX_status(int, int*, int*);
extern "C" int PATHX_modifyhold(int, int);
extern "C" int PATHX_getvolume(int);
extern "C" void PATHX_stop(int);
extern "C" void PATHX_volume(int, int);
void PATHI_setfadevolume(PATHTRACK*);
int PATHI_eventstart(int);
void PATHI_seeknextnode(int);
void PATHI_crossfadetracks(int, unsigned int, PATHFINDEVENT*);

static inline PATHFINDNODE* getnode(int node) {
    return (PATHFINDNODE*)&pfstate->nodes[pfstate->nodeindex[node]];
}

static inline int queuedtime(PATHTRACK* track) {
    int remaining = 0;
    unsigned int i;

    for (i = 0; i < track->queuesize; i++) {
        int left = PATHX_timeremaining(track->handle, i);
        if (left >= 0)
            remaining += left;
    }
    return remaining;
}

static inline char isplaying(PATHTRACK* track) {
    char playing = 0;

    if (track->node >= 0 && track->current)
        playing = 1;
    return playing;
}

static inline int eventtime(PATHTRACK* track, PATHFINDEVENT* event, int time) {
    PATHFINDNODE* node = getnode(track->node);
    unsigned int type = event->type;

    if ((type == 16 || type == 32) && node->type >= 0) {
        int position;
        int status;

        PATHX_status(track->handle, &status, &position);
        time = position % track->length;
        if (event->type == 32)
            time = position % track->beatlength;
    } else {
        short direction = track->direction;
        if (direction > 0)
            time += (node->beats * node->bars - (direction - 1)) * track->length;
    }
    return time;
}

static inline char iswaiting(PATHFINDEVENT* event, unsigned int i) {
    char waiting = 0;

    if (event->mask && !(event->mask & (1 << i)))
        waiting = 1;
    return waiting;
}

static inline void synceventtime(unsigned int handle, short node, unsigned int time) {
    int k;

    for (k = 0; k < 4; k++) {
        if (handle & (0x01000000 << k)) {
            int e;

            for (e = 0; e < 16; e++) {
                PATHFINDEVENT* other = &pfstates[k].events[e];

                if (other->mask && other->node == node)
                    other->time = time;
            }
        }
    }
}

static inline void settlevolumes(void) {
    int j;
    PATHTRACK* track;

    for (j = 0; j < 24; j++) {
        track = pfstate->tracks[j];
        if (track && ((1 << j) & 0xFFFFFF) && track->node >= 0) {
            int volume = track->basevolume * track->volume / 100;

            if (track->target < 0 || track->target * track->volume / 100 == PATHX_getvolume(track->handle)) {
                if (!track->target) {
                    PATHX_stop(track->handle);
                    track->node = -1;
                } else if (volume >= 0) {
                    PATHX_volume(track->handle, volume);
                }
                track->target = -1;
            }
        }
    }
}

static inline int latesttime(PATHFINDEVENT* event) {
    int k;
    int latest = 0;

    for (k = 0; k < 24; k++) {
        PATHTRACK* track = pfstate->tracks[k];
        if (track && !track->paused && track->node >= 0) {
            int queued = queuedtime(track);
            PATHFINDNODE* node = getnode(track->node);
            unsigned int type = event->type;
            int time;

            if ((type == 16 || type == 32) && node->type >= 0) {
                int status;
                int position;

                PATHX_status(track->handle, &status, &position);
                queued = position % track->length;
                if (event->type == 32)
                    queued = position % track->beatlength;
            } else if (track->direction > 0) {
                queued += (node->beats * node->bars - (track->direction - 1)) * track->length;
            }
            time = milliseconds + queued;

            if (latest < time)
                latest = time;
        }
    }
    return latest;
}

void PATHI_serviceproject(int project, int pass) {
    int k;
    PATHTRACK* track;
    unsigned int period;
    unsigned int half;
    unsigned int i;
    PATHFINDEVENT* event;
    unsigned int start;
    signed char playing;
    signed char waiting;
    signed char timer;
    int queued;
    unsigned int time;
    int diff;

    timer = bankservice == 0x42;
    period = timer ? pfstate->timerperiod : pfstate->taskperiod;
    event = &pfstate->events[0];
    if (event->mask && pfstate->leader == -1 && !event->hold)
        event->type = 64;
    half = period / 2;
    for (i = 0; i < 24; i++) {
        if (pass != (bool)(pfstate->leader == i))
            continue;
        track = pfstate->tracks[i];
        if (!track || track->paused)
            continue;
        start = milliseconds + track->latency;
        playing = isplaying(track);
        if (playing && track->fade >= 0)
            PATHI_setfadevolume(track);
        if (track->service != timer)
            continue;
        waiting = iswaiting(event, i);
        if (waiting && (event->type == 64 || event->time)) {
            int wait = 0;

            if (event->time) {
                wait = event->time - start;
                if (event->type == 16 || event->type == 32)
                    wait = event->time - milliseconds;
            }
            if (wait <= (int)period) {
                if (event->type == 16 || event->type == 32)
                    event->type = 64;
                if (event->hold && !playing)
                    track->hold = event->time;
                if (PATHI_eventstart(i) < 0)
                    continue;
                waiting = 0;
                playing = 1;
            }
        }
        if (!playing)
            continue;
        if (track->hold) {
            if (track->hold <= milliseconds + half) {
                if (track->flags & 8)
                    PATHI_crossfadetracks(project, i, event);
                PATHX_modifyhold(track->handle, 0);
                track->hold = 0;
            }
        } else if ((waiting && event->type != 64) || PATHX_readyfornewrequest(track->handle)) {
            queued = queuedtime(track);

            if (waiting && event->type != 64) {
                waiting = 0;
                time = milliseconds + eventtime(track, event, queued);
                diff = event->time - time; if (diff < 0) diff = -diff;
                if (time > event->time || (event->track == i && diff > 10)) {
                    event->time = time;
                    event->track = i;
                    if (event->hold)
                        synceventtime(-1, event->node, time);
                }
            }
            if (track->start) {
                start += period;
                if (track->start > start)
                    continue;
            } else if (queued > track->latency) {
                continue;
            }
            if (!waiting || track->direction > 0 || PATHI_eventstart(i) < 0) {
                if (!track->target && !PATHX_getvolume(track->handle)) {
                    PATHX_stop(track->handle);
                    track->node = -1;
                    track->target = -1;
                } else {
                    PATHI_seeknextnode(i);
                }
            }
        }
    }
    if (event->mask && event->mask != -1 && !event->track && !event->hold && event->type != 64 && !event->time) {
        event->time = latesttime(event);
    }
    if (event->mask == -1 && !pass && event->last < milliseconds) {
        settlevolumes();
        for (k = 0; k < 15; k++)
            pfstate->events[k] = pfstate->events[k + 1];
        pfstate->events[15].mask = 0;
    }
}
