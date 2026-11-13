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

/* ISceneNode's virtuals in table order (slots 20 to 216). They are the weak
   inline defaults of Moh2.cpp in the original; their bodies are not repeated
   here, so they are declared without one. */
#include "game/ISceneNode.h"

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

#include "game/CScene.h"


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

#include "game/CMatrix.h"

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

#include "game/DwiHashMap.h"

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
