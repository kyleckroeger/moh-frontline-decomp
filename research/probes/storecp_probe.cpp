// settings: config/GMFE69/bsbifunc_coverleave.json
// replaces: BIFunc_AIChooseCoverPoint__FPPiPv
//
// Test harness (dol4), not game code. No level script calls the built-in AIStoreBestCoverPoint
// (CAIObject::StoreClosestCoverPoint, the only caller of GetClosestUsableCoverPoint with a walk
// mode and so of CCoverPoint::CheckLineCollision). This copy of BIFunc_AIChooseCoverPoint
// (src/game/bsbifunc_coverleave.cpp) first stores the AI's closest cover point as that
// built-in would, logs the result ("STORECP index"), then chooses as the original does.
// Keep it in the control.
extern "C" void OSReport(const char*, ...);
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
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


enum COVERPOINT_TYPE {};

class CCoverPoint {
public:
    void FlagAsUnoccupied(COVERPOINT_TYPE);
};

enum ECoverPointSelectionType {};

class CAIObject {
public:
    void SetCoverPoint(int, bool);
    CCoverPoint* ChooseCoverPoint(ECoverPointSelectionType, const ISceneNode*, float, float);
    CCoverPoint* StoreClosestCoverPoint();

    unsigned char unknown000[424];
    unsigned int m_flags;
    unsigned char unknown1ac[108];
    int m_coverState;
    unsigned char unknown21c[116];
    CCoverPoint* m_coverPoint;
    COVERPOINT_TYPE m_coverType;
    unsigned char unknown298[40];
    bool m_coverFlag;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObject* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterView* filter;
};

class CAIFilterGlobal {
public:
    int GetCoverPointIndex(CCoverPoint*) const;

    unsigned char unknown00[24];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

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

inline int BSArgInt(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

inline float BSArgFloat(int** stack, int index) {
    return *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

inline CAIObject* GetAIObject(void* object) {
    return ((ISceneNode*)object)->GetAIDoodad()->filter->object;
}

inline int GetCoverPointIndex(CCoverPoint* point) {
    if (point)
        return g_aigAIFilterGlobalObject.GetCoverPointIndex(point);
    return -1;
}

void BIFunc_AIChooseCoverPoint(int** stack, void* object) {
    {
        int stored = GetCoverPointIndex(GetAIObject(object)->StoreClosestCoverPoint());
        OSReport("STORECP %d\n", stored);
    }
    int value;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    ECoverPointSelectionType type = (ECoverPointSelectionType)BSArgInt(stack, 1);
    bool useNode = BSArgBool(stack, 2);
    float a = BSArgFloat(stack, 4);
    float b = BSArgFloat(stack, 5);
    CAIFilterView* filter = ((ISceneNode*)object)->GetAIDoodad()->filter;
    if (useNode)
        value = GetCoverPointIndex(filter->object->ChooseCoverPoint(type, *(const ISceneNode**)(*stack - (count - 3)), a, b));
    else
        value = GetCoverPointIndex(filter->object->ChooseCoverPoint(type, 0, a, b));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&value;
}
