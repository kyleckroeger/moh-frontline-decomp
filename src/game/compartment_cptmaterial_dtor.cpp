// A fragment of compartment.cpp (0x80074234): the weak CPTMaterial destructor
// (its table pointer, then CRenderBin's through the inline destructor, and the
// object freed when asked). The file name is this project's; the original
// record is compartment.cpp. The classes and the destructor are named by the
// mangled symbols; the members before the table pointer are not part of this
// view. Defining the weak destructor out of line makes it CPTMaterial's key
// function, so the compiler emits a (global) copy of CPTMaterial's table; the
// original table is weak, so that copy is a weak duplicate, linked to the
// original, as is the compiler's copy of CRenderBin's weak table.
class CDmaPacket;
class CDmaTag;

#include "game/CRenderBin.h"


class CPTMaterial : public CRenderBin {
public:
    virtual ~CPTMaterial();
};

__declspec(weak) CPTMaterial::~CPTMaterial() {
}
