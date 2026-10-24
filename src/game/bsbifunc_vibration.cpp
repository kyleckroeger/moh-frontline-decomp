// A fragment of bsbifunc.cpp (0x80020d88): script built-ins that fade a soldier
// out, block new controller vibrations (for the script object's player's port
// in multiplayer, otherwise port 0), play a vibration event on the script
// object's player's controller in multiplayer (otherwise the first player's),
// set or clear a bit of a mechanism's flags, force an update of the script
// object's proximity trigger status, and set a mechanism's path flag (mode 1)
// or its path speed (otherwise, 0.01 per unit). Each reads its arguments below the script
// stack top and pops the built-in's arguments. The file name is this project's;
// the original record is bsbifunc.cpp and the built-ins around these are not
// reconstructed. The functions, classes and globals are named by the mangled
// symbols; ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (AsStaticObject at +116, AsPlayerObject at +156) and
// BSGO_Basic in the order of __vt__10BSGO_Basic; the static-object, player,
// input-manager, script-object, scene and built-in record views are inferred
// (members at their offsets, names not original).
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
struct AIDoodadView;

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


class CStaticObject {
public:
    unsigned char unknown000[232];
    float m_pathSpeed;
    unsigned char unknown0ec[245];
    unsigned char m_pathFlag : 1;
    unsigned char unknown1e1 : 7;
    unsigned char unknown1e2[2];
    unsigned int m_mechFlags;
};

class CPlayerObject {
public:
    unsigned char unknown000[1892];
    unsigned long m_controller;
    unsigned char unknown768[4];
    int m_port;
};

class CSoldierObject {
public:
    void FadeOut(float);
};

class CInputManager {
public:
    void SetFeedback(unsigned long, long, long, float, float);

    unsigned char unknown0000[9286];
    bool m_blockVibrations[4];
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;
extern CInputManager g_inputMgr;
extern BSObjectView* g_pBSObject;
extern bool g_bInMultiplayerMode;

void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_FadeOut(int** stack, void* object) {
    ((CSoldierObject*)object)->FadeOut(*(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_BlockNewVibrations(int** stack, void*) {
    int port = 0;
    bool block = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    if (g_bInMultiplayerMode)
        port = g_pBSObject->user->GetSceneNode()->AsPlayerObject()->m_port;
    g_inputMgr.m_blockVibrations[port] = block;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayVibrationEvent(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    long motor = *(*stack - (count - 1));
    float strength = *(float*)(*stack - (count - 2));
    float time = *(float*)(*stack - (count - 3));
    long mode = *(*stack - (count - 4));
    unsigned long controller;
    if (g_bInMultiplayerMode)
        controller = g_pBSObject->user->GetSceneNode()->AsPlayerObject()->m_controller;
    else
        controller = g_scene.GetPlayer(0)->m_controller;
    g_inputMgr.SetFeedback(controller, motor, mode, time, strength);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_MechSetFlag(int** stack, void* object) {
    CStaticObject* mech = ((ISceneNode*)object)->AsStaticObject();
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int set = *(*stack - (count - 2));
    int bit = *(*stack - (count - 1));
    if (set)
        mech->m_mechFlags |= 1 << bit;
    else
        mech->m_mechFlags &= ~(1 << bit);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_MovingMechForceUpdateTrigger(int** stack, void*) {
    ForceUpdateProximityTriggerStatus(g_pBSObject->user, false);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_MechSetPathFlag(int** stack, void* object) {
    CStaticObject* mech = ((ISceneNode*)object)->AsStaticObject();
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int mode = *(*stack - (count - 1));
    int value = *(*stack - (count - 2));
    /* The mode is tested through a copy (the original's register choice). */
    int selector;
    selector = mode;
    if (selector != 1)
        mech->m_pathSpeed = 0.01f * value;
    else
        mech->m_pathFlag = value != 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
