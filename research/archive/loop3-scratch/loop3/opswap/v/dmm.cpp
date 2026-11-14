// Morph blend buffers, the functions at the start of the file: CMemBuffer
// (frees its memory when it owns it), and the blend-buffer cache whose
// entries each hold two memory buffers. CMemBuffer, CBlendBufferCache and
// BufferCacheData are named by the mangled symbols; the members are inferred
// from offsets; the entry's destructor is implicit (a weak symbol in the
// original). The blending functions and the rest of the file are not part
// of this unit.
extern "C" void MEM_free(void*);
#include <math.h>

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
    CMemBuffer* GetBuffer(void* owner) {
        int best = 0;
        int bestFrame = 0;
        for (int i = 0; i < m_count; i++) {
            BufferCacheData* entry = &m_entries[i];
            if (entry->m_owner == owner) {
                m_entries[i].m_frame = 0;
                return &m_entries[i].m_buffers[m_current];
            }
            if (entry->m_owner == 0) {
                best = i;
                bestFrame = 0x7fffffff;
            } else if (entry->m_frame > bestFrame) {
                best = i;
                bestFrame = entry->m_frame;
            }
        }
        m_entries[best].m_owner = owner;
        m_entries[best].m_frame = 0;
        return &m_entries[best].m_buffers[m_current];
    }

    int m_count;
    int m_bufferSize;
    int m_current;
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

struct DMMorphObject_T {
    float weights[64];
    unsigned long count;
};

#pragma pack(push, 1)
struct DMVec3s {
    short x, y, z;
};

struct DMPrimHeader {
    char cmd;
    unsigned short count;
};

struct DMSrcVertex {
    unsigned char attr;
    unsigned short pos;
    unsigned short nrm;
    unsigned short tex;
};

struct DMDstVertex {
    unsigned char attr;
    DMVec3s pos;
    DMVec3s nrm;
    unsigned short tex;
};
#pragma pack(pop)

struct DMDisplayList {
    char* data;
    unsigned short size;
};

struct DMMorphTargetIndex {
    unsigned short pos;
    unsigned short nrm;
};

struct DMMorphTarget {
    DMDisplayList* list;
    unsigned char unknown04[52];
};

struct DMMorphData {
    unsigned char unknown00[48];
    DMMorphTarget targets[1];
};

struct DMMesh {
    unsigned char unknown00[44];
    DMMorphData* morph;
    unsigned char unknown30[48];
    DMVec3s* positions;
    unsigned char unknown64[4];
    DMVec3s* normals;
};

struct DMRenderObject_T {
    unsigned char unknown00[12];
    DMMesh* mesh;
    unsigned char unknown10[326];
    unsigned short dlSize;
    void* dl;
    unsigned char unknown15c[252];
    DMMorphObject_T* morph;
};

struct DMBlendWeight {
    unsigned long index;
    float weight;
};

extern CBlendBufferCache _blendCache;

void DMMorphBlendDisplayLists(DMRenderObject_T* object) {
    DMBlendWeight weights[32];
    unsigned long count = 0;
    DMMorphObject_T* morph = object->morph;
    DMMesh* mesh = object->mesh;
    if (morph) {
        DMBlendWeight* next = weights;
        for (unsigned long i = 0; i < morph->count - 1; i++) {
            float weight = morph->weights[i];
            if (weight >= 0.05f) {
                if (count < 4) {
                    next->index = i;
                    count++;
                    next->weight = weight;
                    next++;
                } else {
                    unsigned long smallest = count - 1;
                    float smallestWeight = fabs(next[-1].weight);
                    for (unsigned long j = 0; j < count; j++) {
                        if (fabs(weights[j].weight) < smallestWeight) {
                            smallest = j;
                            smallestWeight = fabs(weights[j].weight);
                        }
                    }
                    if (fabs(weight) > smallestWeight) {
                        weights[smallest].index = i;
                        weights[smallest].weight = weight;
                    }
                }
            }
        }
    }
    weights[count].index = 0;
    weights[count].weight = 1.0f;
    if (count + 1 == 0)
        return;
    DMMorphData* data = mesh->morph;
    DMDisplayList* base = data->targets[0].list;
    CMemBuffer* buffer = _blendCache.GetBuffer(object);
    int vertexIndex = 0;
    buffer->m_used = 0;
    object->dl = buffer->m_data;
    char* prim = base->data;
    char* end = prim + base->size * 32;
    while (*prim && prim != end) {
        *(DMPrimHeader*)((char*)buffer->m_data + buffer->m_used) = *(DMPrimHeader*)prim;
        buffer->m_used += 3;
        DMSrcVertex* src = (DMSrcVertex*)(prim + 3);
        DMSrcVertex* last = src + ((DMPrimHeader*)prim)->count;
        for (; src != last; src++) {
            DMDstVertex vertex;
            DMVec3s* p = &mesh->positions[src->pos];
            DMVec3s* n = &mesh->normals[src->nrm];
            vertex.attr = src->attr;
            vertex.pos.x = p->x;
            vertex.pos.y = p->y;
            vertex.pos.z = p->z;
            vertex.nrm.x = n->x;
            vertex.nrm.y = n->y;
            vertex.nrm.z = n->z;
            vertex.tex = src->tex;
            DMBlendWeight* w = weights;
            for (unsigned long j = 0; j < count; j++, w++) {
                DMMorphTargetIndex* index = &((DMMorphTargetIndex*)data->targets[w->index].list->data)[vertexIndex];
                DMVec3s* tp = &mesh->positions[index->pos];
                DMVec3s* tn = &mesh->normals[index->nrm];
                vertex.pos.x += (int)(w->weight * tp->x);
                vertex.pos.y += (int)(tp->y * w->weight);
                vertex.pos.z += (int)(tp->z * w->weight);
                vertex.nrm.x += (int)(tn->x * w->weight);
                vertex.nrm.y += (int)(tn->y * w->weight);
                vertex.nrm.z += (int)(tn->z * w->weight);
            }
            vertexIndex++;
            *(DMDstVertex*)((char*)buffer->m_data + buffer->m_used) = vertex;
            buffer->m_used += 15;
        }
        prim = (char*)last;
    }
    while (buffer->m_used & 31) {
        ((char*)buffer->m_data)[buffer->m_used] = ""[0];
        buffer->m_used++;
    }
    object->dlSize = buffer->m_used;
}
