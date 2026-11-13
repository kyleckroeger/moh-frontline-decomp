// hearingvolume.cpp (0x80098360..0x80098de4): CHearingVolume, a scene node that hears bullets
// passing its owner. The collision id accessors (weak inline copies), the bullet whiz-by sound,
// HandleBulletCollision (normally: for a projectile bullet, call the handler with the owner,
// exclude the bullet from the volume and let it pass; with the bullet-shield cheat, a spark
// effect along the bullet's direction reflected about the contact normal), CBullet's empty
// SetExclusionPair and Penetrate (weak inline copies emitted here), the update and row
// accessors, Init, the destructor and the constructor. Merged from the fragments
// hearingvolume, hearingvolume_bullet, hearingvolume_rows, hearingvolume_init and
// hearingvolume_ctor; it defines __vt__14CHearingVolume.
//
// Names from the mangled symbols and the vtables (__vt__14CHearingVolume, __vt__7CBullet,
// __vt__10CVolSphere). Inferred: the members past ISceneNode (owner +0x24, the matrix +0x30,
// the last position +0x70, the local and world spheres +0x80/+0xa0, the collision id +0xc0
// and the bullet handler +0xc4), the sweep-extent records that ISceneNode's words +0x0c..+0x18
// point at (SortKeyView), the contact, cheat and particle-system views and the reflection
// helpers.
//
// The file compiles with RTTI on and deferred inlining, so the definitions below are written
// last to first (the compiler emits them in reverse; the original numbers CHearingVolume's RTTI
// records before every function's, which only the deferred compile reproduces).
class CDrawContext;
class CCollision;
class CPlayerObject;
class CThrownBullet;
class CProjectileBullet;
class BPDLightVolume;

inline double fabs(double x) { return __fabs(x); }

#include "game/CVector3.h"
#include "game/CMatrix.h"
#include "game/IMovingSceneNode.h"

class CStaticObject : public IMovingSceneNode {
public:
    virtual ~CStaticObject();
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
    IVolume() {}
    virtual ~IVolume() {}
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

class CVolSphere : public IVolume {
public:
    CVolSphere() {}
    virtual ~CVolSphere();
    void SetCenter(CVector3);
    void SetRadius(float);

    unsigned char unknown04[28];
};

class CBullet : public IMovingSceneNode {
public:
    virtual ~CBullet();
    virtual void Draw(CDrawContext&);
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual bool IsDrawEnabled() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void Init();
    virtual void Shutdown();
    virtual void GetUp(CVector3&) const;
    virtual float GetDamage() const;
    virtual int GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&);
    virtual void SetExclusionPair(ISceneNode*);
    virtual void Penetrate(const CCollision&);
};

/* inferred: a collision contact (normal, then position at +16) */
struct ContactView {
    CVector3 m_normal;
    CVector3 m_position;
};

class CCollision {
public:
    ISceneNode* m_node0;
    ISceneNode* m_node1;
    unsigned char unknown08[16];
    ContactView* m_contact;
};

class CParticleSystem {
    unsigned char m_unknown00[28];

public:
    static CParticleSystem* Create(unsigned long);
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void SetLocalToWorld(const CMatrix&);
};

/* inferred: the cheat flag read here */
struct CheatsView {
    bool immortality;
    unsigned char unknown01[7];
    bool bulletShield;
    unsigned char unknown09[11];
};

extern CheatsView g_cheats;

#include "game/CScene.h"

extern CScene g_scene;

void PlayBulletWhiz(CVector3, CVector3, CPlayerObject*);

/* inferred: a sweep-extent record; ISceneNode's words +0x0c..+0x18 point at them */
struct SortKeyView {
    unsigned char unknown00[8];
    float m_key;
};

class CHearingVolume : public ISceneNode {
public:
    virtual ~CHearingVolume();
    virtual void BeginUpdate(float);
    virtual void AttemptUpdate(float);
    virtual void CommitUpdate();
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    CHearingVolume();
    void Init(IMovingSceneNode*, EClsnId, float, float, void (*)(IMovingSceneNode*, CBullet*));

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.front; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.pos; }
    SortKeyView* MinX() const { return (SortKeyView*)data0c; }
    SortKeyView* MaxX() const { return (SortKeyView*)data10; }
    SortKeyView* MinY() const { return (SortKeyView*)data14; }
    SortKeyView* MaxY() const { return (SortKeyView*)data18; }

    CStaticObject* m_owner;
    unsigned char unknown28[8];
    CMatrix m_tm;
    CVector3 m_lastPosition;
    CVolSphere m_localVolume;
    CVolSphere m_worldVolume;
    EClsnId m_collisionId;
    void (*m_onBullet)(IMovingSceneNode*, CBullet*);
};

CHearingVolume::CHearingVolume() {
    m_owner = 0;
}

CHearingVolume::~CHearingVolume() {
}

void CHearingVolume::Init(IMovingSceneNode* owner, EClsnId id, float radius, float height,
                          void (*callback)(IMovingSceneNode*, CBullet*)) {
    m_tm.Ident();
    m_localVolume.SetCenter(CVector3(0.0f, 0.0f, height));
    m_localVolume.SetRadius(radius);
    m_owner = (CStaticObject*)owner;
    m_collisionId = id;
    m_onBullet = callback;
    data08 = 0;
    data04 = 0;
    g_scene.Add(*(ISceneNode*)this);
}

void CHearingVolume::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}

void CHearingVolume::GetPosition(CVector3& v) const {
    v = PositionRow();
}

void CHearingVolume::GetRightward(CVector3& v) const {
    v = RightRow();
}

void CHearingVolume::GetForward(CVector3& v) const {
    v = ForwardRow();
}

void CHearingVolume::GetUpward(CVector3& v) const {
    v = UpRow();
}

IVolume* CHearingVolume::GetLocalBoundingVolume(EVolumeType) const {
    return (IVolume*)&m_localVolume;
}

IVolume* CHearingVolume::GetWorldBoundingVolume(EVolumeType) const {
    return (IVolume*)&m_worldVolume;
}

void CHearingVolume::BeginUpdate(float) {
    if (m_owner) {
        m_owner->GetTMLocalToWorld(m_tm);
        m_lastPosition = PositionRow();
    }
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
    MinX()->m_key = minimum.x;
    MinY()->m_key = minimum.y;
    MaxX()->m_key = maximum.x;
    MaxY()->m_key = maximum.y;
}

void CHearingVolume::CommitUpdate() {
    m_lastPosition = PositionRow();
}

__declspec(weak) void CBullet::SetExclusionPair(ISceneNode*) {
}

__declspec(weak) void CBullet::Penetrate(const CCollision&) {
}

inline float Dot3(float x, float y, float z, const CVector3& v) { return x * v.x + y * v.y + z * v.z; }
inline void Reflect(CVector3& r, const CVector3& d, const CVector3& n) {
    float nz, scale, ny, nx, dot;
    nx = n.x;
    ny = n.y;
    nz = n.z;
    dot = Dot3(nx, ny, nz, d);
    scale = -2.0f * dot;
    r.x = scale * nx;
    r.y = scale * ny;
    r.z = scale * nz;
    r.x += d.x;
    r.y += d.y;
    r.z += d.z;
}
void CHearingVolume::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    if (!g_cheats.bulletShield) {
        if (bullet->AsProjectile()) {
            m_onBullet((IMovingSceneNode*)m_owner, bullet);
            bullet->SetExclusionPair((ISceneNode*)this);
            bullet->Penetrate(collision);
            g_scene.ExcludeCollision(*collision.m_node0, *collision.m_node1, -1);
        }
        return;
    }
    ContactView* contact = collision.m_contact;
    CParticleSystem* sparks = CParticleSystem::Create(0x59E08587);
    if (sparks) {
        static CVector3 vRight(1.0f, 0.0f, 0.0f);
        CMatrix matrix;
        CVector3 right;
        CVector3 front;
        CVector3 direction;
        CVector3 up;
        matrix.Ident();
        bullet->GetForward(direction);
        Reflect(up, direction, contact->m_normal);
        up.Normalize();
        if ((float)fabs(up.Dot(vRight)) <= 0.99999f) {
            front.Cross(up, vRight);
            front.Normalize();
            right.Cross(front, up);
        } else {
            right.Cross(up, CVector3(0.0f, 0.0f, 1.0f));
            right.Normalize();
            front.Cross(up, right);
        }
        matrix.SetRight(right);
        matrix.SetFront(front);
        matrix.SetUp(up);
        matrix.SetPos(contact->m_position);
        matrix.Orthonormalize();
        sparks->SetLocalToWorld(matrix);
    }
    m_onBullet((IMovingSceneNode*)m_owner, bullet);
}

void PlayBulletWhizBys(IMovingSceneNode*, CBullet* bullet) {
    CVector3 position;
    CVector3 direction;

    bullet->GetPosition(position);
    bullet->GetForward(direction);
    PlayBulletWhiz(position, direction, g_scene.GetPlayer(0));
}

__declspec(weak) void CHearingVolume::SetCollisionId(EClsnId id) {
    m_collisionId = id;
}

/* ---- definitions ---- */
__declspec(weak) EClsnId CHearingVolume::GetCollisionId() const {
    return m_collisionId;
}
