// A fragment of skybox.cpp (0x800a94a8): the file's static data and its
// initialisation. __sinit_skybox_cpp constructs the projection matrix (its
// inline constructor runs CMatrix::InitClass once), the eight sky-box corner
// vertices (a +-1 cube) and the six face normals, each from a temporary
// vector copied as two doubles, then the six face render bins, whose array
// destructor (__arraydtor) is registered for exit. The source is named
// skybox.cpp because the initialiser's name comes from it. The classes and
// globals are named by the symbols; g_ProjectionMatrix, g_skyBoxVertices and
// g_skyBoxNormals are file-local (static) and g_skyBoxRenderBin is global, as
// in the image; the vector view and the render bin's size are inferred, and
// the bin's weak constructor and destructor are defined in skybox_renderbin.cpp.
// The rest of the file is in other units.
class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

struct CVector3Pair {
    double d[2];
};

class CVector3 {
public:
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3(const CVector3& o) { *(CVector3Pair*)this = *(const CVector3Pair*)&o; }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CSkyBoxRenderBin {
public:
    CSkyBoxRenderBin();
    ~CSkyBoxRenderBin();

    unsigned char unknown00[0x2c];
};

static CMatrix g_ProjectionMatrix;
static CVector3 g_skyBoxVertices[8] = {
    CVector3(-1.0f, -1.0f, -1.0f), CVector3(1.0f, -1.0f, -1.0f), CVector3(1.0f, -1.0f, 1.0f), CVector3(-1.0f, -1.0f, 1.0f),
    CVector3(-1.0f, 1.0f, -1.0f),  CVector3(1.0f, 1.0f, -1.0f),  CVector3(1.0f, 1.0f, 1.0f),  CVector3(-1.0f, 1.0f, 1.0f),
};
static CVector3 g_skyBoxNormals[6] = {
    CVector3(0.0f, -1.0f, 0.0f), CVector3(-1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f),
    CVector3(1.0f, 0.0f, 0.0f),  CVector3(0.0f, 0.0f, 1.0f),  CVector3(0.0f, 0.0f, -1.0f),
};
CSkyBoxRenderBin g_skyBoxRenderBin[6];
