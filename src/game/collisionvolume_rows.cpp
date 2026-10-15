// A fragment of collisionvolume.cpp (0x8009806c): CCollisionVolume's
// BeginUpdate (sets the extent nodes from the volume's extents, or from the
// position when there is no volume), the bounding volume getters (both
// return the volume pointer), the row accessors GetUpward, GetForward,
// GetRightward and GetPosition (each copies one row of the transform into
// the result) and GetTMLocalToWorld (copies the whole transform). The file
// name is this project's; the original record is collisionvolume.cpp
// (collisionvolume.cpp holds the start of the file) and the bullet handler
// before these is not reconstructed. The class names come from the mangled
// symbols; CCollisionVolume declares its virtual functions in the order of
// __vt__16CCollisionVolume, the members, the extent and volume views and
// the row getters returning by value are inferred, and the CVector3 view
// copies through a double pair as in player_weapon.cpp.
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

class ISceneNode {
public:
    enum EVolumeType {};
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
};

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
