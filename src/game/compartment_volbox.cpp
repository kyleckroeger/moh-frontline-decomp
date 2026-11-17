// A fragment of compartment.cpp (0x80074bd0): the weak CCharacterShadow::GetBox
// (the shadow's box at +16) and CVector3 copy constructor (two doublewords
// copied; the double-pair view is inferred), then the weak CVolBox default
// constructor emitted in this file (IVolume's table pointer through the
// inline base constructor, then CVolBox's). The file name is this project's;
// the original record is compartment.cpp, and the rest of the file is not
// part of this unit. IVolume and CVolBox are named by the mangled symbols;
// only the virtuals the constructor needs are declared. CVolBox declares
// Create (defined in box.cpp) before its destructor, so its global virtual
// table is not emitted here; IVolume's destructor is inline (weak in the
// original), so the compiler's copies of it and of IVolume's weak virtual
// table are weak duplicates, linked to the original copies.
/* Inferred: CVector3 copied as two doubles. */
struct CVector3Pair {
    double d[2];
};

class CVector3 {
public:
    CVector3(const CVector3&);

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

#include "game/IVolume.h"


class CVolBox : public IVolume {
public:
    virtual IVolume* Create() const;
    virtual ~CVolBox();
    CVolBox();
};

class CCharacterShadow {
public:
    virtual bool IsDrawEnabled() const;
    const CVolBox& GetBox();

    unsigned char unknown04[12];
    CVolBox m_box;
};

__declspec(weak) const CVolBox& CCharacterShadow::GetBox() {
    return m_box;
}

__declspec(weak) CVector3::CVector3(const CVector3& o) {
    *(CVector3Pair*)this = *(const CVector3Pair*)&o;
}

__declspec(weak) CVolBox::CVolBox() {
}
