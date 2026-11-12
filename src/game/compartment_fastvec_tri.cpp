// A fragment of compartment.cpp (0x800756cc): CCompartment::HandleExplosion
// and the weak dwi::fast_vec<const CDBTri*> destructor after it. The
// explosion collects the compartment triangles its sphere touches, gathers
// their distinct texture-swap indices and advances each swap whose collision
// is not disabled. HandleExplosion inlines TextureSwapCollisionDisabled and
// TextureSwap, which the original defines after it (compartment_texswap.cpp
// holds their own copies); their bodies are repeated here as inline helpers.
// The int vector's weak destructor is compiled too but has no reference; the
// original linker removed it (linker_stripped). Members are inferred from
// offsets; the rest of the file is not part of this unit.
struct CDBTri;
class CVolSphere;

extern "C" void MEM_free(void*);

namespace dwi {
/* Inferred: the vector over caller storage (inline constructor; the
   destructor frees the storage only when the vector owns it). */
template <class T>
class fast_vec {
public:
    fast_vec(T* storage, int capacity, int growth)
        : m_data(storage), m_unknown4(0), m_size(0), m_capacity(capacity), m_owned(false), m_growth(growth) {
        m_size = 0;
    }
    ~fast_vec() {
        if (m_owned && m_data)
            MEM_free(m_data);
    }
    T* begin() { return m_data; }
    T* end() { return m_data + m_size; }
    void push_back(const T& value) { m_data[m_size++] = value; }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};

}

class CCDBObject {
public:
    void GetIntersectingTris(dwi::fast_vec<const CDBTri*>&, const CVolSphere&) const;

    unsigned char unknown00[1];
};

/* Inferred: the explosion's sphere at +16. */
class CExplosion {
public:
    unsigned char unknown00[16];
    unsigned char m_sphere[1];
};

/* Inferred: the triangle's swap flags (high bits first: two unused bits,
   the ping-pong direction, the swap kind and the frame count). */
struct CDBTriSwapBits {
    unsigned char unused : 2;
    unsigned char m_reverse : 1;
    unsigned char m_kind : 2;
    unsigned char m_count : 3;
};

/* Inferred: the triangle's texture-swap index (an 11-bit signed field in the
   word at +12). */
struct CDBTriTexIndex {
    int m_other : 11;
    int m_index : 11;
    int m_rest : 10;
};

struct CDBTri {
    unsigned char unknown00[8];
    void* m_material;
    CDBTriTexIndex m_texIndex;
    unsigned char unknown10[4];
    void* m_frames[5];
    unsigned char m_delay;
    unsigned char m_countdown;
    unsigned char unknown2a;
    union {
        unsigned char m_flags;
        CDBTriSwapBits m_bits;
    } m_swap;
};

struct CCompartmentGeometry {
    unsigned char unknown00[40];
    CDBTri** m_tris;
};

/* Inferred: the cache entry's current frame in its top three bits. */
struct STextureSwapBits {
    unsigned char m_frame : 3;
    unsigned char m_rest : 5;
};

struct STextureSwapEntry {
    union {
        unsigned char m_flags;
        STextureSwapBits m_bits;
    } m_state;
    unsigned char m_data;
};

class CTextureSwapCache {
public:
    STextureSwapEntry m_entries[32][512];
    unsigned char unknown8000[32];
};

extern CTextureSwapCache g_TextureSwapCache;

class CCompartment {
public:
    void HandleExplosion(const CExplosion&);
    bool TextureSwapCollisionDisabled(int index);
    void TextureSwap(int index);

    unsigned char unknown00[40];
    CCDBObject m_cdb;
    unsigned char unknown29[39];
    int m_swapSet;
    unsigned char unknown54[4];
    CCompartmentGeometry* m_geometry;
};

/* Inferred: the bodies of TextureSwapCollisionDisabled and TextureSwap, shared
   with HandleExplosion. */
inline bool CCompartment::TextureSwapCollisionDisabled(int index) {
    const CDBTri* tri = m_geometry->m_tris[index];
    if (((tri->m_swap.m_flags >> 3) & 3) == 0)
        return ((g_TextureSwapCache.m_entries[m_swapSet][index].m_state.m_flags >> 5) & 7) == (tri->m_swap.m_flags & 7) - 1;
    return false;
}

inline void CCompartment::TextureSwap(int index) {
    CDBTri* tri = m_geometry->m_tris[index];
    if (tri->m_countdown-- != 0)
        return;
    tri->m_countdown = tri->m_delay;
    int frame = g_TextureSwapCache.m_entries[m_swapSet][index].m_state.m_bits.m_frame;
    int count = tri->m_swap.m_bits.m_count;
    switch (tri->m_swap.m_bits.m_kind) {
    case 0:
        if (++frame >= count)
            frame = count - 1;
        break;
    case 1:
        if (++frame >= count)
            frame = 0;
        break;
    case 2:
        if (tri->m_swap.m_bits.m_reverse) {
            if (--frame <= 0) {
                frame = 0;
                tri->m_swap.m_bits.m_reverse = 0;
            }
        } else if (++frame >= count) {
            frame = count - 2;
            tri->m_swap.m_bits.m_reverse = 1;
        }
        break;
    }
    tri->m_material = tri->m_frames[frame];
    g_TextureSwapCache.m_entries[m_swapSet][index].m_state.m_bits.m_frame = frame;
}

template <class It, class T> inline It Find(It first, It last, const T& value) {
    while (first != last) {
        if (*first == value)
            break;
        ++first;
    }
    return first;
}

void CCompartment::HandleExplosion(const CExplosion& explosion) {
    const CDBTri* storage[512];
    dwi::fast_vec<const CDBTri*> tris(storage, 512, -1);
    m_cdb.GetIntersectingTris(tris, *(const CVolSphere*)explosion.m_sphere);
    if (tris.m_size > 0) {
        int storage2[512];
        dwi::fast_vec<int> indices(storage2, 512, 1);
        for (int i = 0; i < tris.m_size; i++) {
            if (tris.m_data[i]->m_texIndex.m_index >= 0) {
                int index = tris.m_data[i]->m_texIndex.m_index;
                if (Find(indices.begin(), indices.end(), index) == indices.end())
                    indices.push_back(index);
            }
        }
        for (int j = 0; j < indices.m_size; j++) {
            if (!TextureSwapCollisionDisabled(indices.m_data[j]))
                TextureSwap(indices.m_data[j]);
        }
    }
}

