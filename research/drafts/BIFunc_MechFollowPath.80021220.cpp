// best WIP (harvest1, 2026-11-12): 83 real lines (was 105; index said 105), from
// scratch/loop3/scripting/mfp3/BEST83.cpp: argument locals declared at their reads,
// yaw/pitch/target/value named before the call. Left: GPR rotation in block 1; 1008 argument
// spellings/orders inert (scratch/loop3/scripting/s10).
// A fragment of bsbifunc.cpp (0x80021220): the built-in MechFollowPath sets a
// mechanism (the object's static object) moving along a path: the path, two
// flags (non-zero tests), a speed (above 1000: reversed, less 1000), an
// acceleration (the argument plus one, squared, in thousandths), two angular
// rates in degrees, an object and a value are taken from the script arguments
// and passed to CStaticObject::SetMovementPath, the speed and rates converted
// to per-frame units (1/59.94). It pops the built-in's arguments. The file
// name is this project's; the original record is bsbifunc.cpp. The functions,
// classes and globals are named by the mangled symbols; ISceneNode is
// declared with its virtual functions in the order of __vt__10ISceneNode
// (AsStaticObject at +116); the argument meanings are inferred.
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


struct BSObject;

class CStaticObject {
public:
    void SetMovementPath(int, bool, bool, float, float, float, float, BSObject*, float, bool);
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

void BIFunc_MechFollowPath(int** stack, void* object) {
    CStaticObject* mech = ((ISceneNode*)object)->AsStaticObject();
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int path = *(*stack - (count - 1));
    bool flag2 = (bool)*(*stack - (count - 3));
    bool flag1 = (bool)*(*stack - (count - 2));
    int speed = *(*stack - (count - 4));
    bool reverse;
    if (speed > 1000) {
        reverse = true;
        speed -= 1000;
    } else {
        reverse = false;
    }
    int accel = *(*stack - (count - 5)) + 1;
    float yaw = *(float*)(*stack - (count - 6));
    float pitch = *(float*)(*stack - (count - 7));
    BSObject* target = (BSObject*)*(*stack - (count - 8));
    int value = *(*stack - (count - 9));
    mech->SetMovementPath(path, flag1, flag2, 0.016683351f * speed, 0.001f * (accel * accel),
                          0.017453292f * (0.016683351f * yaw), 0.017453292f * (0.016683351f * pitch), target, value, reverse);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
