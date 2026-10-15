// A fragment of projectilebullet.cpp (0x800d1340): CProjectileBullet's
// BeginUpdate (the matrix position copied to the current and previous
// positions, the value at +148 reset to FLT_MAX, and the life at +144 counted
// down to zero), OnCollision (marks the bullet dead and lets the other node
// handle the hit), Shutdown (sets the shut-down flag, then CBullet's),
// Destroy (returns the bullet to g_pBulletFactory) and MarkForDestruction
// (ISubject's, then marks it dead, removes it from g_scene, clears the
// collision id and releases the attached effect at +352 through its slot
// +28). The classes, functions and globals are named by the mangled symbols;
// the members, flag names and the extent, effect and collision views are
// inferred, and only the virtual slots these functions use are named
// (__vt__10ISceneNode, __vt__7IVolume); CVector3 is the double-pair view of
// projectilebullet_ctor.cpp. CProjectileBullet declares Draw (defined
// elsewhere) first so its table is not emitted here. The rest of the file is
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

void CProjectileBullet::BeginUpdate(float dt) {
    CVector3 pos = m_matrix.pos;
    m_position = pos;
    m_prevPosition = m_position;
    m_value94 = 3.4028235e+38f;
    if (m_life > 0.0f) {
        m_life -= dt;
        if (m_life < 0.0f)
            m_life = 0.0f;
    }
}

void CProjectileBullet::OnCollision(const CCollision& collision) {
    ISceneNode* other = collision.m_nodeA;
    if (other == this)
        other = collision.m_nodeB;
    m_dead = true;
    other->HandleBulletCollision(this, collision);
}

void CProjectileBullet::Shutdown() {
    m_shutdown = true;
    CBullet::Shutdown();
}

void CProjectileBullet::Destroy() {
    g_pBulletFactory->DestroyBullet(this);
}

void CProjectileBullet::MarkForDestruction(int flag) {
    ISubject::MarkForDestruction(flag);
    m_dead = true;
    g_scene.Remove(*this);
    m_collisionId = (EClsnId)-1;
    if (m_effect) {
        m_effect->unknown01c();
        m_effect = 0;
    }
}
