// A fragment of anim_callback.cpp (0x8005f2c4): CAnimCallbacks::floco_cb, then
// CAnimCallbacks::weapon_cb.
// floco_cb drives the player's locomotion blend. It reads the state's info
// from g_AnimDB (the state and the header's object type) and the state's user
// memory. On event 0 it stores the CAnimatedPlayer (last pointer argument),
// takes its floco speed and type (GetFlocoParams), clears them
// (SetFlocoParams(0, 0)), remembers whether the player is crouching and scales
// the speed by the time argument. A jumping, standing player starts the jump
// entry (_floco_anim_ranges[8], plus the type + 1 above speed 0.1) and clears
// a player byte (+4788). Otherwise the entry range comes from
// _floco_anim_ranges (type and type + 1, or type + 4 and type + 5 when
// crouching), the state is flagged, and SelectFlocoStep scans the range's step
// durations (times the time argument, into a local table) for the first one
// above the speed, clamps the speed into the found pair and records the pair
// (durations, indices, range and type) in the memory; the two entries are
// started with weights 1 - w and w, w being the speed's position between the
// two scaled durations (0 and 1 when they do not increase), and their speeds
// set to 1 + (1 - w) and 0.5 * w + 0.5. On event 2, when the type and the
// crouching state are unchanged the pair steps down or up while the speed
// leaves it (switching both channels by duration, clamping at the range's
// ends); otherwise the range is selected again and both channels switched.
// The weights (AnimWgtSet) and speeds are then set as on event 0. Both return
// 0; the empty case 3 is inferred from the switch's comparison tree.
// weapon_cb, on event 0, starts the state's list entry whose record matches
// the callback's last pointer argument (compared as a number; the first entry
// when none does), passing 1.0 as the last argument of AnimChanStartAnim (its
// meaning is unknown), and returns 0. The empty cases 1 and 2 are inferred
// from the switch's comparison tree.
// The class, callbacks and the player, database and animation functions are
// named by the mangled symbols; the list, entry, record, header, channel, state
// memory, step table and player views are inferred (as in
// anim_callback_init.cpp). The step table's durations are read once through
// the table as floats (StepDuration) and otherwise through the step records,
// as the original's two addressing forms require; GetPlayer is an inferred
// inline accessor, the database view is sized only so that g_AnimDB is not
// small data, and _floco_anim_ranges is declared const (its loads are not
// ordered after stores). The constants are .sdata2 pool entries.
struct AnimRecordView {
    unsigned char unknown00[4];
    unsigned int value;
};

struct AnimListEntryView {
    int anim;
    unsigned short flags;
    AnimRecordView* record;
};

struct AnimFileStateAnimList_t {
    unsigned short count;
    AnimListEntryView entries[1];
};

struct AnimObjHdr_t {
    unsigned short type;
};

struct AnimChannel_t {
    unsigned char unknown00[52];
    unsigned char m_weight[84];
};

class CAnimCallbacks {
public:
    static int floco_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int weapon_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
};

extern "C" int AnimChanStartAnim(AnimObjHdr_t*, AnimChannel_t*, int, unsigned short, unsigned short, void*, float);
extern "C" void AnimChanSwitchAnimByDuration(AnimObjHdr_t*, AnimChannel_t*, int, int, int, unsigned short, void*);
extern "C" void* AnimStGetStateUserMemory(struct AnimStateMachineView*, unsigned short);
extern "C" void AnimStSetStateFlags(struct AnimStateMachineView*, unsigned short, int);
extern "C" void AnimWgtSet(void*, int, int, float, float);

struct AnimStateMachineView {
    unsigned char unknown00[4];
};

class CAnimated {
public:
    void SetAnimSpeed(unsigned char, float);

    unsigned char unknown000[2192];
    AnimStateMachineView m_machine;
};

class CPlayerObject {
public:
    bool IsPlayerCrouching() const;
    bool IsPlayerJumping() const;

    unsigned char unknown0000[4788];
    unsigned char m_unknown12b4;
};

class CAnimatedPlayer {
public:
    void GetFlocoParams(float&, long&);
    void SetFlocoParams(float, long);
    CPlayerObject* GetPlayer() { return m_player; }

    unsigned char unknown0000[17584];
    CPlayerObject* m_player;
};

struct FlocoStepView {
    unsigned char unknown00[8];
    float duration;
};

struct FlocoStateInfoView {
    float StepDuration(int index) { return ((float*)this)[index * 3 + 6]; }

    unsigned char unknown00[16];
    FlocoStepView steps[1];
};

class CAnimDatabase {
public:
    FlocoStateInfoView* GetStateInfo(unsigned long, unsigned short);

    unsigned char unknown00[64];
};

extern CAnimDatabase g_AnimDB;
extern const signed char _floco_anim_ranges[];

struct FlocoStateView {
    unsigned char unknown00[4];
    CAnimatedPlayer* player;
    long type;
    int first;
    int last;
    float low;
    float high;
    float speed0;
    float speed1;
    unsigned char channel0;
    unsigned char channel1;
    unsigned char index0;
    unsigned char index1;
    unsigned char unknown28[2];
    bool crouching;
};

static inline void SelectFlocoStep(FlocoStateView* memory, FlocoStateInfoView* info, int first, int last, long& type,
                                  float time, float& speed, float* durations, int& k, int& i) {
    int count = last - first;
    float* d = &durations[1];
    durations[0] = info->steps[first].duration * time;
    for (i = 1; i < count; d++, i++) {
        *d = info->steps[first + i].duration * time;
        if (speed < *d)
            break;
    }
    if (speed < durations[i - 1])
        speed = durations[i - 1];
    if (i >= count) {
        i = count - 1;
        speed = durations[i];
    }
    int j = first + i;
    memory->low = info->steps[j - 1].duration;
    memory->high = info->steps[j].duration;
    memory->index0 = j - 1;
    memory->index1 = j;
    memory->first = first;
    memory->last = last;
    memory->type = type;
    k = j;
}

int CAnimCallbacks::floco_cb(AnimFileStateAnimList_t* list, unsigned short state, AnimObjHdr_t* header,
                             AnimChannel_t* channel, void* user, unsigned long event, void* extra, float time) {
    FlocoStateInfoView* info = g_AnimDB.GetStateInfo(state, header->type);
    FlocoStateView* memory = (FlocoStateView*)AnimStGetStateUserMemory(&((CAnimated*)user)->m_machine, state);
    float durations[6];
    long type;
    float speed;
    switch (event) {
    case 0: {
        memory->player = (CAnimatedPlayer*)extra;
        memory->player->GetFlocoParams(speed, type);
        memory->player->SetFlocoParams(0.0f, 0);
        CPlayerObject* player = memory->player->GetPlayer();
        memory->crouching = player->IsPlayerCrouching();
        speed *= time;
        if (player->IsPlayerJumping() && !memory->crouching) {
            int jump;
            int offset;
            player->m_unknown12b4 = offset = 0;
            jump = _floco_anim_ranges[8];
            if (speed > 0.1f)
                offset = type + 1;
            int index = jump + offset;
            AnimChanStartAnim(header, channel, list->entries[index].anim, list->entries[index].flags, state, user, 1.0f);
            break;
        }
        int first;
        int last;
        CAnimated* animated = (CAnimated*)user;
        if (memory->crouching) {
            first = _floco_anim_ranges[type + 4];
            last = _floco_anim_ranges[type + 5];
        } else {
            first = _floco_anim_ranges[type];
            last = _floco_anim_ranges[type + 1];
        }
        AnimStSetStateFlags(&animated->m_machine, state, 1);
        int k;
        int i;
        SelectFlocoStep(memory, info, first, last, type, time, speed, durations, k, i);
        float low = durations[i - 1];
        float high = durations[i];
        float w0;
        float w1;
        if (high <= low) {
            w0 = 0.0f;
            w1 = 1.0f;
        } else {
            w1 = (speed - low) / (high - low);
            w0 = 1.0f - w1;
        }
        memory->channel0 = AnimChanStartAnim(header, channel, list->entries[k - 1].anim, list->entries[k - 1].flags, state, user, w0);
        memory->channel1 = AnimChanStartAnim(header, channel, list->entries[k].anim, list->entries[k].flags, state, user, w1);
        memory->speed0 = 1.0f + w0;
        memory->speed1 = 0.5f * w1 + 0.5f;
        animated->SetAnimSpeed(memory->channel0, memory->speed0);
        animated->SetAnimSpeed(memory->channel1, memory->speed1);
        break;
    }
    case 2: {
        float w0;
        float w1;
        CAnimated* animated = (CAnimated*)user;
        memory->player->GetFlocoParams(speed, type);
        memory->player->SetFlocoParams(0.0f, 0);
        speed *= time;
        float low = memory->low * time;
        float high = memory->high * time;
        CPlayerObject* player = memory->player->GetPlayer();
        if (memory->type != type || memory->crouching != player->IsPlayerCrouching()) {
            memory->crouching = player->IsPlayerCrouching();
            int first;
            int last;
            if (memory->crouching) {
                first = _floco_anim_ranges[type + 4];
                last = _floco_anim_ranges[type + 5];
            } else {
                first = _floco_anim_ranges[type];
                last = _floco_anim_ranges[type + 1];
            }
            int k;
            int i;
            SelectFlocoStep(memory, info, first, last, type, time, speed, durations, k, i);
            low = memory->low * time;
            high = memory->high * time;
            AnimChanSwitchAnimByDuration(header, channel, memory->channel0, list->entries[memory->index0].anim, list->entries[memory->index0].flags, state, user);
            AnimChanSwitchAnimByDuration(header, channel, memory->channel1, list->entries[memory->index1].anim, list->entries[memory->index1].flags, state, user);
                } else {
            if (speed < low) {
                if (memory->index0 == memory->first) {
                    speed = low;
                } else {
                    memory->index0--;
                    memory->index1--;
                    memory->low = info->StepDuration(memory->index0);
                    memory->high = info->StepDuration(memory->index1);
                    low = memory->low * time;
                    high = memory->high * time;
                    if (speed < low)
                        speed = low;
                    AnimChanSwitchAnimByDuration(header, channel, memory->channel0, list->entries[memory->index0].anim, list->entries[memory->index0].flags, state, user);
                    AnimChanSwitchAnimByDuration(header, channel, memory->channel1, list->entries[memory->index1].anim, list->entries[memory->index1].flags, state, user);
                }
            } else if (speed > high) {
                if (memory->index1 == memory->last - 1) {
                    speed = high;
                } else {
                    memory->index0++;
                    memory->index1++;
                    memory->low = info->StepDuration(memory->index0);
                    memory->high = info->StepDuration(memory->index1);
                    high = memory->high * time;
                    low = memory->low * time;
                    if (speed > high)
                        speed = high;
                    AnimChanSwitchAnimByDuration(header, channel, memory->channel0, list->entries[memory->index0].anim, list->entries[memory->index0].flags, state, user);
                    AnimChanSwitchAnimByDuration(header, channel, memory->channel1, list->entries[memory->index1].anim, list->entries[memory->index1].flags, state, user);
                }
            }
        }
        if (high <= low) {
            w0 = 0.0f;
            w1 = 1.0f;
        } else {
            w1 = (speed - low) / (high - low);
            w0 = 1.0f - w1;
        }
        AnimWgtSet(&channel[memory->channel0].m_weight, 0, 0, w0, w0);
        AnimWgtSet(&channel[memory->channel1].m_weight, 0, 0, w1, w1);
        memory->speed0 = 1.0f + w0;
        memory->speed1 = 0.5f * w1 + 0.5f;
        animated->SetAnimSpeed(memory->channel0, memory->speed0);
        animated->SetAnimSpeed(memory->channel1, memory->speed1);
        break;
    }
    case 3:
        break;
    }
    return 0;
}

int CAnimCallbacks::weapon_cb(AnimFileStateAnimList_t* list, unsigned short state, AnimObjHdr_t* header,
                              AnimChannel_t* channel, void* user, unsigned long event, void* weapon, float) {
    switch (event) {
    case 0: {
        unsigned int i;
        for (i = 0; i < list->count; i++) {
            if ((unsigned int)weapon == list->entries[i].record->value)
                break;
        }
        if (i >= list->count)
            i = 0;
        AnimChanStartAnim(header, channel, list->entries[i].anim, list->entries[i].flags, state, user, 1.0f);
        break;
    }
    case 1:
    case 2:
        break;
    }
    return 0;
}
