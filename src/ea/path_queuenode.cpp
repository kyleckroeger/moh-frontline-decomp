/* A fragment of EA's music path player (path.c, 0x8010e54c): PATHI_queuenode
   queues a track's current node on its stream. A track without a node gets
   no beat and ends its event. Otherwise the time still queued on the stream
   is summed; a track that does not move on (no next node) is marked to play
   backwards (-1) and, when nothing is queued, has its start time set, while
   a moving track starts forwards. The node's sample (length, and offset when
   the node index is valid) is queued with the track's volume scaled by its
   fade level (or -1 while crossfading with something queued), and the beat
   and full lengths are divided by the node's beat and repeat counts. A
   moving track is aligned with the current state's track (when its event
   allows overlap) to the next beat after half its length, and counts its
   repeats in the direction field. A queued beat is recorded in the track's
   queue ring. PATHTRACK, pfstate and the functions are named by their
   symbols; the track, node and state layouts and the member and inline
   names are inferred (a separate view from the other path.c fragments).
   The functions around this one are not part of the unit. */
struct PATHNODEVIEW {
    short sample;
    unsigned char unknown02[4];
    unsigned char beats : 4;
    unsigned char repeats : 4;
};

struct PATHDATAVIEW {
    unsigned char unknown00[18];
    unsigned short nodes;
};

struct PATHQUEUEVIEW {
    int node;
    int length;
    int direction;
    int beat;
};

struct PATHEVENTVIEW {
    unsigned char unknown00[10];
    unsigned char overlap : 1;
};

struct PATHTRACK {
    unsigned char unknown00;
    signed char layer;
    unsigned char unknown02;
    signed char volume;
    unsigned char unknown04[8];
    unsigned int unknown0c : 4;
    signed int direction : 5;
    unsigned int unknown0c_9 : 5;
    unsigned int queuesize : 5;
    unsigned int queueindex : 5;
    unsigned int unknown0c_24 : 8;
    unsigned char unknown10[4];
    void* next;
    unsigned char unknown18[2];
    short event;
    PATHEVENTVIEW* active;
    signed char fade;
    unsigned char mode;
    short node;
    unsigned char unknown24[8];
    int length;
    int beatlength;
    unsigned int start;
    unsigned int time;
    int handle;
    int beat;
    unsigned char name[160];
    PATHQUEUEVIEW* queue;
};

struct PATHSTATEVIEW {
    int current;
    unsigned char unknown004[32];
    PATHDATAVIEW* data;
    short* nodeindex;
    unsigned char unknown02c[20];
    int* samples; /* {offset, length} pairs */
    unsigned char unknown044[16];
    unsigned int crossfade;
    unsigned char unknown058[240];
    PATHTRACK* tracks[24];
};

extern PATHSTATEVIEW* pfstate;
extern int milliseconds;

extern "C" int PATHX_timeremaining(int, int);
extern "C" int PATHX_status(int, int*, int*);
extern "C" int PATHX_queuefile(int, int, void*, int, int, signed char, int, unsigned int);
void PATHI_eventrestore(short);

static inline PATHNODEVIEW* getnode(int node) {
    return (PATHNODEVIEW*)&((int*)pfstate->data)[pfstate->nodeindex[node]];
}

static inline int nodeoffset(int node) {
    int offset = 0;

    if (node < 0 || node >= pfstate->data->nodes) {
        offset = 0;
    } else {
        short sample = getnode(node)->sample;
        if (sample > 0)
            offset = pfstate->samples[sample * 2 - 2];
    }
    return offset;
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

static inline int trackposition(PATHTRACK* track, int* period) {
    int position;
    int status;
    int length;
    PATHNODEVIEW* node;
    int index;

    if (!track)
        return 0;
    node = getnode(track->node);
    index = node->sample - 1;
    length = *(int*)((char*)pfstate->samples + index * 8 + 4);
    if (node->repeats)
        length /= node->repeats * node->beats;
    PATHX_status(track->handle, &status, &position);
    index = position % length;
    if (period)
        *period = length;
    return index;
}

int PATHI_queuenode(PATHTRACK* track, unsigned int flags) {
    int direction;
    int priority = 100;

    if (track->node >= 0) {
        int remaining;
        int volume;
        int offset;
        PATHNODEVIEW* node;

        remaining = queuedtime(track);
        if (!track->next) {
            if (remaining <= 0) {
                if (!track->time)
                    track->time = milliseconds;
                if (track->start)
                    track->time = track->start;
                priority = -1;
            }
            track->direction = -1;
        } else if (track->direction <= 0) {
            track->direction = 1;
        }
        direction = track->direction;
        if (pfstate->crossfade && remaining)
            volume = -1;
        else
            volume = track->fade * track->volume / 100;
        offset = 0;
        node = getnode(track->node);
        track->length = track->beatlength = pfstate->samples[node->sample * 2 - 1];
        offset = nodeoffset(track->node);
        track->beat = PATHX_queuefile(track->handle, priority, track->name, offset, track->length, volume,
                                      track->direction, flags);
        if (node->beats)
            track->beatlength /= node->beats;
        if (node->repeats)
            track->length /= node->repeats * node->beats;
        if (track->next) {
            PATHTRACK* other = pfstate->tracks[pfstate->current];

            if (other && track != other && track->active->overlap) {
                int period;
                int position = trackposition(other, &period);

                if (position < track->length / 2)
                    position += period;
                track->start = milliseconds + position;
            } else {
                track->start = 0;
            }
            track->time = 0;
            if (track->direction++ >= node->repeats * node->beats)
                track->direction = 0;
        }
        if (track->beat >= 0) {
            PATHQUEUEVIEW* entry = &track->queue[track->queueindex];
            PATHNODEVIEW* current = getnode(track->node);
            entry->beat = track->beat;
            entry->node = track->node;
            entry->length = pfstate->samples[current->sample * 2 - 1];
            entry->direction = direction;
            track->queueindex++;
            if (track->queueindex >= track->queuesize)
                track->queueindex = 0;
        }
    } else {
        track->beat = -1;
        if (track->active)
            PATHI_eventrestore(track->event);
        return 0;
    }
    return track->beat;
}
