// A fragment of matrix.cpp (0x8008fba0): CMatrix::Orthographic (the identity,
// then 1/width, 1/height, a depth scale of 2/(far - near) and an offset of
// (far + near)/(far - near), with the last element 0; the constants are
// entries of the file's .sdata2 pool), CMatrix::Perspective (a frustum of the
// given half extents scaled by the near distance, with the far plane at 1000;
// the fourth argument is unused and 1000.0f is an entry of the file's .sdata2
// pool), then CMatrix::Multiply, which passes the result
// and the two matrices to the paired-single helper PSMultMatrix. CMatrix and
// the helper are named by the mangled symbols. Orthographic inlines Ident,
// which matrix.cpp defines earlier (in matrix_endian.cpp here); this fragment
// repeats that body as an inline definition so the compiler can inline it.
// The rest of the file is not part of this unit.
void PSMultMatrix(float*, const float*, const float*);
extern "C" void C_MTXFrustum(float (*)[4], float, float, float, float, float, float);

#include "game/CMatrix.h"


// Ident as in matrix_endian.cpp (see above).
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

void CMatrix::Orthographic(float w, float h, float n, float f) {
    float depth = 1.0f / (f - n);
    Ident();
    right.x = 1.0f / w;
    front.y = 1.0f / h;
    up.z = 2.0f * depth;
    pos.z = depth * (f + n);
    pos.w = 0.0f;
}

void CMatrix::Perspective(float x, float y, float n, float) {
    C_MTXFrustum((float (*)[4])((float*)&right), y * n, -y * n, -x * n, x * n, n, 1000.0f);
}

void CMatrix::Multiply(const CMatrix& a, const CMatrix& b) {
    PSMultMatrix(((float*)&right), ((float*)&a.right), ((float*)&b.right));
}
