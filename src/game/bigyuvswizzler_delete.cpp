// A fragment of bigyuvswizzler.cpp (0x800fce14): DELETE_tBigYUVSwizzler deletes the swizzler's two plane swizzlers (+1536,
// +1540) and frees it (the members are inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
struct tBigSwizzler;

void DELETE_tBigSwizzler(tBigSwizzler*);
extern "C" void MEM_free(void*);

struct tBigYUVSwizzler {
    unsigned char unknown000[1536];
    tBigSwizzler* m_luma;
    tBigSwizzler* m_chroma;
};

void DELETE_tBigYUVSwizzler(tBigYUVSwizzler* swizzler) {
    DELETE_tBigSwizzler(swizzler->m_luma);
    DELETE_tBigSwizzler(swizzler->m_chroma);
    MEM_free(swizzler);
}
