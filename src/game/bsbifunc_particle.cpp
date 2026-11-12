// A fragment of bsbifunc.cpp (0x80029198): script built-ins that create a light
// by type (attached to the scene node at the given offset when asked, otherwise
// placed at the script trigger's position and rotation moved by the offset) and
// return it, deactivate a particle system and set a particle system's initial
// velocity (from three floats), followed by the weak, empty
// CParticleSystem::SetSystemInitialVelocity, then create a particle system by
// type (placed at the scene node's transform, moved to the centre of its world
// bounding volume for static objects that are neither thrown nor hierarchy
// tanks, or at the script trigger's position and rotation; then moved by the
// offset) and return it, followed by the weak, empty
// CParticleSystem::SetLocalToWorld and the weak CStaticObject::AsThrownObject
// (0), then reset a particle system's rotation (its matrix from the virtual
// function at +84, kept position only). Each reads its arguments below the
// script stack top (the system first) and pops the built-in's arguments; a
// null system is skipped. CVector3 is a view with its components and a double
// pair overlaid in a named union, which gives the position's doubleword copy.
// The file name is this project's; the original record is bsbifunc.cpp and the
// built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (SetAttachedLight at +212) and IMovingSceneNode's after them in the order of
// __vt__13CStaticObject (SetTMLocalToWorld at +232), CStaticObject derives from
// IMovingSceneNode with its virtual functions in the order of
// __vt__13CStaticObject up to AsHierTankObject at +328 (the unnamed entry at
// +300 as a placeholder), CLight derives from IMovingSceneNode, the world
// bounding volume view (its centre at +64) is inferred, the trigger and
// script-object views are inferred, CMatrix's constructor is an inline view
// (initialising the class once); CRenderBin and CParticleSystem are declared
// with their virtual functions in the order of __vt__15CParticleSystem (the
// pointer at +28 after 28 bytes of members; the destructors declared and
// defined elsewhere), the 18 null entries at +60 to +128 as pure virtual
// functions whose names are unknown; return types not visible in the code are
// left as int or void, and the built-in record view is inferred.
//
// CVector3 is the shared game/CVector3.h (the copy to the by-value argument is
// two lfd/stfd pairs).


#include "game/CVector3.h"


#include "game/CMatrix.h"


class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

enum EClsnId {};
class CCollision;
class CDrawContext;
class CVector3;
class CBullet;
class CStaticObject;
struct VolumeView;
class CLight;
class CPlayerObject;
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
    virtual const VolumeView* GetWorldBoundingVolume(EVolumeType) const;
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


class IMovingSceneNode : public ISceneNode {
public:
    virtual ~IMovingSceneNode();
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
};

struct VolumeView {
    unsigned char unknown00[64];
    CVector3 center;
};

class BSObject;
class BPDLightVolume;
enum EBSEventEnum {};

class CStaticObject : public IMovingSceneNode {
public:
    virtual ~CStaticObject();
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();
    virtual int GetWorldLinearVelocity() const;
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual int GetLightVolume();
    virtual void Init();
    virtual void SetScript(BSObject*);
    virtual void unknown12c();
    virtual void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void ScaleFromScript(float, float, EBSEventEnum);
    virtual void StartMotionPlayback(int, int, EBSEventEnum);
    virtual void* AsThrownObject();
    virtual void* AsWeaponObject();
    virtual void* AsHierTankObject();
};

class CLight : public IMovingSceneNode {
public:
    virtual ~CLight();
    static CLight* Create(unsigned long);
};

struct TriggerCoreView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    CQuaternion rotation;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
};

extern BSObjectView* g_pBSObject;

class CDmaPacket;
class CDmaTag;
class CMatrix;
class CColor;
class ShapeFile;
class CDrawContext;

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual void Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual int IsUsed();
};

class CParticleSystem : public CRenderBin {
public:
    enum ERenderType {};

    virtual ~CParticleSystem();
    virtual void DeActivate();
    virtual void Terminate();
    virtual int IsActive() const;
    virtual int IsEmitting() const;
    virtual int IsBoundingBoxValid() const;
    virtual int IsMoving() const;
    virtual int HasRotation() const;
    virtual int GetTexture() const;
    virtual void unknown3c() = 0;
    virtual void unknown40() = 0;
    virtual void unknown44() = 0;
    virtual void unknown48() = 0;
    virtual void unknown4c() = 0;
    virtual void unknown50() = 0;
    virtual void unknown54(CMatrix&) const = 0;
    virtual void unknown58() = 0;
    virtual void unknown5c() = 0;
    virtual void unknown60() = 0;
    virtual void unknown64() = 0;
    virtual void unknown68() = 0;
    virtual void unknown6c() = 0;
    virtual void unknown70() = 0;
    virtual void unknown74() = 0;
    virtual void unknown78() = 0;
    virtual void unknown7c() = 0;
    virtual void unknown80() = 0;
    virtual void SetRenderType(ERenderType);
    virtual void SetTexture(ShapeFile*);
    virtual void SetLocalToWorld(const CMatrix&);
    virtual void SetEmmisionRate(float);
    virtual void SetEmmisionDelay(float);
    virtual void SetSystemLifetime(float);
    virtual void SetParticleLifetime(float);
    virtual void SetSystemInitialVelocity(CVector3);
    static CParticleSystem* Create(unsigned long);
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

void BIFunc_CreateLightByType(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float x = *(float*)(*stack - (count - 2));
    float y = *(float*)(*stack - (count - 3));
    float z = *(float*)(*stack - (count - 4));
    bool attach = *(*stack - (count - 5)) != 0;
    unsigned long type = *(*stack - (count - 1));
    CVector3 offset(x, y, z);
    CLight* light = CLight::Create(type);
    if (object && attach) {
        ((ISceneNode*)object)->SetAttachedLight(light, offset);
    } else {
        TriggerCoreView* core = g_pBSObject->trigger->core;
        CVector3 position(core->x, core->y, core->z);
        const CQuaternion& rotation = core->rotation;
        CMatrix matrix;
        rotation.GetMatrix(matrix);
        matrix.SetPos(position);
        matrix.PreTranslate(offset);
        light->SetTMLocalToWorld(matrix);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&light;
}

void BIFunc_DestroyParticleSystem(int** stack, void*) {
    CParticleSystem* system = *(CParticleSystem**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (system)
        system->DeActivate();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetParticleSystemVelocity(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    CParticleSystem* system = *(CParticleSystem**)(*stack - (count - 1));
    float x = *(float*)(*stack - (count - 2));
    float y = *(float*)(*stack - (count - 3));
    float z = *(float*)(*stack - (count - 4));
    if (system) {
        CVector3 velocity(x, y, z);
        system->SetSystemInitialVelocity(velocity);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) void CParticleSystem::SetSystemInitialVelocity(CVector3) {
}

void BIFunc_CreateParticleSystemByType(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float x = *(float*)(*stack - (count - 2));
    float y = *(float*)(*stack - (count - 3));
    float z = *(float*)(*stack - (count - 4));
    unsigned long type = *(*stack - (count - 1));
    CVector3 offset(x, y, z);
    CMatrix matrix;
    if (object) {
        ISceneNode* node = (ISceneNode*)object;
        node->GetTMLocalToWorld(matrix);
        if (node->AsStaticObject() && !node->AsStaticObject()->AsThrownObject() && !node->AsStaticObject()->AsHierTankObject()) {
            const VolumeView* volume = node->GetWorldBoundingVolume((ISceneNode::EVolumeType)3);
            if (volume)
                matrix.SetPos(volume->center);
        }
    } else {
        TriggerCoreView* core = g_pBSObject->trigger->core;
        CVector3 position(core->x, core->y, core->z);
        core->rotation.GetMatrix(matrix);
        matrix.SetPos(position);
    }
    matrix.PreTranslate(offset);
    CParticleSystem* system = CParticleSystem::Create(type);
    if (system)
        system->SetLocalToWorld(matrix);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&system;
}

__declspec(weak) void CParticleSystem::SetLocalToWorld(const CMatrix&) {
}

__declspec(weak) void* CStaticObject::AsThrownObject() {
    return 0;
}

void BIFunc_ResetParticleSystemRotation(int** stack, void*) {
    CParticleSystem* system = *(CParticleSystem**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (system) {
        CMatrix matrix;
        system->unknown54(matrix);
        CVector3 position = *(const CVector3*)&matrix.pos.x;
        matrix.Ident();
        matrix.SetPos(position);
        system->SetLocalToWorld(matrix);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
