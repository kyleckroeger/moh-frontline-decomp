// A fragment of bsbifunc.cpp (0x800287ac): script built-ins that rotate the
// script object's static object (or a hierarchy sub-object, by the same rule;
// the index argument is held as a float, and the angles and speeds are
// converted from degrees with the file's pooled constants) and that move the
// script object's static object (or, for a sub-object index other than -1, that
// sub-object of its hierarchy object) relative to its position by script, with
// a delta, a speed and a completion event, stop and start a motion playback (on
// the static object for a negative sub-object index, otherwise on that
// sub-object), and play an animation on a hierarchy sub-object (after fetching
// the static object, unused) or on the static object. Each reads its arguments
// below the script stack top and pops the built-in's arguments. The file name
// is this project's; the original record is bsbifunc.cpp and the built-ins
// around these are not reconstructed. The functions, classes and globals are
// named by the mangled symbols; ISceneNode is declared with its virtual
// functions in the order of __vt__10ISceneNode (AsStaticObject at +116,
// AsHierObject at +124), IMovingSceneNode's and CStaticObject's after them in
// the order of __vt__13CStaticObject (TranslateFromScript at +304,
// StartMotionPlayback at +316; the unnamed entry at +300 as a placeholder) and
// BSGO_Basic in the order of __vt__10BSGO_Basic; the vector view (four floats,
// 8-byte aligned, built by an inline constructor), script-object and built-in
// record views are inferred.
class CVector3 {
public:
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
class CHierObject;
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
    virtual CHierObject* AsHierObject();
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
    void StopMotionPlayback();
    void PlayAnimation(int, int, int, EBSEventEnum);
};

class CHierObject {
public:
    CStaticObject* GetSubObject(int);
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

extern BSObjectView* g_pBSObject;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline float BSArgFloat(int** stack, int index) {
    return *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}
inline int BSArgInt(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}
void BIFunc_RotateRel(int** stack, void*) {
    float x = BSArgFloat(stack, 2);
    float y = BSArgFloat(stack, 3);
    float z = BSArgFloat(stack, 4);
    float sx = BSArgFloat(stack, 5);
    float sy = BSArgFloat(stack, 6);
    float sz = BSArgFloat(stack, 7);
    float sub = BSArgInt(stack, 1);
    EBSEventEnum event = (EBSEventEnum)BSArgInt(stack, 8);
    CVector3 angles(3.1415927f * x / 180.0f, 3.1415927f * y / 180.0f, 3.1415927f * z / 180.0f);
    CVector3 speed(3.1415927f * sx / 180.0f, 3.1415927f * sy / 180.0f, 3.1415927f * sz / 180.0f);
    if (sub != -1.0f)
        g_pBSObject->user->GetSceneNode()->AsHierObject()->GetSubObject(sub)->RotateFromScript(angles, speed, event);
    else
        g_pBSObject->user->GetSceneNode()->AsStaticObject()->RotateFromScript(angles, speed, event);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_TranslateRel(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int sub = *(*stack - (count - 1));
    float x = *(float*)(*stack - (count - 2));
    float y = *(float*)(*stack - (count - 3));
    float z = *(float*)(*stack - (count - 4));
    float sx = *(float*)(*stack - (count - 5));
    float sy = *(float*)(*stack - (count - 6));
    float sz = *(float*)(*stack - (count - 7));
    EBSEventEnum event = (EBSEventEnum)*(*stack - (count - 8));
    CVector3 delta(x, y, z);
    CVector3 speed(sx, sy, sz);
    if (sub != -1)
        g_pBSObject->user->GetSceneNode()->AsHierObject()->GetSubObject(sub)->TranslateFromScript(delta, speed, event);
    else
        g_pBSObject->user->GetSceneNode()->AsStaticObject()->TranslateFromScript(delta, speed, event);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StopMotionPlayback(int** stack, void*) {
    int sub = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (sub < 0)
        g_pBSObject->user->GetSceneNode()->AsStaticObject()->StopMotionPlayback();
    else
        g_pBSObject->user->GetSceneNode()->AsHierObject()->GetSubObject(sub)->StopMotionPlayback();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StartMotionPlayback(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int sub = *(*stack - (count - 1));
    int motion = *(*stack - (count - 2));
    int mode = *(*stack - (count - 3));
    EBSEventEnum event = (EBSEventEnum)*(*stack - (count - 4));
    if (sub < 0)
        g_pBSObject->user->GetSceneNode()->AsStaticObject()->StartMotionPlayback(motion, mode, event);
    else
        g_pBSObject->user->GetSceneNode()->AsHierObject()->GetSubObject(sub)->StartMotionPlayback(motion, mode, event);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_HierObjectPlayAnimation(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int sub = *(*stack - (count - 1));
    int animation = *(*stack - (count - 2));
    int mode = *(*stack - (count - 3));
    int blend = *(*stack - (count - 4));
    EBSEventEnum event = (EBSEventEnum)*(*stack - (count - 5));
    CStaticObject* object = g_pBSObject->user->GetSceneNode()->AsStaticObject();
    g_pBSObject->user->GetSceneNode()->AsHierObject()->GetSubObject(sub)->PlayAnimation(animation, mode, blend, event);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StaticObjectPlayAnimation(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int animation = *(*stack - (count - 1));
    int mode = *(*stack - (count - 2));
    int blend = *(*stack - (count - 3));
    EBSEventEnum event = (EBSEventEnum)*(*stack - (count - 4));
    CStaticObject* object = g_pBSObject->user->GetSceneNode()->AsStaticObject();
    object->PlayAnimation(animation, mode, blend, event);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
