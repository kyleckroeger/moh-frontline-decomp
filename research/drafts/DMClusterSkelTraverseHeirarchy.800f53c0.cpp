// A fragment of dmcluster.cpp (0x800f53c0): DMClusterSkelTraverseHeirarchy
// walks a skeleton's bones in order: the cluster matrix stack is popped back
// to each bone's parent depth, the bone's position is transformed by the
// parent's matrix, a matrix rotated by the bone's three 16-bit angles is
// placed there and loaded as the bone's frame, and the result is stored to
// the output matrix and the scratch matrix after the root's; the transformed
// positions' bounds are returned when asked. The functions, classes and
// globals are named by the symbols; _dmeshStack, _scratchMats and
// _matrix_rot_yzx are file-local in the original and declared extern here;
// the skeleton, bone and orientation views are inferred. The rest of the file
// is in other units.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3(const CVector3& other) : d(other.d) {}
    CVector3& operator=(const CVector3& o) {
        d.v[0] = o.d.v[0];
        d.v[1] = o.d.v[1];
        d.v[2] = o.d.v[2];
        return *this;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix& operator=(const CMatrix&);
    void TransformPoint(CVector3&, CVector3) const;
    void SetPos(CVector3);
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

class CMatrixStack {
public:
    void Store(CMatrix*);
    void Load(const CMatrix*);
    void Push();
    void Pop();
    CMatrix& Top() { return m_matrices[m_top]; }

    unsigned char unknown00[4];
    int m_top;
    CMatrix* m_matrices;
};

extern CMatrixStack* _dmeshStack; // file-local in the original
extern CMatrix _scratchMats[128]; // file-local in the original
void _matrix_rot_yzx(CMatrix&, long, long, long); // file-local in the original

/* inferred: a bone (16 bytes): position and parent depth */
struct CharBone_t {
    float x;
    float y;
    float z;
    int parent;
};

/* inferred: the bone count (+6) and the bones (+32) */
struct CharSkel_t {
    unsigned char unknown00[6];
    unsigned short count;
    unsigned char unknown08[24];
    CharBone_t bones[1];
};

/* inferred: a bone's three angles */
struct CharOrient3_t {
    short a;
    short b;
    short c;
};

static inline void TransformByTop(CVector3& p) { _dmeshStack->Top().TransformPoint(p, p); }

void DMClusterSkelTraverseHeirarchy(CharOrient3_t* orient_, CharSkel_t* skeleton, CMatrix* matrices_, CVector3* minimum_, CVector3* maximum_) {
    unsigned int count;
    CharBone_t* bone;
    short b;
    short c;
    CMatrix* matrices;
    CharOrient3_t* orient;
    CVector3* maximum;
    CMatrix* scratch;
    CVector3* minimum;
    int depth;
    short a;
    CVector3 pos;
    CVector3 low;
    CVector3 high;
    orient = orient_;
    matrices = matrices_;
    minimum = minimum_;
    maximum = maximum_;
    count = skeleton->count;
    bone = skeleton->bones;
    bone->z = 0.0f;
    scratch = _scratchMats;
    _dmeshStack->Store(scratch);
    scratch++;
    depth = -1;
    low.d.v[0] = low.d.v[1] = low.d.v[2] = low.d.v[3] = 3.4028235e38f;
    high.d.v[0] = high.d.v[1] = high.d.v[2] = high.d.v[3] = -3.4028235e38f;
    while (count--) {
        a = orient->a;
        b = orient->b;
        c = orient->c;
        orient++;
        while (depth > bone->parent) {
            _dmeshStack->Pop();
            depth--;
        }
        pos = CVector3(bone->x, bone->y, bone->z);
        _dmeshStack->Push();
        depth++;
        TransformByTop(pos);
        CMatrix matrix;
        _dmeshStack->Store(&matrix);
        _matrix_rot_yzx(matrix, a, b, c);
        matrix.SetPos(pos);
        _dmeshStack->Load(&matrix);
        for (int i = 0; i < 3; i++) {
            if (low.d.v[i] > pos.d.v[i])
                low.d.v[i] = pos.d.v[i];
            else if (high.d.v[i] < pos.d.v[i])
                high.d.v[i] = pos.d.v[i];
        }
        _dmeshStack->Store(matrices);
        *scratch = *matrices;
        bone++;
        matrices++;
        scratch++;
    }
    if (minimum) {
        minimum->d.v[0] = low.d.v[0];
        minimum->d.v[1] = low.d.v[1];
        minimum->d.v[2] = low.d.v[2];
    }
    if (maximum) {
        maximum->d.v[0] = high.d.v[0];
        maximum->d.v[1] = high.d.v[1];
        maximum->d.v[2] = high.d.v[2];
    }
}
