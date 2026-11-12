// A fragment of bullet.cpp (0x800ccb84): CBullet's transform wrappers, from
// GetRightward to GetTMLocalToWorld, each forwarding to the object's transform
// (a CMatrix at +80). The file
// name is this project's; the original record is bullet.cpp and the functions
// around these are not reconstructed. CBullet and CMatrix's methods are named by
// the mangled symbols; CBullet is an inferred non-virtual view with only the
// transform declared, the bodies are inferred from the calls, and the by-value
// row getters are inferred inline helpers.
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: vectors are
// copied as two lfd/stfd pairs, which an implicit copy through the double pair
// reproduces.
#include "game/CVector3.h"

#include "game/CMatrix.h"


class CBullet {
public:
    void GetRightward(CVector3&) const;
    void GetUp(CVector3&) const;
    void GetTMLocalToWorld(CMatrix&) const;

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.front; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.pos; }

    unsigned char unknown00[80];
    CMatrix m_tm;
};

__declspec(weak) void CBullet::GetRightward(CVector3& v) const {
    v = RightRow();
}

__declspec(weak) void CBullet::GetUp(CVector3& v) const {
    v = UpRow();
}

__declspec(weak) void CBullet::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}
