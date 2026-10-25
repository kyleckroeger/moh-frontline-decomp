/* A fragment of the music path player's path.c (0x8010f748..0x8010fd80).
   PATHI_enternode enters a node (remapped through the current map of the
   node it comes from) and walks the control nodes until it reaches a
   playable one: an event node (type 0) becomes the track's active event,
   may pick a random branch value and reports progress through the song
   callback; a loop node (-1) starts its repeat count on its track; a random
   node (-2) picks a random value. Each step takes the branch whose value
   range holds the value (a track looping on the node uses its remaining
   count and ends the loop when the count runs out) and remaps the result.
   PATHI_setfadevolume moves a fading track's volume along its fade: the fade
   comes from the state's table (the track's curve, from 1) or is the state's
   default shape and duration (reversed when fading up); the elapsed share of
   the duration selects the volume between the start and target levels
   (shape 1 linear, 2 quadratic, 3 a 1/(25t) curve), scaled to 0..127, and a
   finished fade sets the base volume, stops a track faded to silence and
   clears the fade. PATHTRACK is named by the mangled symbol; its fields, the
   node, branch and state views, the fade record and the inline helpers are
   inferred from the code (as in path_stop.c); pfstate, pfactive,
   songprogress, milliseconds and the functions are named by their symbols.
   The rest of the file is in other units. */
struct PATHFADEVIEW {
    unsigned int shape : 7;
    unsigned int reverse : 1;
    unsigned int unknown1 : 8;
    unsigned int duration : 16;
};

struct PATHTRACK {
    unsigned char unknown00[4];
    unsigned int fadetime;
    signed char fadestart;
    signed char fadecurve;
    signed char fadetarget;
    unsigned char unknown0b;
    signed short count : 4;
    signed short direction : 5;
    unsigned short unknown0c_9 : 7;
    unsigned char unknown0e[10];
    short loopnode;
    unsigned char unknown1a[2];
    void* active;
    signed char basevolume;
    unsigned char unknown21;
    short node;
    unsigned char unknown24[24];
    int handle;
};

/* inferred: a branch to a node for values in [low, high] */
struct PATHBRANCH {
    signed char low;
    signed char high;
    unsigned short node;
};

/* inferred: a node record in the project's node data */
struct PATHNODE {
    short type;
    unsigned char track : 5;
    unsigned char unknown02_5 : 3;
    unsigned char unknown03 : 4;
    signed char repeat : 4;
    unsigned char map;
    unsigned char branches : 5;
    unsigned char mode : 3;
    unsigned char unknown06[6];
    PATHBRANCH branch[1];
};

struct PATHSTATEVIEW {
    unsigned char unknown00[20];
    int fadeduration;
    int fadeshape;
    unsigned char unknown1c[8];
    int* nodes;
    short* nodeindex;
    unsigned char unknown2c[12];
    int* maps;
    unsigned char unknown3c[8];
    PATHFADEVIEW* fades;
    unsigned char unknown48[256];
    PATHTRACK* tracks[24];
};

extern PATHSTATEVIEW* pfstate;
extern unsigned int pfactive;
extern int milliseconds;
extern void (*songprogress)(unsigned int, int);
extern "C" void PATHX_volume(int, int);
extern "C" void PATHX_stop(int);

static inline PATHNODE* getnode(int node) {
    return (PATHNODE*)&pfstate->nodes[pfstate->nodeindex[node]];
}

static inline int remapnode(int from, int node) {
    int start;
    int count;
    int* entry;
    int i;
    int result = node;

    if (from < 0)
        return result;
    if (!getnode(from)->map)
        return result;
    start = pfstate->maps[getnode(from)->map - 1];
    entry = &pfstate->nodes[start];
    count = pfstate->maps[getnode(from)->map] - start;
    for (i = 0; i < count; i++, entry++) {
        if (result == *entry >> 16)
            result = *entry & 0xFFFF;
    }
    return result;
}
static inline unsigned char pathrandom(void) {
    return (milliseconds / 23) & 0x7F;
}

static inline int choosebranch(int node, int value) {
    PATHNODE* rec = getnode(node);
    int next = -1;
    PATHBRANCH* branches = rec->branch;
    PATHTRACK* track = pfstate->tracks[rec->track];
    unsigned int i;

    if (track->loopnode == node) {
        int count = track->count;

        value = count & 0x7F;
        if (rec->repeat > 0 && !track->count--)
            track->loopnode = -1;
    }
    for (i = 0; i < rec->branches; i++) {
        if (value >= branches[i].low && value <= branches[i].high) {
            next = (short)branches[i].node;
            break;
        }
    }
    return next;
}

short PATHI_enternode(int from, int node, int value) {
    PATHNODE* rec;
    PATHTRACK* track;
    int saved = value;

    node = (short)remapnode(from, node);
    track = pfstate->tracks[(rec = getnode(node))->track];
    while (rec->type <= 0) {
        if (rec->type == 0) {
            track->active = rec;
            if (rec->mode == 1)
                value = pathrandom();
            if (songprogress)
                songprogress(pfactive, node);
        } else if (rec->type == -1) {
            if (rec->repeat && node != track->loopnode) {
                track->count = rec->repeat;
                track->loopnode = node;
            }
        } else if (rec->type == -2) {
            value = pathrandom();
        }
        node = (short)remapnode(from, choosebranch(node, value));
        value = saved;
        rec = getnode(node);
    }
    return (short)remapnode(from, node);
}

void PATHI_setfadevolume(PATHTRACK* track) {
    signed char start;
    signed char target;
    target = track->fadetarget;
    start = track->fadestart;
    float delta = (float)target - (float)start;
    PATHFADEVIEW fade;
    float t;
    float k = -1.0f;
    float low;

    if (track->fadecurve > 0) {
        fade = pfstate->fades[track->fadecurve - 1];
    } else {
        fade.duration = pfstate->fadeduration;
        fade.shape = pfstate->fadeshape;
        fade.reverse = delta < 0.0f ? 0 : 1;
    }
    t = (float)(milliseconds - track->fadetime) / (float)fade.duration;
    if (t >= 1.0f) {
        PATHX_volume(track->handle, track->basevolume);
        if (!track->fadetarget) {
            PATHX_stop(track->handle);
            track->node = -1;
        }
        track->fadestart = -1;
        track->fadetarget = -1;
        track->fadecurve = -1;
        track->fadetime = 0;
        track->basevolume = track->fadetarget;
        return;
    }
    if (fade.reverse)
        t = 1.0f - t;
    switch (fade.shape) {
    case 1:
        k = 1.0f - t;
        break;
    case 2:
        k = 1.0f - t * t;
        break;
    case 3:
        k = 1.0f / t / 25.0f;
        break;
    }
    low = start;
    if (target < start)
        low = target;
    if (delta < 0.0f)
        delta = -delta;
    if (k >= 0.0f && k <= 1.0f) {
        float volume = delta * k + low;
        volume = 1.27f * volume;
        PATHX_volume(track->handle, (int)volume);
    }
}
