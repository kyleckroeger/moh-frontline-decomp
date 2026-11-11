// A fragment of player.cpp (0x800a0530): CPlayerObject's transform wrappers, from
// GetTMLocalToWorld to GetTMLocalToWorld, each forwarding to the object's transform
// (a CMatrix at +128). The file
// name is this project's; the original record is player.cpp and the functions
// around these are not reconstructed. CPlayerObject and CMatrix's methods are named by
// the mangled symbols; CPlayerObject is an inferred non-virtual view with only the
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

class CPlayerObject {
public:
    void GetTMLocalToWorld(CMatrix&) const;

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.forward; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown00[128];
    CMatrix m_tm;
};

void CPlayerObject::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}
