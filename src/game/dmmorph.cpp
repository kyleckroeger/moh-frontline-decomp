// A fragment of dmmorph.cpp (0x800f7618): DMMorphSetMaxActiveTargets keeps a
// morph object's active-target limit (+260) and DMMorphInitObject stores its
// target count (+256) and clears that many weights; then the file's static
// initialisation, which builds the blend-buffer cache _blendCache (sixteen
// entries whose two memory buffers each get 19456 bytes from the "Animation
// memory buffer" pool) and registers its destructor. DMMorphObject_T,
// CBlendBufferCache, BufferCacheData, CMemBuffer and _blendCache are named by
// the symbols; the views follow dmmorph_buffers.cpp, and the morph object's
// layout (64 float weights, then the counts), the cache's inline constructor (taking the entry count and buffer size)
// and the buffers' inline allocation are inferred. The rest of the file is in
// other units.
extern "C" void MEM_free(void*);
void* DWI_alloc(const char*, int, int);

struct DMMorphObject_T {
    float weights[64];
    unsigned long count;
    unsigned long maxActive;
};

void DMMorphSetMaxActiveTargets(DMMorphObject_T* object, unsigned long count) {
    object->maxActive = count;
}

void DMMorphInitObject(DMMorphObject_T* object, unsigned long count) {
    object->count = count;
    for (unsigned long i = 0; i < object->count; i++)
        object->weights[i] = 0.0f;
}

class CMemBuffer {
public:
    CMemBuffer();
    ~CMemBuffer();
    void Allocate(int size) {
        if (m_owned)
            MEM_free(m_data);
        m_data = DWI_alloc("Animation memory buffer", size, 1024);
        m_size = size;
        m_used = 0;
        m_owned = true;
    }

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

    CBlendBufferCache(int count, int size) : m_count(count), m_bufferSize(size), m_unknown08(0) {
        m_entries = new BufferCacheData[count];
        for (int i = 0; i < count; i++) {
            m_entries[i].m_buffers[0].Allocate(size);
            m_entries[i].m_buffers[1].Allocate(size);
        }
    }
    ~CBlendBufferCache();

    int m_count;
    int m_bufferSize;
    int m_unknown08;
    BufferCacheData* m_entries;
};

CBlendBufferCache _blendCache(16, 19456);
