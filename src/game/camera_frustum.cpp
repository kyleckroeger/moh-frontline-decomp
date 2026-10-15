// A fragment of camera.cpp (0x800793e0): CFrustum::Transform transforms each
// of the frustum's ten planes by the matrix and renormalises its normal, and
// CFrustum::Set builds the planes for a camera: the near and far planes
// (normals down and up, distances the negated near and the far value, not
// renormalised), four planes through the origin from the camera's width and
// height, and four more from the width and height scaled by 2047.9375 * 2 over
// the screen's dimensions (its virtual slots at +0x28 and +0x24), each
// renormalised with a zero distance; the viewport is unused. Then
// CCamera::UpdateWorldToCamera: the camera's transform turned half a turn
// about Z, its right row negated, the world-to-camera matrix's rows set from
// the transposed rows and its position from the negated dot products of the
// rows with the position, and the matrix marked current. The file name is
// this project's; the original record is camera.cpp and the rest of the file
// is in other units. CFrustum, CPlane, CCamera, CScreen, CViewport and
// CMatrix are named by the mangled symbols; the frustum as an array of ten
// 32-byte planes, the plane, camera and screen views, the normal setter and
// normalisation helpers, the locals and their declaration order are inferred
// (the normalisation as in camera_rows.cpp). The constants are entries of the
// file's .sdata2 pool.
//
// CVector3 view: four floats overlaid with two doubles, 16-byte aligned (the
// vector locals sit at 16-byte offsets in a 16-aligned frame), with a
// constructor and a copy constructor that moves the double pair (arguments
// are built in place and copies are lfd/stfd pairs). The alignment, the
// members and the Dot helper are inferred, as is the placement of the row
// copies after the right row is negated.
#include <math.h>

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
    }
    void operator*=(float s) {
        d.v[0] *= s;
        d.v[1] *= s;
        d.v[2] *= s;
    }

    union {
        float v[4];
        double pair[2];
    } d;
} __attribute__((aligned(16)));

inline float Dot(const CVector3& a, const CVector3& b) {
    return a.d.v[0] * b.d.v[0] + a.d.v[1] * b.d.v[1] + a.d.v[2] * b.d.v[2];
}

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);
    void RotateZ(float);
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

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
    void UpdateWorldToCamera();

    unsigned char unknown00[0x24];
    float m_near;
    float m_far;
    float m_width;
    float m_height;
    unsigned char unknown34[12];
    CMatrix m_tm;
    CMatrix m_worldToCamera;
    CMatrix m_projection;
    CMatrix m_worldToScreen;
    bool m_perspective;
    bool m_flag141;
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

void CCamera::UpdateWorldToCamera() {
    CMatrix m;
    m = m_tm;
    m.RotateZ(3.1415927f);
    CVector3 right = m.right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
    right *= -1.0f;
    forward = m.forward;
    up = m.up;
    position = m.position;
    m_worldToCamera.SetRight(CVector3(right.d.v[0], up.d.v[0], forward.d.v[0]));
    m_worldToCamera.SetFront(CVector3(right.d.v[1], up.d.v[1], forward.d.v[1]));
    m_worldToCamera.SetUp(CVector3(right.d.v[2], up.d.v[2], forward.d.v[2]));
    CVector3 t(Dot(right, position), Dot(up, position), Dot(forward, position));
    t.d.v[0] = -t.d.v[0];
    t.d.v[1] = -t.d.v[1];
    t.d.v[2] = -t.d.v[2];
    m_worldToCamera.SetPos(t);
    m_flag141 = true;
}
