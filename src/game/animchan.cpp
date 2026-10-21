// Animation channels: per-object arrays of 136-byte channel records driven by
// state. This unit covers the accessors at the start of the file and
// AnimChanSwitchAnimByDuration, which switches a running channel to another
// animation, keeping its position within the old duration: the new
// animation's rate is set so it lasts as long as the old one, and it is
// advanced by the time already played (the channel is marked done when the
// new animation ends at once), and AnimChanStopByStateIndex, which stops
// every active channel of a state. The record views (channel, owner, sequence
// info and frame-list header) are inferred from offsets; their names are not
// original.
struct AnimSeqFrameList_t;

struct AnimSeqAnimInfo_t {
    AnimSeqFrameList_t* list;
    float rate;
    float life;
    float time;
    unsigned short start;
    unsigned short frame;
    float progress;
    unsigned short position;
    unsigned short frames;
    unsigned char field1C[20];
};

struct AnimFrameListHeaderView {
    unsigned char field00[2];
    unsigned char flags;
};

struct AnimChannelView {
    unsigned char field00;
    unsigned char status;
    unsigned char field02[2];
    unsigned short fileNum;
    unsigned char field06[2];
    void* file;
    unsigned char flags;
    unsigned char field0d;
    unsigned short state;
    unsigned char field10[64];
    float delay;
    float speed;
    AnimSeqAnimInfo_t seq;
};

extern "C" {
unsigned short AnimFileGetFileNum(void*);
AnimFrameListHeaderView* AnimFileGetAnimation(void*, int);
unsigned short AnimSeqGetFrameCount(AnimSeqFrameList_t*);
unsigned char AnimSeqStart(AnimSeqAnimInfo_t*, AnimFrameListHeaderView*);
void AnimSeqSetRate(AnimSeqAnimInfo_t*, float);
unsigned char AnimSeqGrow(AnimSeqAnimInfo_t*, float);
double fmod(double, double);
}
/* inferred inline helper: the call goes through an inline (a direct call
   allocates the surrounding registers differently) */
static inline AnimFrameListHeaderView* GetAnimation(void* file, int anim) {
    return AnimFileGetAnimation(file, anim);
}
void _AnimUserCallbackPushState(unsigned long);
void _AnimUserCallbackPushInfo(void*);
void _AnimUserCallbackPopState();
void _AnimUserCallbackPopInfo();

struct AnimChannelOwnerView {
    unsigned char field00[4];
    unsigned short count;
};

extern "C" void AnimChanSetSpeed(AnimChannelView* channel, float speed) {
    channel->speed = speed;
}

extern "C" void AnimChanSetDelayByState(AnimChannelOwnerView* owner, AnimChannelView* channel, unsigned short state, float delay) {
    for (int i = 0; i < owner->count; channel++, i++) {
        if (channel->status == 2 && state == channel->state)
            channel->delay = delay;
    }
}

extern "C" unsigned short AnimChanGetStateChannelByIndex(AnimChannelOwnerView* owner, AnimChannelView* channel,
                                                          unsigned short state, unsigned short index) {
    int found = 0;

    for (int i = 0; i < owner->count; channel++, i++) {
        if ((channel->status == 2 || channel->status == 1) && state == channel->state) {
            if ((unsigned short)found == index)
                return i;
            found++;
        }
    }
    return 0xFFFF;
}

extern "C" int AnimChanGetNumRunningOrDoneByState(AnimChannelOwnerView* owner, AnimChannelView* channel, unsigned short state) {
    int count = 0;

    for (int i = 0; i < owner->count; channel++, i++) {
        if ((channel->status == 2 || channel->status == 1) && state == channel->state)
            count++;
    }
    return count;
}

extern "C" int AnimChanGetNumActiveByState(AnimChannelOwnerView* owner, AnimChannelView* channel, unsigned short state) {
    int count = 0;

    for (int i = 0; i < owner->count; channel++, i++) {
        if (channel->status == 2 && state == channel->state)
            count++;
    }
    return count;
}

extern "C" void AnimChanSwitchAnimByDuration(AnimChannelOwnerView* owner, AnimChannelView* channels, int index,
                                             void* file, int anim, unsigned short state, void* info) {
    if (channels[index].status == 2) {
        float elapsed;
        float duration;
    channels[index].fileNum = AnimFileGetFileNum(file);
    channels[index].file = file;
    channels[index].state = state;
    if (channels[index].seq.frames == 0xFFFF)
        channels[index].seq.frames = AnimSeqGetFrameCount(channels[index].seq.list);
    duration = channels[index].seq.rate * (float)channels[index].seq.frames;
    elapsed = fmod(channels[index].seq.life, duration);
    AnimFrameListHeaderView* list = GetAnimation(file, anim);
    _AnimUserCallbackPushState(channels[index].state);
    _AnimUserCallbackPushInfo(info);
    unsigned char ended = AnimSeqStart(&channels[index].seq, list);
    _AnimUserCallbackPopState();
    _AnimUserCallbackPopInfo();
    channels[index].flags = list->flags;
    if (ended == 1) {
        channels[index].status = 1;
    } else {
    channels[index].seq.frames = AnimSeqGetFrameCount(channels[index].seq.list);
    AnimSeqSetRate(&channels[index].seq, duration / (float)channels[index].seq.frames);
    _AnimUserCallbackPushInfo(info);
    _AnimUserCallbackPushState(channels[index].state);
    ended = AnimSeqGrow(&channels[index].seq, elapsed);
    _AnimUserCallbackPopState();
    _AnimUserCallbackPopInfo();
    if (ended == 1)
        channels[index].status = 1;
    }
    }
}

extern "C" void AnimChanStopByStateIndex(AnimChannelOwnerView* owner, AnimChannelView* channels, unsigned short state) {
    AnimChannelView* channel = channels;

    for (int i = 0; i < owner->count; channel++, i++) {
        if (channel->status && state == channel->state)
            /* the status byte cleared through a byte offset into the array
               (the original addresses it as base + i * 136 + 1) */
            ((unsigned char*)channels)[i * sizeof(AnimChannelView) + 1] = 0;
    }
}

// AnimChanStartAnim, AnimChanProcess and AnimChanInitChannels follow in the
// original file and are not reconstructed.
