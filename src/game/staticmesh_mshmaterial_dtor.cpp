// A fragment of staticmesh.cpp (0x800f1da0): the weak MSHMaterial destructor
// (its table pointer, then CRenderBin's through the inline destructor, and the
// object freed when asked). The file name is this project's; the original
// record is staticmesh.cpp. The classes and the destructor are named by the
// mangled symbols; the members before the table pointer are not part of this
// view. Defining the weak destructor out of line makes it MSHMaterial's key
// function, so the compiler emits a (global) copy of MSHMaterial's table; the
// original table is weak, so that copy is a weak duplicate, linked to the
// original, as is the compiler's copy of CRenderBin's weak table.
class CDmaPacket;
class CDmaTag;

#include "game/CRenderBin.h"


class MSHMaterial : public CRenderBin {
public:
    virtual ~MSHMaterial();
};

__declspec(weak) MSHMaterial::~MSHMaterial() {
}
