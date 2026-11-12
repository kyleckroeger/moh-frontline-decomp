// A fragment of anim_obj.cpp (0x80093a6c): CAnimObject's transform wrappers, from
// GetTMLocalToWorld to GetTMLocalToWorld, each forwarding to the object's transform
// (a CMatrix at +9024). The file
// name is this project's; the original record is anim_obj.cpp and the functions
// around these are not reconstructed. CAnimObject and CMatrix's methods are named by
// the mangled symbols; CAnimObject is an inferred non-virtual view with only the
// transform declared, the bodies are inferred from the calls, and the by-value
// row getters are inferred inline helpers.
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: vectors are
// copied as two lfd/stfd pairs, which an implicit copy through the double pair
// reproduces.
#include "game/CVector3.h"

#include "game/CMatrix.h"


class CAnimObject {
public:
    void GetTMLocalToWorld(CMatrix&) const;

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.front; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.pos; }

    unsigned char unknown00[9024];
    CMatrix m_tm;
};

void CAnimObject::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}
