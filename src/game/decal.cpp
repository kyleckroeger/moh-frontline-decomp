// The end of decal.cpp (0x80083ec0): the static initialisation of the file's
// decal list (512 bullet decals built through their constructor by
// __construct_array) and of its identity matrix (the inline constructor
// initialises the matrix class once, then the identity is set), followed by
// the weak BulletDecal constructor emitted after it, which sets the colour of
// each of the decal's four vertices to (0, 0, 0, 128), and the weak
// this-adjusting thunks of CBulletDecalManager's table for its render-bin base
// at +36 (IsUsed, Link, destructor). The rest of the file is in other units.
// BulletDecal, g_DecalList and g_Ident are named by the symbols; the object
// sizes come from the symbols, the vertex layout, its colour view and the
// identity-matrix constructor are inferred. The manager's virtual functions
// are named by its table; their declaration order (IsUsed first, as the key
// function that emits the table here) is inferred, and IsUsed itself is placed
// by decal_link.cpp (a file duplicate, like the table).
/* Inferred: a plain colour (the vertex is constructor-less). */
struct ColorView {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    void Set(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) {
        r = ar;
        g = ag;
        b = ab;
        a = aa;
    }
};

class CMatrix {
public:
    /* Inferred: a constructor that also sets the identity. */
    CMatrix(bool identity) {
        if (!s_ClassInit)
            InitClass();
        if (identity)
            Ident();
    }
    static void InitClass();
    void Ident();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

/* Inferred: a decal vertex (position, colour, texture coordinates). */
struct DecalVertexView {
    float position[3];
    ColorView color;
    float uv[2];
};

class BulletDecal {
public:
    BulletDecal() {
        m_verts[0].color.Set(0, 0, 0, 128);
        m_verts[1].color.Set(0, 0, 0, 128);
        m_verts[2].color.Set(0, 0, 0, 128);
        m_verts[3].color.Set(0, 0, 0, 128);
    }

    DecalVertexView m_verts[4];
    unsigned char unknown60[8];
};

static BulletDecal g_DecalList[512];
static CMatrix g_Ident(true);

class CDmaTag;
class CDmaPacket;
class CDrawContext;
class CCollision;
class CBullet;
class CLight;
class CVector3;
struct BSObject;
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

class ISceneNode : public IObserver {
public:
    virtual ~ISceneNode();
    enum EVolumeType {};
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
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual void* AsPlayerObject();
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
    virtual void* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;

    int data04[8];
};

class CRenderBin {
public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    int data04[7];
};

class CUcodeRenderBin : public CRenderBin {
public:
    virtual ~CUcodeRenderBin();
};

/* Inferred: only the bases and the member IsUsed reads. */
class CBulletDecalManager : public ISceneNode, public CUcodeRenderBin {
public:
    virtual bool IsUsed();
    virtual ~CBulletDecalManager();
    virtual void Draw(CDrawContext&);
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual void Link(CDmaTag*, CDmaPacket&);

    unsigned char unknown44[1408];
    int m_active;
};

bool CBulletDecalManager::IsUsed() {
    return m_active > 0;
}
