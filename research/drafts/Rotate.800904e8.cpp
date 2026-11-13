// A fragment of matrix.cpp (0x8008fdc8): CMatrix::RotateZ/RotateY/RotateX,
// first the fixed-point overloads (the angle's top 24 bits are a fraction of
// a turn, 2pi/65536 per unit after the shift; the angle is converted and
// scaled, then the radian overload is inlined), then the radian overloads,
// which rotate the matrix in place about one axis. The angle is negated and
// scaled to MathSinCos's fixed-point units (2^24 / 2pi), as in BuildRotZ;
// each pair of affected rows is rebuilt column by column from the old
// values. CMatrix is named by the mangled symbols; the row layout is inferred
// from the offsets. This part of the file is compiled with deferred inlining
// (functions are emitted in reverse source order), so the radian overloads
// are defined first; the conversion's separate scaling statement is inferred.
// The constants are entries of the file's .sdata2 pool.
// The inline BuildRot body below is the landed out-of-line CMatrix::BuildRot
// (src/game/matrix_translate.cpp) respelled so that it still compiles
// byte-exact out of line: BuildRot must stay as landed. With it, Rotate is 21
// lines off (the 1.0f constant's register and s's register). MulV is an inferred
// value-returning product helper.
void MathSinCos(long, float*, float*);
void PSMultMatrix(float*, const float*, const float*);
extern "C" void* memcpy(void*, const void*, unsigned long);

#include "game/CVector3.h"


class CMatrix;
extern CMatrix _Mat_Unit;
extern const CMatrix _Mat_Data;

#include "game/CMatrix.h"


inline void CMatrix::InitClass() {
    memcpy(&_Mat_Unit, &_Mat_Data, sizeof(CMatrix));
    s_ClassInit = true;
}

inline void CMatrix::Multiply(const CMatrix& a, const CMatrix& b) {
    PSMultMatrix(&right.x, &a.right.x, &b.right.x);
}

inline float MulV(float a, float b) { return a * b; }

inline void CMatrix::BuildRot(CVector3 axis, float angle) {
    float s;
    float c;
    float tyy;
    float txz;
    float tyz;
    float sy;
    MathSinCos(2670176.8f * angle, &s, &c);
    float t = 1.0f - c;
    float tx = t * axis.x;
    float ty = t * axis.y;
    float txy = t * axis.x;
    txy *= axis.y;
    txz = tx;
    txz *= axis.z;
    tyy = ty;
    tyy *= axis.y;
    tyz = t * axis.y;
    tyz *= axis.z;
    float tz = MulV(t, axis.z);
    sy = s;
    sy *= axis.y;
    float txx = tx * axis.x;
    float sx = s * axis.x;
    float tzz = tz * axis.z;
    float sz = s;
    sz *= axis.z;


    right.x = txx + c;
    right.y = txy + sz;
    right.z = txz - sy;
    right.w = 0.0f;
    front.x = txy - sz;
    front.y = tyy + c;
    front.z = tyz + sx;
    front.w = 0.0f;
    up.x = txz + sy;
    up.y = tyz - sx;
    up.z = tzz + c;
    up.w = 0.0f;
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;
    pos.w = 1.0f;
}

void CMatrix::Rotate(CVector3 axis, float angle) {
    CMatrix rot;
    rot.BuildRot(axis, angle);
    Multiply(*this, rot);
}

void CMatrix::RotateX(float angle) {
    float s;
    float c;
    MathSinCos((long)(2670176.8f * -angle), &s, &c);
    float a0 = front.x;
    float b0 = up.x;
    front.x = c * a0 + s * b0;
    float a1 = front.y;
    float b1 = up.y;
    front.y = c * a1 + s * b1;
    float a2 = front.z;
    float b2 = up.z;
    front.z = c * a2 + s * b2;
    float a3 = front.w;
    float b3 = up.w;
    front.w = c * a3 + s * b3;
    up.x = -s * a0 + c * b0;
    up.y = -s * a1 + c * b1;
    up.z = -s * a2 + c * b2;
    up.w = -s * a3 + c * b3;
}

void CMatrix::RotateY(float angle) {
    float s;
    float c;
    MathSinCos((long)(2670176.8f * -angle), &s, &c);
    float a0 = right.x;
    float b0 = up.x;
    right.x = c * a0 + -s * b0;
    float a1 = right.y;
    float b1 = up.y;
    right.y = c * a1 + -s * b1;
    float a2 = right.z;
    float b2 = up.z;
    right.z = c * a2 + -s * b2;
    float a3 = right.w;
    float b3 = up.w;
    right.w = c * a3 + -s * b3;
    up.x = s * a0 + c * b0;
    up.y = s * a1 + c * b1;
    up.z = s * a2 + c * b2;
    up.w = s * a3 + c * b3;
}

void CMatrix::RotateZ(float angle) {
    float s;
    float c;
    MathSinCos((long)(2670176.8f * -angle), &s, &c);
    float a0 = right.x;
    float b0 = front.x;
    right.x = c * a0 + s * b0;
    float a1 = right.y;
    float b1 = front.y;
    right.y = c * a1 + s * b1;
    float a2 = right.z;
    float b2 = front.z;
    right.z = c * a2 + s * b2;
    float a3 = right.w;
    float b3 = front.w;
    right.w = c * a3 + s * b3;
    front.x = -s * a0 + c * b0;
    front.y = -s * a1 + c * b1;
    front.z = -s * a2 + c * b2;
    front.w = -s * a3 + c * b3;
}

void CMatrix::RotateX(long angle) {
    float radians = (float)(angle >> 8);
    radians *= 9.5825199e-05f;
    RotateX(radians);
}

void CMatrix::RotateY(long angle) {
    float radians = (float)(angle >> 8);
    radians *= 9.5825199e-05f;
    RotateY(radians);
}

void CMatrix::RotateZ(long angle) {
    float radians = (float)(angle >> 8);
    radians *= 9.5825199e-05f;
    RotateZ(radians);
}
