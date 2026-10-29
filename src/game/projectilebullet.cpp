// projectilebullet.cpp from CProjectileBulletRenderBin::Render (0x800d0b3c) to
// the end of the file. The render bin's destructor and Link before Render are
// separate units (projectilebullet_dtor, projectilebullet_link).
//
// CProjectileBulletRenderBin::Render: the first time, the camera's rightward
// and upward axes are cached in the file's g_XAxis and g_YAxis; the tracer's
// segment and width go into the packet as one STracer (40 bytes) and the
// camera matrix is kept in g_WorldToCamera.
// CProjectileBullet's GetVelocity (the direction at +96 scaled by the speed at
// +180), SetExclusionPair (re-includes the collision with the previous
// excluded node in g_scene, then stores the new one), GetScriptBulletType
// (maps the projectile type at +164 to the script's bullet type, with a
// warning for an unknown type) and IsVisible (the second fiber volume's
// visibility test).
// Draw (nothing when the cheat flag at g_cheats+11 is set; in multiplayer a
// player's bullet is hidden from the other viewports while it is younger than
// MP_PROJBULLET_DRAW_DELAY; nothing for a zero width; otherwise a tracer from
// the previous position moved along the world fiber's line by the offset at
// +184 to that point plus the velocity, scaled by 0.09 * (width - 0.5) +
// 0.455, goes to g_pRenderList with g_BulletBin, the draw context kept in
// g_pContext). GetCollisionId.
// Penetrate pushes the bullet through a surface: its forward direction scaled
// so its component along the collision normal is CONTACT_EPSILON (or, when
// nearly parallel, the normal times plus or minus CONTACT_EPSILON by the
// sign) is added to the matrix position, and the dead flag is cleared.
// CommitUpdate marks the bullet for destruction when its life (+144) is used
// up or it is dead, else copies the matrix position to the current position.
// AttemptUpdate (the value at +148 reset to FLT_MAX, the matrix moved from
// the position along its forward row by speed * time, the local fiber rebuilt
// from (0, -step, 0) to the origin and transformed into the world fiber, whose
// extents, widened by CONTACT_EPSILON, go to the four extent nodes),
// BeginUpdate (the matrix position copied to the current and previous
// positions, the value at +148 reset to FLT_MAX, and the life at +144 counted
// down to zero), OnCollision (marks the bullet dead and lets the other node
// handle the hit), Shutdown (sets the shut-down flag, then CBullet's),
// Destroy (returns the bullet to g_pBulletFactory) and MarkForDestruction
// (ISubject's, then marks it dead, removes it from g_scene, clears the
// collision id and releases the attached effect at +352 through its slot
// +28). Init: after CBullet's it records the type and properties, the life and
// the speed (scaled), a random value (0.2-0.5 in multiplayer, else 0-0.5),
// the position, collision id and owner; it builds the matrix from the
// position and the normalised direction (the other rows from the axis least
// aligned with it), joins the scene, sets up its fibers and excludes
// collisions with player 0's body when the fiber does not already touch it.
// With the tracer cheat it attaches the tracer particle system (slower in
// mission 6 stage 1 outside multiplayer), and it loads the bullet bin's
// texture once from the level's big file.
// InitClass (keeps the render list and screen in the file's statics and
// registers the bullet render bin with the list), the destructor (the two
// fiber volumes destroyed in reverse order, then CBullet's) and the default
// constructor (CBullet's constructor, the two fibers through their inline
// constructors, and the word at +40 set to 1), operator new[] (DWI_alloc),
// the weak Init, bounding-volume, damage, firer and cast accessors, and the
// static initialiser: the file's camera matrix (its inline constructor
// initialises the matrix class once) and the tracer render bin (built through
// the inline CRenderBinData/CRenderBin constructors with priority 7, its flag
// cleared, and its destructor registered).
//
// The classes, functions and globals are named by the mangled symbols and the
// virtual tables (slot names from __vt__17CProjectileBullet and
// __vt__7CCamera); the members, their names, the views (cheats, shell,
// player, draw context, properties, extent nodes, attached effect) and the
// type numbers are inferred. CVector3 is a double-pair view (copies move
// doubleword pairs). The accessors are inline in the original (weak
// symbols); they are defined __declspec(weak) in the original text order.
#include <math.h>

class CDrawContext;
class CBullet;
class CCollision;
class CLine3;
class CScreen;
class CDmaTag;
class CPlayerObject;
class CLight;
class BPDLightVolume;

void DebugMsg(const char*, ...);
void* DWI_alloc(const char*, int, int);

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
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;

    CMatrix& operator=(const CMatrix&);
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

class CLine3 {
public:
    CVector3 GetDirection() const { return m_dir; }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
};

class ISceneNode;

class CScene {
public:
    void Remove(ISceneNode&);
    void Add(ISceneNode&);
    CPlayerObject* GetPlayer(int) const;
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);
    void IncludeCollision(const ISceneNode&, const ISceneNode&);

    unsigned char unknown000[328];
};

extern CScene g_scene;

class CBulletFactory {
public:
    void DestroyBullet(CBullet*);
};

extern CBulletFactory* g_pBulletFactory;

enum EClsnId {};

class IDestructible {
public:
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
    virtual void Destroy();
};

class ISubject : public IDestructible {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    virtual ~IObserver();
};

class IVolume;
class CScriptObject;
class CAIDoodad;

class ISceneNode : public IObserver {
public:
    enum EVolumeType {};

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
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual CScriptObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual ISceneNode* AsMovingNode();
    virtual const ISceneNode* AsMovingNode() const;
    virtual ISceneNode* AsStaticObject();
    virtual const ISceneNode* AsStaticObject() const;
    virtual ISceneNode* AsHierObject();
    virtual const ISceneNode* AsHierObject() const;
    virtual ISceneNode* AsWorldObject();
    virtual const ISceneNode* AsWorldObject() const;
    virtual ISceneNode* AsAnimObject();
    virtual const ISceneNode* AsAnimObject() const;
    virtual ISceneNode* AsSoldierObject();
    virtual const ISceneNode* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
    virtual const CPlayerObject* AsPlayerObject() const;
    virtual ISceneNode* AsPlayerWeaponObject();
    virtual const ISceneNode* AsPlayerWeaponObject() const;
    virtual ISceneNode* AsAnimatedPlayerObject();
    virtual const ISceneNode* AsAnimatedPlayerObject() const;
    virtual ISceneNode* AsLight();
    virtual const ISceneNode* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual ISceneNode* AsCollisionVolume();
    virtual const ISceneNode* AsCollisionVolume() const;
    virtual CAIDoodad* GetAIDoodad();
    virtual const CAIDoodad* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class IMovingSceneNode : public ISceneNode {
public:
    virtual ISceneNode* AsMovingNode();
    virtual const ISceneNode* AsMovingNode() const;
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
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
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual BPDLightVolume* GetLightVolume();
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
    IVolume() {}
    virtual ~IVolume() {}
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
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CVolFiber : public IVolume {
public:
    CVolFiber() {}
    virtual IVolume* Create() const;
    virtual ~CVolFiber();
    void Set(CVector3, CVector3);
    const CLine3& GetLine() const;

    unsigned char unknown04[12];
    CLine3 m_line;
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

/* inferred: a player object's body node at +704 and its viewport at +1896 */
class CPlayerObject {
public:
    int GetViewport() const { return m_viewport; }

    unsigned char unknown000[704];
    ISceneNode m_body;
    unsigned char unknown2c4[1188];
    int m_viewport;
};

/* inferred: the draw context's camera matrix (+196), camera (+212) and
   viewport (+276) */
class CDrawContext {
public:
    int GetViewport() const { return m_viewport; }

    unsigned char unknown000[196];
    CMatrix* m_worldToCamera;
    unsigned char unknown0c8[12];
    ISceneNode* m_camera;
    unsigned char unknown0d8[60];
    int m_viewport;
};

enum EProjectileBulletTypes {};

/* inferred: the bullet type's properties (speed at +0, damage at +48, life at
   +52, tracer width at +56) */
struct ProjectileBulletProperties_struct {
    float speed;
    unsigned char unknown04[44];
    float damage;
    short life;
    unsigned char unknown36[2];
    float width;
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
    bool flag0b;
    unsigned char unknown0c[8];
};

extern CheatsView g_cheats;
extern bool g_bInMultiplayerMode;
extern float MP_PROJBULLET_DRAW_DELAY;
float MathFunRandomReal(float, float);

class ShapeFile;
class CTexture {
public:
    void Set(ShapeFile*, bool);

    int unknown00[16];
};

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

/* Inferred: the packet's write cursor at +8. */
class CDmaPacket {
public:
    unsigned char unknown00[8];
    unsigned char* m_cur;
};

/* Inferred: a tracer's segment and width (40 bytes, added to the packet). */
struct STracer {
    CVector3 m_start;
    float m_width;
    CVector3 m_end;
};

/* inferred: the tracer handed from Draw to the render bin */
struct TracerData {
    CVector3 start;
    CVector3 unknown10;
    CVector3 end;
    CVector3 unknown30;
    float width;
};

class CRenderBinData {
public:
    CRenderBinData() : m_field0(0), m_field4(0), m_field8(0), m_fieldC(0), m_field10(0), m_priority(3) {}

    int m_field0;
    int m_field4;
    int m_field8;
    int m_fieldC;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CProjectileBulletRenderBin : public CRenderBin {
public:
    virtual ~CProjectileBulletRenderBin();
    virtual int Render(CDmaPacket&, void*);
    CProjectileBulletRenderBin() : m_loaded(false) { m_priority = 7; }

    bool m_loaded;
    CTexture m_texture;
};

class CRenderList {
public:
    void Register(CRenderBin&);
    void Render(CRenderBin&, void*);
};

class CProjectileBullet;

class CBullet : public IMovingSceneNode {
public:
    CBullet();
    virtual ~CBullet();
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
    virtual CBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&);
    virtual void SetExclusionPair(ISceneNode*);
    virtual void Penetrate(const CCollision&);

    unsigned char unknown004[8];
    ExtentNodeView* m_extentMinX;
    ExtentNodeView* m_extentMaxX;
    ExtentNodeView* m_extentMinY;
    ExtentNodeView* m_extentMaxY;
    unsigned char unknown01c[8];
    EClsnId m_collisionId;
    int m_data28;
    unsigned char unknown02c[4];
    CVector3 m_position;
    CVector3 m_prevPosition;
    CMatrix m_matrix;
    float m_life;
    float m_value94;
};

class CProjectileBullet : public CBullet {
public:
    static void InitClass(CRenderList*, CScreen*);
    static void* operator new[](unsigned long);
    CProjectileBullet();
    virtual void MarkForDestruction(int);
    virtual ~CProjectileBullet();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual EClsnId GetCollisionId() const;
    virtual void Init();
    virtual void Shutdown();
    virtual float GetDamage() const;
    virtual int GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&);
    virtual void SetExclusionPair(ISceneNode*);
    virtual void Penetrate(const CCollision&);
    void Init(EProjectileBulletTypes, ProjectileBulletProperties_struct*, const CVector3&, const CVector3&, float, EClsnId, float,
              ISceneNode*);
    void SetPosition(const CVector3& p) { m_matrix.SetPos(p); }

    unsigned char unknown098[12];
    EProjectileBulletTypes m_type;
    ProjectileBulletProperties_struct* m_properties;
    ISceneNode* m_owner;
    ISceneNode* m_excluded;
    float m_speed;
    float m_offset;
    bool m_dead : 1;
    bool m_shutdown : 1;
    unsigned char m_flags0bc : 6;
    unsigned char unknown0bd[3];
    CVolFiber m_fiberA;
    CVolFiber m_fiberB;
    AttachedEffectView* m_effect;
};

extern const float CONTACT_EPSILON;

static CMatrix g_WorldToCamera;
static CProjectileBulletRenderBin g_BulletBin;
static CVector3 g_XAxis;
static CVector3 g_YAxis;
static CRenderList* g_pRenderList;
static CScreen* g_pScreen;
static CDrawContext* g_pContext;
static bool g_bCached;

int CProjectileBulletRenderBin::Render(CDmaPacket& packet, void* data) {
    TracerData* tracer = (TracerData*)data;
    if (!g_bCached) {
        g_bCached = true;
        g_pContext->m_camera->GetRightward(g_XAxis);
        g_pContext->m_camera->GetUpward(g_YAxis);
    }
    STracer copy;
    copy.m_start = tracer->start;
    copy.m_end = tracer->end;
    copy.m_width = tracer->width;
    g_WorldToCamera = *g_pContext->m_worldToCamera;
    STracer* out = (STracer*)packet.m_cur;
    out->m_start = copy.m_start;
    out->m_end = copy.m_end;
    out->m_width = copy.m_width;
    packet.m_cur += sizeof(STracer);
    return 0;
}

void CProjectileBullet::GetVelocity(CVector3& velocity) {
    CVector3 direction = m_matrix.front;
    float speed = m_speed;
    velocity.v[0] = speed * direction.v[0];
    velocity.v[1] = speed * direction.v[1];
    velocity.v[2] = speed * direction.v[2];
}

void CProjectileBullet::SetExclusionPair(ISceneNode* node) {
    if (m_excluded)
        g_scene.IncludeCollision(*m_excluded, *this);
    m_excluded = node;
}

int CProjectileBullet::GetScriptBulletType() const {
    switch (m_type) {
    case 0:
    case 1:
        return 10;
    case 2:
    case 3:
        return 0;
    case 4:
    case 5:
        return 4;
    case 6:
        return 12;
    case 7:
    case 8:
        return 13;
    case 9:
        return 11;
    case 10:
    case 13:
        return 5;
    case 11:
        return 14;
    case 12:
        return 15;
    case 14:
    case 15:
        return 6;
    case 16:
    case 17:
        return 1;
    case 18:
    case 19:
        return 16;
    case 20:
    case 21:
        return 19;
    case 22:
    case 23:
        return 20;
    case 24:
    case 25:
        return 2;
    case 26:
    case 27:
        return 18;
    case 28:
        return 17;
    case 29:
        return 27;
    default:
        DebugMsg("WARNING: Unknown proj bullet type in GetScriptBulletType.  Type %d\n", m_type);
        return 4;
    }
}

bool CProjectileBullet::IsVisible(CDrawContext& context) const {
    return m_fiberB.TestVisibility(context);
}

void CProjectileBullet::Draw(CDrawContext& context) {
    if (g_cheats.flag0b)
        return;
    if (g_bInMultiplayerMode && m_owner && m_owner->AsPlayerObject() &&
        context.GetViewport() != m_owner->AsPlayerObject()->GetViewport() &&
        m_life > (float)m_properties->life - MP_PROJBULLET_DRAW_DELAY)
        return;
    if (m_properties->width == 0.0f)
        return;
    TracerData data;
    CVector3 velocity;
    GetVelocity(velocity);
    velocity *= 0.09f * (m_properties->width - 0.5f) + 0.455f;
    float offset = m_offset;
    const CVector3& dir = m_fiberB.GetLine().GetDirection();
    float x = offset * dir.v[0];
    float y = offset * dir.v[1];
    float z = offset * dir.v[2];
    CVector3 start(m_prevPosition.v[0] + x, m_prevPosition.v[1] + y, m_prevPosition.v[2] + z);
    CVector3 end(start.v[0] + velocity.v[0], start.v[1] + velocity.v[1], start.v[2] + velocity.v[2]);
    data.width = m_properties->width;
    data.start = start;
    data.end = end;
    g_pContext = &context;
    g_pRenderList->Render(g_BulletBin, &data);
}

EClsnId CProjectileBullet::GetCollisionId() const {
    return m_collisionId;
}

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

#undef fabs
inline double fabs(double x) { return __fabs(x); }
void CProjectileBullet::Init(EProjectileBulletTypes type, ProjectileBulletProperties_struct* properties, const CVector3& position,
                             const CVector3& direction, float speed, EClsnId id, float, ISceneNode* owner) {
    CBullet::Init();
    m_type = type;
    m_properties = properties;
    m_life = properties->life;
    m_speed = speed * m_properties->speed;
    if (g_bInMultiplayerMode)
        m_offset = MathFunRandomReal(0.2f, 0.5f);
    else
        m_offset = MathFunRandomReal(0.0f, 0.5f);
    m_position = position;
    m_prevPosition = position;
    m_collisionId = id;
    m_owner = owner;
    m_excluded = 0;
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
    IVolume* volume = body->GetWorldBoundingVolume((EVolumeType)0);
    CCollision collision;
    if (volume->TestCollision(m_fiberB, collision, false) != 1)
        g_scene.ExcludeCollision(*this, *body, -1);
    if (g_cheats.flag0b) {
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

void CProjectileBullet::InitClass(CRenderList* list, CScreen* screen) {
    g_pRenderList = list;
    g_pScreen = screen;
    list->Register(g_BulletBin);
}

CProjectileBullet::~CProjectileBullet() {
}

CProjectileBullet::CProjectileBullet() {
    m_data28 = 1;
}

void* CProjectileBullet::operator new[](unsigned long size) {
    return DWI_alloc(0, size, 1024);
}

__declspec(weak) void CProjectileBullet::Init() {
    CBullet::Init();
}

__declspec(weak) IVolume* CProjectileBullet::GetLocalBoundingVolume(EVolumeType) const {
    return (IVolume*)&m_fiberA;
}

__declspec(weak) IVolume* CProjectileBullet::GetWorldBoundingVolume(EVolumeType) const {
    return (IVolume*)&m_fiberB;
}

__declspec(weak) float CProjectileBullet::GetDamage() const {
    return m_properties->damage;
}

__declspec(weak) ISceneNode* CProjectileBullet::GetFiredBy() const {
    return m_owner;
}

__declspec(weak) CProjectileBullet* CProjectileBullet::AsProjectile() {
    return this;
}
