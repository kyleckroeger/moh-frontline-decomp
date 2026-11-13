// Animation channels: per-object arrays of 136-byte channel records driven by
// state. First the accessors and AnimChanSwitchAnimByDuration, which switches
// a running channel to another animation, keeping its position within the old
// duration: the new animation's rate is set so it lasts as long as the old
// one, and it is advanced by the time already played (the channel is marked
// done when the new animation ends at once), and AnimChanStopByStateIndex,
// which stops every active channel of a state. AnimChanStartAnim takes the
// first free channel (status 0, then status 1), else the faded-out channel
// (state flag 0x8000) with the lowest weight, and starts the animation on it
// (weight, file, sequence start under the callback state and info); it
// returns the channel index or -1. AnimChanProcess advances each of the
// owner's running channels by the elapsed time scaled by its speed: a channel
// finishing last frame (status 1) is stopped; once its delay has run out the
// channel's weight is processed (status 1 when it fades out below 1e-7 with a
// weight curve), then its sequence grows under the channel's callback info and
// state (status 1 when the sequence ends). AnimChanInitChannels, last, resets
// each of the owner's channels: the first byte is 1 when the owner's first
// halfword is zero and 4 otherwise, the status is cleared, the sequence has no
// frame list and the speed is 1. The record views (channel, owner, sequence
// info and frame-list header) are inferred from offsets; their names are not
// original. The weight record at +52 follows animwgt.cpp (its first three
// members; the type is unsigned because the code compares it unsigned).
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

struct AnimWgtInfo_t {
    float weight;
    float target;
    unsigned int type;
};

struct AnimChannelView {
    unsigned char field00;
    unsigned char status;
    unsigned short anim;
    unsigned short fileNum;
    unsigned char field06[2];
    void* file;
    unsigned char flags;
    unsigned char field0d;
    unsigned short state;
    void* field10;
    int field14;
    int field18;
    unsigned char field1c[4];
    float field20;
    float field24;
    float field28;
    unsigned char field2c[4];
    float field30;
    AnimWgtInfo_t weight;
    unsigned char field40[16];
    float delay;
    float speed;
    AnimSeqAnimInfo_t seq;
};

struct AnimChannelOwnerView {
    unsigned short kind;
    unsigned char field02[2];
    unsigned short count;
    unsigned char field06[2];
    void* field08;
};

struct AnimFrameListHeaderView {
    unsigned char field00[2];
    unsigned char flags;
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

extern "C" {
unsigned char AnimSeqGrow(AnimSeqAnimInfo_t*, float);
void AnimWgtProcess(AnimWgtInfo_t*, float);
void AnimWgtSet(AnimWgtInfo_t*, float, float, int, void*);
unsigned short AnimFileGetFileNum(void*);
AnimFrameListHeaderView* AnimFileGetAnimation(void*, int);
unsigned char AnimSeqStart(AnimSeqAnimInfo_t*, AnimFrameListHeaderView*);
}
void _AnimUserCallbackPushState(unsigned long);
void _AnimUserCallbackPushInfo(void*);
void _AnimUserCallbackPopState();
void _AnimUserCallbackPopInfo();

static inline void StartChannel(AnimChannelOwnerView* owner, AnimChannelView* channel, void* file, int anim,
                                unsigned short state, void* info, float weight) {
    AnimWgtSet(&channel->weight, weight, weight, 0, 0);
    channel->fileNum = AnimFileGetFileNum(file);
    channel->file = file;
    channel->anim = anim;
    channel->state = state;
    channel->field10 = owner->field08;
    channel->field14 = 0;
    channel->field28 = 0.0f;
    channel->field24 = 0.0f;
    channel->field20 = 0.0f;
    channel->field30 = 1.0f;
    channel->field18 = 0x1000000;
    channel->delay = 0.0f;
    channel->speed = 1.0f;
    AnimFrameListHeaderView* animation = AnimFileGetAnimation(file, anim);
    _AnimUserCallbackPushState(channel->state);
    _AnimUserCallbackPushInfo(info);
    unsigned char started = AnimSeqStart(&channel->seq, animation);
    _AnimUserCallbackPopState();
    _AnimUserCallbackPopInfo();
    channel->flags = animation->flags;
    if (!started)
        channel->status = 2;
}

static inline unsigned int FindChannel(AnimChannelOwnerView* owner, AnimChannelView* channels) {
    int index;
    unsigned char status;

    for (status = 0; status < 2; status++) {
        for (index = 0; index < owner->count; index++) {
            if (channels[index].status == status)
                return index;
        }
    }
    {
        float lowest = 1.0f;
        index = -1;
        int i;
        AnimChannelView* ch;
        for (ch = channels, i = 0; i < owner->count; ch++, i++) {
            if ((ch->state & 0x8000) && ch->weight.weight <= lowest) {
                lowest = ch->weight.weight;
                index = i;
            }
        }
        if (index != -1)
            channels[index].status = 0;
        else
            index = -1;
    }
    return index;
}

extern "C" unsigned int AnimChanStartAnim(AnimChannelOwnerView* owner, AnimChannelView* channels, void* file, int anim,
                                          unsigned short state, void* info, float weight) {
    int index = FindChannel(owner, channels);
    if ((unsigned int)index != 0xFFFFFFFF)
        StartChannel(owner, &channels[index], file, anim, state, info, weight);
    return index;
}

extern "C" void AnimChanProcess(AnimChannelOwnerView* owner, AnimChannelView* channels, void* info, float elapsed) {
    for (int i = 0; i < owner->count; i++) {
        AnimChannelView* channel = &channels[i];
        float speed = channel->speed;
        if (channel->status == 1)
            channel->status = 0;
        if (channel->status != 0) {
            float step = elapsed * speed;
            float delay = channel->delay - step;
            channel->delay = delay;
            if (delay < 0.0f) {
                channel->delay = 0.0f;
                unsigned int type = channel->weight.type;
                AnimWgtProcess(&channel->weight, step);
                if (channel->weight.weight < 1e-7f && type != 0)
                    channel->status = 1;
                if (channel->status != 0) {
                    _AnimUserCallbackPushInfo(info);
                    _AnimUserCallbackPushState(channel->state);
                    unsigned char ended = AnimSeqGrow(&channel->seq, step);
                    _AnimUserCallbackPopState();
                    _AnimUserCallbackPopInfo();
                    if (ended == 1)
                        channel->status = 1;
                }
            }
        }
    }
}

extern "C" void AnimChanInitChannels(AnimChannelOwnerView* owner, AnimChannelView* channels) {
    unsigned char mode = 4;
    if (owner->kind == 0)
        mode = 1;
    for (int i = 0; i < owner->count; i++) {
        channels->field00 = mode;
        channels->status = 0;
        channels->seq.list = 0;
        channels->speed = 1.0f;
        channels++;
    }
}
