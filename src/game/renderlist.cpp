// renderlist.cpp: CRenderList::Finish (each registered bin is finished, the
// largest amount of the list used so far is kept, and true returned) and
// FinishBin (a used bin links its tag chain into the list; its children are
// finished either way; the compiler inlines the recursion several levels
// deep); Wait (empty), Kick (unless the list is empty, the buffer is filled
// with the clear value when g_bClearRenderList is set, four words per counted
// entry, and the write cursor rewound to the buffer start) and Restart (Draw,
// Kick inlined, then GXDrawDone; the fill is an inferred inline helper, which
// the loop's register use and signed count need; g_ClearValue is file-local in
// the original and declared extern here), Draw (the list is finished and
// g_bEmptyList set when nothing was drawn; otherwise every registered bin is
// reset through its virtual Init) and Render (a tag is opened at the write
// cursor and appended to the bin's tag chain, the bin renders its data into
// the list, and an empty result rolls the cursor and the chain back; the bin's
// first tag is kept). Then the bin list and the destructor: render bins are
// kept in a list ordered by priority (Register inserts a bin before the first
// bin of higher priority, or appends it, walking the list to its end before
// and after inserting without using the result; UnRegister unlinks it), the
// destructor frees the display-list buffer, and the constructor allocates the
// buffer (16 bytes per entry, aligned to 128), fills it with the clear value
// when g_bClearRenderList is set and sets the write cursor to its start. The
// classes, functions and g_bEmptyList are named by the mangled symbols; the
// list is the packet the bins write to (its cursor at +8, as in the bins'
// views), and the layouts are inferred (the bin's link data as in
// dmesh_cremapbin_ctor.cpp). Where the parts of the file read a list member as
// a different pointer type (the buffer as words, bytes or 16-byte entries),
// the list has an anonymous union of the views. The bin's destructor (defined
// elsewhere) is declared first so no table is emitted here.
class CDmaPacket;

extern "C" void GXDrawDone();
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

extern bool g_bEmptyList;
extern bool g_bClearRenderList;
extern unsigned int g_ClearValue;

struct RenderEntryView {
    unsigned long words[4];
};

/* Inferred: a render tag's link and data. */
struct CDmaTag {
    CDmaTag* m_next;
    void* m_data;
};

/* CRenderBin's non-polymorphic base (see dmesh_cremapbin_ctor.cpp): the
   tag chain's last and first tags at +12 and +16. */
struct CRenderBinData {
    union {
        CRenderBinData* data00;
        class CRenderBin* m_prev;
    };
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
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CRenderList {
public:
    CRenderList(unsigned long, unsigned long);
    ~CRenderList();
    bool Finish();
    void FinishBin(CRenderBin*);
    void Wait();
    void Kick();
    void Restart();
    void Draw();
    void Render(CRenderBin&, void*);
    void UnRegister(CRenderBin&);
    void Register(CRenderBin&);

    union {
        unsigned int* m_buffer;
        RenderEntryView* m_entries;
    };
    union {
        void* m_end;
        RenderEntryView* m_entryEnd;
    };
    union {
        unsigned char* m_current;
        RenderEntryView* m_entryCurrent;
    };
    union {
        CDmaTag* m_tag;
        void* m_last;
    };
    unsigned int m_count;
    union {
        unsigned int m_maxUsed;
        int unknown14;
    };
    CRenderBin* m_bins;
};

bool CRenderList::Finish() {
    for (CRenderBin* bin = m_bins; bin; bin = bin->m_next)
        FinishBin(bin);
    int used = m_current - (unsigned char*)m_buffer;
    if (m_maxUsed < used)
        m_maxUsed = used;
    return true;
}

void CRenderList::FinishBin(CRenderBin* bin) {
    if (bin->IsUsed())
        bin->Link(bin->m_firstTag, *(CDmaPacket*)this);
    for (CRenderBin* child = bin->m_children; child; child = child->m_next)
        FinishBin(child);
}

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

void CRenderList::UnRegister(CRenderBin& bin) {
    if (m_bins == &bin)
        m_bins = bin.m_next;
    if (bin.m_prev)
        bin.m_prev->m_next = bin.m_next;
    if (bin.m_next)
        bin.m_next->m_prev = bin.m_prev;
    bin.m_next = 0;
    bin.m_prev = 0;
}

void CRenderList::Register(CRenderBin& bin) {
    CRenderBin* head = m_bins;
    CRenderBin* last;
    CRenderBin* current;
    if (!head) {
        m_bins = &bin;
        return;
    }
    for (last = head; last; last = last->m_next)
        ;
    for (current = head; current; current = current->m_next) {
        if (bin.m_priority < current->m_priority) {
            if (current == head)
                m_bins = &bin;
            bin.m_prev = current->m_prev;
            if (current->m_prev)
                current->m_prev->m_next = &bin;
            bin.m_next = current;
            current->m_prev = &bin;
            break;
        }
        if (!current->m_next) {
            bin.m_next = current->m_next;
            if (current->m_next)
                current->m_next->m_prev = &bin;
            bin.m_prev = current;
            current->m_next = &bin;
            break;
        }
    }
    for (last = m_bins; last->m_next; last = last->m_next)
        ;
}

CRenderList::~CRenderList() {
    MEM_free(m_entries);
}

CRenderList::CRenderList(unsigned long count, unsigned long) {
    m_entries = 0;
    m_entryEnd = 0;
    m_entryCurrent = 0;
    m_count = count;
    unknown14 = 0;
    m_bins = 0;
    m_last = 0;
    m_entries = (RenderEntryView*)DWI_allocalign("Renderlist", m_count * 16, 128, 1024);
    m_entryEnd = m_entries + m_count;
    if (g_bClearRenderList)
        ClearList(m_count, (unsigned int*)m_entries);
    m_entryCurrent = m_entries;
}
