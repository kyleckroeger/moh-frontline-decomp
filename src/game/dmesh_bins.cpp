// A fragment of dmesh.cpp (0x800f5f04): CDMRenderBin::IsUsed (its used flag)
// and Link (the flag cleared; priority-8 bins mark the weapon draw finished,
// the others the scene draw, and a priority-8 bin then clears the depth
// buffer: depth writes on, colour writes off, a copy of the display with a
// black clear colour and the far depth), CPart::Render (returns 0),
// CRemapBin::Link (the target bin's link data is saved, replaced by this
// bin's, the target linked, and the data swapped back), CRemapBin::Render
// (the target bin renders; returns 0) and CPartBin::Init (link words and the
// remap cursor cleared). The file name is this project's; the original record
// is dmesh.cpp. The classes, functions and globals are named by the mangled
// symbols (CRenderBinData by CRenderBin's RTTI record); the members are
// inferred. The flags and cursor are file-local in the original and declared
// extern to link to them. Each class declares its destructor (defined
// elsewhere) first so its table is not emitted here; CRenderBin's Init and
// IsUsed are declared without a body, which keeps its weak table out of this
// unit (the saved copy refers to it).
#include <dolphin/gx.h>

class CDmaTag;
class CDmaPacket;

class CRenderBin;

/* CRenderBin's non-polymorphic base (see dmesh_cremapbin_ctor.cpp). */
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

/* CRenderBin's virtuals in table order; the destructor, Render and Link are
   inline (weak in the original). */
class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed();
};

class CUcodeRenderBin : public CRenderBin {
public:
    virtual ~CUcodeRenderBin();
};

class CDMRenderBin : public CUcodeRenderBin {
public:
    virtual ~CDMRenderBin();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    bool m_used;
    unsigned char unknown21[7];
    int m_remapData;
};

class CPart : public CRenderBin {
public:
    virtual ~CPart();
    virtual int Render(CDmaPacket&, void*);
};

class CRemapBin : public CRenderBin {
public:
    virtual ~CRemapBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);

    CDMRenderBin* m_bin;
    int m_remapData;
};

class CPartBin : public CRenderBin {
public:
    virtual ~CPartBin();
    virtual void Init();
};

/* Inferred: only the display copy's frame buffer at +248. */
struct CScreenView {
    unsigned char unknown00[248];
    void* m_frameBuffer;
};

extern CScreenView g_screen;
extern bool _finish_draw;
extern bool _finish_draw_weapon;
extern int _remap_next;

bool CDMRenderBin::IsUsed() {
    return m_used;
}

void CDMRenderBin::Link(CDmaTag*, CDmaPacket&) {
    m_used = false;
    if (m_priority == 8)
        _finish_draw_weapon = true;
    if (m_priority != 8)
        _finish_draw = true;
    if (m_priority == 8) {
        GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        GXSetColorUpdate(GX_FALSE);
        GXColor clear = {0, 0, 0, 0};
        GXSetCopyClear(clear, 0xFFFFFF);
        GXCopyDisp(g_screen.m_frameBuffer, GX_TRUE);
        GXSetColorUpdate(GX_TRUE);
    }
}

int CPart::Render(CDmaPacket&, void*) {
    return 0;
}

void CRemapBin::Link(CDmaTag* tag, CDmaPacket& packet) {
    CRenderBin saved;
    m_bin->m_remapData = m_remapData;
    CRenderBin& bin = *m_bin;
    saved = bin;
    bin = *this;
    m_bin->Link(tag, packet);
    *(CRenderBin*)this = bin;
    bin = saved;
}

int CRemapBin::Render(CDmaPacket& packet, void* data) {
    m_bin->Render(packet, data);
    return 0;
}

void CPartBin::Init() {
    m_lastTag = 0;
    m_firstTag = 0;
    _remap_next = 0;
    m_children = 0;
}
