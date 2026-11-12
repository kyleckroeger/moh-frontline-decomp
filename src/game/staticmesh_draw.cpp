// A fragment of staticmesh.cpp (0x800f1e64): CStaticMesh::Draw, which sets
// up the file's per-draw state (context, camera position in model space,
// near and far planes, the model, camera, clip and screen matrices, frustum
// and fog), draws the node tree and then renders the mesh's simple and alpha
// material bins that hold vertices, marking the matching file-level bin used.
// The matrices, g_CamPos and the bins belong to the file's .bss block and are
// reached from its base register, so the block is defined here with views
// that construct nothing and pooled; the staticmesh.cpp unit places it. The
// other file statics are declared extern. Classes and functions are named by
// the symbols; member names and the camera's virtual-table view are inferred.
union CVector3Data {
    double pair[2];
    float v[4];
};
class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { d = other.d; }
    // The original assigns x, y and z only: g_CamPos's w is left alone.
    CVector3& operator=(const CVector3& other) {
        d.v[0] = other.d.v[0];
        d.v[1] = other.d.v[1];
        d.v[2] = other.d.v[2];
        return *this;
    }
    CVector3Data d;
} __attribute__((aligned(8)));
class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void Multiply(const CMatrix&, const CMatrix&);
    CVector3 TransformToLocal(CVector3) const;
    CMatrix& operator=(const CMatrix&);
    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));
/* view: the camera through its ISceneNode virtual table (slot 0x40 is
   GetPosition) */
class CameraView {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual void unknown34();
    virtual void unknown38();
    virtual void unknown3c();
    virtual void GetPosition(CVector3&) const;
    unsigned char unknown04[0x20];
    float m_near;
    float m_far;
    unsigned char unknownData2c[0x124];
    unsigned char m_frustum[1]; // +0x150
};
struct CDrawContext {
    unsigned char unknown00[0xc0];
    CMatrix* m_modelToWorld;
    CMatrix* m_worldToCamera;
    CMatrix* m_cameraToClip;
    CMatrix* m_clipToNDC;
    CMatrix* m_NDCToScreen;
    CameraView* m_camera;
    void* m_fog;
};
class CRenderBin {
public:
    unsigned char unknown00[0x20];
};
/* view: the mesh's material bins */
class CMSHMatBin : public CRenderBin {
public:
    unsigned char unknown20[0x48];
    unsigned int m_count;
    unsigned char unknown6c[4];
};
/* view: the two file-level simple bins */
class CMshSimpleBin : public CRenderBin {
public:
    bool m_used; // +0x20, a byte: the original writes it with stb
    unsigned char unknown21[3];
};
class CRenderList {
public:
    void Render(CRenderBin&, void*);
};
struct MSHNode;
struct MSHData {
    unsigned char unknown00[8];
    CMSHMatBin* m_simple;
    int m_numSimple;
    CMSHMatBin* m_alpha;
    int m_numAlpha;
    unsigned char unknown18[8];
    MSHNode* m_root;
};
class CStaticMesh {
public:
    void Draw(CDrawContext&, int);
    void DrawNode(MSHNode&, unsigned long);
    unsigned char unknown00[4];
    MSHData* m_data;
    unsigned char m_allowLighting : 1;
};
struct ConfigView {
    unsigned char m_noDraw;
    unsigned char unknown01[0x13]; // g_config is 0x14 bytes, addressed absolutely
};
extern ConfigView g_config;
extern bool g_bCacheLightParam;
extern unsigned char g_bAllowLighting;
extern CDrawContext* g_pContext;
extern float g_NearPlane;
extern float g_FarPlane;
extern CMatrix* g_pModelToWorld;
extern void* g_pFrustum;
extern void* g_pFog;
extern int g_CurrState;
extern CRenderList* g_pRenderList;

/* view (inferred): the file's .bss block in original order with exact sizes;
   the matrices are CMatrix and g_CamPos a CVector3 in the original, viewed
   here without constructors so that no static initialiser is compiled */
struct MatrixView {
    float m[16];
} __attribute__((aligned(16)));
struct CamPosView {
    float v[4];
} __attribute__((aligned(8)));
struct SLightParam {
    unsigned char unknown00[96];
};
struct DestructorChainView {
    unsigned char unknown00[12];
};

static MatrixView g_ActualWorldToCameraMatrix;
static MatrixView g_WorldToCameraMatrix;
static MatrixView g_ModelToScreenMatrix;
static MatrixView g_ModelToClipMatrix;
static MatrixView g_ClipToScreenMatrix;
static CamPosView g_CamPos;
SLightParam g_LightParam;
static DestructorChainView s_simpleBinChain;
static CMshSimpleBin g_MshSimpleBin;
static DestructorChainView s_simpleAlphaBinChain;
static CMshSimpleBin g_MshSimpleAlphaBin;


void CStaticMesh::Draw(CDrawContext& context, int) {
    if (g_config.m_noDraw)
        return;
    g_pContext = &context;
    g_bCacheLightParam = false;
    g_bAllowLighting = m_allowLighting;
    context.m_camera->GetPosition((CVector3&)g_CamPos);
    CVector3 local = context.m_modelToWorld->TransformToLocal((CVector3&)g_CamPos);
    (CVector3&)g_CamPos = local;
    g_NearPlane = context.m_camera->m_near;
    g_FarPlane = context.m_camera->m_far;
    g_pModelToWorld = context.m_modelToWorld;

    CMatrix modelToCamera;
    modelToCamera.Multiply(*g_pModelToWorld, *context.m_worldToCamera);
    (CMatrix&)g_WorldToCameraMatrix = modelToCamera;
    (CMatrix&)g_ActualWorldToCameraMatrix = *context.m_worldToCamera;
    ((CMatrix&)g_ModelToClipMatrix).Multiply(modelToCamera, *context.m_cameraToClip);
    ((CMatrix&)g_ClipToScreenMatrix).Multiply(*context.m_clipToNDC, *context.m_NDCToScreen);
    ((CMatrix&)g_ModelToScreenMatrix).Multiply((CMatrix&)g_ModelToClipMatrix, (CMatrix&)g_ClipToScreenMatrix);
    g_pFrustum = context.m_camera->m_frustum;
    g_pFog = context.m_fog;
    DrawNode(*m_data->m_root, 0x3ff);

    CMSHMatBin* bin = m_data->m_simple;
    g_CurrState = 1;
    for (int i = 0; i < m_data->m_numSimple; i++, bin++) {
        if (bin->m_count) {
            g_MshSimpleBin.m_used = 1;
            g_pRenderList->Render(*bin, 0);
        }
    }
    g_CurrState = 2;
    bin = m_data->m_alpha;
    for (int i = 0; i < m_data->m_numAlpha; i++, bin++) {
        if (bin->m_count) {
            g_MshSimpleAlphaBin.m_used = 1;
            g_pRenderList->Render(*bin, 0);
        }
    }
}
