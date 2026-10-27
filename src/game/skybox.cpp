// skybox.cpp (0x800a8de4), the whole file. CSkyBox's weak visibility and
// draw-enabled defaults and its vertex constructor; CSkyBox::Draw (the
// projection matrix kept for the bins; with the configuration's sky-box flag
// set, screen clearing on and the six face bins disabled; otherwise screen
// clearing off, the box's matrix is the view matrix without its translation,
// combined with the projection, the eight corners are transformed, and each
// face whose normal points along the camera's forward vector gets its four
// corners and texture coordinates and is rendered through its bin, the others
// disabled); SetTextures (the level's sky-box resource (type 13) names six
// shape files, which are loaded from the level's big file and set as the
// faces' textures; a missing resource is reported through DebugMsg); the
// destructor (the six files closed); the weak, empty CTexture default
// constructor; DestroyInstance, GetInstance, CreateInstance and InitClass
// (the singleton, screen clearing and the face bins; the ready flag is a
// guarded function-local static, never set, as in the image); the static
// initialisation (the projection matrix, the eight corner vertices of a +-1
// cube, the six face normals and the six face bins, whose array destructor is
// registered); and CSkyBoxRenderBin's weak destructor, constructor, Render
// (the sky box given as render data and the face added to the packet), Link
// (for a non-empty tag list: the GX vertex format and TEV state for one
// textured, unlit stage blended over the frame, then each tagged face drawn
// as a triangle fan of white vertices with the box's matrix and the face's
// texture) and IsUsed. The classes, functions and globals are named by the
// symbols (CRenderBinData by CRenderBin's RTTI record); the members, the
// resource record, the tag data and the views are inferred. The file is
// compiled with RTTI, as its RTTI records show.
#include <dolphin/gx.h>

enum EClsnId {};
class CCollision;
class CBullet;
class CLight;
class CPlayerObject;
class CSoldierObject;
struct AIDoodadView;
class CDmaTag;
class CDmaPacket;
class CRenderList;
class CRenderBin;
class CVector3;
class CMatrix;
struct ShapeFile;
struct LevelFileContentsStruct_;
enum TLTResourceID {};

void* operator new(unsigned long);
void DebugMsg(const char*, ...);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);
void TLT_CloseFile(void*);

struct CVector3Pair {
    double d[2];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3(const CVector3& o) { *(CVector3Pair*)this = *(const CVector3Pair*)&o; }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CVector4 {
public:
    float x, y, z, w;
};

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    CMatrix& operator=(const CMatrix&);
    void SetPos(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);
    void Transform(CVector4&, CVector3) const;
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

/* Inferred: the packet's write cursor at +8. */
class CDmaPacket {
public:
    void Add(unsigned int word) {
        *m_cur = word;
        m_cur++;
    }

    unsigned char unknown00[8];
    unsigned int* m_cur;
};

class CTexture {
public:
    CTexture();
    void Set(ShapeFile*, bool);
    void Load(CDmaPacket&, CRenderList*);

    unsigned char unknown00[64];
};

class CScreen {
public:
    virtual ~CScreen();
    virtual void SetCurrent();
    virtual void Flip();
    virtual void Wait();
    virtual void SetClear(bool);
};

/* inferred: the camera interface (its forward vector at slot 0x48) */
class CameraView {
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
    virtual void GetForward(CVector3&) const;
};

/* inferred: the draw context's view matrices and camera */
class CDrawContext {
public:
    unsigned char unknown000[196];
    CMatrix* m_worldToView;
    CMatrix* m_viewToScreen;
    unsigned char unknown0cc[8];
    CameraView* m_camera;
};

/* inferred: the configuration flag at +7 that turns the sky box off */
struct ConfigView {
    unsigned char unknown00[7];
    bool noSkyBox;
    unsigned char unknown08[12];
};
extern ConfigView g_config;

/* Inferred: the sky-box resource's six face entries (8 bytes each, the shape
   file name at +4). */
struct SSkyBoxFace {
    int unknown00;
    const char* m_name;
};

struct SSkyBoxResource {
    unsigned char unknown00[8];
    SSkyBoxFace m_faces[6];
};

struct TLTResourceView {
    unsigned char unknown00[8];
    SSkyBoxResource* data;
};

TLTResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);

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
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual int GetScriptObject() const;
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
    virtual CSoldierObject* AsSoldierObject();
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

class CSkyBox : public ISceneNode {
public:
    struct SVertex {
        SVertex();

        float x, y, z;
        float unknown0c;
        float u, v;
        float unknown18[2];
    } __attribute__((aligned(16)));

    CSkyBox() : data24(0), m_drawEnabled(true) {
        for (int i = 0; i < 6; i++)
            m_shapes[i] = 0;
    }
    virtual ~CSkyBox();
    virtual void Draw(CDrawContext&);
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    void SetTextures();
    static void DestroyInstance();
    static CSkyBox* GetInstance();
    static CSkyBox* CreateInstance();
    static void InitClass(CRenderList*, CScreen*);

    int data24;
    CTexture m_textures[6];
    ShapeFile* m_shapes[6];
    bool m_drawEnabled;
    SVertex m_vertices[6][10];
    int m_vertexCounts[6];
    unsigned char unknown948[8];
    CMatrix m_matrix;
    CMatrix m_matrix2;

    static CRenderList* sm_pRenderList;
    static CScreen* sm_pScreen;
    static CSkyBox* sm_pSingleton;
};

CRenderList* CSkyBox::sm_pRenderList;
CScreen* CSkyBox::sm_pScreen;
CSkyBox* CSkyBox::sm_pSingleton;

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
};

/* CRenderBin's virtuals in table order; all are weak (inline) in the
   original. */
class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed();
};

/* The face bins (44 bytes): the kept sky box, the face and the enabled flag. */
class CRenderList {
public:
    void Register(CRenderBin&);
    void Render(CRenderBin&, void*);
};

/* Inferred: a render tag's link and its data: the sky box and a face. */
struct SSkyFace {
    CSkyBox* m_sky;
    int m_face;
};

class CDmaTag {
public:
    CDmaTag* m_next;
    SSkyFace* m_data;
};

static CMatrix g_ProjectionMatrix;

class CSkyBoxRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket& packet, void* data) {
        m_sky = (CSkyBox*)data;
        m_sky = (CSkyBox*)data;
        packet.Add((unsigned int)m_sky);
        packet.Add(m_face);
        return 0;
    }

    CSkyBoxRenderBin() : m_sky(0), m_face(0), m_enabled(false) {
        m_priority = 1;
        Init();
    }
    virtual ~CSkyBoxRenderBin() {}
    virtual void Link(CDmaTag* tag, CDmaPacket& packet) {
        if (tag) {
            CDmaTag* t = tag;
            if (t) {
                GXClearVtxDesc();
                GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
                GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
                GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
                GXSetProjection(g_ProjectionMatrix.m, GX_PERSPECTIVE);
                GXSetCurrentMtx(3);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
                GXSetNumChans(1);
                GXSetNumTexGens(1);
                GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 60, GX_FALSE, 125);
                GXSetNumTevStages(1);
                GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
                GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
                GXSetCullMode(GX_CULL_NONE);
                GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
                GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
                GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
                GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
                GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
                for (; t; t = t->m_next) {
                    CSkyBox* sky = t->m_data->m_sky;
                    int face = t->m_data->m_face;
                    float matrix[3][4];
                    sky->m_matrix.GetGAMECUBEMatrix34(matrix);
                    GXLoadPosMtxImm(matrix, 3);
                    sky->m_textures[face].Load(packet, 0);
                    GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, sky->m_vertexCounts[face]);
                    for (int i = 0; i < sky->m_vertexCounts[face]; i++) {
                        CSkyBox::SVertex& v = m_sky->m_vertices[face][i];
                        GXPosition3f32(v.x, v.y, v.z);
                        GXColor4u8(255, 255, 255, 255);
                        GXTexCoord2f32(v.u, v.v);
                    }
                }
                GXSetCurrentMtx(0);
            }
        }
    }

    virtual bool IsUsed() {
        return m_sky && m_enabled;
    }

    CSkyBox* m_sky;
    int m_face;
    bool m_enabled;
};

static CVector3 g_skyBoxVertices[8] = {
    CVector3(-1.0f, -1.0f, -1.0f), CVector3(1.0f, -1.0f, -1.0f), CVector3(1.0f, -1.0f, 1.0f), CVector3(-1.0f, -1.0f, 1.0f),
    CVector3(-1.0f, 1.0f, -1.0f),  CVector3(1.0f, 1.0f, -1.0f),  CVector3(1.0f, 1.0f, 1.0f),  CVector3(-1.0f, 1.0f, 1.0f),
};
static CVector3 g_skyBoxNormals[6] = {
    CVector3(0.0f, -1.0f, 0.0f), CVector3(-1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f),
    CVector3(1.0f, 0.0f, 0.0f),  CVector3(0.0f, 0.0f, 1.0f),  CVector3(0.0f, 0.0f, -1.0f),
};
CSkyBoxRenderBin g_skyBoxRenderBin[6];

static const int g_skyBoxVertIndices[6][4] = {
    {2, 3, 0, 1}, {3, 7, 4, 0}, {7, 6, 5, 4}, {6, 2, 1, 5}, {3, 2, 6, 7}, {4, 5, 1, 0},
};
static const float g_skyBoxUV[4][2] = {
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
};

__declspec(weak) bool CSkyBox::IsVisible(CDrawContext&) const {
    return true;
}

__declspec(weak) bool CSkyBox::IsDrawEnabled() const {
    return m_drawEnabled;
}

__declspec(weak) CSkyBox::SVertex::SVertex() {
}

void CSkyBox::Draw(CDrawContext& context) {
    g_ProjectionMatrix = *context.m_viewToScreen;
    if (g_config.noSkyBox) {
        sm_pScreen->SetClear(true);
        for (int i = 0; i < 6; i++)
            g_skyBoxRenderBin[i].m_enabled = false;
    } else {
        sm_pScreen->SetClear(false);
        m_matrix = *context.m_worldToView;
        m_matrix.SetPos(CVector3(0.0f, 0.0f, 0.0f));
        m_matrix2.Multiply(m_matrix, *context.m_viewToScreen);
        CVector3 forward;
        CVector4 corners[8];
        for (int i = 0; i < 8; i++)
            m_matrix2.Transform(corners[i], g_skyBoxVertices[i]);
        context.m_camera->GetForward(forward);
        for (int face = 0; face < 6; face++) {
            CVector3& n = g_skyBoxNormals[face];
            if (forward.x * n.x + forward.y * n.y + forward.z * n.z > 0.0f) {
                g_skyBoxRenderBin[face].m_enabled = true;
                for (int j = 0; j < 4; j++) {
                    SVertex& vert = m_vertices[face][j];
                    int index = g_skyBoxVertIndices[face][j];
                    float v = g_skyBoxUV[j][1];
                    vert.x = g_skyBoxVertices[index].x;
                    vert.y = g_skyBoxVertices[index].y;
                    vert.z = g_skyBoxVertices[index].z;
                    vert.u = g_skyBoxUV[j][0];
                    vert.v = v;
                    vert.unknown18[0] = 0.0f;
                }
                m_vertexCounts[face] = 4;
                sm_pRenderList->Render(g_skyBoxRenderBin[face], this);
            } else {
                g_skyBoxRenderBin[face].m_enabled = false;
            }
        }
    }
}

void CSkyBox::SetTextures() {
    TLTResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)13, 0);
    if (!resource) {
        DebugMsg("No Skybox textures found in level contents file\n");
        return;
    }
    m_shapes[0] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[0].m_name, 0);
    m_shapes[1] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[1].m_name, 0);
    m_shapes[2] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[2].m_name, 0);
    m_shapes[3] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[3].m_name, 0);
    m_shapes[4] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[4].m_name, 0);
    m_shapes[5] = (ShapeFile*)TLT_LoadFileFromLevelBigFile(resource->data->m_faces[5].m_name, 0);
    for (int i = 0; i < 6; i++) {
        if (m_shapes[i])
            m_textures[i].Set(m_shapes[i], false);
    }
}

CSkyBox::~CSkyBox() {
    for (int i = 0; i < 6; i++)
        TLT_CloseFile(m_shapes[i]);
}

__declspec(weak) CTexture::CTexture() {
}

void CSkyBox::DestroyInstance() {
    delete sm_pSingleton;
    sm_pScreen->SetClear(true);
    sm_pSingleton = 0;
    for (int i = 0; i < 6; i++)
        g_skyBoxRenderBin[i].m_enabled = false;
}

CSkyBox* CSkyBox::GetInstance() {
    return sm_pSingleton;
}

CSkyBox* CSkyBox::CreateInstance() {
    if (!sm_pSingleton) {
        sm_pSingleton = new CSkyBox;
        sm_pScreen->SetClear(false);
        for (int i = 0; i < 6; i++)
            g_skyBoxRenderBin[i].m_enabled = true;
    }
    return sm_pSingleton;
}

void CSkyBox::InitClass(CRenderList* list, CScreen* screen) {
    static bool ready = false;
    if (!ready) {
        sm_pRenderList = list;
        sm_pScreen = screen;
        for (int i = 0; i < 6; i++) {
            g_skyBoxRenderBin[i].m_face = i;
            sm_pRenderList->Register(g_skyBoxRenderBin[i]);
        }
    }
}
