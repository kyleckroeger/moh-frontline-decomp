// A fragment of camera.cpp (0x800793e0): CFrustum::Transform transforms each
// of the frustum's ten planes by the matrix and renormalises its normal. The
// file name is this project's; the original record is camera.cpp and the rest
// of the file is in other units. CFrustum, CPlane and CMatrix are named by the
// mangled symbols; the frustum as an array of ten 32-byte planes, the plane
// view and the normalisation helper are inferred (as in camera_rows.cpp).
#include <math.h>

class CVector3 {
public:
    union {
        float v[4];
        double pair[2];
    } d;
} __attribute__((aligned(8)));

class CMatrix;

/* Inferred: a plane, its normal first (32 bytes). */
class CPlane {
public:
    void Transform(const CMatrix&);
    void NormalizeNormal() {
        float length = sqrtf(m_normal.d.v[0] * m_normal.d.v[0] + m_normal.d.v[1] * m_normal.d.v[1] + m_normal.d.v[2] * m_normal.d.v[2]);
        if (length != 0.0f) {
            m_normal.d.v[0] *= 1.0f / length;
            m_normal.d.v[1] *= 1.0f / length;
            m_normal.d.v[2] *= 1.0f / length;
        }
    }

    CVector3 m_normal;
    unsigned char unknown10[16];
};

/* Inferred: the frustum's ten planes. */
class CFrustum {
public:
    void Transform(const CMatrix&);

    CPlane m_planes[10];
};

void CFrustum::Transform(const CMatrix& tm) {
    for (int i = 0; i < 10; i++) {
        m_planes[i].Transform(tm);
        m_planes[i].NormalizeNormal();
    }
}
