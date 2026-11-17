// A fragment of compartment.cpp (0x80073ac0): the CCPTMatBin destructor (its virtual table
// pointer, which follows 28 bytes of members, then each base's through the
// inline destructors of CRenderBin, CPTMaterial, and the object freed when asked). The file
// name is this project's; the original record is compartment.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CCPTMatBin declares an override defined elsewhere before its
// destructor, so its own (global) virtual table is not emitted here; the
// bases' tables are weak in the original (no key function), so the compiler's
// copies of them and of the inline base destructors are weak duplicates,
// linked to the original copies.
class CDmaTag;
class CDmaPacket;

#include "game/CRenderBin.h"


class CPTMaterial : public CRenderBin {
public:
    virtual ~CPTMaterial() {}
};

class CCPTMatBin : public CPTMaterial {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CCPTMatBin();
};

__declspec(weak) CCPTMatBin::~CCPTMatBin() {
}
