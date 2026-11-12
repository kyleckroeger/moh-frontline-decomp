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
// helpers are named by the mangled symbols; the 4x4 float layout is as in
// matrix_endian.cpp. The rest of the file is not part
// of this unit.

// CVector3 is the shared game/CVector3.h (its copy constructor copies the
// 8-byte-aligned base, see research/notes/cvector3-header-variants.md). Sub,
// Scale, Dot, Length and Normalize are the original's CVector3 members (Sub
// and Scale write into this, as the weak copies at 800a4dac and 800a4cec do).
#include <math.h>



#include "game/CVector3.h"


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
    m[0][0] = right.x; m[0][1] = right.y; m[0][2] = right.z;
    m[1][0] = f.x; m[1][1] = f.y; m[1][2] = f.z;
    m[2][0] = u.x; m[2][1] = u.y; m[2][2] = u.z;
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
    m[3][0] = -(pos.x * right.x + pos.y * right.y + pos.z * right.z);
    m[3][1] = -(pos.x * up.x + pos.y * up.y + pos.z * up.z);
    m[3][2] = -(pos.x * front.x + pos.y * front.y + pos.z * front.z);
    m[3][3] = 1.0f;
}

CVector3 CMatrix::TransformToLocal(CVector3 v) const {
    CVector3 result;
    CVector3 delta;
    delta.z = v.z - m[3][2];
    delta.y = v.y - m[3][1];
    delta.x = v.x - m[3][0];
    PSTransformToLocal(&result.x, m[0], &delta.x);
    result.w = 1.0f;
    return result;
}

void CMatrix::TransformVector(CVector3& result, CVector3 v) const {
    PSTransformVector(&result.x, m[0], &v.x);
}

void CMatrix::TransformPoint(CVector3& result, CVector3 v) const {
    PSTransformPoint(&result.x, m[0], &v.x);
}

void CMatrix::Transform(CVector4& result, CVector3 v) const {
    result.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0];
    result.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1];
    result.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2];
    result.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + m[3][3];
}
