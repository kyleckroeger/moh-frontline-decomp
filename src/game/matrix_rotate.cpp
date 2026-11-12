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
void MathSinCos(long, float*, float*);

#include "game/CMatrix.h"


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
