// A fragment of renderlist.cpp (0x800f0ed4): CRenderList::Wait (empty),
// Kick (unless the list is empty, the buffer is filled with the clear value
// when g_bClearRenderList is set, four words per counted entry, and the write
// cursor rewound to the buffer start) and Restart (Draw, Kick inlined, then
// GXDrawDone; the fill is an inferred inline helper,
// which the loop's register use and signed count need; g_ClearValue is
// file-local in the original and declared extern here), CRenderList::Draw (the list
// is finished and g_bEmptyList set when nothing was drawn; otherwise every
// registered bin is reset through its virtual Init) and Render (a tag is
// opened at the write cursor and appended to the bin's tag chain, the bin
// renders its data into the list, and an empty result rolls the cursor and
// the chain back; the bin's first tag is kept). The file name is this
// project's; the original record is renderlist.cpp. The classes, functions
// and g_bEmptyList are named by the mangled symbols; the list is the packet
// the bins write to (its cursor at +8, as in the bins' views), and the
// layouts are inferred (as in renderlist.cpp; the bin's link data as in
// dmesh_cremapbin_ctor.cpp). The bin's destructor (defined elsewhere) is
// declared first so no table is emitted here.
class CDmaPacket;

extern "C" void GXDrawDone();

extern bool g_bEmptyList;
extern bool g_bClearRenderList;
extern unsigned int g_ClearValue;

/* Inferred: a render tag's link and data. */
struct CDmaTag {
    CDmaTag* m_next;
    void* m_data;
};

/* CRenderBin's non-polymorphic base (see dmesh_cremapbin_ctor.cpp): the
   tag chain's last and first tags at +12 and +16. */
struct CRenderBinData {
    CRenderBinData* data00;
    class CRenderBin* m_next;
    class CRenderBin* m_children;
    CDmaTag* m_lastTag;
    CDmaTag* m_firstTag;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
};

class CRenderList {
public:
    bool Finish();
    void Wait();
    void Kick();
    void Restart();
    void Draw();
    void Render(CRenderBin&, void*);

    unsigned int* m_buffer;
    void* m_end;
    unsigned char* m_current;
    CDmaTag* m_tag;
    unsigned int m_count;
    int unknown14;
    CRenderBin* m_bins;
};

void CRenderList::Wait() {
}

static inline void ClearList(int count, unsigned int* p) {
    while (count--) {
        p[0] = g_ClearValue;
        p[1] = g_ClearValue;
        p[2] = g_ClearValue;
        p[3] = g_ClearValue;
        p += 4;
    }
}

void CRenderList::Kick() {
    if (g_bEmptyList)
        return;
    if (g_bClearRenderList)
        ClearList(m_count, m_buffer);
    m_current = (unsigned char*)m_buffer;
}

void CRenderList::Restart() {
    Draw();
    Kick();
    GXDrawDone();
}

void CRenderList::Draw() {
    g_bEmptyList = !Finish();
    if (!g_bEmptyList) {
        for (CRenderBin* bin = m_bins; bin; bin = bin->m_next)
            bin->Init();
    }
}

void CRenderList::Render(CRenderBin& bin, void* data) {
    unsigned char* before;
    CDmaTag* start = (CDmaTag*)m_current;
    m_tag = start;
    CDmaTag* tag = (CDmaTag*)m_current;
    tag->m_next = 0;
    tag->m_data = 0;
    m_current += sizeof(CDmaTag);
    m_tag->m_data = m_current;
    CDmaTag* last = bin.m_lastTag;
    if (last)
        last->m_next = m_tag;
    bin.m_lastTag = m_tag;
    before = m_current;
    bin.Render(*(CDmaPacket*)this, data);
    if (before == m_current) {
        m_current = (unsigned char*)start;
        bin.m_lastTag = last;
    } else if (!bin.m_firstTag) {
        bin.m_firstTag = m_tag;
    }
    m_tag = 0;
}
