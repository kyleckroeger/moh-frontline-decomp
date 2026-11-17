// A fragment of dmesh.cpp (0x800f5860): the CDMRenderBin destructor (its virtual table
// pointer, which follows 28 bytes of members, then each base's through the
// inline destructors of CRenderBin, CUcodeRenderBin, and the object freed when asked). The file
// name is this project's; the original record is dmesh.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CDMRenderBin declares an override defined elsewhere before its
// destructor, so its own (global) virtual table is not emitted here; the
// bases' tables are weak in the original (no key function), so the compiler's
// copies of them and of the inline base destructors are weak duplicates,
// linked to the original copies.
class CDmaTag;
class CDmaPacket;

#include "game/CRenderBin.h"


class CUcodeRenderBin : public CRenderBin {
public:
    virtual ~CUcodeRenderBin() {}
};

class CDMRenderBin : public CUcodeRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual ~CDMRenderBin();
};

__declspec(weak) CDMRenderBin::~CDMRenderBin() {
}
