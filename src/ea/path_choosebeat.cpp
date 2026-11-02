/* A fragment of EA's music path player (path.c, 0x8010e254): PATHI_choosebeat
   picks when a path may branch from the leading track to another node, by its
   sync mode (next file, next beat or a stepped position). PATHFINDNODE,
   PATHTRACK, pfstate and the functions are named by their symbols; the layouts
   and the member names are inferred (a separate view from the other path.c
   fragments). The functions around this one are not part of the unit. */
struct PATHFINDNODE {
    short type;
    unsigned char track : 5;
    unsigned char unknown02_5 : 3;
    unsigned char unknown03;
    unsigned char map;
    unsigned char unknown05;
    unsigned char bars : 4;
    unsigned char beats : 4;
    unsigned char step : 4;
    unsigned char start : 4;
    unsigned char unit : 4;
    unsigned char sync : 2;
    unsigned char unknown08_6 : 2;
};

struct PATHFILE {
    int offset;
    int length;
};

struct PATHTRACK {
    unsigned char unknown00[12];
    signed short count : 4;
    signed short direction : 5;
    unsigned short unknown0c_9 : 7;
    unsigned char unknown0e[2];
    int latency;
    void* next;
    unsigned char unknown18[4];
    PATHFINDNODE* current;
    unsigned char unknown20[2];
    short node;
    unsigned char unknown24[8];
    int barlength;
    unsigned char unknown30[12];
    int handle;
};

struct PATHSTATEVIEW {
    int leader;
    unsigned char unknown004[32];
    int* nodes;
    short* nodeindex;
    unsigned char unknown02c[20];
    PATHFILE* files;
    unsigned char unknown044[260];
    PATHTRACK* tracks[24];
};

extern PATHSTATEVIEW* pfstate;

extern "C" void PATHX_status(int, int*, int*);

static inline PATHFILE* getfile(int file) {
    return &pfstate->files[file - 1];
}

int PATHI_choosebeat(PATHFINDNODE* from, PATHFINDNODE* to, int* offset, int* extra) {
    int length;
    int remaining;
    int position;
    int fromlength;
    PATHTRACK* leader;
    PATHFINDNODE* node;
    PATHTRACK* track;
    int beat;
    int frombeat;
    int time;
    int result;

    *offset = 0;
    if (pfstate->leader < 0)
        return 0;
    leader = pfstate->tracks[pfstate->leader];
    if (!leader->current)
        return 0;
    if (!to->sync)
        return 0;
    node = (PATHFINDNODE*)&pfstate->nodes[pfstate->nodeindex[leader->node]];
    length = getfile(node->type)->length;
    fromlength = ((int*)pfstate->files)[(from->type - 1) * 2 + 1];
    beat = length;
    if (node->beats)
        beat = length / (node->beats * node->bars);
    frombeat = fromlength;
    if (from->beats)
        frombeat = fromlength / (from->beats * from->bars);
    PATHX_status(leader->handle, &remaining, &position);
    if (remaining < 0 || position < 0)
        return 0;
    if (leader->direction > 0) {
        position += (node->beats * node->bars - (leader->direction - 1)) * leader->barlength;
        remaining = length - position;
    }
    result = -1;
    track = pfstate->tracks[from->track];
    time = remaining + track->latency;
    if (track->next)
        result = 1;
    if (to->sync == 2) {
        for (*offset = position - fromlength; *offset < track->latency; *offset += length)
            ;
    } else if (to->sync == 1) {
        time %= fromlength;
        result = time / frombeat + 2;
        *offset = (result - 1) * frombeat - time;
        if (extra)
            *extra = *offset + beat;
    } else if (node->beats && to->start && to->step) {
        int step = (int)((float)beat * ((float)node->unit / (float)to->unit));
        for (*offset = (to->start - 1) * step; *offset < time; *offset += to->step * step)
            ;
        *offset -= remaining;
    }
    return result;
}
