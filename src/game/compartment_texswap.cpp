// A fragment of compartment.cpp (0x80075a14): CCompartment::
// TextureSwapCollisionDisabled. A triangle whose flags carry a swap kind
// (bits 3-4) never has its collision disabled; otherwise the result is
// whether the compartment's texture-swap cache entry for the triangle selects
// the triangle's own material slot (its low three bits, counted from 1). The
// cache is the file-local g_TextureSwapCache (0x8020 bytes: 32 rows of 512
// two-byte entries, then 32 bytes), declared extern here. CCompartment,
// CDBTri and CTextureSwapCache are named by the mangled symbols; the members,
// the entry layout and the result type are inferred from the code. The rest
// of the file is not part of this unit.
struct CDBTri {
    unsigned char unknown00[43];
    unsigned char m_flags;
};

struct CCompartmentGeometry {
    unsigned char unknown00[40];
    const CDBTri** m_tris;
};

struct STextureSwapEntry {
    unsigned char m_flags;
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

    unsigned char unknown00[80];
    int m_swapSet;
    unsigned char unknown54[4];
    CCompartmentGeometry* m_geometry;
};

bool CCompartment::TextureSwapCollisionDisabled(int index) {
    const CDBTri* tri = m_geometry->m_tris[index];
    if (((tri->m_flags >> 3) & 3) == 0)
        return ((g_TextureSwapCache.m_entries[m_swapSet][index].m_flags >> 5) & 7) == (tri->m_flags & 7) - 1;
    return false;
}
