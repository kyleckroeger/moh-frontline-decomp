// collisionvolume.cpp: CCollisionVolume, a scene node made of a collision
// volume (a trigger or blocking volume placed by the level). The whole file,
// with the class's virtual table and RTTI record (slot order from
// __vt__16CCollisionVolume; MarkForDestruction is the key function). First
// the weak collision id and script accessors and identity casts (inline in
// the original), Destroy (through the script object's user object) and
// MarkForDestruction (ISubject's, then removed from g_scene); then
// HandleBulletCollision (for projectile bullets, fills a 24-byte script event
// block with the shooter's script object, the damage, the bullet type and the
// side hit - 3 and 2 beyond 0.7071 along the right axis, else 0 behind and 1
// in front - and fires script event 43), BeginUpdate (sets the extent nodes
// from the volume's extents, or from the position when there is no volume),
// the bounding volume getters (both return the volume pointer), the row
// accessors GetUpward, GetForward, GetRightward and GetPosition (each copies
// one row of the transform into the result) and GetTMLocalToWorld (copies
// the whole transform); last Init (keeps the volume, copies the transform and
// keeps the collision ID), the destructor and the constructor (the scene-node
// base chain's virtual table pointers, eight cleared words, the transform's
// class initialisation and a null volume).
// The class names, the base chain and the member functions come from the
// mangled symbols and the virtual tables; the members, their names, the
// event block, the extent node and volume views, the script user object and
// the row getters returning by value are inferred. CBullet declares the used
// slots of its table (placeholders named by offset). SetScript is inline in
// the class and emitted by another file. ISceneNode's inline virtuals and
// destructor give weak copies here that are weak duplicates, linked to the
// original copies. The file is compiled with deferred inlining, so its
// functions are written here in reverse of their order in the original.
class IVolume;

#include "game/CVector3.h"

#include "game/CMatrix.h"

#include "game/ISceneNode.h"

#include "game/CScene.h"

extern CScene g_scene;

void* BSEventGetEventMemoryBlock(unsigned int);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

// The object at BSObject+12; only its first virtual slot is called here, and
// its name is unknown (as in bsmachin.cpp).
class BSObjectUserView {
    unsigned char unknown00[12];

public:
    virtual void unknownVirtual0();
};

struct BSObject {
    unsigned char unknown00[12];
    BSObjectUserView* user;
};

class CProjectileBullet;

class CBullet {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void GetForward(CVector3&) const;
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual void vf0cc();
    virtual void vf0d0();
    virtual void vf0d4();
    virtual void vf0d8();
    virtual void vf0dc();
    virtual void vf0e0();
    virtual void vf0e4();
    virtual void vf0e8();
    virtual void vf0ec();
    virtual void vf0f0();
    virtual void vf0f4();
    virtual void vf0f8();
    virtual void vf0fc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual void vf120();
    virtual void vf124();
    virtual void vf128();
    virtual void vf12c();
    virtual float GetDamage() const;
    virtual int GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual void vf13c();
    virtual CProjectileBullet* AsProjectile();
};

/* inferred: the bullet event block (24 bytes) passed with script event 43 */
struct BulletHitEvent {
    BSObject* m_shooter;
    int m_damage;
    int m_side;
    int m_type;
};

/* inferred: the volume's extents slot (+72) */
class IVolumeView {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void GetExtents(CVector3&, CVector3&) const;
};

/* inferred: a node of the scene's extent lists, its value at +8 */
struct ExtentNodeView {
    unsigned char unknown00[8];
    float m_value;
};

inline ExtentNodeView* Extent(int node) {
    return (ExtentNodeView*)node;
}

class CCollisionVolume : public ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~CCollisionVolume();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObject* GetScriptObject() const;
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual CCollisionVolume* AsCollisionVolume();
    virtual const CCollisionVolume* AsCollisionVolume() const;
    virtual void SetScript(BSObject* script) { m_script = script; }
    CCollisionVolume();
    void Init(IVolume*, CMatrix*, EClsnId);

    CVector3 GetUp() const { return m_tm.up; }
    CVector3 GetFwd() const { return m_tm.front; }
    CVector3 GetRight() const { return m_tm.right; }
    CVector3 GetPos() const { return m_tm.pos; }

    unsigned char unknown24[12];
    CMatrix m_tm;
    IVolume* m_volume;
    EClsnId m_collisionId;
    BSObject* m_script;
};

CCollisionVolume::CCollisionVolume() {
    m_volume = 0;
}

CCollisionVolume::~CCollisionVolume() {
}

void CCollisionVolume::Init(IVolume* volume, CMatrix* matrix, EClsnId id) {
    m_volume = volume;
    m_tm = *matrix;
    m_collisionId = id;
}

void CCollisionVolume::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}

void CCollisionVolume::GetPosition(CVector3& v) const {
    v = GetPos();
}

void CCollisionVolume::GetRightward(CVector3& v) const {
    v = GetRight();
}

void CCollisionVolume::GetForward(CVector3& v) const {
    v = GetFwd();
}

void CCollisionVolume::GetUpward(CVector3& v) const {
    v = GetUp();
}

IVolume* CCollisionVolume::GetWorldBoundingVolume(ISceneNode::EVolumeType) const {
    return m_volume;
}

IVolume* CCollisionVolume::GetLocalBoundingVolume(ISceneNode::EVolumeType) const {
    return m_volume;
}

void CCollisionVolume::BeginUpdate(float) {
    if (m_volume) {
        CVector3 min;
        CVector3 max;
        ((IVolumeView*)m_volume)->GetExtents(min, max);
        Extent(data0c)->m_value = min.x;
        Extent(data14)->m_value = min.y;
        Extent(data10)->m_value = max.x;
        Extent(data18)->m_value = max.y;
    } else {
        CVector3 position;
        GetPosition(position);
        Extent(data0c)->m_value = position.x;
        Extent(data14)->m_value = position.y;
        Extent(data10)->m_value = position.x;
        Extent(data18)->m_value = position.y;
    }
}

void CCollisionVolume::HandleBulletCollision(CBullet* bullet, const CCollision&) {
    if (!bullet->AsProjectile())
        return;
    BulletHitEvent* event = (BulletHitEvent*)BSEventGetEventMemoryBlock(24);
    event->m_type = bullet->GetScriptBulletType();
    event->m_damage = (int)bullet->GetDamage();
    ISceneNode* shooter = bullet->GetFiredBy();
    if (shooter)
        event->m_shooter = shooter->GetScriptObject();
    else
        event->m_shooter = 0;
    CVector3 direction;
    bullet->GetForward(direction);
    CVector3 forward;
    GetForward(forward);
    CVector3 right;
    GetRightward(right);
    float front = forward.x * direction.x + forward.y * direction.y + forward.z * direction.z;
    float side = right.x * direction.x + right.y * direction.y + right.z * direction.z;
    if (side < -0.7071f)
        event->m_side = 3;
    else if (side > 0.7071f)
        event->m_side = 2;
    else if (front < 0.0f)
        event->m_side = 0;
    else
        event->m_side = 1;
    BSObjectTriggerEvent(m_script, 43, event, 0, false);
}

void CCollisionVolume::MarkForDestruction(int flag) {
    ISubject::MarkForDestruction(flag);
    g_scene.Remove(*this);
}

void CCollisionVolume::Destroy() {
    if (m_script && m_script->user)
        m_script->user->unknownVirtual0();
}

__declspec(weak) void CCollisionVolume::SetCollisionId(EClsnId id) {
    m_collisionId = id;
}

__declspec(weak) BSObject* CCollisionVolume::GetScriptObject() const {
    return m_script;
}

__declspec(weak) CCollisionVolume* CCollisionVolume::AsCollisionVolume() {
    return this;
}

__declspec(weak) const CCollisionVolume* CCollisionVolume::AsCollisionVolume() const {
    return this;
}

__declspec(weak) EClsnId CCollisionVolume::GetCollisionId() const {
    return m_collisionId;
}
