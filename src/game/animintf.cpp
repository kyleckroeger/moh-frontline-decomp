// The animation system's start-up and shutdown, which register the user-opcode
// handlers with the animation module. CAnimCallbacks is named by the mangled
// symbols. _AnimIntfOpcodeSkip follows them, then _AnimIntfOpcodeProcess:
// for the current animation state (or any state) it handles the user opcodes
// of an animated object (finishing the state, enemy movement, reload and
// triggered sounds, script events, controller feedback for the player,
// head tracking for soldiers and the remaining parameterised opcodes), and
// reports an unprocessed parameter opcode or an invalid one. The scene node,
// animated object, player, soldier and weapon views are inferred; the
// head-tracking cases go through an inferred inline helper that marks the
// opcode handled before the call.
extern "C" {
void AnimStShutdown(void);
int AnimWgtShutdown(void);
void AnimShutdown(void);
void AnimInit(void);
int AnimWgtInit(void);
void AnimStInit(void);
int AnimSeqGetOpcodeParameter(void);
int sprintf(char*, const char*, ...);
}
void AnimCallback_Init();

class CAnimCallbacks {
public:
    static int _AnimIntfOpcodeSkip(void*, unsigned short, unsigned long);
    static int _AnimIntfOpcodeProcess(void*, unsigned short, unsigned long);
};

extern "C" void AnimSetUserOpcodeCallbacks(int (*)(void*, unsigned short, unsigned long),
                                           int (*)(void*, unsigned short, unsigned long));

extern "C" void AnimSystem_Shutdown(void) {
    AnimStShutdown();
    AnimWgtShutdown();
    AnimShutdown();
}

extern "C" void AnimSystem_Init(void) {
    AnimInit();
    AnimSetUserOpcodeCallbacks(CAnimCallbacks::_AnimIntfOpcodeProcess, CAnimCallbacks::_AnimIntfOpcodeSkip);
    AnimWgtInit();
    AnimStInit();
    AnimCallback_Init();
}

// The skip handler consumes the parameters of the user opcodes it knows
// (the opcode values and their parameter counts are read from the code; their
// meanings are unknown) and formats a message for any other opcode; the
// message buffer is not used further.
int CAnimCallbacks::_AnimIntfOpcodeSkip(void*, unsigned short opcode, unsigned long) {
    int result = 0;

    switch (opcode) {
    case 0xC900:
        result = 1;
        break;
    case 0xC80D:
    case 0xC80E:
    case 0xC80F:
    case 0xC811:
    case 0xC812:
    case 0xCA00:
    case 0xCA0A:
    case 0xCA13:
        AnimSeqGetOpcodeParameter();
        result = 1;
        break;
    case 0xC80B:
        AnimSeqGetOpcodeParameter();
        AnimSeqGetOpcodeParameter();
        result = 1;
        break;
    case 0xC803:
    case 0xC804:
    case 0xC805:
    case 0xC808:
    case 0xC809:
    case 0xC80C:
        result = 1;
        break;
    case 0xC806:
        result = 0;
        break;
    default:
        if ((opcode & 0xE000) == 0xE000) {
            char message[80];

            sprintf(message, "Opcode Parameter Not Processed: %x\n", opcode);
        } else {
            char message[80];

            sprintf(message, "Invalid User Animation Opcode: %x\n", opcode);
        }
        break;
    }
    return result;
}

class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
struct AIDoodadView;
enum EClsnId {};

class ISceneNode {
public:
    enum EVolumeType {};
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class CAnimObject : public ISceneNode {};

class CPlayerObject {
public:
    unsigned char unknown000[1892];
    unsigned long m_controller;
};

class CSoldierObject {
public:
    void SetHeadTrackPlayer(bool, int);
};

class CPlayerWeaponObject {
public:
    unsigned char unknown0000[12512];
    CPlayerObject* m_player;
};

class CAnimatedPlayer {
public:
    unsigned char unknown0000[17584];
    CPlayerObject* m_player;
};

class CInputManager {
public:
    void SetFeedback(unsigned long, long, long, float, float);

    unsigned char unknown0000[9296];
};

class CAnimated {
public:
    void FinishAnimationState(unsigned short);

    unsigned char unknown000[2192];
    unsigned char m_states[424];
    unsigned int m_flags;
    unsigned char unknownA3C[636];
    CAnimObject* m_object;
    unsigned char unknownCBC[4];
    unsigned char m_flagCC0;
    unsigned char m_flagCC1;
};

extern bool g_bInMultiplayerMode;
extern CInputManager g_inputMgr;

void PlayEnemyMovement(unsigned short, long, CAnimObject*);
void PlayReloadSound(long, CAnimObject*);
void PlayAnimTriggeredSound(long, CAnimObject*);
extern "C" unsigned short AnimStGetCurrentState(void*);

/* inferred: marks the opcode handled before passing the parameter on */
static inline void SetHeadTrack(int& result, CSoldierObject* soldier, bool on, int parameter) {
    result = 1;
    soldier->SetHeadTrackPlayer(on, parameter);
}

int CAnimCallbacks::_AnimIntfOpcodeProcess(void* data, unsigned short opcode, unsigned long state) {
    CAnimObject* object;
    int result = 0;
    unsigned long controller = 0;
    CAnimated* animated = (CAnimated*)data;
    object = animated->m_object;
    CSoldierObject* soldier = (CSoldierObject*)object->AsSoldierObject();
    CAnimatedPlayer* animatedPlayer = (CAnimatedPlayer*)object->AsAnimatedPlayerObject();
    CPlayerWeaponObject* playerWeapon = (CPlayerWeaponObject*)object->AsPlayerWeaponObject();

    if (state == AnimStGetCurrentState(animated->m_states) || state == 0xFFFF) {
        switch (opcode) {
        case 0xC900:
            animated->m_flags |= 0x10;
            animated->FinishAnimationState(state);
            result = 1;
            break;
        case 0xCA00:
        case 0xCA0A:
        case 0xCA13:
        case 0xCA16:
        case 0xCA18:
        case 0xCA19:
        case 0xCA1E:
        case 0xCA22:
        case 0xCA24:
        case 0xCA29:
        case 0xCA2F:
        case 0xCA34:
        case 0xCA38:
        case 0xCA39:
        case 0xCA3B:
        case 0xCA3D:
        case 0xCA43:
        case 0xCA53:
        case 0xCA5A:
        case 0xCA5D:
            {
                long parameter = AnimSeqGetOpcodeParameter();

                result = 1;
                PlayEnemyMovement(opcode, parameter, object);
            }
            break;
        case 0xC804:
            object->TriggerScriptEvent(125, 0, false);
            result = 1;
            break;
        case 0xC803:
            object->TriggerScriptEvent(126, (void*)AnimSeqGetOpcodeParameter(), false);
            result = 1;
            break;
        case 0xC805:
            object->TriggerScriptEvent(127, 0, false);
            result = 1;
            break;
        case 0xC806:
            PlayReloadSound(AnimSeqGetOpcodeParameter(), object);
            result = 1;
            break;
        case 0xC807:
            object->TriggerScriptEvent(129, 0, false);
            result = 1;
            break;
        case 0xC808:
            object->TriggerScriptEvent(226, 0, false);
            result = 1;
            break;
        case 0xC809:
            object->TriggerScriptEvent(227, 0, false);
            result = 1;
            break;
        case 0xC80A:
            object->TriggerScriptEvent(167, 0, false);
            result = 1;
            break;
        case 0xC810:
            object->TriggerScriptEvent(252, 0, false);
            result = 1;
            break;
        case 0xC811:
            object->TriggerScriptEvent(255, (void*)(AnimSeqGetOpcodeParameter() - 1), false);
            result = 1;
            break;
        case 0xC812: {
            int value = AnimSeqGetOpcodeParameter();
            animated->m_flagCC1 = 1;
            animated->m_flagCC0 = value - 1 != 0;
            break;
        }
        case 0xC80B: {
            int motor = AnimSeqGetOpcodeParameter();
            int time = AnimSeqGetOpcodeParameter();
            if (playerWeapon)
                controller = playerWeapon->m_player->m_controller;
            else if (animatedPlayer)
                controller = animatedPlayer->m_player->m_controller;
            g_inputMgr.SetFeedback(controller, motor - 1, 0, 1.0f, (float)time / 10.0f);
            result = 1;
            break;
        }
        case 0xC80C:
            if (!g_bInMultiplayerMode || playerWeapon)
                object->TriggerScriptEvent(179, 0, false);
            result = 1;
            break;
        case 0xC80D:
            SetHeadTrack(result, soldier, true, AnimSeqGetOpcodeParameter());
            break;
        case 0xC80E:
            SetHeadTrack(result, soldier, false, AnimSeqGetOpcodeParameter());
            break;
        case 0xC80F:
            {
                int p = AnimSeqGetOpcodeParameter();
                result = 1;
                PlayAnimTriggeredSound(p, object);
            }
            break;
        default:
            if ((opcode & 0xE000) == 0xE000) {
                char message[80];

                sprintf(message, "Opcode Parameter Not Processed: %x\n", opcode);
            } else {
                char message[80];

                sprintf(message, "Invalid User Animation Opcode: %x\n", opcode);
            }
            break;
        }
    } else {
        result = 1;
    }
    return result;
}
