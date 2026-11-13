// A fragment of AIFilter.cpp (0x8005bea4): the static
// CAIFilterGlobal::GetCollisionDistLineToEnvironment. A line from the start to
// the end position (CLine3's inline constructor through SetSE, as in
// fiber_dtor.cpp) becomes a fiber volume and is tested against the world
// object's bounding volume (a hit scales the distance by the collision's t);
// then every static object is visited with a MechanicCollisionChecker that
// skips the filter's own scene node (through its script object) and keeps
// the nearest distance. The result is the distance when anything was hit,
// otherwise -1.0f. The file name is this project's; the original record is
// AIFilter.cpp, between AIFilter_caifilterobject_dtor.cpp and
// AIFilter_mechchecker_dtor.cpp. The classes, the template and the functions
// are named by the mangled symbols; the scene-node and volume virtuals are
// declared in table order; CAIFilterRealPosition is viewed as an 8-aligned
// 16-byte vector (the positions are copied as double pairs), CLine3 as
// 16-byte aligned (the function realigns its stack; see AIFilter_linechecks.cpp), and the members, the
// checker's constructor and the owner chain from the filter are inferred.
// BSGO_Basic's three words precede its table pointer, as in
// thrown_obj_ctor.cpp. CLine3 has an inline (weak) destructor: the original's
// exception table covers the line. The checker's and the line's inline
// destructors and the visitor's weak table and destructor the compiler emits
// are weak duplicates, linked to the
// original copies (the checker's in AIFilter_mechchecker_dtor.cpp). The -1.0f
// constant is an item of the file's .sdata2 pool.
class CBullet;
class CCollision;
class CDrawContext;
class CLight;
class CPlayerObject;
class CStaticObject;
class CMatrix;
struct AIDoodadView;
enum EClsnId {};

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

/* inferred: a 16-byte, 8-aligned position (copied as two doubles) */
class CAIFilterRealPosition : public CVector3 {};

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    ~CLine3() {}
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));
class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
    virtual void Destroy();
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

/* ISceneNode's virtuals in table order (slots 20 to 216). They are the weak
   inline defaults of Moh2.cpp in the original; their bodies are not repeated
   here, so they are declared without one. */
class ISceneNode : public IObserver {
public:
    enum EVolumeType {};
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}
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
    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class CVolFiber;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;

/* IVolume's virtuals in table order up to the fiber test. */
class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
};

class CVolFiber : public IVolume {
public:
    CVolFiber(const CLine3&);
    CVolFiber(const CVolFiber&);
    virtual ~CVolFiber();

    unsigned char unknown04[12];
    CLine3 m_line;
};

class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[12];
    float m_t;
    unsigned char unknown10[16];
};

namespace dwi {
template <class T>
class IVisitor {
public:
    virtual ~IVisitor() {}
    virtual bool Visit(T&) = 0;
};
}

class MechanicCollisionChecker : public dwi::IVisitor<ISceneNode> {
public:
    MechanicCollisionChecker(const CVolFiber& fiber, float distance, ISceneNode* skip)
        : m_fiber(fiber), data60(0), data61(0), m_distance(distance), m_nearest(distance), m_skip(skip) {}
    virtual bool Visit(ISceneNode&);
    virtual ~MechanicCollisionChecker() {}

    unsigned char unknown04[12];
    CVolFiber m_fiber;
    bool data60;
    bool data61;
    float m_distance;
    float m_nearest;
    ISceneNode* m_skip;
};

#include "game/CScene.h"


extern CScene g_scene;

class BSGO_Basic {
    int m_field0;
    int m_field4;
    int m_field8;

public:
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

/* inferred: the filter's object (+12) and its script object (+12) */
struct AIFilterOwnerView {
    unsigned char unknown00[12];
    BSGO_Basic* m_script;
};

class CAIFilterObject {
public:
    unsigned char unknown00[12];
    AIFilterOwnerView* m_owner;
};

class CAIFilterGlobal {
public:
    static float GetCollisionDistLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                                   CAIFilterObject*, float);
};

float CAIFilterGlobal::GetCollisionDistLineToEnvironment(const CAIFilterRealPosition& start,
                                                         const CAIFilterRealPosition& end, CAIFilterObject* filter,
                                                         float distance) {
    CLine3 line(start, end);
    CCollision collision;
    ISceneNode* worldObject = g_scene.m_world;
    int result = 1;
    CVolFiber fiber(line);
    IVolume* world = (IVolume*)worldObject->GetWorldBoundingVolume((ISceneNode::EVolumeType)0);
    if (world)
        result = world->TestCollision(fiber, collision, true);
    bool hit = result != 1;
    if (hit)
        distance *= collision.m_t;
    ISceneNode* skip = 0;
    if (filter)
        skip = filter->m_owner->m_script->GetSceneNode();
    MechanicCollisionChecker checker(fiber, distance, skip);
    g_scene.VisitAllStaticObjects(checker);
    if (checker.m_nearest < distance) {
        distance = checker.m_nearest;
        hit = true;
    }
    if (hit)
        return distance;
    return -1.0f;
}
