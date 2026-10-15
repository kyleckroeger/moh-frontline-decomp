// A fragment of compartment.cpp (0x80075a14): CCompartment::
// TextureSwapCollisionDisabled. A triangle whose flags carry a swap kind
// (bits 3-4) never has its collision disabled; otherwise the result is
// whether the compartment's texture-swap cache entry for the triangle selects
// the triangle's own material slot (its low three bits, counted from 1).
// Then CCompartment::TextureSwap, which counts down the triangle's swap delay
// and, when it runs out, restarts it and advances the triangle's frame in the
// cache (kind 0 stops at the last frame, kind 1 wraps, kind 2 ping-pongs),
// setting the triangle's material to that frame. The
// cache is the file-local g_TextureSwapCache (0x8020 bytes: 32 rows of 512
// two-byte entries, then 32 bytes), declared extern here. CCompartment,
// CDBTri and CTextureSwapCache are named by the mangled symbols; the members,
// the entry layout, the flag bit-fields and the result type are inferred from
// the code. The rest
// of the file is not part of this unit.
/* Inferred: the triangle's swap flags (high bits first: two unused bits,
   the ping-pong direction, the swap kind and the frame count). */
struct CDBTriSwapBits {
    unsigned char unused : 2;
    unsigned char m_reverse : 1;
    unsigned char m_kind : 2;
    unsigned char m_count : 3;
};

struct CDBTri {
    unsigned char unknown00[8];
    void* m_material;
    unsigned char unknown0c[8];
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
    bool TextureSwapCollisionDisabled(int index);
    void TextureSwap(int index);

    unsigned char unknown00[80];
    int m_swapSet;
    unsigned char unknown54[4];
    CCompartmentGeometry* m_geometry;
};

bool CCompartment::TextureSwapCollisionDisabled(int index) {
    const CDBTri* tri = m_geometry->m_tris[index];
    if (((tri->m_swap.m_flags >> 3) & 3) == 0)
        return ((g_TextureSwapCache.m_entries[m_swapSet][index].m_state.m_flags >> 5) & 7) == (tri->m_swap.m_flags & 7) - 1;
    return false;
}

void CCompartment::TextureSwap(int index) {
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
