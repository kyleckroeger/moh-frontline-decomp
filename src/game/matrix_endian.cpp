// A fragment of matrix.cpp (0x8008e9ec): the file's paired-single helpers
// PSMultMatrix (a 4x4 product, using f14-f19 without saving them, as the
// image does), PSTransformPoint, PSTransformVector and PSTransformToLocal
// (the point through the transposed rotation), written as asm functions from
// the image; then CMatrix::EndianSwap (each element,
// column by column), GetGAMECUBEMatrix34 (the transposed upper 3x4) and
// TibToMOHFL (negates the first row and swaps the second and third), then
// ToEulerXZY and ToEulerXYZ (asin/atan2 of the rotation entries, with the
// +-pi/2 cases when the sine entry reaches +-1; the constants are entries of
// the file's .sdata2 pool), and Ident (paired-single stores of 0 and 1, as in
// the SDK's PSMTXIdentity but for all four rows; the asm block ends with the
// nop the image has, which the SDK version does not). CMatrix
// is named by the mangled symbols; the 4x4 float layout is inferred. The
// byte-order helpers are the inlined ones described in propdat.cpp (inferred).
extern "C" double atan2(double, double);
extern "C" double asin(double);

asm void PSMultMatrix(register float* dst, register const float* a, register const float* b) {
    nofralloc
    psq_l f0,0(a),0,0
    psq_l f8,0(b),0,0
    ps_muls0 f16,f8,f0
    psq_l f9,16(b),0,0
    psq_l f10,32(b),0,0
    ps_madds1 f16,f9,f0,f16
    psq_l f1,8(a),0,0
    psq_l f11,48(b),0,0
    ps_madds0 f16,f10,f1,f16
    ps_madds1 f16,f11,f1,f16
    psq_st f16,0(dst),0,0
    psq_l f2,16(a),0,0
    psq_l f3,24(a),0,0
    ps_muls0 f17,f8,f2
    ps_madds1 f17,f9,f2,f17
    ps_madds0 f17,f10,f3,f17
    ps_madds1 f17,f11,f3,f17
    psq_st f17,16(dst),0,0
    psq_l f4,32(a),0,0
    psq_l f5,40(a),0,0
    ps_muls0 f18,f8,f4
    ps_madds1 f18,f9,f4,f18
    ps_madds0 f18,f10,f5,f18
    ps_madds1 f18,f11,f5,f18
    psq_st f18,32(dst),0,0
    psq_l f6,48(a),0,0
    psq_l f7,56(a),0,0
    ps_muls0 f19,f8,f6
    ps_madds1 f19,f9,f6,f19
    ps_madds0 f19,f10,f7,f19
    ps_madds1 f19,f11,f7,f19
    psq_st f19,48(dst),0,0
    psq_l f12,8(b),0,0
    ps_muls0 f16,f12,f0
    psq_l f13,24(b),0,0
    psq_l f14,40(b),0,0
    ps_madds1 f16,f13,f0,f16
    psq_l f15,56(b),0,0
    ps_madds0 f16,f14,f1,f16
    ps_madds1 f16,f15,f1,f16
    psq_st f16,8(dst),0,0
    ps_muls0 f17,f12,f2
    ps_madds1 f17,f13,f2,f17
    ps_madds0 f17,f14,f3,f17
    ps_madds1 f17,f15,f3,f17
    psq_st f17,24(dst),0,0
    ps_muls0 f18,f12,f4
    ps_madds1 f18,f13,f4,f18
    ps_madds0 f18,f14,f5,f18
    ps_madds1 f18,f15,f5,f18
    psq_st f18,40(dst),0,0
    ps_muls0 f19,f12,f6
    ps_madds1 f19,f13,f6,f19
    ps_madds0 f19,f14,f7,f19
    ps_madds1 f19,f15,f7,f19
    psq_st f19,56(dst),0,0
    blr
}

asm void PSTransformPoint(register float* dst, register const float* m, register const float* v) {
    nofralloc
    psq_l f0,0(m),0,0
    psq_l f2,16(m),0,0
    psq_l f4,32(m),0,0
    psq_l f6,48(m),0,0
    lfs f1,8(m)
    lfs f3,24(m)
    lfs f5,40(m)
    lfs f7,56(m)
    psq_l f8,0(v),0,0
    ps_merge11 f9,f8,f8
    ps_merge00 f8,f8,f8
    lfs f10,8(v)
    ps_muls0 f11,f0,f8
    ps_madds1 f11,f2,f9,f11
    ps_madds0 f11,f4,f10,f11
    ps_add f11,f6,f11
    fmuls f12,f1,f8
    fmadds f12,f3,f9,f12
    fmadds f12,f5,f10,f12
    fadds f12,f7,f12
    psq_st f11,0(dst),0,0
    stfs f12,8(dst)
    blr
}

asm void PSTransformVector(register float* dst, register const float* m, register const float* v) {
    nofralloc
    psq_l f0,0(m),0,0
    psq_l f2,16(m),0,0
    psq_l f4,32(m),0,0
    lfs f1,8(m)
    lfs f3,24(m)
    lfs f5,40(m)
    psq_l f8,0(v),0,0
    ps_merge11 f9,f8,f8
    ps_merge00 f8,f8,f8
    lfs f10,8(v)
    ps_muls0 f11,f0,f8
    ps_madds1 f11,f2,f9,f11
    ps_madds0 f11,f4,f10,f11
    fmuls f12,f1,f8
    fmadds f12,f3,f9,f12
    fmadds f12,f5,f10,f12
    psq_st f11,0(dst),0,0
    stfs f12,8(dst)
    blr
}

asm void PSTransformToLocal(register float* dst, register const float* m, register const float* v) {
    nofralloc
    psq_l f0,0(m),0,0
    psq_l f2,16(m),0,0
    psq_l f4,32(m),0,0
    lfs f1,8(m)
    lfs f3,24(m)
    lfs f5,40(m)
    ps_merge10 f8,f0,f0
    ps_merge10 f9,f2,f2
    ps_merge01 f0,f0,f9
    ps_merge01 f2,f8,f2
    ps_mr f8,f1
    ps_merge01 f1,f4,f1
    ps_merge01 f4,f8,f4
    ps_merge10 f8,f4,f4
    ps_merge10 f9,f3,f3
    ps_merge01 f4,f4,f9
    ps_merge01 f3,f8,f3
    psq_l f8,0(v),0,0
    ps_merge11 f9,f8,f8
    ps_merge00 f8,f8,f8
    lfs f10,8(v)
    ps_muls0 f11,f0,f8
    ps_madds1 f11,f2,f9,f11
    ps_madds0 f11,f4,f10,f11
    fmuls f12,f1,f8
    fmadds f12,f3,f9,f12
    fmadds f12,f5,f10,f12
    psq_st f11,0(dst),0,0
    stfs f12,8(dst)
    blr
}

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

#include "game/CMatrix.h"


void CMatrix::EndianSwap() {
    ChangeEndian(right.x);
    ChangeEndian(front.x);
    ChangeEndian(up.x);
    ChangeEndian(pos.x);
    ChangeEndian(right.y);
    ChangeEndian(front.y);
    ChangeEndian(up.y);
    ChangeEndian(pos.y);
    ChangeEndian(right.z);
    ChangeEndian(front.z);
    ChangeEndian(up.z);
    ChangeEndian(pos.z);
    ChangeEndian(right.w);
    ChangeEndian(front.w);
    ChangeEndian(up.w);
    ChangeEndian(pos.w);
}

void CMatrix::GetGAMECUBEMatrix34(float (&out)[3][4]) const {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++)
            out[i][j] = ((float(*)[4])&right)[j][i];
    }
}

void CMatrix::TibToMOHFL() {
    right.x = -right.x;
    right.y = -right.y;
    right.z = -right.z;
    for (int i = 0; i < 3; i++) {
        float t = ((float(*)[4])&right)[1][i];
        ((float(*)[4])&right)[1][i] = ((float(*)[4])&right)[2][i];
        ((float(*)[4])&right)[2][i] = t;
    }
}

void CMatrix::ToEulerXZY(float& x, float& y, float& z) const {
    if (right.y < 1.0f) {
        if (right.y > -1.0f) {
            y = atan2(-right.z, right.x);
            z = asin(right.y);
            x = atan2(-up.y, front.y);
        } else {
            x = -(float)atan2(front.z, up.z);
            z = -1.5707964f;
            y = 0.0f;
        }
    } else {
        x = atan2(front.z, up.z);
        z = 1.5707964f;
        y = 0.0f;
    }
}

void CMatrix::ToEulerXYZ(float& x, float& y, float& z) const {
    if (right.z < 1.0f) {
        if (right.z > -1.0f) {
            x = asin(-front.z);
            y = atan2(-right.z, up.z);
            z = atan2(right.y, right.x);
        } else {
            x = -1.5707964f;
            y = -(float)atan2(front.x, front.y);
            z = 0.0f;
        }
    } else {
        x = 1.5707964f;
        y = atan2(front.x, front.y);
        z = 0.0f;
    }
}

void CMatrix::Ident() {
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
