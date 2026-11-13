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

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix;
extern CMatrix _Mat_Unit;
extern const CMatrix _Mat_Data;

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void BuildRot(CVector3, float);
    void Multiply(const CMatrix&, const CMatrix&);
    void Rotate(CVector3, float);
    void RotateZ(long);
    void RotateY(long);
    void RotateX(long);
    void RotateZ(float);
    void RotateY(float);
    void RotateX(float);

    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

inline void CMatrix::InitClass() {
    memcpy(&_Mat_Unit, &_Mat_Data, sizeof(CMatrix));
    s_ClassInit = true;
}

inline void CMatrix::Multiply(const CMatrix& a, const CMatrix& b) {
    PSMultMatrix(&m[0][0], &a.m[0][0], &b.m[0][0]);
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


    m[0][0] = txx + c;
    m[0][1] = txy + sz;
    m[0][2] = txz - sy;
    m[0][3] = 0.0f;
    m[1][0] = txy - sz;
    m[1][1] = tyy + c;
    m[1][2] = tyz + sx;
    m[1][3] = 0.0f;
    m[2][0] = txz + sy;
    m[2][1] = tyz - sx;
    m[2][2] = tzz + c;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
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
