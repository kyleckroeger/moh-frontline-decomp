// A fragment of compartment.cpp (0x80074cc8): CCompartment::Draw, which sets
// up the file's per-draw state (camera position, near clip, the GameCube
// world-to-camera matrix, the clip and screen matrices, frustum, fog, planes
// and the mipmap bias and scale against the first field of view), then, unless
// world drawing is off, draws the node tree and renders the compartment's and
// the global compartment's simple and alpha material bins that hold vertices,
// marking the matching file-level bin used, with profile timers around it.
// The matrices, g_CamPos, g_GlobalCpt and the bins belong to the file's .bss
// block and are reached from its base register, so the block up to the bins
// is defined here with views that construct nothing and pooled; the
// compartment.cpp unit places it. The other file statics are declared extern.
// Classes and functions are named by the symbols; member names and the
// camera's virtual-table view are inferred.
#include "game/CVector3.h"

#include "game/CMatrix.h"

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
    unsigned char unknownData04[0x20];
    float m_near;   // +0x24
    float m_far;    // +0x28
    float m_fovX;   // +0x2c: compared against its first value to scale mipmaps
    unsigned char unknownData30[0x120];
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

class CRenderBin {
public:
    unsigned char unknown00[0x20];
};
/* view: the compartment's material bins */
class CCPTMatBin : public CRenderBin {
public:
    unsigned char unknown20[0x48];
    unsigned int m_count; // +0x68
    unsigned char unknown6c[4];
};
/* view: the file-level simple and shadow bins */
class CCptSimpleBin : public CRenderBin {
public:
    bool m_used; // +0x20, a byte: the original writes it with stb
    unsigned char unknown21[3];
};
class CRenderList {
public:
    void Register(CRenderBin&);
    void Render(CRenderBin&, void*);
};
struct CPTData {
    unsigned char unknown00[8];
    CCPTMatBin* m_simple;
    int m_numSimple;
    CCPTMatBin* m_alpha;
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
    unsigned char unknown5c[0xc];
} __attribute__((aligned(8)));
struct CPTNode;
extern CRenderList* g_pRenderList;
extern CScreen* g_pScreen;
extern float g_NearClip;
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
    unsigned char unknown02[0x12]; // g_config is 0x14 bytes, addressed absolutely
};
extern ConfigView g_config;

/* view (inferred): the file's .bss block in original order up to the bins
   with exact sizes; the matrices are CMatrix (the GameCube one 3x4) and
   g_CamPos a CVector3 in the original, viewed here without constructors so
   that no static initialiser is compiled; each object with a destructor
   follows its destructor-chain record (@537..@540) */
struct MatrixView {
    float m[16];
} __attribute__((aligned(16)));
struct CamPosView {
    float v[4];
};
struct DestructorChainView {
    unsigned char unknown00[12];
};
static MatrixView g_ModelToScreenMatrix;
static MatrixView g_ModelToClipMatrix;
static MatrixView g_ClipToScreenMatrix;
static float g_WorldToCameraGCMatrix[3][4];
static CamPosView g_CamPos;
static DestructorChainView s_globalCptChain;
static CCompartment g_GlobalCpt;
static DestructorChainView s_simpleBinChain;
static CCptSimpleBin g_CptSimpleBin;
static DestructorChainView s_simpleAlphaBinChain;
static CCptSimpleBin g_CptSimpleAlphaBin;
static DestructorChainView s_shadowBinChain;
static CCptSimpleBin g_CptShadowBin;

void CCompartment::Draw(CDrawContext& context) {
    context.m_camera->GetPosition((CVector3&)g_CamPos);
    g_NearClip = context.m_camera->m_near;
    context.m_worldToCamera->GetGAMECUBEMatrix34(g_WorldToCameraGCMatrix);
    ((CMatrix&)g_ModelToClipMatrix).Multiply(*context.m_worldToCamera, *context.m_cameraToClip);
    ((CMatrix&)g_ClipToScreenMatrix).Multiply(*context.m_clipToNDC, *context.m_NDCToScreen);
    ((CMatrix&)g_ModelToScreenMatrix).Multiply((CMatrix&)g_ModelToClipMatrix, (CMatrix&)g_ClipToScreenMatrix);
    g_pFrustum = context.m_camera->m_frustum;
    g_pFog = context.m_fog;
    g_NearPlane = context.m_camera->m_near;
    g_FarPlane = context.m_camera->m_far;

    static float g_DefaultHW = context.m_camera->m_fovX;
    g_bMipMapScale = context.m_camera->m_fovX != g_DefaultHW;
    g_MipMapBias = 1.0f;
    if (g_bDisableMipMapBias) {
        g_bMipMapScale = false;
        g_MipMapBias = 0.0f;
    }
    g_TexturesCached = 0;

    if (!g_config.m_noWorldDraw) {
        g_ProfileBins[13].m_time -= OSGetTick();
        g_ProfileBins[14].m_time -= OSGetTick();
        DrawNode(*m_data->m_root, 0x3ff, *(dwi::fast_vec<CLight*>*)context.m_lights);
        g_ProfileBins[14].m_time += OSGetTick();
        g_ProfileBins[14].m_count++;
        CCPTMatBin* bin;
        unsigned int tick = OSGetTick();
        g_CurrState = 1;
        g_ProfileBins[15].m_time -= tick;
        bin = m_data->m_simple;
        for (int i = 0; i < m_data->m_numSimple; i++, bin++) {
            if (bin->m_count) {
                g_CptSimpleBin.m_used = 1;
                g_pRenderList->Render(*bin, 0);
            }
        }
        g_CurrState = 2;
        bin = m_data->m_alpha;
        for (int i = 0; i < m_data->m_numAlpha; i++, bin++) {
            if (bin->m_count) {
                g_CptSimpleAlphaBin.m_used = 1;
                g_pRenderList->Render(*bin, 0);
            }
        }
        g_CurrState = 1;
        bin = g_GlobalCpt.m_data->m_simple;
        for (int i = 0; i < g_GlobalCpt.m_data->m_numSimple; i++, bin++) {
            if (bin->m_count) {
                g_CptSimpleBin.m_used = 1;
                g_pRenderList->Render(*bin, 0);
            }
        }
        g_CurrState = 2;
        bin = g_GlobalCpt.m_data->m_alpha;
        for (int i = 0; i < g_GlobalCpt.m_data->m_numAlpha; i++, bin++) {
            if (bin->m_count) {
                g_CptSimpleAlphaBin.m_used = 1;
                g_pRenderList->Render(*bin, 0);
            }
        }
        g_ProfileBins[15].m_time += OSGetTick();
        g_ProfileBins[15].m_count++;
        g_ProfileBins[13].m_time += OSGetTick();
        g_ProfileBins[13].m_count++;
    }
}

