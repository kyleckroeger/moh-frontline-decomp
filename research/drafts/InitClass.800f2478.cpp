extern "C" void* memset(void*, int, unsigned long);

class CRenderBin {
    unsigned char unknown00[36];
};

class CRenderList {
public:
    void Register(CRenderBin&);
};

class CScreen;
struct BPDLightVolume;

struct SLightParam {
    unsigned char unknown00[96];
};

class CStaticMesh {
public:
    static void InitClass(CRenderList*, CScreen*);
};

extern CRenderList* g_pRenderList;
extern CScreen* g_pScreen;
extern BPDLightVolume* g_pLightVolume;
extern SLightParam g_LightParam;
extern CRenderBin g_MshSimpleBin;
extern CRenderBin g_MshSimpleAlphaBin;

void CStaticMesh::InitClass(CRenderList* list, CScreen* screen) {
    static bool bReady = false;
    if (!bReady) {
        g_pScreen = screen;
        g_pRenderList = list;
        list->Register(g_MshSimpleBin);
        list->Register(g_MshSimpleAlphaBin);
        g_pLightVolume = 0;
        memset(&g_LightParam, 0, sizeof(g_LightParam));
    }
}
