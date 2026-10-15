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

class CMatrix {
public:
    void RotateZ(long);
    void RotateY(long);
    void RotateX(long);
    void RotateZ(float);
    void RotateY(float);
    void RotateX(float);

    float m[4][4];
};

void CMatrix::RotateX(float angle) {
    float s;
    float c;
    MathSinCos((long)(2670176.8f * -angle), &s, &c);
    float a0 = m[1][0];
    float b0 = m[2][0];
    m[1][0] = c * a0 + s * b0;
    float a1 = m[1][1];
    float b1 = m[2][1];
    m[1][1] = c * a1 + s * b1;
    float a2 = m[1][2];
    float b2 = m[2][2];
    m[1][2] = c * a2 + s * b2;
    float a3 = m[1][3];
    float b3 = m[2][3];
    m[1][3] = c * a3 + s * b3;
    m[2][0] = -s * a0 + c * b0;
    m[2][1] = -s * a1 + c * b1;
    m[2][2] = -s * a2 + c * b2;
    m[2][3] = -s * a3 + c * b3;
}

void CMatrix::RotateY(float angle) {
    float s;
    float c;
    MathSinCos((long)(2670176.8f * -angle), &s, &c);
    float a0 = m[0][0];
    float b0 = m[2][0];
    m[0][0] = c * a0 + -s * b0;
    float a1 = m[0][1];
    float b1 = m[2][1];
    m[0][1] = c * a1 + -s * b1;
    float a2 = m[0][2];
    float b2 = m[2][2];
    m[0][2] = c * a2 + -s * b2;
    float a3 = m[0][3];
    float b3 = m[2][3];
    m[0][3] = c * a3 + -s * b3;
    m[2][0] = s * a0 + c * b0;
    m[2][1] = s * a1 + c * b1;
    m[2][2] = s * a2 + c * b2;
    m[2][3] = s * a3 + c * b3;
}

void CMatrix::RotateZ(float angle) {
    float s;
    float c;
    MathSinCos((long)(2670176.8f * -angle), &s, &c);
    float a0 = m[0][0];
    float b0 = m[1][0];
    m[0][0] = c * a0 + s * b0;
    float a1 = m[0][1];
    float b1 = m[1][1];
    m[0][1] = c * a1 + s * b1;
    float a2 = m[0][2];
    float b2 = m[1][2];
    m[0][2] = c * a2 + s * b2;
    float a3 = m[0][3];
    float b3 = m[1][3];
    m[0][3] = c * a3 + s * b3;
    m[1][0] = -s * a0 + c * b0;
    m[1][1] = -s * a1 + c * b1;
    m[1][2] = -s * a2 + c * b2;
    m[1][3] = -s * a3 + c * b3;
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
