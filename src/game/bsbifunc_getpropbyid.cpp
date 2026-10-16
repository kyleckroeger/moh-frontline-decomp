// A fragment of bsbifunc.cpp (0x80025174): the script built-in GetPropById,
// which finds the nearest prop with an id. It reads the id and a script
// object (the current one when null) below the script stack top, takes that
// object's position (its scene node's, or its trigger's when it has no
// user), and searches the box list and then, when no box was found, the tank
// list for the closest scene node (closer than 1000) whose script trigger has
// the id and that is not the object itself; the built-in's arguments are
// popped and the found script object (or null) is stored at the new top. The
// distance is recomputed for the new best, as in the original. The file name
// is this project's; the original record is bsbifunc.cpp, and the rest of
// the file is in other units. The functions, classes and globals are named
// by the mangled symbols; ISceneNode is declared with its virtual functions
// in the order of __vt__10ISceneNode (GetPosition at +64, GetScriptObject at
// +96); the vector view (four floats, 8-byte aligned, member-wise
// assignment, an inline length through sqrtf), the trigger (the id at +0x38),
// script-object, game-object and built-in record views are inferred (members
// at their offsets, names not original). The result is stored through its
// address, which keeps it on the stack as in the original.
#include <math.h>

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3& operator=(const CVector3& o) {
        x = o.x;
        y = o.y;
        z = o.z;
        return *this;
    }
    CVector3& operator-=(const CVector3& o) {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        return *this;
    }
    float Length() const { return sqrtf(x * x + y * y + z * z); }

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
class CAIDoodad;
struct BSObjectView;

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
    virtual BSObjectView* GetScriptObject() const;
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
    virtual CPlayerObject* AsSoldierObject();
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
    virtual CAIDoodad* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

struct TriggerCoreView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    unsigned char unknown1c[28];
    short id;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

class BSGO_Basic {
    unsigned char unknown00[4];

public:
    BSGO_Basic* next;
    unsigned char unknown08[4];
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
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
extern BSObjectView* g_pBSObject;
extern BSGO_Basic* g_pBoxList;
extern BSGO_Basic* g_pTankList;

void BIFunc_GetPropById(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int id = *(*stack - (count - 1));
    BSObjectView* self = *(BSObjectView**)(*stack - (count - 2));
    if (!self)
        self = g_pBSObject;
    CVector3 position;
    CVector3 delta;
    if (self->user) {
        self->user->GetSceneNode()->GetPosition(position);
    } else {
        TriggerCoreView* core = self->trigger->core;
        position = CVector3(core->x, core->y, core->z);
    }
    BSObjectView* closest = 0;
    BSGO_Basic* user = g_pBoxList;
    float best = 1000.0f;
    for (; user; user = user->next) {
        if (user->GetSceneNode() && user->GetSceneNode()->GetScriptObject() &&
            id == user->GetSceneNode()->GetScriptObject()->trigger->core->id) {
            ISceneNode* node = user->GetSceneNode();
            BSObjectView* script = user->GetSceneNode()->GetScriptObject();
            if (script != self) {
                node->GetPosition(delta);
                delta -= position;
                if (delta.Length() < best) {
                    closest = script;
                    best = delta.Length();
                }
            }
        }
    }
    if (!closest) {
        for (user = g_pTankList; user; user = user->next) {
            if (user->GetSceneNode() && user->GetSceneNode()->GetScriptObject() &&
                id == user->GetSceneNode()->GetScriptObject()->trigger->core->id) {
                ISceneNode* node = user->GetSceneNode();
                BSObjectView* script = user->GetSceneNode()->GetScriptObject();
                if (script != self) {
                        node->GetPosition(delta);
                    delta -= position;
                    if (delta.Length() < best) {
                        closest = script;
                        best = delta.Length();
                    }
                }
            }
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&closest;
}
