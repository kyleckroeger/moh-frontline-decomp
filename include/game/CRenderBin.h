// CRenderBin: a node of the render list (a bin draws the primitives linked into it), shared by every
// file that uses it (PortReadiness step 3.3).
//
// From the original: CRenderBin derives from a non-polymorphic CRenderBinData (RTTI
// __RTTI__10CRenderBin lists __RTTI__14CRenderBinData at offset 0), so the table pointer is at
// +0x1c and a bin is 0x20 bytes (81 of 87 matched views). __vt__10CRenderBin (weak): the
// destructor, Render, Init, Link, IsUsed, all inline in the original. Their weak copies are placed
// by compartment_renderbin.cpp (Init, IsUsed), compartment_bins.cpp (Render returns 0, Link does
// nothing) and compartment_renderbin_dtor.cpp (the destructor), whose bodies these follow.
//
// Inferred, not in any mangled name: the data members, named as compartment_renderbin.cpp names them
// (Init clears +0x0c and +0x10 and walks the children list at +0x08 through each child's +0x04;
// IsUsed tests +0x10); the priority at +0x14 from the constructors that set it.
#ifndef GAME_CRENDERBIN_H
#define GAME_CRENDERBIN_H

class CDmaPacket;
class CDmaTag;
class CRenderBin;

struct CRenderBinData {
    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init() {
        data0c = 0;
        data10 = 0;
        for (CRenderBin* child = m_children; child; child = child->m_next)
            child->Init();
    }
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed() { return data10 != 0; }
};

#endif
