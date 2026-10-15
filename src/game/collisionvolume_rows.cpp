// A fragment of collisionvolume.cpp (0x80097ea0): CCollisionVolume's
// HandleBulletCollision (for projectile bullets, fills a 24-byte script
// event block with the shooter's script object, the damage, the bullet type
// and the side hit - 3 and 2 beyond 0.7071 along the right axis, else 0
// behind and 1 in front - and fires script event 43), BeginUpdate (sets the
// extent nodes from the volume's extents, or from the position when there is
// no volume), the bounding volume getters (both return the volume pointer),
// the row accessors GetUpward, GetForward, GetRightward and GetPosition (each
// copies one row of the transform into the result) and GetTMLocalToWorld
// (copies the whole transform). The file name is this project's; the
// original record is collisionvolume.cpp (collisionvolume.cpp holds the
// start of the file). The class names come from the mangled symbols;
// CCollisionVolume declares its virtual functions in the order of
// __vt__16CCollisionVolume, CBullet and ISceneNode the used slots of theirs
// (placeholders named by offset); the members, the event block, the extent
// and volume views and the row getters returning by value are inferred, and
// the CVector3 view copies through a double pair as in player_weapon.cpp.
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

struct BSObject;
class CCollision;

void* BSEventGetEventMemoryBlock(unsigned int);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class ISceneNode {
public:
    enum EVolumeType {};

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
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual BSObject* GetScriptObject() const;
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
class IVolume {
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

class CCollisionVolume {
public:
    virtual void MarkForDestruction(int);
    virtual ~CCollisionVolume();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const void*);
    virtual void CommitUpdate();
    virtual void Draw(void*);
    virtual IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;

    void HandleBulletCollision(CBullet*, const CCollision&);

    CVector3 GetUp() const { return m_tm.up; }
    CVector3 GetFwd() const { return m_tm.forward; }
    CVector3 GetRight() const { return m_tm.right; }
    CVector3 GetPos() const { return m_tm.position; }

    unsigned char unknown04[8];
    ExtentNodeView* m_extentMinX;
    ExtentNodeView* m_extentMaxX;
    ExtentNodeView* m_extentMinY;
    ExtentNodeView* m_extentMaxY;
    unsigned char unknown1c[20];
    CMatrix m_tm;
    IVolume* m_volume;
    unsigned char unknown74[4];
    BSObject* m_script;
};

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

void CCollisionVolume::BeginUpdate(float) {
    if (m_volume) {
        CVector3 min;
        CVector3 max;
        m_volume->GetExtents(min, max);
        m_extentMinX->m_value = min.x;
        m_extentMinY->m_value = min.y;
        m_extentMaxX->m_value = max.x;
        m_extentMaxY->m_value = max.y;
    } else {
        CVector3 position;
        GetPosition(position);
        m_extentMinX->m_value = position.x;
        m_extentMinY->m_value = position.y;
        m_extentMaxX->m_value = position.x;
        m_extentMaxY->m_value = position.y;
    }
}

IVolume* CCollisionVolume::GetLocalBoundingVolume(ISceneNode::EVolumeType) const {
    return m_volume;
}

IVolume* CCollisionVolume::GetWorldBoundingVolume(ISceneNode::EVolumeType) const {
    return m_volume;
}

void CCollisionVolume::GetUpward(CVector3& v) const {
    v = GetUp();
}

void CCollisionVolume::GetForward(CVector3& v) const {
    v = GetFwd();
}

void CCollisionVolume::GetRightward(CVector3& v) const {
    v = GetRight();
}

void CCollisionVolume::GetPosition(CVector3& v) const {
    v = GetPos();
}

void CCollisionVolume::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}
