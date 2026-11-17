// A fragment of bsbifunc.cpp (0x80032778): AI built-ins that engage obstacle
// avoidance, perform a collision test in a direction (returning the float
// result through its address), fire the mounted machine gun of the given
// script object's trigger (the box- or tank-list node, through the scene
// node's AI doodad), followed by the weak, empty
// CAIDoodad::FireMMGAtCurrentTarget and the weak CSoldierObject::GetAIDoodad
// (the doodad at +12512), and fire at the current target, followed by the
// weak, empty CAIDoodad::FireAtCurrentTarget, and set a run-away move point
// (the filter finds a point away from the given node; it is stored in the
// script data at +172 and set as an arbitrary-point update of type 3 with
// 0.8, an entry of the file's .sdata2 pool; the declarations at the top
// reproduce the original register use). Each reads its arguments below the
// script stack top (through inferred inline argument helpers) and pops the
// built-in's arguments. The file name is this project's; the original record
// is bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols;
// ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (GetScriptObject at +96, AsStaticObject at +116,
// AsHierObject at +124, GetAIDoodad at +204), CAIDoodad in the order of
// __vt__9CAIDoodad (the unnamed entry at +12 as a placeholder), BSGO_Basic
// in the order of __vt__10BSGO_Basic (the list link at +4); CSoldierObject
// derives from ISceneNode (its destructor declared and defined elsewhere);
// the filter, AI object, script-object and built-in record views, the node
// search and the argument helpers are inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CAIDoodad;
class CStaticObject;
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
    virtual CAIDoodad* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};


enum ECollisionTestDirection {};

union BSValueView {
    int i;
    float f;
};

enum EMovePointType {};
struct BS_STRUCT_Vector_struct;

class CAIObject {
public:
    void SetupArbitraryPointUpdate(EMovePointType, BS_STRUCT_Vector_struct*, float);
    void EngageObstacleAvoidance(bool, float, float, float);
    float PerformCollisionTest(ECollisionTestDirection, float);
};

#include "game/CAIFilterRealPosition.h"


class CAIFilterObject {
public:
    bool FindRunAwayPoint(const ISceneNode*, float, CAIFilterRealPosition*);

    unsigned char unknown00[8];
    CAIObject* object;
};

class CAIDoodad {
public:
    virtual ~CAIDoodad();
    virtual void unknown0c();
    virtual void FireAtCurrentTarget(float);
    virtual void FireMMGAtCurrentTarget(CStaticObject*);

    CAIFilterObject* filter;
};

class CSoldierObject : public ISceneNode {
public:
    virtual ~CSoldierObject();
    virtual CAIDoodad* GetAIDoodad();

    unsigned char unknown0004[12508];
    CAIDoodad m_doodad;
};

struct TriggerObject_struct;

class BSGO_Basic;

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

// Inferred: the script data's move point at +172.
struct ScriptMovePointView {
    unsigned char unknown00[172];
    float x;
    float y;
    float z;
};

class BSGO_Basic {
    unsigned char unknown00[4];

public:
    BSGO_Basic* next;
    unsigned char unknown08[4];
    virtual void Destroy();
    virtual ScriptMovePointView* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

extern BSGO_Basic* g_pBoxList;
extern BSGO_Basic* g_pTankList;

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

inline ISceneNode* FindMMGNode(TriggerObject_struct* trigger) {
    BSGO_Basic* user;
    ISceneNode* node;
    BSObjectView* script;
    for (user = g_pBoxList; user; user = user->next) {
        node = user->GetSceneNode();
        if (node && node->AsStaticObject() && (script = node->GetScriptObject()) && script->trigger == trigger)
            return node;
    }
    for (user = g_pTankList; user; user = user->next) {
        node = user->GetSceneNode();
        if (node && node->AsHierObject() && (script = node->GetScriptObject()) && script->trigger == trigger)
            return node;
    }
    return 0;
}

inline float BSArgFloat(int** stack, int index) {
    return *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

inline CAIObject* GetAIObject(void* object) {
    return ((ISceneNode*)object)->GetAIDoodad()->filter->object;
}

inline int BSArgInt(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

void BIFunc_AIEngageObstacleAvoidance(int** stack, void* object) {
    bool engage = BSArgBool(stack, 1);
    float a = BSArgFloat(stack, 2);
    float b = BSArgFloat(stack, 3);
    float c = BSArgFloat(stack, 4);
    GetAIObject(object)->EngageObstacleAvoidance(engage, a, b, c);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AICollisionTest(int** stack, void* object) {
    float distance;
    ECollisionTestDirection direction = (ECollisionTestDirection)BSArgInt(stack, 1);
    distance = BSArgFloat(stack, 2);
    distance = GetAIObject(object)->PerformCollisionTest(direction, distance);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&distance;
}

void BIFunc_AIFireMMGAtCurrentTarget(int** stack, void* object) {
    BSObjectView* gun = *(BSObjectView**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (gun) {
        TriggerObject_struct* trigger;
        ISceneNode* self = (ISceneNode*)object;
        trigger = gun->trigger;
        ISceneNode* node = FindMMGNode(trigger);
        if (node)
            self->GetAIDoodad()->FireMMGAtCurrentTarget((CStaticObject*)node);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) void CAIDoodad::FireMMGAtCurrentTarget(CStaticObject*) {
}

__declspec(weak) CAIDoodad* CSoldierObject::GetAIDoodad() {
    return &m_doodad;
}

void BIFunc_AIFireAtCurrentTarget(int** stack, void* object) {
    float time = BSArgFloat(stack, 1);
    ((ISceneNode*)object)->GetAIDoodad()->FireAtCurrentTarget(time);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) void CAIDoodad::FireAtCurrentTarget(float) {
}

void BIFunc_AISetRunAwayMovepoint(int** stack, void* object) {
    float distance;
    CAIDoodad* doodad;
    CAIFilterObject* filter;
    ISceneNode* from;
    bool found;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    from = (ISceneNode*)*(*stack - (count - 1));
    distance = *(float*)(*stack - (count - 2));
    doodad = ((ISceneNode*)object)->GetAIDoodad();
    found = false;
    if (from) {
        filter = doodad->filter;
        CAIFilterRealPosition point;
        found = filter->FindRunAwayPoint(from, distance, &point);
        if (found) {
            ScriptMovePointView* data = ((ISceneNode*)object)->GetScriptObject()->user->GetScriptData();
            data->x = point.x;
            data->y = point.y;
            data->z = point.z;
            filter->object->SetupArbitraryPointUpdate((EMovePointType)3, (BS_STRUCT_Vector_struct*)&data->x, 0.8f);
        }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = found;
}
