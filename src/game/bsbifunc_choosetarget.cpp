// A fragment of bsbifunc.cpp (0x80033de4): AI built-ins that set the hearing
// radius (at most 0.001 replaced by 20 with a warning; stored at +528
// truncated to sixteenths), choose a target by
// script object (none for 0 or -1, returning false; otherwise that object's AI
// object, reached through its user object's scene node, AI doodad and filter,
// or the object itself as a non-AI target, returning true) and choose a target
// (the AI filter's virtual ChooseTarget with the given time, its result
// returned through an integer/float union), and set up the AI filter's target
// match list (its virtual SetupTargetMatchList with the mode, the list read
// through the second argument and the count). Each pops the built-in's
// arguments. The file name is this project's; the original record is
// bsbifunc.cpp. The functions,
// classes and globals are named by the mangled symbols; ISceneNode is declared
// with its virtual functions in the order of __vt__10ISceneNode,
// CAIFilterObject in the order of __vt__15CAIFilterObject (its AI object at +8)
// and BSGO_Basic in the order of __vt__10BSGO_Basic; BSObject (its user object
// at +12), the doodad, value and built-in record views, the AI object's
// hearing radius and the filter and argument helpers are inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
struct BSObject;
class CPlayerObject;
struct AIDoodadView;
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

enum aifilter_target_mode {};
struct aistatus_match;
class CAIObject;

class CAIObject {
public:
    void SetTarget(CAIObject*);
    void SetNonAITarget(BSObject*);

    unsigned char unknown000[528];
    float m_hearingRadius;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObject {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual int ChooseTarget(float);
    virtual void SetTargetMode(aifilter_target_mode);
    virtual void ResetTargetMatchList(aifilter_target_mode);
    virtual void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    int ShouldILeaveMMG(BSObject*);

    unsigned char unknown04[4];
    CAIObject* m_object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterObject* filter;
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

inline float BSArgFloat(int** stack, int index) {
    return *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

extern "C" int printf(const char*, ...);

inline CAIFilterObject* GetAIFilter(BSObject* script) {
    if (script && script->user) {
        ISceneNode* node = script->user->GetSceneNode();
        if (node) {
            AIDoodadView* doodad = node->GetAIDoodad();
            if (doodad)
                return doodad->filter;
        }
    }
    return 0;
}

void BIFunc_AISetHearingRadius(int** stack, void* object) {
    AIDoodadView* doodad = ((ISceneNode*)object)->GetAIDoodad();
    CAIFilterObject* filter = doodad->filter;
    float radius = BSArgFloat(stack, 1);
    CAIObject* ai = filter->m_object;
    if (radius <= 0.001f) {
        printf("Warning: Hearing Radius is zero, changing to default\n");
        radius = 20.0f;
    }
    ai->m_hearingRadius = 0.0625f * (short)(16.0f * radius);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIChooseTargetByID(int** stack, void* object) {
    CAIObject* ai;
    CAIObject* other;
    BSObject* target;
    AIDoodadView* doodad = ((ISceneNode*)object)->GetAIDoodad();
    target = *(BSObject**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ai = doodad->filter->m_object;
    bool result;
    if (!target || target == (BSObject*)0xffffffff) {
        ai->SetTarget(0);
        result = false;
    } else {
        other = 0;
        CAIFilterObject* filter = GetAIFilter(target);
        if (filter)
            other = filter->m_object;
        if (other)
            ai->SetTarget(other);
        else
            ai->SetNonAITarget(target);
        result = true;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}

void BIFunc_AIChooseTarget(int** stack, void* object) {
    BSValueView value;
    float time = *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    value.i = ((ISceneNode*)object)->GetAIDoodad()->filter->ChooseTarget(time);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_AISetupTargetMatchList(int** stack, void* object);


void BIFunc_AISetupTargetMatchList(int** stack, void* object) {
    AIDoodadView* doodad = ((ISceneNode*)object)->GetAIDoodad();
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    doodad->filter->SetupTargetMatchList((aifilter_target_mode)*(*stack - (count - 1)),
                                 *(aistatus_match**)*(*stack - (count - 2)), *(*stack - (count - 3)));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
