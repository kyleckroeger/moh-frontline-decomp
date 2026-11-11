// staticmesh.cpp: per-frame drawing setup for a static mesh, as partial source.
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
    static void Multiply(CMatrix&, const CMatrix&, const CMatrix&);
    CVector3 TransformToLocal(CVector3) const;
    CMatrix& operator=(const CMatrix&);
    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));
struct CameraView {
    void** m_table;
    unsigned char unknown04[0x20];
    float m_near;
    float m_far;
    unsigned char unknown2c[0x124];
    unsigned char m_frustum[1]; // +0x150
};
typedef void (*GetPositionFn)(CameraView*, CVector3*);
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
    bool m_pending; // +0x20, a byte: the original writes it with stb
    unsigned char unknown21[3];
    unsigned char unknown24[0x44];
    int m_count;
    unsigned char unknown6c[4];
};
class CRenderList {
public:
    void Render(CRenderBin&, void*);
};
struct MSHNode;
struct MSHData {
    unsigned char unknown00[8];
    CRenderBin* m_simple;
    int m_numSimple;
    CRenderBin* m_alpha;
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
extern CVector3 g_CamPos;
extern float g_NearPlane;
extern float g_FarPlane;
extern CMatrix* g_pModelToWorld;
extern CMatrix g_WorldToCameraMatrix;
extern CMatrix g_ActualWorldToCameraMatrix;
extern CMatrix g_ModelToClipMatrix;
extern CMatrix g_ClipToScreenMatrix;
extern CMatrix g_ModelToScreenMatrix;
extern void* g_pFrustum;
extern void* g_pFog;
extern int g_CurrState;
extern CRenderBin g_MshSimpleBin;
extern CRenderBin g_MshSimpleAlphaBin;
extern CRenderList* g_pRenderList;

void CStaticMesh::Draw(CDrawContext& context, int) {
    if (g_config.m_noDraw)
        return;
    g_bCacheLightParam = false;
    g_bAllowLighting = m_allowLighting;
    g_pContext = &context;
    ((GetPositionFn)context.m_camera->m_table[0x40 / 4])(context.m_camera, &g_CamPos);
    g_CamPos = context.m_modelToWorld->TransformToLocal(g_CamPos);
    g_NearPlane = context.m_camera->m_near;
    g_FarPlane = context.m_camera->m_far;
    g_pModelToWorld = context.m_modelToWorld;

    CMatrix modelToCamera;
    CMatrix::Multiply(modelToCamera, *g_pModelToWorld, *context.m_worldToCamera);
    g_WorldToCameraMatrix = modelToCamera;
    g_ActualWorldToCameraMatrix = *context.m_worldToCamera;
    CMatrix::Multiply(g_ModelToClipMatrix, modelToCamera, *context.m_cameraToClip);
    CMatrix::Multiply(g_ClipToScreenMatrix, *context.m_clipToNDC, *context.m_NDCToScreen);
    CMatrix::Multiply(g_ModelToScreenMatrix, g_ModelToClipMatrix, g_ClipToScreenMatrix);
    g_pFrustum = context.m_camera->m_frustum;
    g_pFog = context.m_fog;
    DrawNode(*m_data->m_root, 0x3ff);

    CRenderBin* bin = m_data->m_simple;
    g_CurrState = 1;
    int i;
    for (i = 0; i < m_data->m_numSimple; i++) {
        if (bin->m_count) {
            g_MshSimpleBin.m_pending = 1;
            g_pRenderList->Render(*bin, 0);
        }
        bin++;
    }
    g_CurrState = 2;
    bin = m_data->m_alpha;
    for (i = 0; i < m_data->m_numAlpha; i++) {
        if (bin->m_count) {
            g_MshSimpleAlphaBin.m_pending = 1;
            g_pRenderList->Render(*bin, 0);
        }
        bin++;
    }
}
