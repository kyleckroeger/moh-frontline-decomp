// A fragment of bsbifunc.cpp (0x8001f9f8): the script built-ins that eject the
// calling soldier's helmet (a direction with only the given z set) and that
// give the pan angle from the calling node's position and rightward axis to a
// given point (for noise type 1, else 0; the result replaces the popped
// arguments on the stack), then those that copy a sound information structure
// (32 bytes, given by address) into the calling node's soldier, return the
// address of that structure, and set the soldier's head tracking of the player
// (a boolean and an integer argument converted through float). Each reads its
// arguments below the script stack top and pops the built-in's arguments. The
// file name is this project's; the original record is bsbifunc.cpp. The
// functions and classes are named by the mangled symbols; ISceneNode is
// declared with its virtual functions in the order of __vt__10ISceneNode
// (GetPosition at +64, GetRightward at +68, AsSoldierObject at +148); the
// vector view (four floats with an inline constructor), the sound information
// view (its first two words a pair, copied together), the soldier view (the
// structure at +12608), the result union, the boolean argument helper and the
// built-in record view are inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
#include "game/CVector3.h"

class CBullet;
class CLight;
class CPlayerObject;
class CSoldierObject;
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
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual CSoldierObject* AsSoldierObject();
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


struct SoundInfoPairView {
    int unknown00;
    int unknown04;
};

struct SoundInfoView {
    SoundInfoPairView unknown00;
    int unknown08;
    int unknown0c;
    float unknown10;
    int unknown14;
    int unknown18;
    int unknown1c;
};

class CSoldierObject {
public:
    void EjectHelmet(CVector3*);
    void SetHeadTrackPlayer(bool, int);

    unsigned char unknown0000[12608];
    SoundInfoView m_soundInfo;
};

union BSValueView {
    int i;
    float f;
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline int BSArgBool(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index)) != 0;
}

float MathFunGetPanAngleDiffNoRoll(CVector3*, CVector3*, CVector3*);

void BIFunc_EjectHelmet(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    CVector3 direction;
    direction.x = direction.y = direction.z = 0.0f;
    direction.z = *(float*)(*stack - (count - 1));
    ((ISceneNode*)object)->AsSoldierObject()->EjectHelmet(&direction);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_GetNoiseInfo(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int type = *(*stack - (count - 1));
    float angle = 0.0f;
    CVector3* point = *(CVector3**)(*stack - (count - 2));
    switch (type) {
    case 1: {
        ISceneNode* node = (ISceneNode*)object;
        CVector3 rightward;
        CVector3 position;
        node->GetPosition(position);
        node->GetRightward(rightward);
        CVector3 target(point->x, point->y, point->z);
        angle = MathFunGetPanAngleDiffNoRoll(&position, &target, &rightward);
        break;
    }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&angle;
}

void BIFunc_SetMySoundInfoStruct(int** stack, void* object) {
    SoundInfoView* info = *(SoundInfoView**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ((ISceneNode*)object)->AsSoldierObject()->m_soundInfo = *info;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_GetMySoundInfoStruct(int** stack, void* object) {
    int info;
    BSValueView value;
    CSoldierObject* soldier = ((ISceneNode*)object)->AsSoldierObject();
    info = (int)&soldier->m_soundInfo;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    value.i = info;
    *(BSValueView*)*stack = value;
}

void BIFunc_SetHeadTrackPlayer(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    bool track = BSArgBool(stack, 1);
    float time = *(*stack - (count - 2));
    CSoldierObject* soldier = ((ISceneNode*)object)->AsSoldierObject();
    soldier->SetHeadTrackPlayer(track, time);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
