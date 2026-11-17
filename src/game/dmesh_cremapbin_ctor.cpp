// A fragment of dmesh.cpp (0x800f6d20): the weak CRemapBin default
// constructor (CRenderBinData's inline constructor: the link words cleared and
// the priority 3; then the CRenderBin and CRemapBin table pointers) and the
// weak CFinishPartBin destructor after it (0x800f6d5c). The file
// name is this project's; the original record is dmesh.cpp. The classes and
// the function are named by the mangled symbols (CRenderBinData by
// CRenderBin's RTTI record). CRemapBin declares its Render override (defined
// elsewhere) first so its global virtual table is not emitted here. CRenderBin
// has no key function: its weak destructor and default Render/Link copies the
// compiler emits here are weak duplicates, linked to the original copies.
class CDmaTag;
class CDmaPacket;

class CRenderBin;

/* CRenderBin's non-polymorphic base, named by CRenderBin's RTTI record; the
   members are inferred (CRenderBin::Init walks children from +8 through the
   siblings at +4, IsUsed tests +16, +20 is the render priority, 3 by
   default). */
struct CRenderBinData {
    CRenderBinData() : m_prev(0), m_next(0), m_children(0), m_lastTag(0), m_firstTag(0), m_priority(3) {}

    int m_prev;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int m_lastTag;
    int m_firstTag;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order; all are weak (inline) in the
   original. Init's and IsUsed's bodies are not part of this view, so they
   are declared without one, which also keeps CRenderBin's table out of this
   unit. */
class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed();
};

class CRemapBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CRemapBin();
    CRemapBin();
};

__declspec(weak) CRemapBin::CRemapBin() {
}

/* The next class in the file. Its Render override (defined elsewhere) is
   declared first so its table is not emitted here; the original table is
   weak and is referenced. */
class CFinishPartBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CFinishPartBin();
};

__declspec(weak) CFinishPartBin::~CFinishPartBin() {
}
