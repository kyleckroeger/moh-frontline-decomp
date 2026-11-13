class CDmaTag;
class CDmaPacket;
class CBullet;
class CCollision;
class CDrawContext;
class CLight;
class CPlayerObject;
class CStaticObject;
class CMatrix;
class CVector3;
struct AIDoodadView;
enum EClsnId {};

class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
    virtual void Destroy();
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual void MarkForDestruction(int);
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

/* ISceneNode's virtuals in table order (slots 20 to 216). They are the weak
   inline defaults of Moh2.cpp in the original; their bodies are not repeated
   here, so they are declared without one. */
class ISceneNode : public IObserver {
public:
    enum EVolumeType {};
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}
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
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
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
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class CRenderBin;

/* CRenderBin's non-polymorphic base, named by CRenderBin's RTTI record; the
   members are inferred (CRenderBin::Init walks children from +8 through the
   siblings at +4, IsUsed tests +16, +20 is the render priority, 3 by
   default). */
struct CRenderBinData {
    CRenderBinData() : data00(0), m_next(0), m_children(0), data0c(0), data10(0), m_priority(3) {}

    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;

    /* Inferred (the name is this project's): a child is linked in after the
       first one. */
    void AddChild(CRenderBin* child);
};

/* CRenderBin's virtuals in table order; all are weak (inline) in the
   original. Init's and IsUsed's bodies are not part of this view, so they
   are declared without one, which also keeps CRenderBin's table out of this
   unit. */
class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed();
};

inline void CRenderBinData::AddChild(CRenderBin* child) {
    CRenderBin* first = m_children;
    if (first) {
        child->m_next = first->m_next;
        if (first->m_next)
            first->m_next->data00 = (int)child;
        child->data00 = (int)first;
        first->m_next = child;
    } else {
        m_children = child;
    }
}

class CUcodeRenderBin : public CRenderBin {
public:
    CUcodeRenderBin() {}
    virtual ~CUcodeRenderBin() {}
    virtual void Link(CDmaTag*, CDmaPacket&);
};


class CRenderList {
public:
    void Register(CRenderBin&);
};

class CScreen;
struct ShapeFile;
struct LevelFileContentsStruct_;
enum TLTResourceID {};

void DebugMsg(const char*, ...);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

struct DecalParam {
    int key;
    union {
        float value;
        const char* name;
    };
};

struct DecalResourceData {
    int data00;
    int m_count;
    DecalParam m_params[6]; /* six pairs per decal type */
};

struct TLTResourceView {
    int data00;
    int data04;
    DecalResourceData* data;
};

TLTResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);

class CTexture {
public:
    void Set(ShapeFile*, bool);

    unsigned char unknown00[64];
};

class CScene {
public:
    void Add(ISceneNode&);

    unsigned char unknown00[0x148];
};

extern CScene g_scene;

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

struct SDecalVertex {
    float x, y, z;
    ColorView color;
    float u, v;
};

class BulletDecal {
public:
    BulletDecal() {
        m_vertices[0].color.Set(0, 0, 0, 128);
        m_vertices[1].color.Set(0, 0, 0, 128);
        m_vertices[2].color.Set(0, 0, 0, 128);
        m_vertices[3].color.Set(0, 0, 0, 128);
    }

    SDecalVertex m_vertices[4];
    BulletDecal* m_next;
    int m_bin;
};

class CBulletDecalTextureBin : public CUcodeRenderBin {
public:
    virtual ~CBulletDecalTextureBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
    CBulletDecalTextureBin();
    void Init(ShapeFile* shape) {
        m_decals = 0;
        m_count = 0;
        m_shape = shape;
        m_texture.Set(m_shape, false);
    }

    int m_count;
    BulletDecal* m_decals;
    CTexture m_texture;
    ShapeFile* m_shape;
};

class CBulletDecalManager : public ISceneNode, private CUcodeRenderBin {
public:
    virtual ~CBulletDecalManager();
    virtual void Draw(CDrawContext&);
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
    CBulletDecalManager();

    void Init__NotTheVirtualOne();
    static void InitClass(CRenderList*, CScreen*);

    CBulletDecalTextureBin m_bins[13];
    BulletDecal* m_decals;
    int m_active;
    TLTResourceView* m_resource;
};

extern CBulletDecalManager g_BulletDecalManager;

namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

static BulletDecal g_DecalList[512];
static CMatrix g_Ident(true);
static CRenderList* g_pRenderList;
static CScreen* g_pScreen;
static CDrawContext* g_pContext;

CBulletDecalTextureBin::CBulletDecalTextureBin() {
    m_decals = 0;
}

CBulletDecalTextureBin::~CBulletDecalTextureBin() {
}

CBulletDecalManager::CBulletDecalManager() {
}

CBulletDecalManager::~CBulletDecalManager() {
}

void CBulletDecalManager::InitClass(CRenderList* list, CScreen* screen) {
    g_pRenderList = list;
    g_pScreen = screen;
}

static inline void InitDecalList() {
    int i;
    for (i = 0; i < 512; i++) {
        BulletDecal& d = g_DecalList[i];
        d.m_vertices[0].u = 0.0f;
        d.m_vertices[0].v = 0.0f;
        d.m_vertices[0].color.Set(255, 255, 255, 255);
        d.m_vertices[1].u = 1.0f;
        d.m_vertices[1].v = 0.0f;
        d.m_vertices[1].color.Set(255, 255, 255, 255);
        d.m_vertices[2].u = 1.0f;
        d.m_vertices[2].v = 1.0f;
        d.m_vertices[2].color.Set(255, 255, 255, 255);
        d.m_vertices[3].u = 0.0f;
        d.m_vertices[3].v = 1.0f;
        d.m_vertices[3].color.Set(255, 255, 255, 255);
    }
}

void CBulletDecalManager::Init__NotTheVirtualOne() {
    TLTResourceView* resource;
    int i;

    m_decals = g_DecalList;
    InitDecalList();
    // g2 (11-13): read back from the member: every callee register as in the original (28 -> 14), but the
    // original copies the call result (mr r31,r3) where this reloads it; from the call result resource ranks lowest.
    m_resource = TLT_FindNextResourceByType((TLTResourceID)20, 0);
    resource = m_resource;
    if (resource->data->m_count > 13) {
        DebugMsg("WARNING:  WARNING:  WARNING:  WARNING:  WARNING:  WARNING:  \n");
        DebugMsg("WARNING:  The engine is only aware of %d number of decals while\n", 13);
        DebugMsg("decal.crp is specifying %d.  That's wasting memory\n", resource->data->m_count);
    } else if (resource->data->m_count < 13) {
        DebugMsg("WARNING:  WARNING:  WARNING:  WARNING:  WARNING:  WARNING:  \n");
        DebugMsg("WARNING:  The engine is aware of %d number of decals while\n", 13);
        DebugMsg("decal.crp is specifying %d.\n", resource->data->m_count);
        DebugMsg("Either specify more in decal.crp or reduce the engine number\n");
    }
    for (i = 0; i < 13; i++) {
        int size;
        const char* name;
        if (i < resource->data->m_count)
            name = ((DecalParam*)resource->data)[(unsigned int)(i * 6 + 1)].name;
        else
            name = resource->data->m_params[0].name;
        m_bins[i].Init((ShapeFile*)TLT_LoadFileFromLevelBigFile(name, &size));
        AddChild(&m_bins[i]);
        resource->data->m_params[i * 6 + 5].value *= 3.14159265f / 180.0;
    }
    m_active = 0;
    g_scene.Add(*this);
    m_priority = 6;
    g_pRenderList->Register(g_BulletDecalManager);
}
