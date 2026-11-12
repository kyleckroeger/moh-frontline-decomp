// A fragment of matrix.cpp (0x8008f1e8): CMatrix::Orthonormalize (Gram-Schmidt
// on the rows: right normalized, forward made orthogonal to it, up to both,
// each normalized; the position row is left alone), Transpose (in place),
// Inverse (the general inverse of an affine matrix: the 3x3 determinant
// accumulated from its positive and negative terms, nothing done when it is
// zero or below 1e-15 relative to their difference, then the scaled adjoint
// and the translation through it; the constants are entries of the file's
// .sdata2 pool), FastInverse (the transpose of the rotation and the
// translation through it, with the last column 0, 0, 0, 1), TransformToLocal
// (the point relative to the matrix position through the paired-single
// helper PSTransformToLocal, with w set to 1), TransformVector and
// TransformPoint, which pass the result, the matrix and the vector to the
// paired-single helpers PSTransformVector and PSTransformPoint, and Transform
// (a point to homogeneous coordinates, in C). CMatrix, CVector3 and the
// helpers are named by the mangled symbols; the CVector3 view and the 4x4
// float layout are as in matrix_endian.cpp. The rest of the file is not part
// of this unit.

// Inferred: CVector3 (no constructors, so its copies stay plain doubleword
// moves; the shared header's copy constructor recolours Orthonormalize) as
// four floats overlaid with two doubles. Sub, Scale, Dot, Length and
// Normalize are the original's CVector3 members (Sub and Scale write into
// this, as the weak copies at 800a4dac and 800a4cec do); their bodies here
// are inferred from those copies and inline as in the original.
#include <math.h>

union CVector3Data {
    double pair[2];
    float v[4];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    float Dot(const CVector3& v) const { return d.c.x * v.d.c.x + d.c.y * v.d.c.y + d.c.z * v.d.c.z; }
    float Length() const { return sqrtf(d.c.x * d.c.x + d.c.y * d.c.y + d.c.z * d.c.z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            d.c.x *= scale;
            d.c.y *= scale;
            d.c.z *= scale;
        }
    }
    void Scale(const CVector3& o, float k) { d.c.x = k * o.d.c.x; d.c.y = k * o.d.c.y; d.c.z = k * o.d.c.z; }
    void Sub(const CVector3& a, const CVector3& b) { d.c.x = a.d.c.x - b.d.c.x; d.c.y = a.d.c.y - b.d.c.y; d.c.z = a.d.c.z - b.d.c.z; }

    CVector3Data d;
} __attribute__((aligned(8)));

// Inferred: a four-float vector.
class CVector4 {
public:
    float x;
    float y;
    float z;
    float w;
};

void PSTransformToLocal(float*, const float*, const float*);
void PSTransformVector(float*, const float*, const float*);
void PSTransformPoint(float*, const float*, const float*);

class CMatrix {
public:
    void Orthonormalize();
    void Transpose();
    void Inverse(const CMatrix&);
    const CVector3& Row(int i) const { return *(const CVector3*)m[i]; }
    void FastInverse(const CMatrix&);
    CVector3 TransformToLocal(CVector3) const;
    void TransformVector(CVector3&, CVector3) const;
    void TransformPoint(CVector3&, CVector3) const;
    void Transform(CVector4&, CVector3) const;

    float m[4][4];
} __attribute__((aligned(16)));

void CMatrix::Orthonormalize() {
    CVector3 forward = Row(1);
    CVector3 up = Row(2);
    CVector3 right = Row(0);
    right.Normalize();
    CVector3 t;
    t.Scale(right, forward.Dot(right));
    CVector3 f;
    f.Sub(forward, t);
    f.Normalize();
    CVector3 u;
    t.Scale(right, up.Dot(right));
    u.Sub(up, t);
    t.Scale(f, up.Dot(f));
    u.Sub(u, t);
    u.Normalize();
    m[0][0] = right.d.c.x; m[0][1] = right.d.c.y; m[0][2] = right.d.c.z;
    m[1][0] = f.d.c.x; m[1][1] = f.d.c.y; m[1][2] = f.d.c.z;
    m[2][0] = u.d.c.x; m[2][1] = u.d.c.y; m[2][2] = u.d.c.z;
}

void CMatrix::Transpose() {
    for (int i = 1; i < 4; i++) {
        for (int j = 0; j < i; j++) {
            float t = m[i][j];
            m[i][j] = m[j][i];
            m[j][i] = t;
        }
    }
}

void CMatrix::Inverse(const CMatrix& in) {
    float pos = 0.0f;
    float neg = 0.0f;
    float temp;
    temp = in.m[0][0] * in.m[1][1] * in.m[2][2];
    if (temp > 0.0f)
        pos += temp;
    else
        neg += temp;
    temp = in.m[0][1] * in.m[1][2] * in.m[2][0];
    if (temp > 0.0f)
        pos += temp;
    else
        neg += temp;
    temp = in.m[0][2] * in.m[1][0] * in.m[2][1];
    if (temp > 0.0f)
        pos += temp;
    else
        neg += temp;
    temp = -in.m[0][2] * in.m[1][1] * in.m[2][0];
    if (temp > 0.0f)
        pos += temp;
    else
        neg += temp;
    temp = -in.m[0][1] * in.m[1][0] * in.m[2][2];
    if (temp > 0.0f)
        pos += temp;
    else
        neg += temp;
    temp = -in.m[0][0] * in.m[1][2] * in.m[2][1];
    if (temp > 0.0f)
        pos += temp;
    else
        neg += temp;
    float det = pos + neg;
    if (det == 0.0 || (float)__fabs(det / (pos - neg)) < 1e-15f)
        return;
    {
        det = 1.0f / det;
        m[0][0] = det * (in.m[1][1] * in.m[2][2] - in.m[1][2] * in.m[2][1]);
        m[0][1] = det * -(in.m[0][1] * in.m[2][2] - in.m[0][2] * in.m[2][1]);
        m[0][2] = det * (in.m[0][1] * in.m[1][2] - in.m[0][2] * in.m[1][1]);
        m[0][3] = 0.0f;
        m[1][0] = det * -(in.m[1][0] * in.m[2][2] - in.m[1][2] * in.m[2][0]);
        m[1][1] = det * (in.m[0][0] * in.m[2][2] - in.m[0][2] * in.m[2][0]);
        m[1][2] = det * -(in.m[0][0] * in.m[1][2] - in.m[0][2] * in.m[1][0]);
        m[1][3] = 0.0f;
        m[2][0] = det * (in.m[1][0] * in.m[2][1] - in.m[1][1] * in.m[2][0]);
        m[2][1] = det * -(in.m[0][0] * in.m[2][1] - in.m[0][1] * in.m[2][0]);
        m[2][2] = det * (in.m[0][0] * in.m[1][1] - in.m[0][1] * in.m[1][0]);
        m[2][3] = 0.0f;
        m[3][0] = -(in.m[3][0] * m[0][0] + in.m[3][1] * m[1][0] + in.m[3][2] * m[2][0]);
        m[3][1] = -(in.m[3][0] * m[0][1] + in.m[3][1] * m[1][1] + in.m[3][2] * m[2][1]);
        m[3][2] = -(in.m[3][0] * m[0][2] + in.m[3][1] * m[1][2] + in.m[3][2] * m[2][2]);
        m[3][3] = 1.0f;
    }
}

void CMatrix::FastInverse(const CMatrix& src) {
    CVector3 right = src.Row(0);
    CVector3 up = src.Row(1);
    CVector3 front = src.Row(2);
    CVector3 pos = src.Row(3);
    m[0][0] = src.m[0][0];
    m[0][1] = src.m[1][0];
    m[0][2] = src.m[2][0];
    m[0][3] = 0.0f;
    m[1][0] = src.m[0][1];
    m[1][1] = src.m[1][1];
    m[1][2] = src.m[2][1];
    m[1][3] = 0.0f;
    m[2][0] = src.m[0][2];
    m[2][1] = src.m[1][2];
    m[2][2] = src.m[2][2];
    m[2][3] = 0.0f;
    m[3][0] = -(pos.d.v[0] * right.d.v[0] + pos.d.v[1] * right.d.v[1] + pos.d.v[2] * right.d.v[2]);
    m[3][1] = -(pos.d.v[0] * up.d.v[0] + pos.d.v[1] * up.d.v[1] + pos.d.v[2] * up.d.v[2]);
    m[3][2] = -(pos.d.v[0] * front.d.v[0] + pos.d.v[1] * front.d.v[1] + pos.d.v[2] * front.d.v[2]);
    m[3][3] = 1.0f;
}

CVector3 CMatrix::TransformToLocal(CVector3 v) const {
    CVector3 result;
    CVector3 delta;
    delta.d.v[2] = v.d.v[2] - m[3][2];
    delta.d.v[1] = v.d.v[1] - m[3][1];
    delta.d.v[0] = v.d.v[0] - m[3][0];
    PSTransformToLocal(result.d.v, m[0], delta.d.v);
    result.d.v[3] = 1.0f;
    return result;
}

void CMatrix::TransformVector(CVector3& result, CVector3 v) const {
    PSTransformVector(result.d.v, m[0], v.d.v);
}

void CMatrix::TransformPoint(CVector3& result, CVector3 v) const {
    PSTransformPoint(result.d.v, m[0], v.d.v);
}

void CMatrix::Transform(CVector4& result, CVector3 v) const {
    result.x = v.d.v[0] * m[0][0] + v.d.v[1] * m[1][0] + v.d.v[2] * m[2][0] + m[3][0];
    result.y = v.d.v[0] * m[0][1] + v.d.v[1] * m[1][1] + v.d.v[2] * m[2][1] + m[3][1];
    result.z = v.d.v[0] * m[0][2] + v.d.v[1] * m[1][2] + v.d.v[2] * m[2][2] + m[3][2];
    result.w = v.d.v[0] * m[0][3] + v.d.v[1] * m[1][3] + v.d.v[2] * m[2][3] + m[3][3];
}
