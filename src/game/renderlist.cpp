// CRenderList's bin list and destructor: render bins are kept in a list
// ordered by priority (Register inserts a bin before the first bin of higher
// priority, or appends it; UnRegister unlinks it), and the destructor frees
// the display-list buffer. CRenderList and CRenderBin are named by the
// symbols; the list and bin layouts are inferred from offsets. The
// constructor allocates the buffer (16 bytes per entry, aligned to 128), fills
// it with the clear value when g_bClearRenderList is set (the fill is the
// inferred inline helper of renderlist_draw.cpp; g_ClearValue is file-local in
// the original and declared extern here) and sets the write cursor to its
// start. Register walks the list to its end
// before and after inserting without using the result; both walks are kept.
// CRenderBin is an inferred view: its links and priority, then its virtual
// table pointer after 28 bytes, with the virtual destructor in the first slot.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

struct RenderEntryView {
    unsigned long words[4];
};

extern bool g_bClearRenderList;
extern unsigned int g_ClearValue;

class CRenderBin {
public:
    CRenderBin* m_prev;
    CRenderBin* m_next;
    unsigned char unknown08[12];
    int m_priority;
    unsigned char unknown18[4];

    virtual ~CRenderBin();
};

class CRenderList {
public:
    CRenderList(unsigned long, unsigned long);
    ~CRenderList();
    void UnRegister(CRenderBin&);
    void Register(CRenderBin&);

    RenderEntryView* m_buffer;
    RenderEntryView* m_end;
    RenderEntryView* m_current;
    void* m_last;
    unsigned long m_count;
    int unknown14;
    CRenderBin* m_bins;
};

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
    MEM_free(m_buffer);
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

CRenderList::CRenderList(unsigned long count, unsigned long) {
    m_buffer = 0;
    m_end = 0;
    m_current = 0;
    m_count = count;
    unknown14 = 0;
    m_bins = 0;
    m_last = 0;
    m_buffer = (RenderEntryView*)DWI_allocalign("Renderlist", m_count * 16, 128, 1024);
    m_end = m_buffer + m_count;
    if (g_bClearRenderList)
        ClearList(m_count, (unsigned int*)m_buffer);
    m_current = m_buffer;
}
