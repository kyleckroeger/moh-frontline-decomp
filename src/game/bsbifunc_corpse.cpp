// A fragment of bsbifunc.cpp (0x8002e1bc): the script built-ins that set up a
// target check on another script object (RegisterTargetCheck: field of view for
// mode 2, line of sight for modes 3 and 5, vision seeking for mode 4, toward its
// position raised by 0.8 unless it is already the target), the calling node's AI
// object's corpse field of view (RegisterCorpseSearch) or a target search over a
// match list (RegisterTargetSearch: field of view for mode 2, line of sight for
// mode 3) and register the script object with the target-check, corpse- or
// target-search schedule, returning the registration. They read their arguments
// below the script stack top and pop the built-in's arguments. The file name is
// this project's; the original record is bsbifunc.cpp.
// RegisterTargetCheck and RegisterTargetSearch declare their locals first and assign them
// afterwards; RegisterTargetCheck passes its flag argument as '(bool)flag' (a '!= 0' test colours the
// registers differently)
// (the declaration order sets the register use). The functions, classes and globals are named by the
// mangled symbols; ISceneNode is declared with its virtual functions in the
// order of __vt__10ISceneNode (GetAIDoodad at +204), IMovingSceneNode's and
// CStaticObject's after them in the order of __vt__13CStaticObject,
// CAIFilterObject's in the order of its table (SetTargetMode at +16); the
// doodad, filter, AI object, schedule (BSSchedule's size is not known),
// thrown-object and built-in record views and the helpers are inferred.
class CVector3 {
public:
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
class CThrownObject;
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
    virtual CThrownObject* AsThrownObject();
    virtual void* AsWeaponObject();
    virtual void* AsHierTankObject();
};

class CThrownObject : public ISceneNode {
public:
    unsigned char unknown004[756];
    int m_drop;
};

struct aistatus_match;

struct MatchListView {
    aistatus_match* list;
};

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition() {}
    CAIFilterRealPosition(float ax, float az, float ay) { x = ax; y = ay; z = az; }
    float x;
    float y;
    float z;
};

class CAIObject {
public:
    void SetupLineOfSightModeObject(CAIObject*, float);
    void SetupFieldOfViewModeObject(CAIObject*, float, float);
    void SetupVisionSeekingMode(CAIObject*, CAIFilterRealPosition&, float);
    void SetupFieldOfViewCorpse(float, float);
    void SetupFieldOfViewModeList(aistatus_match*, int, float, float);
    void SetupLineOfSightModeList(aistatus_match*, int, float);

    unsigned char unknown000[192];
    bool m_targetStored;
    unsigned char unknown0c1[3];
    CAIObject* m_target;
};

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual void ChooseTarget(float);
    virtual void SetTargetMode(int);

    unsigned char unknown04[4];
    CAIObject* m_object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterObject* filter;
};

class BSSchedule {
public:
    int Register(BSObject*, unsigned short, bool, int, int, int);

    unsigned char unknown00[40];
};

extern BSSchedule g_TargetCheckSchedule;
extern BSSchedule g_CorpseSearchSchedule;
extern BSSchedule g_TargetSearchSchedule;
extern BSSchedule g_DistanceToTargetSchedule;
extern BSObject* g_pBSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

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

inline CAIObject* GetAIObject(void* object) {
    return ((ISceneNode*)object)->GetAIDoodad()->filter->m_object;
}

/* Inferred: a script object's scene node (through its user's GetSceneNode)
   and that node's AI filter, or null. */
struct BSUserView {
    unsigned char unknown00[12];
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSUserView* user;
};

inline CAIFilterObject* GetScriptFilter(BSObjectView* script) {
    ISceneNode* node;
    AIDoodadView* doodad;
    if (script && script->user && (node = script->user->GetSceneNode()) && (doodad = node->GetAIDoodad()))
        return doodad->filter;
    return 0;
}

inline CAIObject* GetScriptAIObject(BSObjectView* script) {
    CAIObject* object = 0;
    CAIFilterObject* filter = GetScriptFilter(script);
    if (filter)
        object = filter->m_object;
    return object;
}

inline bool HasTarget(CAIObject* ai) {
    return ai && (ai->m_targetStored || ai->m_target);
}

void BIFunc_RegisterTargetCheck(int** stack, void* object) {
    int* args;
    int count;
    CAIFilterObject* filter;
    int result;
    int mode;
    float range;
    float angle;
    int target;
    unsigned short time;
    int flag;
    result = 0;
    filter = ((ISceneNode*)object)->GetAIDoodad()->filter;
    count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    args = *stack;
    mode = *(args - (count - 1));
    range = *(float*)(args - (count - 2));
    angle = *(float*)(args - (count - 3));
    target = *(args - (count - 4));
    time = *(args - (count - 5));
    flag = *(args - (count - 6));
    if (target && GetScriptFilter((BSObjectView*)target)) {
        filter->SetTargetMode(0);
        switch (mode) {
        case 1:
            break;
        case 2:
            filter->m_object->SetupFieldOfViewModeObject(GetScriptAIObject((BSObjectView*)target), range, angle);
            break;
        case 3:
            filter->m_object->SetupLineOfSightModeObject(GetScriptAIObject((BSObjectView*)target), range);
            break;
        case 4: {
            CAIObject* other = GetScriptAIObject((BSObjectView*)target);
            CAIObject* self = filter->m_object;
            if (other == self->m_target) {
                self->SetupVisionSeekingMode(other, *(CAIFilterRealPosition*)((char*)self + 336), range);
            } else {
                CAIFilterRealPosition point(((CAIFilterRealPosition*)((char*)other + 24))->x, ((CAIFilterRealPosition*)((char*)other + 24))->z, ((CAIFilterRealPosition*)((char*)other + 24))->y);
                point.z += 0.8f;
                self->SetupVisionSeekingMode(other, point, range);
            }
            break;
        }
        case 5:
            filter->m_object->SetupLineOfSightModeObject(GetScriptAIObject((BSObjectView*)target), range);
            break;
        }
        result = g_TargetCheckSchedule.Register(g_pBSObject, time, (bool)flag, mode, (int)range, target);
    } else {
        BSObjectTriggerEvent(g_pBSObject, 69, 0, g_pBSObject, true);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}

void BIFunc_RegisterCorpseSearch(int** stack, void* object) {
    AIDoodadView* doodad = ((ISceneNode*)object)->GetAIDoodad();
    unsigned short time = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 3));
    doodad->filter->m_object->SetupFieldOfViewCorpse(*(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)), *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 2)));
    int result = g_CorpseSearchSchedule.Register(g_pBSObject, time, true, 0, 0, 0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}

void BIFunc_RegisterTargetSearch(int** stack, void* object) {
    CAIFilterObject* filter;
    MatchListView* list;
    int params;
    float range;
    float angle;
    int mode;
    int count;
    unsigned short time;
    int repeat;
    filter = ((ISceneNode*)object)->GetAIDoodad()->filter;
    params = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    mode = *(*stack - (params - 1));
    range = *(float*)(*stack - (params - 2));
    angle = *(float*)(*stack - (params - 3));
    list = (MatchListView*)*(*stack - (params - 4));
    count = *(*stack - (params - 5));
    time = *(*stack - (params - 6));
    repeat = *(*stack - (params - 7)) != 0;
    filter->SetTargetMode(0);
    switch (mode) {
    case 2:
        filter->m_object->SetupFieldOfViewModeList(list->list, count, range, angle);
        break;
    case 3:
        filter->m_object->SetupLineOfSightModeList(list->list, count, range);
        break;
    }
    int result = g_TargetSearchSchedule.Register(g_pBSObject, time, repeat, mode, 0, 0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}
