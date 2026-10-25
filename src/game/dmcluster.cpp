// The deformable-mesh cluster's synthesized bones, set-up and shutdown.
// DMClusterSynthesizeMatrices builds the matrices of the mesh's extra bones:
// each one takes its parent's scratch matrix, rotates it by the bone's
// angles blended from a base orientation set (the mesh's own or the
// skeleton's) towards the frame's (the difference wrapped to 16 bits, scaled
// by a 4.12 weight), places it at the bone's scratch position, applies the
// global matrix stack's top and stores the 3x4 result. A matrix stack of 16
// entries is created at start-up and deleted at shutdown. CMatrixStack,
// _dmeshStack and g_matStack are named by the symbols; the stack is 12 bytes
// (from the allocation). The file's static initialiser builds the 128
// matrices of the file-local _scratchMats array through CMatrix's array
// constructor wrapper (__defctor__7CMatrixFv: CMatrix's default constructor
// takes a default argument, unknown here); the compiler's weak copy of that
// wrapper is a weak duplicate, linked to the original copy. _matrix_rot_yzx
// is file-local in the original and declared extern here (it is in another
// unit). The cluster, skeleton, mesh and synthesized-bone views and CMatrix's
// copy constructor (assignment, no class set-up) are inferred.
class CMatrix;

class CMatrixStack {
public:
    CMatrixStack();
    ~CMatrixStack();
    void Allocate(unsigned long);
    void Store(CMatrix*);

    unsigned char unknown00[12];
};

class CVector3 {
public:
    float x, y, z, w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix(const CMatrix& other) { *this = other; }
    CMatrix& operator=(const CMatrix&);
    void SetPos(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
} __attribute__((aligned(16)));

static CMatrixStack* _dmeshStack;
static CMatrix _scratchMats[128];
extern CMatrixStack* g_matStack;
void _matrix_rot_yzx(CMatrix&, long, long, long); // file-local in the original

struct CharSkel_t {
    unsigned char unknown00[1312];
    unsigned char m_orients[1]; /* inferred: the default orientations */
};

struct DMClusterObject_T {
    unsigned long count;
    float (*matrices[2])[3][4];
    CharSkel_t* skeleton;
};

struct CharOrient3_t {
    short a;
    short b;
    short c;
};

/* inferred: one synthesized bone: the bone, its parent and the blend */
struct DMSynthEntry {
    unsigned char bone;
    unsigned char parent;
    unsigned short t;
};

class CDMesh {
public:
    unsigned char unknown00[5];
    unsigned char m_orientSource;
    unsigned char unknown06[26];
    int m_synthCount;
    DMSynthEntry* m_synth;
    unsigned char unknown28[40];
    CharOrient3_t* m_orients;
};

void DMClusterSynthesizeMatrices(unsigned long index, DMClusterObject_T* cluster, CDMesh* mesh, CharOrient3_t* orients) {
    DMSynthEntry* entry;
    int count;
    float (*out)[3][4];
    CharOrient3_t* base;
    int bone;
    unsigned long t;
    CMatrix* mats;
    out = cluster->matrices[index];
    mats = &_scratchMats[1];
    if (mesh->m_orientSource == 1)
        base = mesh->m_orients;
    else
        base = (CharOrient3_t*)cluster->skeleton->m_orients;
    count = mesh->m_synthCount;
    entry = mesh->m_synth;
    while (count > 0) {
        bone = entry->bone;
        int parent = entry->parent;
        count--;
        t = entry->t;
        entry++;
        CharOrient3_t* from = &base[bone];
        CharOrient3_t* to = &orients[bone];
        long a = from->a * 16;
        long b = from->b * 16;
        long c = from->c * 16;
        long da = to->a - a;
        long db = to->b - b;
        long dc = to->c - c;
        da = (da << 16) >> 16;
        db = (db << 16) >> 16;
        dc = (dc << 16) >> 16;
        CMatrix m(mats[parent]);
        _matrix_rot_yzx(m, a + ((da * t) >> 12), b + ((db * t) >> 12), c + ((dc * t) >> 12));
        m.SetPos(mats[bone].position);
        CMatrix top;
        g_matStack->Store(&top);
        CMatrix result;
        result.Multiply(m, top);
        result.GetGAMECUBEMatrix34(*out);
        out++;
    }
}

void DMClusterShutdown() {
    delete _dmeshStack;
}

void DMClusterInit() {
    _dmeshStack = new CMatrixStack;
    _dmeshStack->Allocate(16);
}

