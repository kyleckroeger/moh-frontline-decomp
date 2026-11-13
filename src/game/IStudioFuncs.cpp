// IStudioFuncs.cpp: iSirenTransformfnc, the
// interface studio's transform callback: action 1 pushes the matrix stack and
// builds the transform on the current matrix (translate by the position and
// by the pivot, rotate about x, y and z by angles in degrees converted
// through a 24-bit fixed-point fraction of a turn, scale the row, record the
// scale in the g_fScaleValue globals, translate back by the pivot), action 2
// pops it, and actions 0 and 3 do nothing (empty cases, kept by the
// compiler's dispatch). Then ResUnloadScreen closes a
// loaded screen's file data when there is any, and ResLoadScreen loads one
// resolution's file of a shell screen from the shell big file (the screen
// table's records are 8 bytes, their file-name lists at +4), returning the
// loaded data or none. The functions, the matrix stack, CMatrix and the
// globals are named by the symbols; the transform record, the vector view (a
// double pair overlaid on the components) with its setter, the matrix
// stack's inline current-matrix wrappers and the angle conversion are
// inferred (the wrappers give the argument copies their stack order). The
// constants are entries of the file's .sdata2 pool. Then the shell's big
// file: unloading frees it, loading frees any previous copy and loads the
// shell big file through the level loader (alignment 256); the globals' types
// are inferred. The empty DummyPluginProcessFnc is last (its parameter
// meanings are not known).
#include "game/CVector3.h"


#include "game/CMatrix.h"


class CMatrixStack {
public:
    void Push();
    void Pop();
    void ScaleRow(CVector3);
    CMatrix& Current() { return m_stack[m_index]; }
    void PreTranslate(CVector3 v) { Current().PreTranslate(v); }
    void RotateX(float angle) { Current().RotateX(angle); }
    void RotateY(float angle) { Current().RotateY(angle); }
    void RotateZ(float angle) { Current().RotateZ(angle); }

    unsigned long m_size;
    int m_index;
    CMatrix* m_stack;
};

extern CMatrixStack* g_matStack;
extern float g_fScaleValueX;
extern float g_fScaleValueY;
extern float g_fScaleValueZ;

enum UISTransformAction {};

/* inferred: a transform record (position, pivot, rotation in degrees, scale) */
struct UISTransform_t {
    float position[3];
    float pivot[3];
    float rotation[3];
    float scale[3];
};

/* inferred: degrees to radians through 24-bit fixed point */
static inline float DegreesToRadians(float degrees) {
    return (6.2831855f * (int)(46603.379f * degrees)) / 16777216.0f;
}

void iSirenTransformfnc(UISTransformAction action, UISTransform_t* transform) {
    switch (action) {
    case 0:
        break;
    case 1: {
        g_matStack->Push();
        CVector3 v;
        v.Set(transform->position[0], transform->position[1], transform->position[2]);
        g_matStack->PreTranslate(v);
        v.Set(transform->pivot[0], transform->pivot[1], transform->pivot[2]);
        g_matStack->PreTranslate(v);
        g_matStack->RotateX(DegreesToRadians(transform->rotation[0]));
        g_matStack->RotateY(DegreesToRadians(transform->rotation[1]));
        g_matStack->RotateZ(DegreesToRadians(transform->rotation[2]));
        v.Set(transform->scale[0], transform->scale[1], transform->scale[2]);
        g_matStack->ScaleRow(v);
        g_fScaleValueX = transform->scale[0] != 1.0f ? transform->scale[0] : 1.0f;
        g_fScaleValueY = transform->scale[1] != 1.0f ? transform->scale[1] : 1.0f;
        g_fScaleValueZ = transform->scale[2] != 1.0f ? transform->scale[2] : 1.0f;
        v.Set(-transform->pivot[0], -transform->pivot[1], -transform->pivot[2]);
        g_matStack->PreTranslate(v);
        break;
    }
    case 2:
        g_matStack->Pop();
        break;
    case 3:
        break;
    }
}

void* TLT_LoadFileFromShellBigFile(const char*, int*);
void TLT_CloseFile(void*);

/* inferred: a shell screen record, its file names per resolution at +4 */
struct ScreenInfoView {
    int unknown00;
    char** files;
};

extern ScreenInfoView* ScreenInfo;

void ResUnloadScreen(unsigned short, unsigned short, void* data) {
    if (data)
        TLT_CloseFile(data);
}

void* ResLoadScreen(unsigned short screen, unsigned short resolution) {
    void* data = TLT_LoadFileFromShellBigFile(ScreenInfo[screen].files[resolution], 0);
    if (data)
        return data;
    return 0;
}

extern "C" void MEM_free(void*);
void* TLT_LoadFileNormal(const char*, int*, int);

extern void* g_pLevelBigFile;
extern int g_LevelBigFileSize;
extern const char* szShellBigFileName;

void UnloadShellBigFile() {
    if (g_pLevelBigFile) {
        MEM_free(g_pLevelBigFile);
        g_pLevelBigFile = 0;
    }
}

void LoadShellBigFile() {
    if (g_pLevelBigFile)
        MEM_free(g_pLevelBigFile);
    g_pLevelBigFile = TLT_LoadFileNormal(szShellBigFileName, &g_LevelBigFileSize, 256);
}

struct UISParam_t;

void DummyPluginProcessFnc(void*, unsigned long, unsigned long, UISParam_t*, UISParam_t*) {
}
