// A fragment of matrix.cpp (0x80090194): CMatrix::RotateZ/RotateY/RotateX
// rotate the matrix in place about one axis by an angle in radians. The angle
// is negated and scaled to MathSinCos's fixed-point units (2^24 / 2pi), as in
// BuildRotZ; each pair of affected rows is rebuilt column by column from the
// old values. CMatrix is named by the mangled symbols; the row
// layout is inferred from the offsets. The fixed-point overloads before this
// range (RotateZ/Y/X(long)) inline these bodies and are not part of this unit.
void MathSinCos(long, float*, float*);

class CMatrix {
public:
    void RotateZ(float);
    void RotateY(float);
    void RotateX(float);

    float m[4][4];
};

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
