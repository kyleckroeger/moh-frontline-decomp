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
// Inferred, not in any mangled name: the data members. The list links and the tag chain are named as
// renderlist.cpp names them (Register/UnRegister link bins through +0x00 and +0x04; a bin's DMA tag
// chain runs from the first tag at +0x10 to the last at +0x0c), the children list at +0x08 as
// compartment_renderbin.cpp's Init walks it (through each child's +0x04); IsUsed tests +0x10; the
// priority at +0x14 from the constructors that set it. One spelling per member: rename_members.py.
#ifndef GAME_CRENDERBIN_H
#define GAME_CRENDERBIN_H

class CDmaPacket;
class CDmaTag;
class CRenderBin;

struct CRenderBinData {
    CRenderBin* m_prev;
    CRenderBin* m_next;
    CRenderBin* m_children;
    CDmaTag* m_lastTag;
    CDmaTag* m_firstTag;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init() {
        m_lastTag = 0;
        m_firstTag = 0;
        for (CRenderBin* child = m_children; child; child = child->m_next)
            child->Init();
    }
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed() { return m_firstTag != 0; }
};

#endif
