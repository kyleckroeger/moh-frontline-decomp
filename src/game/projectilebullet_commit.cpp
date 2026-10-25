#include <math.h>
// A fragment of projectilebullet.cpp (0x800d1000): CProjectileBullet::
// Penetrate pushes the bullet through a surface: its forward direction scaled
// so its component along the collision normal is CONTACT_EPSILON (or, when
// nearly parallel, the normal times plus or minus CONTACT_EPSILON by the
// sign) is added to the matrix position, and the dead flag is cleared.
// CommitUpdate marks the bullet for destruction when its life (+144) is used
// up or it is dead, else copies the matrix position to the current position.
// The position setter (an inline taking the sum by reference) is inferred.
// The views are those of projectilebullet_update.cpp (inferred; CVector3 is
// the double-pair view of projectilebullet_ctor.cpp). The rest of the file is
// not part of this unit.
class CDrawContext;
class CBullet;
class CCollision;
class CMatrix;
class CLine3;
class CRenderList;

/* inferred: CVector3 as four floats (copies move doubleword pairs) */
class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) {
        v[0] = ax;
        v[1] = ay;
        v[2] = az;
    }

    CVector3 operator+(const CVector3& o) const { return CVector3(v[0] + o.v[0], v[1] + o.v[1], v[2] + o.v[2]); }
    void operator*=(float s) {
        v[0] *= s;
        v[1] *= s;
        v[2] *= s;
    }

    union {
        double pair[2];
        float v[4];
    };
} __attribute__((aligned(8)));

inline float Dot(const CVector3& a, const CVector3& b) {
    return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
}

class CMatrix {
public:
    void SetPos(CVector3);
    CVector3 GetPos() const { return pos; }

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
};

class ISceneNode;

class CScene {
public:
    void Remove(ISceneNode&);

    unsigned char unknown000[328];
};

extern CScene g_scene;

class CBulletFactory {
public:
    void DestroyBullet(CBullet*);
};

extern CBulletFactory* g_pBulletFactory;

enum EClsnId {};

class ISubject {
public:
    virtual void MarkForDestruction(int);
};

class ISceneNode : public ISubject {
public:
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
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void GetForward(CVector3&) const;
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual EClsnId GetCollisionId() const;
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
};

class CCollision {
public:
    ISceneNode* m_nodeA;
    ISceneNode* m_nodeB;
    unsigned char unknown08[16];
    CVector3* m_normal;
};

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
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
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CVolFiber : public IVolume {
public:
    void Set(CVector3, CVector3);

    unsigned char unknown04[76];
};

/* inferred: a node of the scene's extent lists, its value at +8 */
struct ExtentNodeView {
    unsigned char unknown00[8];
    float m_value;
};

/* inferred: an attached effect whose table is at +28 */
class AttachedEffectView {
    unsigned char unknown00[28];

public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
};

class CBullet : public ISceneNode {
public:
    void Shutdown();

    unsigned char unknown004[8];
    ExtentNodeView* m_extentMinX;
    ExtentNodeView* m_extentMaxX;
    ExtentNodeView* m_extentMinY;
    ExtentNodeView* m_extentMaxY;
    unsigned char unknown01c[8];
    EClsnId m_collisionId;
    unsigned char unknown028[8];
    CVector3 m_position;
    CVector3 m_prevPosition;
    CMatrix m_matrix;
    float m_life;
    float m_value94;
};

class CProjectileBullet : public CBullet {
public:
    virtual void Draw(CDrawContext&);
    virtual EClsnId GetCollisionId() const;
    virtual void MarkForDestruction(int);
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    void Penetrate(const CCollision&);
    void SetPosition(const CVector3& p) { m_matrix.SetPos(p); }
    void Shutdown();

    unsigned char unknown098[28];
    float m_speed;
    float m_value0b8;
    bool m_dead : 1;
    bool m_shutdown : 1;
    unsigned char m_flags0bc : 6;
    unsigned char unknown0bd[3];
    CVolFiber m_fiberA;
    CVolFiber m_fiberB;
    AttachedEffectView* m_effect;
};

extern const float CONTACT_EPSILON;

void CProjectileBullet::Penetrate(const CCollision& collision) {
    const CVector3& normal = *collision.m_normal;
    CVector3 dir;
    GetForward(dir);
    float d = Dot(dir, normal);
    if ((float)fabs(d) > 1e-5f) {
        dir *= CONTACT_EPSILON / (float)fabs(d);
    } else if (d > 0.0f) {
        dir.v[0] = CONTACT_EPSILON * normal.v[0];
        dir.v[1] = CONTACT_EPSILON * normal.v[1];
        dir.v[2] = CONTACT_EPSILON * normal.v[2];
    } else {
        dir.v[0] = -CONTACT_EPSILON * normal.v[0];
        dir.v[1] = -CONTACT_EPSILON * normal.v[1];
        dir.v[2] = -CONTACT_EPSILON * normal.v[2];
    }
    SetPosition(m_matrix.GetPos() + dir);
    m_dead = false;
}

void CProjectileBullet::CommitUpdate() {
    if (m_life <= 0.0f || m_dead) {
        MarkForDestruction(1);
    } else {
        CVector3 pos = m_matrix.pos;
        m_position = pos;
    }
}
