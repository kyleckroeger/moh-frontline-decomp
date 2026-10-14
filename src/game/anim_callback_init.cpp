// A fragment of anim_callback.cpp (0x8005ff80): CAnimCallbacks::turn_cb
// (on event 0 picks the turn animation through _TurnFindTurnAnimInfo - its
// 7th argument is read as a float - starts it, scales its sequence by the
// returned speed, keeps the per-frame yaw step, clears flag 0x10 of the
// object, remembers whether to face the target (the float is 0) and, when
// the owner has a move pending, starts moving it toward its target over the
// turn's duration; on event 2, while the state is current, either turns the
// owner toward its AI target (stored position raised by 1, or the target's
// position) like pathing_cb, or applies the yaw step and continues the move;
// on event 1 clears the step), then CAnimCallbacks::pathing_cb
// (on event 0 starts the first entry, flagging the state when the animated
// object's skinned animation has an owner, and keeps the channel in the
// state's memory; on event 2, while the state is current and a turn is
// pending with none under way, turns the owner toward its target: the
// 24-bit yaw difference in degrees, wrapped to +-180 and clamped to 6 degrees
// times the time argument, added in radians, and the channel's yaw reset),
// then the weak CAnimated::AsAnimSkinned it calls (emitted after it), then
// CAnimCallbacks::random_cb
// (on event 0, starts the list entry picked at random by the entries' weights
// against the first entry's total) and face_cb (on event 0, starts the first
// entry), each passing 1.0 as the last argument (its meaning is unknown) and
// returning 0; the empty cases (1 and 2, or 1 and 3 in pathing_cb) are
// inferred from the switches' comparison trees. CAnimated's virtuals are
// declared in their table order; the owner, skinned-object and channel views
// are inferred (member names by role or offset). Then AnimCallback_Init, which
// fills the file's state-selection table (nine state ids with their
// CAnimCallbacks selection callbacks, pathing_cb serving two states), passes
// the file's state definition to the animation system and registers the
// finish callback. The table and the definition are file-local in the
// original. The callbacks are named by the mangled symbols; the table entry
// layout (id, callback) and the definition view are inferred, and the
// callback result types are not known.
// Inferred views: a state's animation list (a count, then 12-byte entries of
// animation, flags and weight record).
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    void operator-=(const CVector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

// Inferred: the object the skinned animation drives (members by offset).
class CSoldierObject;

/* Inferred: scene-node view with the table slots used here (ISceneNode). */
class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual CSoldierObject* AsSoldierObject();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual struct AIDoodadView* GetAIDoodad();
};

class CSoldierObject : public ISceneNode {
};

struct CAIFilterRealPosition {
    float x;
    float y;
    float z;
};

class CAIObject {
public:
    unsigned char unknown000[192];
    bool m_targetStored;
    unsigned char unknown0c1[3];
    CAIObject* m_target;
    unsigned char unknown0c8[40];
    CAIFilterRealPosition m_unknown0f0;
    unsigned char unknown0fc[84];
    CAIFilterRealPosition m_unknown150;
};

struct CAIFilterObject {
    void* unknown00;
    unsigned char unknown04[4];
    CAIObject* m_object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterObject* filter;
};

inline bool HasTarget(CAIObject* ai) {
    return ai && (ai->m_targetStored || ai->m_target);
}

// Inferred: the object the skinned animation drives (members by offset).
struct PathingOwnerView : ISceneNode {
    unsigned char unknown0004[9068];
    CVector3 m_position;
    unsigned char unknown2380[32];
    CVector3 m_target;
    int m_yaw;
    unsigned char unknown23b4[4];
    CVector3 m_unknown23b8;
    unsigned char unknown23c8[16];
    CVector3 m_unknown23d8;
    float m_unknown23e8;
    unsigned char unknown23ec[4];
    float m_turn;
    unsigned char unknown2414[99];
    bool m_unknown2457;
    bool m_unknown2458;
    unsigned char unknown2459[88];
    bool m_turnPending;
};

// Inferred: 136-byte channel records from +0; the owner at +3256.
struct SkinnedChannelView {
    unsigned char unknown00[32];
    int m_yaw;
    unsigned char unknown24[68];
    unsigned char m_sequence[32];
};

class CAnimSkinned {
public:
    SkinnedChannelView m_channels[23];
    unsigned char unknown0c38[128];
    PathingOwnerView* m_owner;
};

class CharSkel_t;
class CDrawContext;
class CAnimMorph;

struct AnimStateMachineView {
    unsigned char unknown00[4];
};

class CAnimated {
public:
    virtual ~CAnimated();
    virtual void UpdateAnimation(float); /* result type not known */
    virtual void Draw(CDrawContext&);
    virtual void ExtractRotation(long*);
    virtual void ExtractTranslation(CVector3*);
    virtual void SetRotation(long);
    virtual void BindSkeleton(CharSkel_t*);
    virtual CAnimMorph* AsAnimMorph();
    virtual CAnimSkinned* AsAnimSkinned() { return 0; }

    unsigned char unknown004[2188];
    AnimStateMachineView m_machine;
    unsigned char unknown894[420];
    unsigned int m_flags;
};


struct AnimRandomInfoView {
    unsigned char unknown00[4];
    unsigned int weight;
    unsigned int totalWeight;
};

struct AnimListEntryView {
    int anim;
    unsigned short flags;
    AnimRandomInfoView* info;
};

struct AnimFileStateAnimList_t {
    unsigned short count;
    AnimListEntryView entries[1];
};

struct AnimObjHdr_t;
struct AnimChannel_t;
class CAnimObject;

class CAnimCallbacks {
public:
    static int pathing_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int random_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int turn_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int death_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int weapon_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int aim_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int face_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int floco_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int animfinish_cb(unsigned short, void*);
    static unsigned short _TurnFindTurnAnimInfo(AnimFileStateAnimList_t*, CAnimObject*, unsigned short, long*, float*, float*, float*, float*, float);
};

typedef int (*AnimStateSelFunc)(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);

// Inferred: a state id and its selection callback (8 bytes each, nine
// entries); the 8-byte state definition passed to the animation system.
struct AnimStateSelView {
    int state;
    AnimStateSelFunc callback;
};

struct AnimStateDefView {
    unsigned char unknown00[8];
};

extern AnimStateSelView _AnimObj_StateSelFuncArray[9];
extern AnimStateDefView _AnimObj_StateDef;

extern "C" void AnimStSetCallbackTable(AnimStateDefView*);
extern "C" void AnimStRegisterFinishCallback(int (*)(unsigned short, void*));

extern "C" int rand();
extern "C" int AnimChanStartAnim(AnimObjHdr_t*, AnimChannel_t*, int, unsigned short, unsigned short, void*, float);
extern "C" void* AnimStGetStateUserMemory(AnimStateMachineView*, unsigned short);
extern "C" void AnimStSetStateFlags(AnimStateMachineView*, unsigned short, int);
extern "C" unsigned short AnimStGetCurrentState(AnimStateMachineView*);
int MathArcTan2(float, float);
extern "C" void AnimSeqScaleDuration(void*, float);

// Inferred: the per-state memory keeps the started channel.
struct PathingStateView {
    unsigned char unknown00[4];
    int channel;
};

// Inferred: the animated object's state machine at +2192.

// Inferred: the turn state's memory (the started channel, the frame step,
// the remaining and total move time, whether to face the target, whether a
// move is under way and the move's destination).
struct TurnStateView {
    unsigned char unknown00[4];
    int channel;
    int step;
    float remaining;
    float total;
    bool face;
    bool moving;
    unsigned char unknown16[2];
    CAIFilterRealPosition destination;
};

int CAnimCallbacks::turn_cb(AnimFileStateAnimList_t* list, unsigned short state, AnimObjHdr_t* header,
                            AnimChannel_t* channel, void* user, unsigned long event, void* extra, float time) {
    CAnimSkinned* skinned = (CAnimSkinned*)user;
    CAnimated* animated = (CAnimated*)user;
    PathingOwnerView* owner = skinned->m_owner;
    float duration;
    float speed;
    long frames;
    float unknownA = 0.0f;
    float unknownB = 0.0f;
    TurnStateView* memory = (TurnStateView*)AnimStGetStateUserMemory(&animated->m_machine, state);
    switch (event) {
    case 0: {
        AnimStSetStateFlags(&animated->m_machine, state, 1);
        unsigned short index = _TurnFindTurnAnimInfo(list, (CAnimObject*)owner, state, &frames, &duration, &speed, &unknownA, &unknownB, *(float*)&extra);
        memory->channel = AnimChanStartAnim(header, channel, list->entries[index].anim, list->entries[index].flags, state, user, 1.0f);
        AnimSeqScaleDuration(skinned->m_channels[memory->channel].m_sequence, 1.0f / speed);
        memory->step = frames / duration;
        animated->m_flags &= ~0x10;
        if (*(float*)&extra == 0.0f)
            memory->face = 1;
        else
            memory->face = 0;
        if (owner->m_unknown2458) {
            memory->moving = 1;
            owner->m_unknown2458 = 0;
            memory->destination.x = owner->m_target.x;
            memory->destination.y = owner->m_target.y;
            memory->destination.z = owner->m_target.z;
            float dx = memory->destination.x;
            float dy = memory->destination.y;
            float dz = memory->destination.z;
            dx -= owner->m_unknown23b8.x;
            dy -= owner->m_unknown23b8.y;
            dz -= owner->m_unknown23b8.z;
            memory->total = duration;
            memory->remaining = duration;
            float step = time;
            if (time > memory->remaining)
                step = memory->remaining;
            float ratio = step / memory->remaining;
            memory->remaining -= step;
            float progress = memory->remaining / memory->total;
            owner->m_unknown23e8 = progress;
            float scale = (1.0f - progress) * ratio;
            owner->m_unknown23d8.x = scale * dx;
            owner->m_unknown23d8.y = scale * dy;
            owner->m_unknown23d8.z = scale * dz;
        } else {
            memory->moving = 0;
        }
        break;
    }
    case 2:
        if (state == AnimStGetCurrentState(&animated->m_machine)) {
            CVector3 target;
            CVector3 move;
            CVector3 position;
            CVector3 delta;
            if (memory->face) {
                CSoldierObject* soldier = owner->AsSoldierObject();
                if (!soldier)
                    return 0;
                CAIObject* ai = soldier->GetAIDoodad()->filter->m_object;
                if (HasTarget(ai)) {
                    if (!ai->m_target) {
                        float x = ai->m_unknown150.x;
                        float z = ai->m_unknown150.z;
                        float y = ai->m_unknown150.y;
                        target.x = x;
                        target.y = y;
                        target.z = z;
                        target.z += 1.0f;
                    } else {
                        float x = ai->m_unknown0f0.x;
                        float z = ai->m_unknown0f0.z;
                        float y = ai->m_unknown0f0.y;
                        target.x = x;
                        target.y = y;
                        target.z = z;
                    }
                    owner->m_target = target;
                    delta = owner->m_target;
                    position = owner->m_position;
                    delta -= position;
                    int yaw = MathArcTan2(-delta.x, delta.y);
                    float angle = 360.0f * (int)(((yaw & 0xffffff) - owner->m_yaw) & 0xffffff) / 16777216.0f;
                    if (angle > 180.0f)
                        angle -= 360.0f;
                    if (angle > 6.0f * time)
                        angle = 6.0f * time;
                    else if (angle < -6.0f * time)
                        angle = -6.0f * time;
                    owner->m_turn += 0.017453292f * angle;
                    skinned->m_channels[memory->channel].m_yaw = owner->m_yaw;
                    owner->m_unknown2457 = 1;
                }
            } else {
                owner->m_turn += 6.2831855f * memory->step / 16777216.0f * time;
                SkinnedChannelView* record = &skinned->m_channels[memory->channel];
                record->m_yaw = (record->m_yaw + (int)(memory->step * time)) & 0xffffff;
                if (memory->moving) {
                    move.x = memory->destination.x;
                    move.y = memory->destination.y;
                    move.z = memory->destination.z;
                    move.x = move.x - owner->m_unknown23b8.x;
                    move.y = move.y - owner->m_unknown23b8.y;
                    move.z = move.z - owner->m_unknown23b8.z;
                    if (memory->remaining > 0.0f) {
                        float step = time;
                        if (time > memory->remaining)
                            step = memory->remaining;
                        float ratio = step / memory->remaining;
                        memory->remaining -= step;
                        float progress = memory->remaining / memory->total;
                        owner->m_unknown23e8 = progress;
                        float scale = (1.0f - progress) * ratio;
                        owner->m_unknown23d8.x = scale * move.x;
                        owner->m_unknown23d8.y = scale * move.y;
                        owner->m_unknown23d8.z = scale * move.z;
                    } else {
                        owner->m_unknown23e8 = 0.0f;
                        owner->m_unknown23d8 = move;
                    }
                }
            }
            if (animated->m_flags & 0x10) {
                memory->step = 0;
                memory->remaining = 0.0f;
            }
        }
        break;
    case 1:
        if (state == AnimStGetCurrentState(&animated->m_machine))
            memory->step = 0;
        break;
    }
    return 0;
}

int CAnimCallbacks::pathing_cb(AnimFileStateAnimList_t* list, unsigned short state, AnimObjHdr_t* header,
                               AnimChannel_t* channel, void* user, unsigned long event, void*, float time) {
    CAnimated* animated = (CAnimated*)user;
    CAnimSkinned* skinned = animated->AsAnimSkinned();
    PathingOwnerView* owner = skinned ? skinned->m_owner : 0;
    PathingStateView* memory = (PathingStateView*)AnimStGetStateUserMemory(&animated->m_machine, state);
    switch (event) {
    case 0:
        if (owner)
            AnimStSetStateFlags(&animated->m_machine, state, 1);
        memory->channel = AnimChanStartAnim(header, channel, list->entries[0].anim, list->entries[0].flags, state, user, 1.0f);
        break;
    case 1:
        break;
    case 3:
        break;
    case 2:
        if (owner && state == AnimStGetCurrentState(&animated->m_machine)) {
            if (owner->m_turnPending && !owner->m_unknown2457 && !owner->m_turn) {
                CVector3 position;
                CVector3 delta;
                delta = owner->m_target;
                position = owner->m_position;
                delta -= position;
                int yaw = MathArcTan2(-delta.x, delta.y);
                float angle = 360.0f * (int)(((yaw & 0xffffff) - owner->m_yaw) & 0xffffff) / 16777216.0f;
                if (angle > 180.0f)
                    angle -= 360.0f;
                if (angle > 6.0f * time)
                    angle = 6.0f * time;
                else if (angle < -6.0f * time)
                    angle = -6.0f * time;
                owner->m_turn += 0.017453292f * angle;
                skinned->m_channels[memory->channel].m_yaw = owner->m_yaw;
            }
            owner->m_turnPending = 0;
        }
        break;
    }
    return 0;
}

int CAnimCallbacks::random_cb(AnimFileStateAnimList_t* list, unsigned short state, AnimObjHdr_t* header,
                              AnimChannel_t* channel, void* user, unsigned long event, void*, float) {
    switch (event) {
    case 0: {
        AnimRandomInfoView* info = list->entries[0].info;
        unsigned int pick = rand() % info->totalWeight;
        int i = 0;
        unsigned int sum = 0;
        while (i < list->count - 1 && info->weight + sum <= pick) {
            sum += info->weight;
            info = list->entries[++i].info;
        }
        AnimChanStartAnim(header, channel, list->entries[i].anim, list->entries[i].flags, state, user, 1.0f);
        break;
    }
    case 1:
    case 2:
        break;
    }
    return 0;
}

int CAnimCallbacks::face_cb(AnimFileStateAnimList_t* list, unsigned short state, AnimObjHdr_t* header,
                            AnimChannel_t* channel, void* user, unsigned long event, void*, float) {
    switch (event) {
    case 0:
        AnimChanStartAnim(header, channel, list->entries[0].anim, list->entries[0].flags, state, user, 1.0f);
        break;
    case 1:
    case 2:
        break;
    }
    return 0;
}

void AnimCallback_Init() {
    _AnimObj_StateSelFuncArray[0].state = 1000;
    _AnimObj_StateSelFuncArray[0].callback = CAnimCallbacks::pathing_cb;
    _AnimObj_StateSelFuncArray[1].state = 1100;
    _AnimObj_StateSelFuncArray[1].callback = CAnimCallbacks::random_cb;
    _AnimObj_StateSelFuncArray[2].state = 1001;
    _AnimObj_StateSelFuncArray[2].callback = CAnimCallbacks::turn_cb;
    _AnimObj_StateSelFuncArray[3].state = 1200;
    _AnimObj_StateSelFuncArray[3].callback = CAnimCallbacks::death_cb;
    _AnimObj_StateSelFuncArray[4].state = 1300;
    _AnimObj_StateSelFuncArray[4].callback = CAnimCallbacks::weapon_cb;
    _AnimObj_StateSelFuncArray[5].state = 1002;
    _AnimObj_StateSelFuncArray[5].callback = CAnimCallbacks::aim_cb;
    _AnimObj_StateSelFuncArray[6].state = 1010;
    _AnimObj_StateSelFuncArray[6].callback = CAnimCallbacks::face_cb;
    _AnimObj_StateSelFuncArray[7].state = 1020;
    _AnimObj_StateSelFuncArray[7].callback = CAnimCallbacks::pathing_cb;
    _AnimObj_StateSelFuncArray[8].state = 1400;
    _AnimObj_StateSelFuncArray[8].callback = CAnimCallbacks::floco_cb;
    AnimStSetCallbackTable(&_AnimObj_StateDef);
    AnimStRegisterFinishCallback(CAnimCallbacks::animfinish_cb);
}
