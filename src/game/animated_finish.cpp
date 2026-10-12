// A fragment of animated.cpp (0x80061314): CAnimated::UpdateAnimation (steps
// the state machine and the channels by the time, then rebuilds the motion
// weight list kept at +2612), CAnimated::FinishAnimationState
// finishes the animation recorded for a state (read from the state machine's
// user memory, unless it is -1) on the animation object, passing the state's
// top bit as the flag, and returns 0. SetAnimationState refuses a lip-synch
// state (callback ID 3) while +3264 is set and the object's +9392 flag is
// clear (0xFFFF0000), reports a pending transition (0xF0F0F0F0), otherwise
// starts the state and records the animation in the state's user memory.
// ValidState warns when the database has no callback for the state. Init
// resets the channels and state machine and keeps the animation object;
// InitClass forwards to CAnimSkinned::InitClass. CAnimated, CAnimObject,
// CAnimDatabase, g_AnimDB and the functions are named by the symbols; the
// members, parameter meanings and result types are inferred (CAnimated is a
// non-virtual view; the fields Init clears between +2616 and +3248 may belong
// to the state machine's structure). The rest of the file is not part of
// this unit.
extern "C" {
void* AnimStGetStateUserMemory(void*, unsigned short);
int AnimStTransPreCheck(void*, void*, unsigned short);
long AnimStStartState(void*, void*, void*, unsigned short, void*, void*, float);
void AnimChanInitChannels(void*, void*);
void AnimStInitStructure(void*);
void AnimStProcess(void*, void*, void*, void*);
void AnimChanProcess(float, void*, void*, void*);
void* AnimMotCreateWeightList(void*, void*, void*);
}

void DebugMsg(const char*, ...);

class CAnimDatabase {
public:
    int GetStateCallbackID(unsigned long, unsigned short);

    unsigned char unknown0000[176];
};

extern CAnimDatabase g_AnimDB;

class CAnimSkinned {
public:
    static void InitClass();
};

class CAnimObject {
public:
    void FinishAnimationState(long, bool);

    unsigned char unknown0000[9392];
    bool m_flag24b0;
};

class CAnimated {
public:
    void UpdateAnimation(float);
    int FinishAnimationState(unsigned short);
    long SetAnimationState(unsigned short, float, unsigned short&, long, void*);
    bool ValidState(unsigned short);
    void Init(CAnimObject*, unsigned short);
    static void InitClass();

    unsigned char unknown0000[4];
    unsigned short unknown0004;      /* start of the channel data at +4 */
    unsigned short m_animSet;        /* passed to GetStateCallbackID */
    unsigned char unknown0008[4];
    long unknown000c;
    unsigned char m_channels[2176];  /* +16 */
    unsigned char m_stateMachine[420];
    void* m_weightList;
    long unknown0a38;
    unsigned char unknown0a3c;
    unsigned char unknown0a3d[339];
    long unknown0b90;
    unsigned char unknown0b94[284];
    long unknown0cb0;
    unsigned char unknown0cb4[4];
    CAnimObject* m_animObject;
    unsigned char unknown0cbc[4];
    bool unknown0cc0;
    bool unknown0cc1;
};

void CAnimated::UpdateAnimation(float time) {
    AnimStProcess(m_stateMachine, &unknown0004, m_channels, this);
    AnimChanProcess(time, &unknown0004, m_channels, this);
    m_weightList = AnimMotCreateWeightList(&unknown0004, m_channels, m_stateMachine);
}

int CAnimated::FinishAnimationState(unsigned short state) {
    long* animation = (long*)AnimStGetStateUserMemory(m_stateMachine, state);

    if (*animation != -1)
        m_animObject->FinishAnimationState(*animation, (state & 0x8000) != 0);
    return 0;
}

long CAnimated::SetAnimationState(unsigned short state, float blend, unsigned short& transition, long animation, void* data) {
    if (unknown0cc0 && !m_animObject->m_flag24b0 && g_AnimDB.GetStateCallbackID(state, m_animSet) == 3)
        return 0xFFFF0000;
    int pending = AnimStTransPreCheck(m_stateMachine, &unknown0004, state);
    if (pending >= 0) {
        transition = pending;
        return 0xF0F0F0F0;
    }
    unknown0cc1 = false;
    long result = AnimStStartState(m_stateMachine, &unknown0004, m_channels, state, this, data, blend);
    *(long*)AnimStGetStateUserMemory(m_stateMachine, state) = animation;
    return result;
}

bool CAnimated::ValidState(unsigned short state) {
    if (g_AnimDB.GetStateCallbackID(state, m_animSet) == -1) {
        DebugMsg("WARNING: No callback fn found for lip synch 0x%X (Search in anim_ids.h for %d to find missing anim\n)", state, state);
        return false;
    }
    return true;
}

void CAnimated::Init(CAnimObject* object, unsigned short) {
    unknown0cb0 = 0;
    unknown0b90 = 0;
    AnimChanInitChannels(&unknown0004, m_channels);
    AnimStInitStructure(m_stateMachine);
    m_animObject = object;
    unknown000c = 0;
    unknown0a3c = 128;
    unknown0a38 = 0;
    unknown0cc1 = false;
}

void CAnimated::InitClass() {
    CAnimSkinned::InitClass();
}
