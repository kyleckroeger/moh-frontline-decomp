// A fragment of compartment.cpp (0x80076e24): the weak destructors of the
// compartment's render bins CCptShadowCleanUpBin, CCptShadowBin and
// CCptSimpleBin (each resets its virtual table pointer, which follows 28
// bytes of members, through the inline CUcodeRenderBin and CRenderBin
// destructors, then frees the object when asked). The file name is this
// project's; the original record is compartment.cpp, between
// compartment_bins.cpp and compartment_weak.cpp. The classes and functions
// are named by the mangled symbols; the class layout is an inferred view
// (members before the table pointer), and only the virtuals the destructors
// need are declared, Link first so the bins' own tables (global, in
// compartment.cpp) are not emitted here. CRenderBin and CUcodeRenderBin have
// no key function (their virtuals are the weak inline defaults of
// compartment_bins.cpp and the weak destructors): their tables are weak in
// the original, so the compiler's copies of the tables and of the inline
// functions they list are weak duplicates, linked to the original copies.
class CDmaTag;
class CDmaPacket;

#include "game/CRenderBin.h"


class CUcodeRenderBin : public CRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual ~CUcodeRenderBin() {}
};

class CCptShadowCleanUpBin : public CRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual ~CCptShadowCleanUpBin();
};

class CCptShadowBin : public CUcodeRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual ~CCptShadowBin();
};

class CCptSimpleBin : public CUcodeRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual ~CCptSimpleBin();
};

__declspec(weak) CCptShadowCleanUpBin::~CCptShadowCleanUpBin() {
}

__declspec(weak) CCptShadowBin::~CCptShadowBin() {
}

__declspec(weak) CCptSimpleBin::~CCptSimpleBin() {
}
