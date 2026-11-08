// A fragment of animtran.cpp (0x800681b0, the start of the file):
// _AnimTranQuickScan checks whether an object's animation set has a
// transition for its current state (the database fills the transition list
// array): a direct match of from/to (a 0xFFFF from matches any entry whose
// from has no 0x8000 bit, otherwise the 0x8000 wildcard), then the wildcard
// from, each with the given state (or with any state, 0xFFFF) returns -1;
// otherwise every state 14250..14299 reachable from a wildcard entry is
// collected and the first one whose own list has a wildcard transition to the
// target is returned (-1 when none). CAnimDatabase, the globals, the list
// array type and the function are named by the symbols; the entry/list
// layouts, the search helper (inlined four times; its frame argument is 0
// here) and the state range are inferred. The rest of the file is in other
// units.
struct AnimObjHdr_t {
    unsigned short kind;
    unsigned short set;
};

struct AnimTranEntry_t {
    unsigned short from;
    unsigned short to;
    unsigned short state;
    unsigned short low;
    unsigned short high;
    unsigned short pad;
    void* transition;
};

struct AnimTranList_t {
    int count;
    AnimTranEntry_t entries[1];
};

struct AnimTranListRef_t {
    int unknown00;
    int unknown04;
    AnimTranList_t* list;
};

struct AnimTranTransListArray_t {
    unsigned short count;
    unsigned short pad;
    int unknown04;
    AnimTranListRef_t lists[8];
};

struct AnimFileFormat_t;

class CAnimDatabase {
public:
    void GetTransList(unsigned long, unsigned long, AnimTranTransListArray_t*);
    AnimFileFormat_t* GetFileFromFileNum(unsigned long, unsigned short);

    unsigned char unknown00[176];
};

extern CAnimDatabase g_AnimDB;
extern AnimTranTransListArray_t AnimTran_TransListArray;

static inline void* FindTransition(AnimTranTransListArray_t* array, unsigned short from, unsigned short file,
                                   unsigned short to, unsigned short state, unsigned short frame) {
    int i;
    void* found;
    unsigned short key;
    found = 0;
    if (from != 0xffff) {
        key = file | 0x8000;
    } else {
        from = to;
        key = 0xffff;
    }
    for (i = 0; i < array->count; i++) {
        AnimTranList_t* list = array->lists[i].list;
        for (int j = 0; j < list->count; j++) {
            AnimTranEntry_t* entry = &list->entries[j];
            if (entry->from == key || (key == 0xffff && !(entry->from & 0x8000))) {
                if (entry->to == from && entry->state == state) {
                    if (state != 0xffff) {
                        if (frame >= entry->low && frame <= entry->high)
                            found = entry->transition;
                    } else {
                        found = entry->transition;
                    }
                }
            }
        }
    }
    return found;
}

struct AnimWgtInfo_t {
    float weight;
    float target;
    unsigned int type;
};

struct AnimStSlot_t {
    unsigned char unknown00[4];
    unsigned short state;
    unsigned short trigger;
    unsigned char unknown08[72];
    AnimWgtInfo_t weight;
    unsigned char unknown5c[12];
};

struct AnimStInfo_t {
    AnimStSlot_t slots[6];
};

struct AnimSeqFrameList_t;

struct AnimChannel_t {
    unsigned char unknown00[2];
    short anim;
    unsigned short fileNum;
    unsigned char unknown06[46];
    float weight;
    unsigned char unknown38[32];
    AnimSeqFrameList_t* list;
    float rate;
    unsigned char unknown60[16];
    unsigned short position;
    unsigned char unknown72[22];
};

struct AnimTransition_t {
    unsigned char unknown00[4];
    unsigned short type;
    unsigned short frames;
    float fraction;
    unsigned short fileNum;
    unsigned short anim;
};

struct AnimFileFormat_t;

extern "C" {
int AnimChanGetNumRunningOrDoneByState(AnimObjHdr_t*, AnimChannel_t*, unsigned short);
unsigned short AnimChanGetStateChannelByIndex(AnimObjHdr_t*, AnimChannel_t*, unsigned short, unsigned short);
unsigned int AnimChanStartAnim(AnimObjHdr_t*, AnimChannel_t*, void*, int, unsigned short, void*, float);
float AnimSeqGetLife(AnimSeqFrameList_t*);
void AnimWgtSet(AnimWgtInfo_t*, float, float, int, void*);
void AnimWgtSetLinear(AnimWgtInfo_t*, float, float, unsigned int);
void AnimWgtSetLinearInAndOut(AnimWgtInfo_t*, float, float, unsigned char, unsigned char);
void AnimStSetStateDelay(AnimStInfo_t*, unsigned short, float);
void AnimChanSetDelayByState(AnimObjHdr_t*, AnimChannel_t*, unsigned short, float);
}

static inline int StrongestChannel(AnimObjHdr_t* object, AnimChannel_t* channels, unsigned short state) {
    float weight = 0.0f;
    int best = 0xffff;
    unsigned short count = AnimChanGetNumRunningOrDoneByState(object, channels, state);
    for (unsigned short i = 0; i < count; i++) {
        unsigned short channel = AnimChanGetStateChannelByIndex(object, channels, state, i);
        if (channel != 0xffff && weight < channels[channel].weight) {
            weight = channels[channel].weight;
            best = channel;
        }
    }
    return best;
}

static inline bool StartTransition(AnimStInfo_t* states, AnimObjHdr_t* object, AnimChannel_t* channels, unsigned short to,
                                   void* info, AnimTransition_t* transition, AnimStSlot_t* slot, int k) {
    void* file = g_AnimDB.GetFileFromFileNum(object->set, transition->fileNum);
    if (file) {
        unsigned int channel = AnimChanStartAnim(object, channels, file, transition->anim, 0xffff, info, 1.0f);
        if (channel != 0xffffffff) {
            float life = AnimSeqGetLife(channels[channel].list);
            unsigned int frames = (unsigned int)(life * (1.0f - transition->fraction) * 0.5f);
            unsigned int hold = (unsigned int)life - frames * 2;
            AnimWgtSetLinear(&slot->weight, -1.0f, 0.0f, frames);
            AnimWgtSetLinearInAndOut(&states->slots[k + 1].weight, -1.0f, 1.0f, frames, hold);
            AnimWgtSetLinear(&states->slots[k + 2].weight, 0.0f, states->slots[k + 2].weight.target, frames);
            float delay = (float)hold + (float)frames;
            AnimStSetStateDelay(states, to, delay);
            AnimChanSetDelayByState(object, channels, to, delay);
            return false;
        } else {
            return true;
        }
    } else {
        return true;
    }
}

void _AnimTranDoTransition(AnimStInfo_t* states, AnimObjHdr_t* object, AnimChannel_t* channels, unsigned short to,
                           unsigned short current, unsigned long trigger, void* info) {
    AnimTranTransListArray_t* array = &AnimTran_TransListArray;
    bool failed = true;
    for (int k = 0; k < 4; k++) {
        AnimStSlot_t* slot = &states->slots[k];
        if (slot->trigger == trigger && slot->state != 0xffff) {
            slot->trigger = 1;
            g_AnimDB.GetTransList(object->set, current, &AnimTran_TransListArray);
            if (array->count != 0) {
                unsigned short fromChannel = StrongestChannel(object, channels, slot->state);
                unsigned short toChannel = StrongestChannel(object, channels, to);
                if (fromChannel != 0xffff && toChannel != 0xffff) {
                    AnimChannel_t* next = &channels[toChannel];
                    AnimChannel_t* prev = &channels[fromChannel];
                    unsigned short prevAnim = prev->anim;
                    AnimTransition_t* transition =
                        (AnimTransition_t*)FindTransition(array, next->anim, next->fileNum, to, prevAnim, prev->position);
                    if (!transition)
                        transition = (AnimTransition_t*)FindTransition(array, 0xffff, 0, to, prevAnim, prev->position);
                    if (!transition)
                        transition = (AnimTransition_t*)FindTransition(array, next->anim, next->fileNum, to, 0xffff, 0);
                    if (!transition)
                        transition = (AnimTransition_t*)FindTransition(array, 0xffff, 0, to, 0xffff, 0);
                    if (transition) {
                        switch (transition->type) {
                        case 0:
                            AnimWgtSet(&slot->weight, 0.0f, 0.0f, 0, 0);
                            failed = false;
                            break;
                        case 1: {
                            unsigned int frames = (unsigned int)(transition->frames * prev->rate);
                            AnimWgtSetLinear(&slot->weight, -1.0f, 0.0f, frames);
                            AnimWgtSetLinear(&states->slots[k + 2].weight, 0.0f, states->slots[k + 2].weight.target,
                                             frames);
                            failed = false;
                            break;
                        }
                        case 2:
                            AnimWgtSet(&slot->weight, 0.0f, 0.0f, 0, 0);
                            failed = false;
                            break;
                        case 3:
                            failed = StartTransition(states, object, channels, to, info, transition, slot, k);
                            break;
                        }
                    }
                }
            }
            if (failed == true)
                AnimWgtSetLinear(&slot->weight, -1.0f, 0.0f, 10);
        }
    }
}
