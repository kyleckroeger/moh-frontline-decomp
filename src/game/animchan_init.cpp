// The end of animchan.cpp (0x80061bf8): AnimChanInitChannels resets each of
// the owner's channels: the first byte is 1 when the owner's first halfword
// is zero and 4 otherwise, the status is cleared, the sequence has no frame
// list and the speed is 1. The record views follow animchan.cpp (the owner's
// first halfword is inferred; what the first byte means is not known). The
// rest of the file is in other units or not reconstructed.
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

struct AnimChannelOwnerView {
    unsigned short kind;
    unsigned char field02[2];
    unsigned short count;
};

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
