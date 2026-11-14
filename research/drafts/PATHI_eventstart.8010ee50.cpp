struct PATHEVENTVIEW;

/* inferred: a branch to a node for values in [low, high] */
struct PATHBRANCHVIEW {
    signed char low;
    signed char high;
    unsigned short node;
};

struct PATHNODEVIEW {
    short type;
    unsigned char owner : 5;
    unsigned char unknown02_5 : 3;
    signed char unknown03_0 : 3;
    unsigned char sync : 1;
    signed char count : 4;
    unsigned char unknown04;
    unsigned char branches : 5;
    unsigned char unknown05_5 : 3;
    unsigned char beats : 4;
    unsigned char repeats : 4;
    unsigned char unknown07[5];
    PATHBRANCHVIEW branch[1];
};

struct PATHBEATVIEW {
    short type;
    unsigned short owner : 5;
    unsigned short beat : 6;
    unsigned short unknown2 : 5;
};

/* inferred: a transition word (also the track's play and restore words) */
struct PATHTRANS {
    signed int fade : 8;
    unsigned int flags : 8;
    signed int node : 16;
};

struct PATHPLAYVIEW {
    PATHTRANS trans;
    PATHTRANS restore;
    int unknown28;
    int length;
    int beatlength;
    unsigned int queued;
    unsigned int time;
    int handle;
    int beat;
};

struct PATHTRACK {
    unsigned int index : 8;
    signed int layer : 8;
    signed int paused : 8;
    signed int volume : 8;
    int unknown04;
    int unknown08;
    signed int counter : 4;
    signed int direction : 5;
    unsigned int unknown0c_9 : 5;
    unsigned int queuesize : 5;
    unsigned int queueindex : 5;
    unsigned int unknown0c_24 : 8;
    union { unsigned int latency; };
    union { void* next; };
    short loopNode;
    short event;
    PATHNODEVIEW* current;
    PATHPLAYVIEW play;
    char name[128];
    int unknownc4;
    int unknownc8;
    int unknowncc;
    int unknownd0;
    int unknownd4;
    int unknownd8;
    int unknowndc;
    int unknowne0;
    void* queue;
};

struct PATHDATAVIEW {
    unsigned char unknown00[14];
    unsigned char rows;
    unsigned char cols;
};

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

struct PATHSTATEVIEW {
    int leader;
    unsigned int latency;
    int beat;
    unsigned char unknown00c[24];
    PATHDATAVIEW* data;
    short* nodeindex;
    unsigned char unknown02c[4];
    unsigned char* matrix;
    PATHTRANS* transitions;
    unsigned char unknown038[16];
    PATHFINDEVENT events[16];
    PATHTRACK* tracks[24];
};

extern PATHSTATEVIEW* pfstate;
extern unsigned int milliseconds;

extern "C" void PATHX_volume(int, int);
extern "C" int PATHX_readyfornewrequest(int);
extern "C" void PATHX_stop(int);
int PATHI_enternode(int, int, int);
int PATHI_queuenode(PATHTRACK*, unsigned int);
int PATHI_choosebeat(PATHNODEVIEW*, PATHNODEVIEW*, int*, int*);

static inline PATHNODEVIEW* getnode(int node) {
    return (PATHNODEVIEW*)&((int*)pfstate->data)[pfstate->nodeindex[node]];
}

static inline int choosebranch(int node, int value) {
    PATHNODEVIEW* rec;
    PATHBRANCHVIEW* branches;
    int next;
    PATHTRACK* track;
    unsigned int i;

    rec = getnode(node);
    next = -1;
    branches = rec->branch;
    track = pfstate->tracks[rec->owner];

    if (track->loopNode == node) {
        int count = track->counter;

        value = count;
        value &= 0x7F;
        if (rec->count > 0 && !track->counter--)
            track->loopNode = -1;
    }
    for (i = 0; i < rec->branches; i++) {
        if (value >= branches[i].low && value <= branches[i].high) {
            next = (short)branches[i].node;
            break;
        }
    }
    return next;
}

static inline bool isplaying(PATHTRACK* track) {
    bool playing = false;

    if (track->play.trans.node >= 0 && track->current)
        playing = true;
    return playing;
}

static inline void findleader(void) {
    int j;
    PATHTRACK* track;
    pfstate->latency = 0;
    pfstate->leader = -1;
    pfstate->beat = 0;
    for (j = 0; j < 24; j++) {
        track = pfstate->tracks[j];
        if (track && !track->paused && isplaying(track) && pfstate->latency < track->latency) {
            pfstate->latency = track->latency;
            pfstate->leader = j;
            pfstate->beat = ((PATHBEATVIEW*)track->current)->beat;
        }
    }
}

int PATHI_eventstart(int index) {
    char moving;
    int result;
    char queued;
    short event;
    char restore;
    PATHTRACK* track;
    int direction;
    PATHTRACK saved;
    PATHTRANS back;
    PATHTRANS trans;
    int node;

    queued = 1;
    restore = 0;
    moving = 0;
    result = 0;
    event = pfstate->events[0].node;
    track = pfstate->tracks[index];
    direction = track->next ? 1 : -1;

    if (!pfstate->events[0].type && !PATHX_readyfornewrequest(track->play.handle))
        return -1;
    saved = *track;
    trans = track->play.trans;
    back = trans;
    trans.flags = 0;
    if (track->play.trans.node >= 0)
        back.node = choosebranch(track->play.trans.node, track->layer);

    if (event < 0) {
        short first = -(event + 1);
        PATHNODEVIEW* rec = getnode(first);

        track->play.trans.node = PATHI_enternode(track->play.trans.node, first, track->layer);
        result = PATHI_queuenode(track, pfstate->events[0].type);
        queued = result >= 0;
        if (rec->type == 0)
            track->current = rec;
    } else {
        PATHTRANS entry;
        int offset = event * pfstate->data->rows;
        offset += pfstate->data->rows * (track->index * pfstate->data->cols);
        int cell;
        short next;

        if (pfstate->leader < 0)
            findleader();
        cell = offset + pfstate->beat;
        entry = pfstate->transitions[pfstate->matrix[cell]];
        if (entry.node >= 0 && getnode(entry.node)->sync)
            entry.flags |= 4;
        if ((short)cell < 0) {
            next = -1;
        } else {
            if ((entry.flags & 3) || entry.fade >= 0)
                trans = entry;
            next = entry.node;
        }
        node = next;
        if (pfstate->events[0].hold)
            trans.flags |= 8;
        if (trans.flags & 4)
            restore = 1;
        if (trans.flags & 3)
            moving = 1;
        if (trans.fade >= 0) {
            if (!moving)
                restore = 1;
            track->play.trans.fade = trans.fade;
        }
        if (trans.flags & 8) {
            track->play.trans.flags |= 8;
            if (track->play.trans.node >= 0) {
                result = -1;
                goto done;
            }
            track->play.trans.fade = 0;
        }
        if (!moving) {
            result = -1;
        } else {
            track->play.queued = 0;
            track->unknown0c_9 = 0;
            if (node >= 0)
                node = PATHI_enternode(track->play.trans.node, node, track->layer);
            if (node >= 0) {
                PATHNODEVIEW* rec = getnode(node);

                if (track->current->sync && !restore) {
                    restore = 1;
                    back.node = trans.node;
                }
                if (index != pfstate->leader)
                    direction = PATHI_choosebeat(rec, track->current, (int*)&track->play.queued, 0);
                track->play.trans.node = node;
                track->direction = (signed char)direction;
                if (track->play.queued) {
                    track->play.queued += milliseconds;
                    if (pfstate->events[0].last < track->play.queued)
                        pfstate->events[0].last = track->play.queued;
                } else {
                    result = PATHI_queuenode(track, pfstate->events[0].type);
                    queued = result >= 0;
                }
            } else {
                if (back.node >= 0) {
                    if (pfstate->events[0].type == 64) {
                        PATHX_stop(track->play.handle);
                    } else if (pfstate->events[0].time < pfstate->events[0].last) {
                        pfstate->events[0].time = pfstate->events[0].last;
                        *track = saved;
                        return -1;
                    }
                }
                track->play.beat = -1;
                track->current = 0;
                track->play.trans.node = -1;
            }
        }
    }
done:
    if (restore) {
        track->play.restore = back;
        if (trans.flags & 3)
            track->play.restore.flags = trans.flags & 3;
        track->event = event;
    } else {
        track->play.restore.flags = 0;
        track->play.restore.node = -1;
        track->event = -1;
    }
    if (queued)
        pfstate->events[0].mask |= 1 << track->index;
    if (result < 0) {
        if (trans.fade < 0)
            *track = saved;
        return -1;
    }
    return track->play.trans.node;
}
