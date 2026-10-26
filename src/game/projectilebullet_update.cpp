// A fragment of projectilebullet.cpp (0x800d11d4): CProjectileBullet's
// AttemptUpdate (the value at +148 reset to FLT_MAX, the matrix moved from
// the position along its forward row by speed * time, the local fiber rebuilt
// from (0, -step, 0) to the origin and transformed into the world fiber, whose
// extents, widened by CONTACT_EPSILON, go to the four extent nodes), BeginUpdate (the matrix position copied to the current and previous
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
// elsewhere) first so its table is not emitted here. CProjectileBullet::Init
// follows: after CBullet's it records the type and properties, the life and
// the speed (scaled), a random value (0.2-0.5 in multiplayer, else 0-0.5),
// the position, collision id and owner; it builds the matrix from the
// position and the normalised direction (the other rows from the axis least
// aligned with it), joins the scene, sets up its fibers and excludes
// collisions with player 0's body when the fiber does not already touch it.
// With the tracer cheat it attaches the tracer particle system (slower in
// mission 6 stage 1 outside multiplayer), and it loads the bullet bin's
// texture once from the level's big file. sqrtf is the MSL inline, fabs an
// inline over __fabs; the properties, cheat, shell, player-body, particle
// system and bin views are inferred. The rest of the file is not part of this
// unit.
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
    CVector3(const CVector3& o) {
        pair[0] = o.pair[0];
        pair[1] = o.pair[1];
    }
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
    void Set(float x, float y, float z) {
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }
    void Cross(const CVector3& a, const CVector3& b) {
        v[0] = a.v[1] * b.v[2] - a.v[2] * b.v[1];
        v[1] = a.v[2] * b.v[0] - a.v[0] * b.v[2];
        v[2] = a.v[0] * b.v[1] - a.v[1] * b.v[0];
    }
    float Length() const { return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;
        }
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
    void BuildTrans(CVector3);
    void SetFront(CVector3);
    void SetRight(CVector3);
    void SetUp(CVector3);
    CVector3 GetPos() const { return pos; }
    CVector3 GetFront() const { return front; }

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
};

class ISceneNode;

class CPlayerObject;

class CScene {
public:
    void Remove(ISceneNode&);
    void Add(ISceneNode&);
    CPlayerObject* GetPlayer(int) const;
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);

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
    virtual class IVolume* GetWorldBoundingVolume(int) const;
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
    CCollision();
    ~CCollision();

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
    virtual int TestCollision(const class CVolFiber&, CCollision&, bool) const;
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
    virtual void SetMatrix(const CMatrix&);
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void SetVelocity(CVector3);
};

class CParticleSystem {
public:
    static AttachedEffectView* Create(unsigned long);
};

/* inferred: a player object's body node at +704 */
class CPlayerObject {
public:
    unsigned char unknown000[704];
    ISceneNode m_body;
};

enum EProjectileBulletTypes {};

/* inferred: the bullet type's properties (speed at +0, life at +52) */
struct ProjectileBulletProperties_struct {
    float speed;
    unsigned char unknown04[48];
    short life;
};

class CShellMenu {
public:
    unsigned int Get_currentMission();
    unsigned int Get_currentStage();

    unsigned char unknown0000[0x19dc];
};

extern CShellMenu g_Shell;

/* inferred: the cheat flags */
struct CheatsView {
    unsigned char unknown00[11];
    bool m_tracers;
};

extern CheatsView g_cheats;
extern bool g_bInMultiplayerMode;
float MathFunRandomReal(float, float);

class ShapeFile;
class CTexture {
public:
    void Set(ShapeFile*, bool);
};

/* inferred: the bullet render bin: its texture loaded once */
struct BulletBinView {
    unsigned char unknown00[32];
    bool m_loaded;
    unsigned char unknown21[3];
    CTexture m_texture;
};

extern BulletBinView g_BulletBin;

enum TLTResourceID {};
struct LevelFileInfo {
    unsigned char unknown00[4];
    const char* m_name;
};
struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    LevelFileInfo* m_info;
};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
ShapeFile* TLT_LoadFileFromLevelBigFile(const char*, int*);

class CBullet : public ISceneNode {
public:
    void Shutdown();
    void Init();

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
    void Init(EProjectileBulletTypes, ProjectileBulletProperties_struct*, const CVector3&, const CVector3&, float, EClsnId, float,
              ISceneNode*);

    unsigned char unknown098[12];
    EProjectileBulletTypes m_type;
    ProjectileBulletProperties_struct* m_properties;
    ISceneNode* m_owner;
    int m_value0b0;
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

void CProjectileBullet::AttemptUpdate(float dt) {
    m_value94 = 3.4028235e+38f;
    float step = m_speed * dt;
    const CVector3& dir = m_matrix.GetFront();
    float dx = step * dir.v[0];
    float dy = step * dir.v[1];
    float dz = step * dir.v[2];
    CVector3 newPos(m_position.v[0] + dx, m_position.v[1] + dy, m_position.v[2] + dz);
    m_matrix.SetPos(newPos);
    CVector3 start(0.0f, -step, 0.0f);
    m_fiberA.Set(start, CVector3(0.0f, 0.0f, 0.0f));
    m_fiberB.TransformedCopy(m_fiberA, m_matrix);
    CVector3 low;
    CVector3 high;
    m_fiberB.GetExtents(low, high);
    m_extentMinX->m_value = low.v[0] - CONTACT_EPSILON;
    m_extentMinY->m_value = low.v[1] - CONTACT_EPSILON;
    m_extentMaxX->m_value = CONTACT_EPSILON + high.v[0];
    m_extentMaxY->m_value = CONTACT_EPSILON + high.v[1];
}

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

inline double fabs(double x) { return __fabs(x); }
void CProjectileBullet::Init(EProjectileBulletTypes type, ProjectileBulletProperties_struct* properties, const CVector3& position,
                             const CVector3& direction, float speed, EClsnId id, float, ISceneNode* owner) {
    CBullet::Init();
    m_type = type;
    m_properties = properties;
    m_life = properties->life;
    m_speed = speed * m_properties->speed;
    if (g_bInMultiplayerMode)
        m_value0b8 = MathFunRandomReal(0.2f, 0.5f);
    else
        m_value0b8 = MathFunRandomReal(0.0f, 0.5f);
    m_position = position;
    m_prevPosition = position;
    m_collisionId = id;
    m_owner = owner;
    m_value0b0 = 0;
    m_dead = false;
    m_shutdown = false;
    m_effect = 0;
    m_matrix.BuildTrans(position);
    CVector3 front = direction;
    front.Normalize();
    CVector3 right;
    CVector3 up;
    if (fabs(front.v[2]) < fabs(front.v[0])) {
        up.Set(0.0f, 0.0f, 1.0f);
        right.Cross(front, up);
        up.Cross(right, front);
    } else {
        if (front.v[1] < 0.0f)
            right.Set(-1.0f, 0.0f, 0.0f);
        else
            right.Set(1.0f, 0.0f, 0.0f);
        up.Cross(right, front);
        right.Cross(front, up);
    }
    m_matrix.SetFront(front);
    m_matrix.SetRight(right);
    m_matrix.SetUp(up);
    g_scene.Add(*this);
    m_fiberA.Set(CVector3(0.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, 0.0f));
    CPlayerObject* player = g_scene.GetPlayer(0);
    m_fiberB.TransformedCopy(m_fiberA, m_matrix);
    ISceneNode* body = &player->m_body;
    IVolume* volume = body->GetWorldBoundingVolume(0);
    CCollision collision;
    if (volume->TestCollision(m_fiberB, collision, false) != 1)
        g_scene.ExcludeCollision(*this, *body, -1);
    if (g_cheats.m_tracers) {
        float factor;
        if (g_Shell.Get_currentMission() == 6 && g_Shell.Get_currentStage() == 1 && !g_bInMultiplayerMode)
            factor = 0.85f;
        else
            factor = 0.3f;
        m_effect = CParticleSystem::Create(0x6cdc8936);
        if (m_effect) {
            m_speed *= factor;
            CVector3 velocity = m_matrix.front;
            velocity *= 60.0f * m_speed;
            m_effect->SetVelocity(velocity);
            m_effect->SetMatrix(m_matrix);
        }
    }
    if (!g_BulletBin.m_loaded) {
        g_BulletBin.m_loaded = true;
        LevelFileContentsStruct_* entry = TLT_FindNextResourceByType((TLTResourceID)13, 0);
        g_BulletBin.m_texture.Set(TLT_LoadFileFromLevelBigFile(entry->m_info->m_name, 0), false);
    }
}
