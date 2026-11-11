// compartment.cpp: per-frame drawing setup and class initialisation, as partial
// source. Views are inferred from these functions.
class CVector3 {
public:
    float x, y, z, w;
};
class CMatrix {
public:
    static void Multiply(CMatrix&, const CMatrix&, const CMatrix&);
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;
    float m[4][4];
};
class CCameraView {
public:
    virtual void Function08();
    unsigned char unknown[0x20];
};
struct CameraView {
    void** m_table;
    unsigned char unknown04[0x20];
    float m_near;   // +0x24
    float m_far;    // +0x28
    float m_fovX;   // +0x2c: compared against its first value to scale mipmaps
    unsigned char unknown30[0x120];
    unsigned char m_frustum[1]; // +0x150
};
struct CDrawContext {
    unsigned char unknown00[0xc4];
    CMatrix* m_worldToCamera;
    CMatrix* m_cameraToClip;
    CMatrix* m_clipToNDC;
    CMatrix* m_NDCToScreen;
    CameraView* m_camera;
    void* m_fog;
    unsigned char unknownDC[0x14];
    void* m_lights;
};
typedef void (*GetPositionFn)(CameraView*, CVector3*);

class CRenderBin {
public:
    unsigned char unknown00[0x20];
    bool m_pending; // +0x20, a byte: the original writes it with stb
    unsigned char unknown21[3];
    unsigned char unknown24[0x44];
    int m_count;   // +0x68
    unsigned char unknown6c[4];
};
class CRenderList {
public:
    void Register(CRenderBin&);
    void Render(CRenderBin&, void*);
};
struct CPTData {
    unsigned char unknown00[8];
    CRenderBin* m_simple;
    int m_numSimple;
    CRenderBin* m_alpha;
    int m_numAlpha;
    unsigned char unknown18[8];
    struct CPTNode* m_root; // +0x20
};
struct CProfileBin {
    unsigned char unknown00[8];
    unsigned int m_time;
    unsigned char unknown0c[8];
    unsigned int m_count;
};
extern CProfileBin g_ProfileBins[32];
extern "C" unsigned int OSGetTick();
namespace dwi { template <class T> class fast_vec; }
class CLight;
class CScreen;

class CCompartment {
public:
    void Draw(CDrawContext&);
    void DrawNode(CPTNode&, unsigned long, dwi::fast_vec<CLight*>&);
    static void InitClass(CRenderList*, CScreen*);
    unsigned char unknown00[0x58];
    CPTData* m_data;
};
struct CPTNode;
extern CCompartment g_GlobalCpt;
extern CRenderBin g_CptSimpleBin;
extern CRenderBin g_CptSimpleAlphaBin;
extern CRenderBin g_CptShadowBin;
extern CRenderList* g_pRenderList;
extern CScreen* g_pScreen;
extern CVector3 g_CamPos;
extern float g_NearClip;
extern float g_WorldToCameraGCMatrix[3][4];
extern CMatrix g_ModelToClipMatrix;
extern CMatrix g_ClipToScreenMatrix;
extern CMatrix g_ModelToScreenMatrix;
extern void* g_pFrustum;
extern void* g_pFog;
extern float g_NearPlane;
extern float g_FarPlane;
extern float g_MipMapBias;
extern bool g_bDisableMipMapBias;
extern bool g_bMipMapScale;
extern int g_TexturesCached;
extern int g_CurrState;
struct ConfigView {
    unsigned char m_noDraw;
    unsigned char m_noWorldDraw;
};
extern ConfigView g_config;

static inline void RenderBins(CRenderBin* bins, int count, CRenderBin& target) {
    for (int i = 0; i < count; i++) {
        if (bins[i].m_count) {
            target.m_pending = 1;
            g_pRenderList->Render(bins[i], 0);
        }
    }
}

void CCompartment::Draw(CDrawContext& context) {
    ((GetPositionFn)context.m_camera->m_table[0x40 / 4])(context.m_camera, &g_CamPos);
    g_NearClip = context.m_camera->m_near;
    context.m_worldToCamera->GetGAMECUBEMatrix34(g_WorldToCameraGCMatrix);
    CMatrix::Multiply(g_ModelToClipMatrix, *context.m_worldToCamera, *context.m_cameraToClip);
    CMatrix::Multiply(g_ClipToScreenMatrix, *context.m_clipToNDC, *context.m_NDCToScreen);
    CMatrix::Multiply(g_ModelToScreenMatrix, g_ModelToClipMatrix, g_ClipToScreenMatrix);
    g_pFrustum = context.m_camera->m_frustum;
    g_pFog = context.m_fog;
    g_NearPlane = context.m_camera->m_near;
    g_FarPlane = context.m_camera->m_far;

    static float g_DefaultHW = context.m_camera->m_fovX;
    g_MipMapBias = 1.0f;
    if (g_bDisableMipMapBias)
        g_MipMapBias = 0.0f;
    g_bMipMapScale = !g_bDisableMipMapBias && context.m_camera->m_fovX != g_DefaultHW;
    g_TexturesCached = 0;

    if (!g_config.m_noWorldDraw) {
        g_ProfileBins[13].m_time -= OSGetTick();
        g_ProfileBins[14].m_time -= OSGetTick();
        DrawNode(*m_data->m_root, 0x3ff, *(dwi::fast_vec<CLight*>*)context.m_lights);
        g_ProfileBins[14].m_time += OSGetTick();
        g_ProfileBins[14].m_count++;
        unsigned int tick = OSGetTick();
        g_CurrState = 1;
        g_ProfileBins[15].m_time -= tick;
        RenderBins(m_data->m_simple, m_data->m_numSimple, g_CptSimpleBin);
        g_CurrState = 2;
        RenderBins(m_data->m_alpha, m_data->m_numAlpha, g_CptSimpleAlphaBin);
        g_CurrState = 1;
        RenderBins(g_GlobalCpt.m_data->m_simple, g_GlobalCpt.m_data->m_numSimple, g_CptSimpleBin);
        g_CurrState = 2;
        RenderBins(g_GlobalCpt.m_data->m_alpha, g_GlobalCpt.m_data->m_numAlpha, g_CptSimpleAlphaBin);
        g_ProfileBins[15].m_time += OSGetTick();
        g_ProfileBins[15].m_count++;
        g_ProfileBins[13].m_time += OSGetTick();
        g_ProfileBins[13].m_count++;
    }
}

// A fixed pool of 256 lights linked through a free list.
struct LightPoolNode {
    unsigned char unknown00[8];
    LightPoolNode* m_next;
};
// As matched in compartment_shadowbins.cpp: 256 entries of 12 bytes linked
// through +8, then the pointers and counts (0xc14 bytes in all).
struct LightPool {
    LightPoolNode m_nodes[256];
    LightPoolNode* m_base;     // +0xc00
    LightPoolNode* m_used;     // +0xc04
    LightPoolNode* m_free;     // +0xc08
    int m_capacity;            // +0xc0c
    int m_count;               // +0xc10
};
extern LightPool g_LightPool;

void CCompartment::InitClass(CRenderList* list, CScreen* screen) {
    static bool bReady = false;
    if (!bReady) {
        g_pRenderList = list;
        g_pScreen = screen;
        list->Register(g_CptSimpleBin);
        list->Register(g_CptSimpleAlphaBin);
        list->Register(g_CptShadowBin);
        g_LightPool.m_capacity = 256;
        g_LightPool.m_base = g_LightPool.m_nodes;
        g_LightPool.m_used = 0;
        g_LightPool.m_free = g_LightPool.m_nodes;
        g_LightPool.m_count = 0;
        g_LightPool.m_base[g_LightPool.m_capacity - 1].m_next = 0;
        for (int i = 0; i < g_LightPool.m_capacity - 1; i++)
            g_LightPool.m_free[i].m_next = &g_LightPool.m_free[i + 1];
    }
}
