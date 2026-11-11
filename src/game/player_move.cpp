// A fragment of player.cpp (0x800a0168): CPlayerObject's transform wrappers,
// from Rotate to SetPosition, each forwarding to the object's transform (a
// CMatrix at +128), and SetTMLocalToWorld (copies the transform, takes two
// of its Euler angles, rebuilds the camera transform at +192 from
// g_CameraOffset, tilted by the pitch unless one flag is set without the
// other, and copies the position row). The file name is this project's; the
// original record is player.cpp and the functions around these are not
// reconstructed. CPlayerObject and CMatrix's methods are named by the
// mangled symbols; CPlayerObject is an inferred non-virtual view with only
// the members used declared, the bodies are inferred from the calls, and the
// by-value row getters are inferred inline helpers.
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
    void ToEulerXYZ(float&, float&, float&) const;
    void BuildTrans(CVector3);

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class CPlayerObject {
public:
    void Rotate(CVector3, float);
    void Move(CVector3);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetPosition(CVector3);
    void SetTMLocalToWorld(const CMatrix&);

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.forward; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown00[128];
    CMatrix m_tm;
    CMatrix m_cameraTM;
    unsigned char unknown100[392];
    CVector3 m_position;
    unsigned char unknown298[4];
    float m_yaw;
    float m_pitch;
    unsigned char unknown2a4[242];
    unsigned char unknown396 : 6;
    unsigned char m_flag396a : 1;
    unsigned char m_flag396b : 1;
};

extern CVector3 g_CameraOffset;

void CPlayerObject::Rotate(CVector3 axis, float angle) {
    m_tm.Rotate(axis, angle);
}

void CPlayerObject::Move(CVector3 delta) {
    m_tm.Translate(delta);
}

void CPlayerObject::SetBasis(CVector3 right, CVector3 forward, CVector3 up) {
    m_tm.SetRight(right);
    m_tm.SetFront(forward);
    m_tm.SetUp(up);
}

void CPlayerObject::SetPosition(CVector3 position) {
    m_tm.SetPos(position);
}

void CPlayerObject::SetTMLocalToWorld(const CMatrix& tm) {
    m_tm = tm;
    float roll;
    tm.ToEulerXYZ(m_pitch, roll, m_yaw);
    m_cameraTM.BuildTrans(g_CameraOffset);
    if (!m_flag396a || m_flag396b)
        m_cameraTM.RotateX(m_pitch);
    m_position = PositionRow();
}
