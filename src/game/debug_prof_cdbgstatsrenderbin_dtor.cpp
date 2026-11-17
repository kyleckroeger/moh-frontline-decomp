// A fragment of debug_prof.cpp (0x800f45ac): the CDbgStatsRenderBin destructor (its virtual table
// pointer, which follows 28 bytes of members, then each base's through the
// inline destructors of CRenderBin, and the object freed when asked). The file
// name is this project's; the original record is debug_prof.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CDbgStatsRenderBin declares an override defined elsewhere before its
// destructor, so its own (global) virtual table is not emitted here; the
// bases' tables are weak in the original (no key function), so the compiler's
// copies of them and of the inline base destructors are weak duplicates,
// linked to the original copies.
class CDmaTag;
class CDmaPacket;

#include "game/CRenderBin.h"


class CDbgStatsRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CDbgStatsRenderBin();
};

__declspec(weak) CDbgStatsRenderBin::~CDbgStatsRenderBin() {
}
