// A fragment of anim_callback.cpp (0x80060554): CAnimCallbacks::pathing_cb
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
struct PathingOwnerView {
    unsigned char unknown0000[9072];
    CVector3 m_position;
    unsigned char unknown2380[32];
    CVector3 m_target;
    int m_yaw;
    unsigned char unknown23d4[60];
    float m_turn;
    unsigned char unknown2414[99];
    bool m_unknown2457;
    unsigned char unknown2458[89];
    bool m_turnPending;
};

// Inferred: 136-byte channel records from +0; the owner at +3256.
struct SkinnedChannelView {
    unsigned char unknown00[32];
    int m_yaw;
    unsigned char unknown24[100];
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

// Inferred: the per-state memory keeps the started channel.
struct PathingStateView {
    unsigned char unknown00[4];
    int channel;
};

// Inferred: the animated object's state machine at +2192.

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
