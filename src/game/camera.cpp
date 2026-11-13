// camera.cpp (0x800793e0..0x8007a5f8): the view frustum and the camera. CFrustum::Transform
// moves the ten planes by a matrix and renormalises them; CFrustum::Set builds them from the
// camera (near, far, the four side planes from the half-width and half-height slopes, and four
// more from the screen's pixel size). CCamera: UpdateWorldToCamera (the inverse of the
// camera transform turned half a circle about z), IsPointVisible, SetPerspective, the
// IMovingSceneNode transform overrides (each marks the view and the frustum stale), the row
// accessors, AttemptUpdate (rebuilds the view, projection, world-to-screen matrix and the
// frustum, and outside multiplayer stores the camera's x/y in the sweep-extent records),
// BeginUpdate and the destructor; CCamera::ASPECT_4x3. Merged from the fragments
// camera_frustum, camera_rows and camera_dtor; it defines __vt__7CCamera.
//
// Names from the mangled symbols and the vtable. Inferred: the file-private CMatrix (below),
// CCamera's planes as a CFrustum member (below), NormalizeNormal, the draw-context and
// sweep-extent views. The file compiles with RTTI on and '-inline deferred' (no auto): the
// original numbers the RTTI records before every function's, and AttemptUpdate keeps its call
// to UpdateWorldToCamera; the definitions are written last to first (the compiler emits them
// in reverse).
#include <math.h>
extern "C" double tan(double);

#include "game/CVector3.h"

// File-private CMatrix (inferred): the shared header's, except that the default constructor
// does nothing. UpdateWorldToCamera's local matrix has no constructor call in the original
// (the shared default constructor runs the class initialisation check).
#define GAME_CMATRIX_H


#include "game/CVector3.h"

class CVector4;

class CMatrix {
public:
    // The original has a weak __defctor__7CMatrixFv (0x80095b40): the default constructor
    // takes a defaulted argument, and with it the body is only the class-init check.
    // Inferred: the argument asks for the identity (a non-zero argument calls Ident() in
    // the matched constructors that pass one); its type is not in any mangled name (int,
    // as in 13 of the 16 matched views that declare one).
    CMatrix() {}
    CMatrix(int identity) {
        if (!s_ClassInit)
            InitClass();
        if (identity)
            Ident();
    }
    // Inferred: the original keeps no copy constructor, and matched code copies a matrix
    // through a call to the out-of-line operator= (matching-tricks.md, "lines").
    CMatrix(const CMatrix& other) { *this = other; }
    CMatrix& operator=(const CMatrix&);                                  // original, out of line

    // Out-of-line members of the original (matrix.cpp, 0x8008ebf0..0x80090bb0), in order.
    void EndianSwap();
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;
    void TibToMOHFL();
    void ToEulerXZY(float&, float&, float&) const;
    void ToEulerXYZ(float&, float&, float&) const;
    void Ident();
    void Orthonormalize();
    void Transpose();
    void Inverse(const CMatrix&);
    void FastInverse(const CMatrix&);
    CVector3 TransformToLocal(CVector3) const;
    void TransformVector(CVector3&, CVector3) const;
    void TransformPoint(CVector3&, CVector3) const;
    void Transform(CVector4&, CVector3) const;
    void Orthographic(float, float, float, float);
    void Perspective(float, float, float, float);
    void Multiply(const CMatrix&, const CMatrix&);
    void Scale(float);
    void Scale(CVector3);
    void RotateZ(long);
    void RotateY(long);
    void RotateX(long);
    void RotateZ(float);
    void RotateY(float);
    void RotateX(float);
    void Rotate(CVector3, float);
    void Translate(CVector3);
    void PreTranslate(CVector3);
    void SetPos(CVector3);
    void SetUp(CVector3);
    void SetFront(CVector3);
    void SetRight(CVector3);
    void BuildTrans(CVector3);
    void BuildRotZ(float);
    void BuildRotX(float);
    void BuildRot(CVector3, float);
    void BuildScale(CVector3);
    void BuildScale(float);
    static void InitClass();

    // original: weak copy at 0x8009b91c, Multiply(*this, m).
    void PostMultiply(const CMatrix& m) { Multiply(*this, m); }

    // Inferred: the original inlines these everywhere (no weak copies). By-value returns,
    // the most common form among the matched views.
    CVector3 GetRight() const { return right; }
    CVector3 GetFront() const { return front; }
    CVector3 GetUp() const { return up; }
    CVector3 GetPos() const { return pos; }
    CVector3 GetRow(int i) const { return (&right)[i]; }
    // Inferred: inverse in place through the class's scratch matrix (box_pointdist.cpp).
    void Inverse() {
        s_TempMat = *this;
        FastInverse(s_TempMat);
    }

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;

    static bool s_ClassInit;                                             // original (.sbss)
    static CMatrix s_TempMat;                                            // original (.bss)
} __attribute__((aligned(16)));


class CDrawContext {
public:
    unsigned char unknown000[256];
    int m_value100;
};

struct BSObject;
void DestroyBSObject(BSObject*);

#include "game/IMovingSceneNode.h"
#include "game/CPlane.h"
#include "game/CScreen.h"

class CViewport;
class CCamera;

/* Inferred: the frustum's ten planes. */
class CFrustum {
public:
    ~CFrustum() {}
    void Transform(const CMatrix&);
    void Set(const CCamera&, CScreen&, CViewport&);

    CPlane m_planes[10];
};

// CCamera as include/game/CCamera.h declares it, but the ten planes are a CFrustum member
// (inferred: the destructor checks the planes' address before their array destructor, as
// for a member object with an inline destructor; CFrustum's planes are the same ten).
#define GAME_CCAMERA_H
class CDrawContext;

class CCamera : public IMovingSceneNode {
public:
    CCamera() : m_tm(true), m_worldToCamera(true), m_flag141(true), m_projectionValid(false), m_flag143(false), m_script(0), data294(0) {}
    virtual ~CCamera();
    virtual void BeginUpdate(float);
    virtual void AttemptUpdate(float);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;
    virtual void Reset();
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();
    void UpdateWorldToCamera();
    void SetPerspective(float, float);
    bool IsPointVisible(CVector3, CDrawContext&) const;

    static const float ASPECT_4x3;

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
    CFrustum m_frustum;
    BSObject* m_script;
    int data294;
    unsigned char unknown298[8];
};


/* Inferred: a sweep-extent record whose +8 float follows the camera (ISceneNode's words
   +0x0c..+0x18 point at them). */
struct CameraFollowerView {
    unsigned char unknown00[8];
    float value;
};

extern bool g_bInMultiplayerMode;

/* Inferred: scales a plane's normal to unit length. */
inline void NormalizeNormal(CPlane& plane) {
    float length = sqrtf(plane.m_normal.x * plane.m_normal.x + plane.m_normal.y * plane.m_normal.y + plane.m_normal.z * plane.m_normal.z);
    if (length != 0.0f) {
        plane.m_normal.x *= 1.0f / length;
        plane.m_normal.y *= 1.0f / length;
        plane.m_normal.z *= 1.0f / length;
    }
}


const float CCamera::ASPECT_4x3 = 0.75f;

CCamera::~CCamera() {
    if (m_script) {
        DestroyBSObject(m_script);
        m_script = 0;
    }
}

void CCamera::BeginUpdate(float) {
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
        CPlane* plane = m_frustum.m_planes;
        for (i = 0; i < 10; i++) {
            plane->Transform(m_tm);
            NormalizeNormal(*plane);
            plane++;
        }
        m_flag143 = true;
    }
    if (!g_bInMultiplayerMode) {
        CVector3 position;
        GetPosition(position);
        ((CameraFollowerView*)data0c)->value = position.x;
        ((CameraFollowerView*)data14)->value = position.y;
        ((CameraFollowerView*)data10)->value = position.x;
        ((CameraFollowerView*)data18)->value = position.y;
    }
}

void CCamera::CommitUpdate() {
}

void CCamera::Draw(CDrawContext&) {
}

void CCamera::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}

void CCamera::GetPosition(CVector3& v) const {
    v = m_tm.GetPos();
}

void CCamera::GetRightward(CVector3& v) const {
    v = m_tm.GetRight();
}

void CCamera::GetForward(CVector3& v) const {
    v = m_tm.GetFront();
}

void CCamera::GetUpward(CVector3& v) const {
    v = m_tm.GetUp();
}

bool CCamera::IsDrawEnabled() const {
    return false;
}

bool CCamera::IsVisible(CDrawContext&) const {
    return false;
}

void CCamera::PreTransform(const CMatrix& matrix) {
    m_tm.Multiply(matrix, m_tm);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Reset() {
    m_tm.Ident();
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Transform(const CMatrix& matrix) {
    m_tm.Multiply(m_tm, matrix);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::SetTMLocalToWorld(const CMatrix& matrix) {
    m_tm = matrix;
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::SetPosition(CVector3 position) {
    m_tm.SetPos(position);
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

void CCamera::Move(CVector3 delta) {
    m_tm.Translate(delta);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Rotate(CVector3 axis, float angle) {
    m_tm.Rotate(axis, angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Pitch(float angle) {
    m_tm.RotateX(angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Roll(float angle) {
    m_tm.RotateY(angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Yaw(float angle) {
    m_tm.RotateZ(angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Orthonormalize() {
    m_tm.Orthonormalize();
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::SetPerspective(float horizontal, float vertical) {
    m_width = tan(horizontal);
    m_height = tan(vertical);
    m_projectionValid = false;
    m_flag143 = false;
    m_perspective = true;
}

bool CCamera::IsPointVisible(CVector3, CDrawContext& context) const {
    return context.m_value100 == 0;
}

void CCamera::UpdateWorldToCamera() {
    CMatrix m;
    m = m_tm;
    m.RotateZ(3.1415927f);
    CVector3 right = m.right;
    right *= -1.0f;
    CVector3 forward = m.front;
    CVector3 up = m.up;
    CVector3 position = m.pos;
    m_worldToCamera.SetRight(CVector3(right.x, up.x, forward.x));
    m_worldToCamera.SetFront(CVector3(right.y, up.y, forward.y));
    m_worldToCamera.SetUp(CVector3(right.z, up.z, forward.z));
    CVector3 t(right.Dot(position), up.Dot(position), forward.Dot(position));
    t.x = -t.x;
    t.y = -t.y;
    t.z = -t.z;
    m_worldToCamera.SetPos(t);
    m_flag141 = true;
}

void CFrustum::Set(const CCamera& camera, CScreen& screen, CViewport&) {
    float height, width, y, x;
    m_planes[0].m_normal.Set(0.0f, -1.0f, 0.0f);
    m_planes[0].m_d = -camera.m_near;
    m_planes[1].m_normal.Set(0.0f, 1.0f, 0.0f);
    m_planes[1].m_d = camera.m_far;
    m_planes[2].m_normal.Set(-1.0f, -camera.m_width, 0.0f);
    NormalizeNormal(m_planes[2]);
    m_planes[2].m_d = 0.0f;
    m_planes[3].m_normal.Set(1.0f, -camera.m_width, 0.0f);
    NormalizeNormal(m_planes[3]);
    m_planes[3].m_d = 0.0f;
    m_planes[4].m_normal.Set(0.0f, -camera.m_height, 1.0f);
    NormalizeNormal(m_planes[4]);
    m_planes[4].m_d = 0.0f;
    m_planes[5].m_normal.Set(0.0f, -camera.m_height, -1.0f);
    NormalizeNormal(m_planes[5]);
    m_planes[5].m_d = 0.0f;
    width = camera.m_width;
    x = 2047.9375f * (2.0f * width) / screen.GetWidth();
    height = camera.m_height;
    y = 2047.9375f * (2.0f * height) / screen.GetHeight();
    m_planes[6].m_normal.Set(-1.0f, -x, 0.0f);
    NormalizeNormal(m_planes[6]);
    m_planes[6].m_d = 0.0f;
    m_planes[7].m_normal.Set(1.0f, -x, 0.0f);
    NormalizeNormal(m_planes[7]);
    m_planes[7].m_d = 0.0f;
    m_planes[8].m_normal.Set(0.0f, -y, 1.0f);
    NormalizeNormal(m_planes[8]);
    m_planes[8].m_d = 0.0f;
    m_planes[9].m_normal.Set(0.0f, -y, -1.0f);
    NormalizeNormal(m_planes[9]);
    m_planes[9].m_d = 0.0f;
}


void CFrustum::Transform(const CMatrix& tm) {
    for (int i = 0; i < 10; i++) {
        m_planes[i].Transform(tm);
        NormalizeNormal(m_planes[i]);
    }
}
