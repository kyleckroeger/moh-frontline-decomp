// The end of matrix.cpp (0x80090aa0): CMatrix::BuildScale for a vector and
// for a single factor (identity with the scale on the diagonal), operator=
// (the 64-byte copy as paired-single loads and stores, in asm), InitClass
// (copies the unit matrix data into _Mat_Unit and sets s_ClassInit) and the
// static initialisation of the file's matrices: s_TempMat through the inline
// default constructor (class initialisation on first use) and _Mat_Unit as an
// identity. It follows BuildRot, which is not reconstructed yet. CMatrix,
// CVector3, s_TempMat, _Mat_Unit, _Mat_Data and s_ClassInit are named by the
// symbols; the row layout (right, front, up, position, 16 bytes each) is
// inferred from the offsets, as are the identity-setting constructor (as in
// decal.cpp) and the default constructor. Ident is the paired-single routine
// of matrix_endian, inlined here (inferred from the identical store
// sequence); its 0 and 1 are entries of the file's .sdata2 pool. The unit
// defines the file's .bss block (s_TempMat, _Mat_Unit).
#include "game/CVector3.h"

extern "C" void* memcpy(void*, const void*, unsigned long);

class CMatrix;
extern const CMatrix _Mat_Data;

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    /* Inferred: a constructor that also sets the identity. */
    CMatrix(bool identity) {
        if (!s_ClassInit)
            InitClass();
        if (identity)
            Ident();
    }
    void Ident();
    void BuildScale(CVector3);
    void BuildScale(float);
    CMatrix& operator=(const CMatrix&);
    static void InitClass();
    static bool s_ClassInit;
    static CMatrix s_TempMat;

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
};

inline void CMatrix::Ident() {
    register CMatrix* mtx = this;
    register float c_zero = 0.0f;
    register float c_one = 1.0f;
    register float c_01;
    register float c_10;

    asm {
        psq_st c_zero, 8(mtx), 0, 0
        ps_merge01 c_01, c_zero, c_one
        psq_st c_zero, 24(mtx), 0, 0
        ps_merge10 c_10, c_one, c_zero
        psq_st c_zero, 32(mtx), 0, 0
        psq_st c_zero, 48(mtx), 0, 0
        psq_st c_01, 16(mtx), 0, 0
        psq_st c_10, 0(mtx), 0, 0
        psq_st c_10, 40(mtx), 0, 0
        psq_st c_01, 56(mtx), 0, 0
        nop
    }
}

CMatrix CMatrix::s_TempMat;
static CMatrix _Mat_Unit(true);

void CMatrix::BuildScale(CVector3 scale) {
    Ident();
    right.x = scale.x;
    front.y = scale.y;
    up.z = scale.z;
}

void CMatrix::BuildScale(float scale) {
    Ident();
    up.z = scale;
    front.y = scale;
    right.x = scale;
}

asm CMatrix& CMatrix::operator=(const CMatrix& m) {
    nofralloc
    psq_l f0, 0(r4), 0, 0
    psq_l f1, 8(r4), 0, 0
    psq_l f2, 16(r4), 0, 0
    psq_l f3, 24(r4), 0, 0
    psq_st f0, 0(r3), 0, 0
    psq_st f1, 8(r3), 0, 0
    psq_st f2, 16(r3), 0, 0
    psq_st f3, 24(r3), 0, 0
    psq_l f0, 32(r4), 0, 0
    psq_l f1, 40(r4), 0, 0
    psq_l f2, 48(r4), 0, 0
    psq_l f3, 56(r4), 0, 0
    psq_st f0, 32(r3), 0, 0
    psq_st f1, 40(r3), 0, 0
    psq_st f2, 48(r3), 0, 0
    psq_st f3, 56(r3), 0, 0
    blr
}

void CMatrix::InitClass() {
    memcpy(&_Mat_Unit, &_Mat_Data, sizeof(CMatrix));
    s_ClassInit = true;
}
