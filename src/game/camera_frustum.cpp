// A fragment of camera.cpp (0x800793e0): CFrustum::Transform transforms each
// of the frustum's ten planes by the matrix and renormalises its normal, and
// CFrustum::Set builds the planes for a camera: the near and far planes
// (normals down and up, distances the negated near and the far value, not
// renormalised), four planes through the origin from the camera's width and
// height, and four more from the width and height scaled by 2047.9375 * 2 over
// the screen's dimensions (its virtual slots at +0x28 and +0x24), each
// renormalised with a zero distance; the viewport is unused. The file name is
// this project's; the original record is camera.cpp and the rest of the file
// is in other units. CFrustum, CPlane, CCamera, CScreen, CViewport and
// CMatrix are named by the mangled symbols; the frustum as an array of ten
// 32-byte planes, the plane, camera and screen views, the normal setter and
// normalisation helpers, the locals and their declaration order are inferred
// (the normalisation as in camera_rows.cpp). The constants are entries of the
// file's .sdata2 pool.
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

    void SetNormal(float x, float y, float z) {
        m_normal.d.v[0] = x;
        m_normal.d.v[1] = y;
        m_normal.d.v[2] = z;
    }

    CVector3 m_normal;
    float m_d;
    unsigned char unknown14[12];
};

class CCamera {
public:
    unsigned char unknown00[0x24];
    float m_near;
    float m_far;
    float m_width;
    float m_height;
};

class CScreen {
public:
    virtual void vf08();
    virtual void vf0C();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1C();
    virtual void vf20();
    virtual int vf24();
    virtual int vf28();
};

class CViewport;

/* Inferred: the frustum's ten planes. */
class CFrustum {
public:
    void Transform(const CMatrix&);
    void Set(const CCamera&, CScreen&, CViewport&);

    CPlane m_planes[10];
};

void CFrustum::Transform(const CMatrix& tm) {
    for (int i = 0; i < 10; i++) {
        m_planes[i].Transform(tm);
        m_planes[i].NormalizeNormal();
    }
}

void CFrustum::Set(const CCamera& camera, CScreen& screen, CViewport&) {
    float height, width, y, x;
    m_planes[0].SetNormal(0.0f, -1.0f, 0.0f);
    m_planes[0].m_d = -camera.m_near;
    m_planes[1].SetNormal(0.0f, 1.0f, 0.0f);
    m_planes[1].m_d = camera.m_far;
    m_planes[2].SetNormal(-1.0f, -camera.m_width, 0.0f);
    m_planes[2].NormalizeNormal();
    m_planes[2].m_d = 0.0f;
    m_planes[3].SetNormal(1.0f, -camera.m_width, 0.0f);
    m_planes[3].NormalizeNormal();
    m_planes[3].m_d = 0.0f;
    m_planes[4].SetNormal(0.0f, -camera.m_height, 1.0f);
    m_planes[4].NormalizeNormal();
    m_planes[4].m_d = 0.0f;
    m_planes[5].SetNormal(0.0f, -camera.m_height, -1.0f);
    m_planes[5].NormalizeNormal();
    m_planes[5].m_d = 0.0f;
    width = camera.m_width;
    x = 2047.9375f * (2.0f * width) / screen.vf28();
    height = camera.m_height;
    y = 2047.9375f * (2.0f * height) / screen.vf24();
    m_planes[6].SetNormal(-1.0f, -x, 0.0f);
    m_planes[6].NormalizeNormal();
    m_planes[6].m_d = 0.0f;
    m_planes[7].SetNormal(1.0f, -x, 0.0f);
    m_planes[7].NormalizeNormal();
    m_planes[7].m_d = 0.0f;
    m_planes[8].SetNormal(0.0f, -y, 1.0f);
    m_planes[8].NormalizeNormal();
    m_planes[8].m_d = 0.0f;
    m_planes[9].SetNormal(0.0f, -y, -1.0f);
    m_planes[9].NormalizeNormal();
    m_planes[9].m_d = 0.0f;
}
