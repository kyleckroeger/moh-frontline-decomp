// A fragment of bsutil.cpp (0x8003bdd0):
// BSUtilObjectInstanceMemoryAllocator::Init (the pool laid out over the given
// memory: element and stride sizes in words, the count, the total size from
// ComputeNeededSize, then every element's trailing link word chained to the
// next element's by an inline helper, the last one null, and the free list
// started at the base) and ComputeNeededSize, the pool size for a count of
// objects (each with a 4-byte header, padded to the alignment), rounded up
// past the next 16-byte boundary. The file name is this project's; the
// original record is bsutil.cpp and the functions around it are not
// reconstructed. The class is named by the mangled symbols; its members and
// the link helper are inferred from offsets and are not original. The file is
// compiled with deferred inlining (Init inlines ComputeNeededSize, which
// follows it in the image), so functions are listed in reverse image order.
class BSUtilObjectInstanceMemoryAllocator {
public:
    BSUtilObjectInstanceMemoryAllocator() {}
    ~BSUtilObjectInstanceMemoryAllocator();
    int ComputeNeededSize(int, int, int);
    void Init(void*, int, int, int);
    void* GetFreeElement(bool);
    void FreeElement(void*);
    bool DoWeOwnThisMemory(void*);
    void LinkElements(int elementWords, int strideWords) {
        unsigned long* link = m_base + elementWords;
        for (int i = 0; i < m_maxCount - 1; i++) {
            *link = (unsigned long)(link + (strideWords - elementWords));
            link += m_strideWords;
        }
        *link = 0;
    }

    int m_elementWords;
    int m_strideWords;
    int m_maxCount;
    int m_count;
    int m_totalWords;
    unsigned long* m_base;
    unsigned long* m_free;
};

extern BSUtilObjectInstanceMemoryAllocator g_pMemAllocators[3];

int BSUtilObjectInstanceMemoryAllocator::ComputeNeededSize(int size, int count, int align) {
    int stride = size + 4;

    if (align && (stride & (align - 1)))
        stride += align - (stride & (align - 1));
    int total = count * stride;
    return total + (16 - (total & 15));
}

void BSUtilObjectInstanceMemoryAllocator::Init(void* memory, int size, int count, int align) {
    m_base = (unsigned long*)memory;
    int stride = size + 4;
    m_elementWords = size >> 2;
    m_maxCount = count;
    if (align && (stride & (align - 1)))
        stride += align - (stride & (align - 1));
    m_strideWords = stride >> 2;
    m_count = 0;
    m_totalWords = ComputeNeededSize(size, count, align) >> 2;
    LinkElements(m_elementWords, m_strideWords);
    m_free = m_base;
}

void* BSUtilObjectInstanceMemoryAllocator::GetFreeElement(bool);

void BSUtilObjectInstanceMemoryAllocator::FreeElement(void* element);

bool BSUtilObjectInstanceMemoryAllocator::DoWeOwnThisMemory(void* memory);


