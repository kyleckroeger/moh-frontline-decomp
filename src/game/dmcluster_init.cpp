// A fragment of dmcluster.cpp (0x800f5290): DMClusterBindSkeleton keeps a
// cluster object's skeleton (+12) and DMClusterInitObject its count and two
// matrix arrays (+0, +4, +8). DMClusterSkelTraverse makes the cluster matrix
// stack current and resets it, translates its top to the frame's position
// (+8), rotates it about Y and X by nothing and about Z by the frame's 16.16
// turn (+40, its whole part, as a fraction of 2pi), walks the skeleton
// hierarchy from the frame's orientations (+56) and makes the global matrix
// stack current again. The functions, classes and globals are named by the
// symbols; the members are inferred from offsets; the stack view follows
// dmcluster.cpp (_dmeshStack is file-local in the original and declared extern
// here), the top-of-stack accessor, the stack local and the frame view are
// inferred. The rest of the file is in other units.
struct CharSkel_t;

struct DMClusterObject_T {
    unsigned long count;
    float (*matrices)[3][4];
    float (*matrices2)[3][4];
    CharSkel_t* skeleton;
};

void DMClusterBindSkeleton(DMClusterObject_T* object, CharSkel_t* skeleton) {
    object->skeleton = skeleton;
}

void DMClusterInitObject(DMClusterObject_T* object, float (*matrices)[3][4], float (*matrices2)[3][4],
                         unsigned long count) {
    object->count = count;
    object->matrices = matrices;
    object->matrices2 = matrices2;
}

struct CharOrient3_t;

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { d = other.d; }

    CVector3Data d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    void PreTranslate(CVector3);
    void RotateX(float);
    void RotateY(float);
    void RotateZ(float);

    float m[4][4];
} __attribute__((aligned(16)));

class CMatrixStack {
public:
    void SetCurrent();
    void Reset();
    CMatrix& Top() { return m_matrices[m_top]; }

    unsigned char unknown00[4];
    int m_top;
    CMatrix* m_matrices;
};

extern CMatrixStack* _dmeshStack; // file-local in the original
extern CMatrixStack* g_matStack;

/* inferred: the frame's position, turn and orientations */
struct CharFrame_t {
    unsigned char unknown00[8];
    CVector3 position;
    unsigned char unknown18[16];
    int turn;
    unsigned char unknown2c[12];
    CharOrient3_t* orients;
};

void DMClusterSkelTraverseHeirarchy(CharOrient3_t*, CharSkel_t*, CMatrix*, CVector3*, CVector3*);

void DMClusterSkelTraverse(CharFrame_t* frame, CharSkel_t* skeleton, CMatrix* matrices, CVector3* a, CVector3* b) {
    _dmeshStack->SetCurrent();
    _dmeshStack->Reset();
    int turn = frame->turn >> 8;
    _dmeshStack->Top().PreTranslate(frame->position);
    _dmeshStack->Top().RotateY(0.0f);
    CMatrixStack* stack = _dmeshStack;
    stack->Top().RotateZ(6.2831855f * turn / 65536.0f);
    _dmeshStack->Top().RotateX(0.0f);
    DMClusterSkelTraverseHeirarchy(frame->orients, skeleton, matrices, a, b);
    g_matStack->SetCurrent();
}
