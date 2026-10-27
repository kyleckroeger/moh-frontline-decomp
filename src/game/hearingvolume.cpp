// CHearingVolume, the functions at the start of the file: the weak collision
// id accessors and the bullet whiz-by sound, played at the bullet's position
// and heading for the scene's first player. The class names come from the
// mangled symbols. CHearingVolume is an inferred, non-virtual view (its
// virtual table is not reproduced); CBullet is declared with the ISceneNode
// virtual functions in the order of __vt__10ISceneNode, flattened, and
// CVector3 is the game's 16-byte, 8-aligned vector (copied by value).
// HandleBulletCollision follows: normally, for a projectile bullet, it calls
// the volume's handler (+196) with its owner (+36), excludes the bullet from
// the volume and lets it pass through (the collision pair is excluded from
// the scene); with the bullet-shield cheat it instead creates a spark effect
// oriented along the bullet's direction reflected about the contact normal
// (a frame built against the function-static vRight, or against z when the
// reflection is nearly along it) at the contact position, and then calls the
// handler. The CBullet slot names, the contact, cheat and effect views and
// the reflection helpers are inferred; the effect id is from the code.
class CDrawContext;
class CCollision;
class CMatrix;
class CPlayerObject;
class IMovingSceneNode;

enum EClsnId {};

extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

inline double fabs(double x) { return __fabs(x); }

class CVector3 {
public:
    CVector3() {}
    CVector3(float nx, float ny, float nz) {
        x = nx;
        y = ny;
        z = nz;
    }
    float Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    void Cross(const CVector3& a, const CVector3& b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;
    void Ident();
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    void Orthonormalize();

    float m[4][4];
} __attribute__((aligned(16)));

class ISceneNode;

class CBullet {
public:
    enum EVolumeType {};

    virtual void MarkForDestruction(int);
    virtual ~CBullet();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual const void* GetLocalBoundingVolume(EVolumeType) const;
    virtual const void* GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
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
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual void* AsProjectile();
    virtual void unknown144();
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

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);

    char data[328];
};

extern CScene g_scene;

void PlayBulletWhiz(CVector3, CVector3, CPlayerObject*);

class CHearingVolume {
public:
    EClsnId GetCollisionId() const;
    void SetCollisionId(EClsnId);
    void HandleBulletCollision(CBullet*, const CCollision&);

    unsigned char unknown000[36];
    IMovingSceneNode* m_owner;
    unsigned char unknown028[152];
    EClsnId m_collisionId;
    void (*m_onBullet)(IMovingSceneNode*, CBullet*);
};

__declspec(weak) EClsnId CHearingVolume::GetCollisionId() const {
    return m_collisionId;
}

__declspec(weak) void CHearingVolume::SetCollisionId(EClsnId id) {
    m_collisionId = id;
}

void PlayBulletWhizBys(IMovingSceneNode*, CBullet* bullet) {
    CVector3 position;
    CVector3 direction;

    bullet->GetPosition(position);
    bullet->GetForward(direction);
    PlayBulletWhiz(position, direction, g_scene.GetPlayer(0));
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
            m_onBullet(m_owner, bullet);
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
    m_onBullet(m_owner, bullet);
}
