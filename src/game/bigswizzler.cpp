// bigswizzler.cpp (0x800fc198, the whole file): NEW_tBigSwizzlerTexture and
// DELETE_tBigSwizzler. A "big swizzler" re-maps a texture's tiled layout for
// indirect texturing: the source texture's tile size comes from its format,
// and two IA8 maps of the texture's width by one tile row are built - the
// tile map holds each texel's source tile column/row, the pixel map its
// position inside the tile (the row relative to the texel's own row, biased
// by 128) - each wrapped in a texture object (IA8, clamp, no mipmaps) and
// flushed to memory. The names come from the symbols and the allocation
// labels; the layout (shared with bigyuvswizzler.cpp) and the local names
// are inferred. Allocations go to the heap in the RCMP system's fourth word.
#include <dolphin/gx.h>
#include <dolphin/os.h>

namespace RCMP {
class RCMP_SYSTEM {
public:
    virtual ~RCMP_SYSTEM();

    void* (*m_alloc)(unsigned long);
    void (*m_free)(void*);
    int m_field0C;
};

extern RCMP_SYSTEM rcmp_sys;
}

extern "C" void* MEM_alloc(const char*, int, int);
extern "C" void MEM_free(void*);

struct tBigSwizzler {
    unsigned int tileWidth;
    unsigned int tileHeight;
    unsigned int mapWidth;
    unsigned int mapHeight;
    void* tileMap;
    GXTexObj tileTexture;
    unsigned int pixWidth;
    unsigned int pixHeight;
    void* pixMap;
    GXTexObj pixTexture;
};

/* Inferred: the byte offset of texel (x, y) in a 16-bit 4x4-tiled texture. */
inline int TexelOffset16(int x, int y, int width) {
    return (y & 3) * 8 + (x & 3) * 2 + ((x >> 2) + (y >> 2) * ((width + 3) >> 2)) * 32;
}

tBigSwizzler* NEW_tBigSwizzlerTexture(GXTexObj* texture) {
    unsigned int tileWidth = 0;
    unsigned int tileHeight = 0;
    unsigned int width = GXGetTexObjWidth(texture);
    tBigSwizzler* swizzler;
    OSReport("NEW_tBigSwizzlerTexture %d,%d\n", width, GXGetTexObjHeight(texture));
    switch (GXGetTexObjFmt(texture)) {
    case GX_TF_I4:
    case GX_TF_C4:
        tileWidth = 8;
        tileHeight = 8;
        break;
    case GX_TF_I8:
    case GX_TF_IA4:
    case GX_TF_C8:
        tileWidth = 8;
        tileHeight = 4;
        break;
    case GX_TF_IA8:
    case GX_TF_RGB565:
    case GX_TF_RGB5A3:
        tileWidth = 4;
        tileHeight = 4;
        break;
    }
    unsigned int tilesAcross = (tileWidth - 1 + width) / tileWidth;
    unsigned int tileSize = tileWidth * tileHeight;
    swizzler = (tBigSwizzler*)MEM_alloc("tBigSwizzler", sizeof(tBigSwizzler), RCMP::rcmp_sys.m_field0C);
    swizzler->tileWidth = tileWidth;
    swizzler->tileHeight = tileHeight;
    swizzler->mapWidth = width;
    swizzler->mapHeight = tileHeight;
    swizzler->tileMap = MEM_alloc("tBigSwizzler::TileMap", swizzler->mapWidth * swizzler->mapHeight * 2, RCMP::rcmp_sys.m_field0C);
    swizzler->pixWidth = width;
    swizzler->pixHeight = tileHeight;
    swizzler->pixMap = MEM_alloc("tBigSwizzler::PixMap", swizzler->pixWidth * swizzler->pixHeight * 2, RCMP::rcmp_sys.m_field0C);
    unsigned int x;
    unsigned int y;
    for (y = 0; y < swizzler->mapHeight; y++) {
        for (x = 0; x < swizzler->mapWidth; x++) {
            unsigned int i = y * width + x;
            unsigned int tile = i / tileSize;
            unsigned int inTile = i % tileSize;
            unsigned int tileX = tile % tilesAcross;
            unsigned int srcX = x % tileWidth + tileX * tileWidth;
            unsigned int tileY = y / tileHeight;
            unsigned int srcY = inTile / tileWidth + tileY * tileHeight;
            unsigned char* texel = (unsigned char*)&((unsigned short*)swizzler->tileMap)[TexelOffset16(x, y, swizzler->pixWidth) / 2];
            texel[0] = srcX / tileWidth;
            texel[1] = srcY / tileHeight;
        }
    }
    GXInitTexObj(&swizzler->tileTexture, swizzler->tileMap, swizzler->mapWidth, swizzler->mapHeight, GX_TF_IA8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&swizzler->tileTexture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    for (y = 0; y < swizzler->pixHeight; y++) {
        for (x = 0; x < swizzler->pixWidth; x++) {
            unsigned int i = y * width + x;
            unsigned int tile = i / tileSize;
            unsigned int inTile = i % tileSize;
            unsigned int tileX = tile % tilesAcross;
            unsigned int srcX = x % tileWidth + tileX * tileWidth;
            unsigned int tileY = y / tileHeight;
            unsigned int srcY = inTile / tileWidth + tileY * tileHeight;
            unsigned char* texel = (unsigned char*)&((unsigned short*)swizzler->pixMap)[TexelOffset16(x, y, swizzler->pixWidth) / 2];
            texel[0] = srcX % tileWidth;
            texel[1] = srcY % tileHeight - y + 128;
        }
    }
    GXInitTexObj(&swizzler->pixTexture, swizzler->pixMap, swizzler->pixWidth, swizzler->pixHeight, GX_TF_IA8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&swizzler->pixTexture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    DCFlushRange(swizzler->tileMap, swizzler->mapWidth * swizzler->mapHeight * 2);
    DCFlushRange(swizzler->pixMap, swizzler->pixWidth * swizzler->pixHeight * 2);
    return swizzler;
}

void DELETE_tBigSwizzler(tBigSwizzler* swizzler) {
    MEM_free(swizzler->tileMap);
    MEM_free(swizzler->pixMap);
    MEM_free(swizzler);
}
