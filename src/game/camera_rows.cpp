// A fragment of camera.cpp (0x80079e7c): CCamera::SetPerspective (the
// projection's half extents from the tangents of the half angles, through the
// double tan; marks the projection stale, clears +0x143 and selects the
// perspective projection), then CCamera's transform wrappers, from
// Orthonormalize to CommitUpdate, each forwarding to the object's transform
// (a CMatrix at +64) and then clearing two flags (+0x141 and +0x143; their
// meaning is unknown), with IsVisible and IsDrawEnabled (both false) and an
// empty Draw among them, then AttemptUpdate (the world-to-camera matrix
// rebuilt when stale, the projection rebuilt as a perspective with the far
// plane scaled by 100 or as an orthographic one, the world-to-screen matrix
// and the ten frustum planes transformed and renormalised, and outside
// multiplayer the camera's position copied to four followers). The file
// name is this project's; the original record is camera.cpp and the functions
// around these are not reconstructed. CCamera and CMatrix's methods are named by
// the mangled symbols; CCamera is an inferred non-virtual view with only the
// transform declared, the bodies are inferred from the calls, and the by-value
// row getters are inferred inline helpers. For AttemptUpdate the camera's
// members (followers, near/far/size, matrices, flags and planes), the plane
// and follower views and the plane-normal normalisation helper are
// inferred; the camera's GetPosition is reached through a view of its table
// (slot 14) because CCamera is a non-virtual view here. 100.0f, 0.0f and
// 1.0f are entries of the file's .sdata2 pool.
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles (a
// named union member; CW mishandles anonymous ones). The union is inferred
// from the code, not the original declaration: vectors are copied as two
// lfd/stfd pairs, which an implicit copy through the double pair reproduces.
#include <math.h>

extern "C" double tan(double);

class CVector3 {
public:
    union {
        float v[4];
        double pair[2];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);
    void Orthographic(float, float, float, float);
    void Perspective(float, float, float, float);
    void Orthonormalize();
    void RotateX(float);
    void RotateY(float);
    void RotateZ(float);
    void Rotate(CVector3, float);
    void Translate(CVector3);
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);
    void Ident();

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class CDrawContext;

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

/* Inferred: an object whose +8 float follows the camera. */
struct CameraFollowerView {
    unsigned char unknown00[8];
    float value;
};

/* Inferred: the camera's virtual GetPosition (table slot 14). */
class CameraNodeView {
public:
    virtual void Slot02();
    virtual void Slot03();
    virtual void Slot04();
    virtual void Slot05();
    virtual void Slot06();
    virtual void Slot07();
    virtual void Slot08();
    virtual void Slot09();
    virtual void Slot10();
    virtual void Slot11();
    virtual void Slot12();
    virtual void Slot13();
    virtual void Slot14();
    virtual void Slot15();
    virtual void GetPosition(CVector3&) const;
};

extern bool g_bInMultiplayerMode;

class CCamera {
public:
    void SetPerspective(float, float);
    bool IsVisible(CDrawContext&) const;
    bool IsDrawEnabled() const;
    void Draw(CDrawContext&);
    void Orthonormalize();
    void Yaw(float);
    void Roll(float);
    void Pitch(float);
    void Rotate(CVector3, float);
    void Move(CVector3);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetPosition(CVector3);
    void SetTMLocalToWorld(const CMatrix&);
    void Transform(const CMatrix&);
    void Reset();
    void PreTransform(const CMatrix&);
    void GetUpward(CVector3&) const;
    void GetForward(CVector3&) const;
    void GetRightward(CVector3&) const;
    void GetPosition(CVector3&) const;
    void GetTMLocalToWorld(CMatrix&) const;
    void CommitUpdate();

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.forward; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.position; }

    void UpdateWorldToCamera();
    void AttemptUpdate(float);

    unsigned char unknown00[12];
    CameraFollowerView* m_follower0c;
    CameraFollowerView* m_follower10;
    CameraFollowerView* m_follower14;
    CameraFollowerView* m_follower18;
    unsigned char unknown1c[8];
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
    bool m_projectionValid;
    bool m_flag143;
    unsigned char unknown144[12];
    CPlane m_planes[10];
};

void CCamera::SetPerspective(float horizontal, float vertical) {
    m_width = tan(horizontal);
    m_height = tan(vertical);
    m_projectionValid = false;
    m_flag143 = false;
    m_perspective = true;
}

void CCamera::Orthonormalize() {
    m_tm.Orthonormalize();
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Yaw(float angle) {
    m_tm.RotateZ(angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Roll(float angle) {
    m_tm.RotateY(angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Pitch(float angle) {
    m_tm.RotateX(angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Rotate(CVector3 axis, float angle) {
    m_tm.Rotate(axis, angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Move(CVector3 delta) {
    m_tm.Translate(delta);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::SetBasis(CVector3 right, CVector3 forward, CVector3 up) {
    m_tm.SetRight(right);
    m_tm.SetFront(forward);
    m_tm.SetUp(up);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::SetPosition(CVector3 position) {
    m_tm.SetPos(position);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::SetTMLocalToWorld(const CMatrix& matrix) {
    m_tm = matrix;
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Transform(const CMatrix& matrix) {
    m_tm.Multiply(m_tm, matrix);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Reset() {
    m_tm.Ident();
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::PreTransform(const CMatrix& matrix) {
    m_tm.Multiply(matrix, m_tm);
    m_flag141 = false;
    m_flag143 = false;
}

bool CCamera::IsVisible(CDrawContext&) const {
    return false;
}

bool CCamera::IsDrawEnabled() const {
    return false;
}

void CCamera::GetUpward(CVector3& v) const {
    v = UpRow();
}

void CCamera::GetForward(CVector3& v) const {
    v = ForwardRow();
}

void CCamera::GetRightward(CVector3& v) const {
    v = RightRow();
}

void CCamera::GetPosition(CVector3& v) const {
    v = PositionRow();
}

void CCamera::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}

void CCamera::Draw(CDrawContext&) {
}

void CCamera::CommitUpdate() {
}

void CCamera::AttemptUpdate(float) {
    if (!m_flag141)
        UpdateWorldToCamera();
    if (!m_projectionValid) {
        if (m_perspective)
            m_projection.Perspective(m_width, m_height, m_near, 100.0f * m_far);
        else
            m_projection.Orthographic(m_width, m_height, m_near, m_far);
        m_projectionValid = true;
    }
    if (!m_flag143) {
        m_worldToScreen.Multiply(m_worldToCamera, m_projection);
        int i;
        CPlane* plane = m_planes;
        for (i = 0; i < 10; i++) {
            plane->Transform(m_tm);
            plane->NormalizeNormal();
            plane++;
        }
        m_flag143 = true;
    }
    if (!g_bInMultiplayerMode) {
        CVector3 position;
        ((const CameraNodeView*)this)->GetPosition(position);
        m_follower0c->value = position.d.v[0];
        m_follower14->value = position.d.v[1];
        m_follower10->value = position.d.v[0];
        m_follower18->value = position.d.v[1];
    }
}
