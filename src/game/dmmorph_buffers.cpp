// Morph blend buffers, the functions at the start of the file: CMemBuffer
// (frees its memory when it owns it), and the blend-buffer cache whose
// entries each hold two memory buffers. CMemBuffer, CBlendBufferCache and
// BufferCacheData are named by the mangled symbols; the members are inferred
// from offsets; the entry's destructor is implicit (a weak symbol in the
// original). The blending functions and the rest of the file are not part
// of this unit.
extern "C" void MEM_free(void*);

class CMemBuffer {
public:
    CMemBuffer();
    ~CMemBuffer();

    void* m_data;
    int m_size;
    int m_used;
    bool m_owned;
};

class CBlendBufferCache {
public:
    class BufferCacheData {
    public:
        BufferCacheData();

        void* m_owner;
        int m_frame;
        CMemBuffer m_buffers[2];
    };

    ~CBlendBufferCache();

    unsigned char unknown00[12];
    BufferCacheData* m_entries;
};

CMemBuffer::~CMemBuffer() {
    if (m_owned)
        MEM_free(m_data);
}

CMemBuffer::CMemBuffer() {
    m_data = 0;
    m_size = 0;
    m_used = 0;
    m_owned = false;
}

CBlendBufferCache::~CBlendBufferCache() {
    delete[] m_entries;
}

CBlendBufferCache::BufferCacheData::BufferCacheData() : m_owner(0), m_frame(0) {
}
