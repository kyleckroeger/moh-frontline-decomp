// A fragment of staticmesh.cpp (0x800f23b0): the weak MSHNode copy
// constructor, which copies the node member by member (four words, four
// vectors and four words). MSHNode is named by the mangled symbol; its
// members are inferred from the copy (the vectors are the double-pair
// CVector3 view, copied as doubleword pairs). The constructor is a weak copy
// of a header inline emitted in this file, so it is defined __declspec(weak).
// It is followed by CStaticMesh::SetLightVolume, which stores the light volume,
// clears the light-cache flag and, for a volume, clears the 96-byte light
// parameters (the file's data are declared here; g_pLightVolume is file-local
// in the original). The rest of the file is not part of this unit.
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Pair d;
} __attribute__((aligned(8)));

struct MSHNode {
    MSHNode(const MSHNode&);

    int word00;
    int word04;
    int word08;
    int word0c;
    CVector3 vectors[4];
    int word50;
    int word54;
    int word58;
    int word5c;
};

__declspec(weak) MSHNode::MSHNode(const MSHNode& other)
    : word00(other.word00), word04(other.word04), word08(other.word08), word0c(other.word0c) {
    vectors[0] = other.vectors[0];
    vectors[1] = other.vectors[1];
    vectors[2] = other.vectors[2];
    vectors[3] = other.vectors[3];
    word50 = other.word50;
    word54 = other.word54;
    word58 = other.word58;
    word5c = other.word5c;
}

struct BPDLightVolume;

struct LightParamView {
    unsigned char unknown00[96];
};

extern "C" void* memset(void*, int, unsigned long);

extern BPDLightVolume* g_pLightVolume;
extern bool g_bCacheLightParam;
extern LightParamView g_LightParam;

class CStaticMesh {
public:
    static void SetLightVolume(BPDLightVolume* volume);
};

void CStaticMesh::SetLightVolume(BPDLightVolume* volume) {
    g_pLightVolume = volume;
    g_bCacheLightParam = false;
    if (volume)
        memset(&g_LightParam, 0, sizeof(g_LightParam));
}
