// A fragment of light.cpp (0x80077f2c): CLight's transform wrappers, from
// GetUpward to Reset, each forwarding to the object's transform
// (a CMatrix at +112). The file
// name is this project's; the original record is light.cpp and the functions
// around these are not reconstructed. CLight and CMatrix's methods are named by
// the mangled symbols; CLight is an inferred non-virtual view with only the
// transform declared, the bodies are inferred from the calls, and the by-value
// row getters are inferred inline helpers.
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: vectors are
// copied as two lfd/stfd pairs, which an implicit copy through the double pair
// reproduces.
#include "game/CVector3.h"

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);
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

class CLight {
public:
    void GetUpward(CVector3&) const;
    void GetForward(CVector3&) const;
    void GetRightward(CVector3&) const;
    void GetPosition(CVector3&) const;
    void GetTMLocalToWorld(CMatrix&) const;
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
    void PreTransform(const CMatrix&);
    void Reset();

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.forward; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown00[112];
    CMatrix m_tm;
};

void CLight::GetUpward(CVector3& v) const {
    v = UpRow();
}

void CLight::GetForward(CVector3& v) const {
    v = ForwardRow();
}

void CLight::GetRightward(CVector3& v) const {
    v = RightRow();
}

void CLight::GetPosition(CVector3& v) const {
    v = PositionRow();
}

void CLight::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}

void CLight::Orthonormalize() {
    m_tm.Orthonormalize();
}

void CLight::Yaw(float angle) {
    m_tm.RotateZ(angle);
}

void CLight::Roll(float angle) {
    m_tm.RotateY(angle);
}

void CLight::Pitch(float angle) {
    m_tm.RotateX(angle);
}

void CLight::Rotate(CVector3 axis, float angle) {
    m_tm.Rotate(axis, angle);
}

void CLight::Move(CVector3 delta) {
    m_tm.Translate(delta);
}

void CLight::SetBasis(CVector3 right, CVector3 forward, CVector3 up) {
    m_tm.SetRight(right);
    m_tm.SetFront(forward);
    m_tm.SetUp(up);
}

void CLight::SetPosition(CVector3 position) {
    m_tm.SetPos(position);
}

void CLight::SetTMLocalToWorld(const CMatrix& matrix) {
    m_tm = matrix;
}

void CLight::Transform(const CMatrix& matrix) {
    m_tm.Multiply(m_tm, matrix);
}

void CLight::PreTransform(const CMatrix& matrix) {
    m_tm.Multiply(matrix, m_tm);
}

void CLight::Reset() {
    m_tm.Ident();
}
