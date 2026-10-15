// A fragment of bsutil.cpp; the file name is this project's. Behaviour-script object memory: fixed-size element pools with an embedded
// free-list link after each element, three of which back object instances.
// BSUtilObjectInstanceMemoryAllocator is named by the mangled symbols; its
// members are inferred from offsets and are not original. The file is
// compiled with deferred inlining (Init inlines ComputeNeededSize, which
// follows it in the image), so functions are listed in reverse image order.
// This unit covers DoWeOwnThisMemory, GetFreeElement and FreeElement (the
// copy inlined in bsutil_update.cpp needs the offset computed first instead;
// both local orders are inferred): Init after them is drafted in scratch but
// differs in register numbering, and the static initialisation of g_pMemAllocators cannot be
// verified because its array destructor's compiler-numbered name
// (__arraydtor$623) depends on the rest of the file.
class BSUtilObjectInstanceMemoryAllocator {
public:
    BSUtilObjectInstanceMemoryAllocator() {}
    ~BSUtilObjectInstanceMemoryAllocator();
    int ComputeNeededSize(int, int, int);
    void Init(void*, int, int, int);
    void* GetFreeElement(bool);
    void FreeElement(void*);
    bool DoWeOwnThisMemory(void*);

    int m_elementWords;
    int m_strideWords;
    int m_maxCount;
    int m_count;
    int m_totalWords;
    unsigned long* m_base;
    unsigned long* m_free;
};

void* BSUtilObjectInstanceMemoryAllocator::GetFreeElement(bool) {
    if (m_count < m_maxCount) {
        unsigned long* element = m_free;

        m_free = (unsigned long*)element[m_elementWords];
        m_count++;
        return element;
    }
    return 0;
}

void BSUtilObjectInstanceMemoryAllocator::FreeElement(void* element) {
    ((unsigned long*)element)[m_elementWords] = (unsigned long)m_free;
    m_free = (unsigned long*)element;
    m_count--;
}

bool BSUtilObjectInstanceMemoryAllocator::DoWeOwnThisMemory(void* element) {
    bool owned = false;
    unsigned long* base = m_base;
    int offset = (char*)element - (char*)base;
    if ((unsigned long*)element >= base && (unsigned long*)element < base + m_totalWords &&
        offset % (m_strideWords * 4) == 0)
        owned = true;
    return owned;
}
