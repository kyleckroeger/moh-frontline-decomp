/* A fragment of EA's music path player (path.c, 0x8010ed14):
   PATHI_eventrestore, which ends an event on every active track of the
   current player state that it holds: it restores the track's fade level
   (setting the volume scaled by the track's volume percentage), clears the
   event, and then either stops the track (restore flag 2) or re-enters its
   saved node (flag 1), setting the direction from whether a next node is
   linked, and queues it. PATHTRACK, pfstate and the functions are named by
   their symbols; the track and state layouts (and their member names) are
   inferred. The functions around this one are not part of the unit. */
struct PATHEVENTVIEW;

/* inferred: a branch to a node for values in [low, high] */
struct PATHBRANCHVIEW {
    signed char low;
    signed char high;
    unsigned short node;
};

/* inferred: a node record (byte 2: owner track index in bits 3..7; byte 3:
   a signed count in the low nibble; byte 5: the number of branches in bits
   3..7; byte 6: beat and repeat nibbles; branches of {signed char low,
   signed char high, unsigned short node} from +12) */
struct PATHNODEVIEW {
    unsigned char unknown00[2];
    unsigned char owner : 5;
    unsigned char unknown02_5 : 3;
    signed char unknown03_0 : 4;
    signed char count : 4;
    unsigned char unknown04;
    unsigned char branches : 5;
    unsigned char unknown05_5 : 3;
    unsigned char beats : 4;
    unsigned char repeats : 4;
    unsigned char unknown07[5];
    PATHBRANCHVIEW branch[1];
};

struct PATHPLAYVIEW {
    unsigned int fade : 8;
    unsigned int mode : 8;
    signed int node : 16;
    unsigned int level : 8;
    unsigned int restore : 8;
    signed int restoreNode : 16;
    int unknown28;
    int length;
    int beatlength;
    unsigned int queued;
    unsigned int time;
    int handle;
    int beat;
};

struct PATHTRACK {
    unsigned int unknown00 : 8;
    signed int layer : 8;
    unsigned int unknown02 : 8;
    signed int volume : 8;
    int unknown04;
    int unknown08;
    signed int counter : 4;
    signed int direction : 5;
    unsigned int unknown0c_9 : 5;
    unsigned int queuesize : 5;
    unsigned int queueindex : 5;
    unsigned int unknown0c_24 : 8;
    int unknown10;
    void* next;
    short loopNode;
    short event;
    PATHEVENTVIEW* active;
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

struct PATHSTATEVIEW {
    unsigned char unknown000[36];
    int* nodes;
    short* nodeindex;
    unsigned char unknown02c[328 - 44];
    PATHTRACK* tracks[24];
};

extern PATHSTATEVIEW* pfstate;

extern "C" void PATHX_volume(int, int);
int PATHI_enternode(int, int, int);
int PATHI_queuenode(PATHTRACK*, unsigned int);

static inline PATHNODEVIEW* getnode(int node) {
    return (PATHNODEVIEW*)&pfstate->nodes[pfstate->nodeindex[node]];
}

static inline int choosebranch(int node, int value) {
    PATHNODEVIEW* rec = getnode(node);
    int next = -1;
    PATHBRANCHVIEW* branches = rec->branch;
    PATHTRACK* track = pfstate->tracks[rec->owner];
    unsigned int i;

    if (track->loopNode == node) {
        int count = track->counter;

        value = count & 0x7F;
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

void PATHI_seeknextnode(int index) {
    long next = -1;
    PATHTRACK* track;
    PATHTRACK saved;
    PATHNODEVIEW* info;
    unsigned int direction;

    track = pfstate->tracks[index];
    saved = *track;
    info = getnode(track->play.node);
    direction = saved.next ? 1 : -1;

    if (track->direction > 0 && track->direction <= info->repeats * info->beats) {
        next = track->play.node;
        direction = track->direction;
    } else if (track->play.node >= 0) {
        next = choosebranch(track->play.node, track->layer);
    }
    if (next >= 0)
        next = PATHI_enternode(track->play.node, next, track->layer);
    track->play.node = next;
    track->direction = (signed char)direction;
    track->play.time = track->play.queued;
    track->play.queued = 0;
    if (PATHI_queuenode(track, 0) < 0)
        *track = saved;
}

void PATHI_eventrestore(short event) {
    PATHTRACK* track;
    int i;

    for (i = 0; i < 24; i++) {
        track = pfstate->tracks[i];
        if (track && track->active && event >= 0 && track->event == event) {
            int level = track->play.level & 0x7F;

            PATHX_volume(track->play.handle, level * track->volume / 100);
            track->play.fade = level;
            track->event = -1;
            if (track->play.restore & 2) {
                track->play.beat = -1;
                track->play.node = -1;
                track->play.queued = 0;
                track->active = 0;
            } else if (track->play.restore & 1) {
                track->play.node = PATHI_enternode(0, track->play.restoreNode, track->layer);
                track->play.mode = track->play.restore;
                track->direction = track->next ? 1 : -1;
                track->play.queued = 0;
                PATHI_queuenode(track, 0);
            }
        }
    }
}
