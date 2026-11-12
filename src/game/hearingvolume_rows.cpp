// A fragment of hearingvolume.cpp (0x80098950): CHearingVolume's updates
// (CommitUpdate and BeginUpdate keep the transform's position; BeginUpdate
// first takes the owner's transform; AttemptUpdate moves the transform by the
// owner's velocity over the step when it has an owner, then transforms the
// local volume into the world volume and stores its x and y extents in four sort-key records), the
// bounding volumes (the world volume at +160, the local one at +128), then
// the transform wrappers, from GetUpward to GetTMLocalToWorld, each
// forwarding to the object's transform (a CMatrix at +48). The file
// name is this project's; the original record is hearingvolume.cpp and the functions
// around these are not reconstructed. CHearingVolume and CMatrix's methods are named by
// the mangled symbols; CHearingVolume is an inferred non-virtual view with only the
// transform declared, the bodies are inferred from the calls, and the by-value
// row getters are inferred inline helpers.
//
// CVector3 is the shared game/CVector3.h. The scene-node views are as in bsbifunc_corpse.cpp (the owner
// is taken to be a CStaticObject, whose velocity getter is at +276); the
// owner at +36, the sort-key records, the held volumes and the last position
// at +112 are inferred.


#include "game/CVector3.h"


class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);
    void Orthonormalize();
    void RotateX(float);
    void RotateY(float);
    void RotateZ(float);
    void Rotate(CVector3, float);
    void Translate(CVector3);
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);
    void Ident();

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

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
    virtual CVector3 GetWorldLinearVelocity() const;
};

class CTriangle;
class CPlane;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

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
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
};

// Inferred: a volume held in the object (its virtual table first, 32 bytes).
class CHeldVolumeView : public IVolume {
public:
    unsigned char unknown04[28];
};

// Inferred: a record whose sort key (a float at +8) the extents update.
struct SortKeyView {
    unsigned char unknown00[8];
    float m_key;
};

class CHearingVolume {
public:
    void CommitUpdate();
    void AttemptUpdate(float);
    void BeginUpdate(float);
    const IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    const IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    void GetUpward(CVector3&) const;
    void GetForward(CVector3&) const;
    void GetRightward(CVector3&) const;
    void GetPosition(CVector3&) const;
    void GetTMLocalToWorld(CMatrix&) const;

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.forward; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown00[12];
    SortKeyView* m_minX;
    SortKeyView* m_maxX;
    SortKeyView* m_minY;
    SortKeyView* m_maxY;
    unsigned char unknown1c[8];
    CStaticObject* m_owner;
    unsigned char unknown28[8];
    CMatrix m_tm;
    CVector3 m_lastPosition;
    CHeldVolumeView m_localVolume;
    CHeldVolumeView m_worldVolume;
};

void CHearingVolume::CommitUpdate() {
    m_lastPosition = PositionRow();
}

void CHearingVolume::AttemptUpdate(float dt) {
    if (m_owner) {
        const CVector3& velocity = m_owner->GetWorldLinearVelocity();
        CVector3 step;
        step.x = dt * velocity.x;
        step.y = dt * velocity.y;
        step.z = dt * velocity.z;
        m_tm.SetPos(m_lastPosition);
        m_tm.Translate(step);
    }
    m_worldVolume.TransformedCopy(m_localVolume, m_tm);
    CVector3 minimum;
    CVector3 maximum;
    m_worldVolume.GetExtents(minimum, maximum);
    m_minX->m_key = minimum.x;
    m_minY->m_key = minimum.y;
    m_maxX->m_key = maximum.x;
    m_maxY->m_key = maximum.y;
}

void CHearingVolume::BeginUpdate(float) {
    if (m_owner) {
        m_owner->GetTMLocalToWorld(m_tm);
        m_lastPosition = PositionRow();
    }
}

const IVolume* CHearingVolume::GetWorldBoundingVolume(ISceneNode::EVolumeType) const {
    return &m_worldVolume;
}

const IVolume* CHearingVolume::GetLocalBoundingVolume(ISceneNode::EVolumeType) const {
    return &m_localVolume;
}

void CHearingVolume::GetUpward(CVector3& v) const {
    v = UpRow();
}

void CHearingVolume::GetForward(CVector3& v) const {
    v = ForwardRow();
}

void CHearingVolume::GetRightward(CVector3& v) const {
    v = RightRow();
}

void CHearingVolume::GetPosition(CVector3& v) const {
    v = PositionRow();
}

void CHearingVolume::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}
