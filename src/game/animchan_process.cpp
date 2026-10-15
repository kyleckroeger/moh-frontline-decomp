// A fragment of animchan.cpp (0x80061acc): AnimChanProcess advances each of
// the owner's running channels by the elapsed time scaled by its speed: a
// channel finishing last frame (status 1) is stopped; once its delay has
// run out the channel's weight is processed (status 1 when it fades out
// below 1e-7 with a weight curve), then its sequence grows under the
// channel's callback info and state (status 1 when the sequence ends). The
// record views follow animchan.cpp and animchan_init.cpp; the weight record
// at +52 follows animwgt.cpp (its first three members; the type is unsigned
// here because the code compares it unsigned); the meanings are
// inferred. The rest of the file is in other units.
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
    unsigned char field02[2];
    unsigned short fileNum;
    unsigned char field06[2];
    void* file;
    unsigned char flags;
    unsigned char field0d;
    unsigned short state;
    unsigned char field10[36];
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
};

extern "C" {
unsigned char AnimSeqGrow(AnimSeqAnimInfo_t*, float);
void AnimWgtProcess(AnimWgtInfo_t*, float);
}
void _AnimUserCallbackPushState(unsigned long);
void _AnimUserCallbackPushInfo(void*);
void _AnimUserCallbackPopState();
void _AnimUserCallbackPopInfo();

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
