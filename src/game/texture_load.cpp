// texture.cpp (0x80082a60), the whole file: CTexture::Load loads the
// texture's palette (when it has one) into TLUT 0 and its texture object into
// map 0, and returns 0 (the packet and render list are unused).
// CTexture::Set(Shape*, bool) records the shape's size and mip count, maps
// its format (20 RGB565, 22 RGBA8, 24 C4 with 16 palette entries, 25 C8 with
// 256, 30 CMPR) and initialises the texture object, or for the indexed formats
// the TLUT object from the shape's palette record (type '0' IA8, '1' RGB565,
// '2' RGB5A3) and the colour-indexed texture object; with mipmaps it sets the
// LOD range and reports the mip count. Set(ShapeFile*, bool) does the same for
// the shape inside a "SHPG" shape file and reports any other file.
// CTexture, Shape, ShapeFile, CDmaPacket and CRenderList are named by the
// mangled symbols; the members (a GX texture object at +8, a TLUT object at
// +0x28, a word at +0x34 that is nonzero when there is a palette, the size
// and mip bit fields at +0x3c), the shape and shape-file layouts and the
// inline palette lookup are inferred, as is the int result. The palette type
// tests compare the type character as unsigned (cmplwi); '0'/'1' are one
// unsigned-char range test.
extern "C" {
#include <stdio.h>
#include <string.h>
#include <dolphin/gx.h>
}

class CDmaPacket;
class CRenderList;

/* inferred: a shape record: the format byte with a signed 24-bit offset of
   its palette record, the size, the mip count in the high nibble of +14 and
   the image data at +16 (a palette record has its type character first and
   its entries at +16) */
struct Shape {
    unsigned int format : 8;
    int paletteOffset : 24;
    short width;
    short height;
    unsigned char unknown08[6];
    unsigned char mips : 4;
    unsigned char unknown0e : 4;
    unsigned char unknown0f;
    unsigned char data[1];
};

/* inferred: a shape file: its "SHPG" tag and the offset of its shape */
struct ShapeFile {
    char tag[4];
    unsigned char unknown04[16];
    int shapeOffset;
};

void DebugMsg(const char*, ...);

class CTexture {
public:
    int Load(CDmaPacket&, CRenderList*);
    void Set(Shape*, bool);
    void Set(ShapeFile*, bool);

    unsigned char unknown00[8];
    GXTexObj m_texObj;
    GXTlutObj m_tlutObj;
    int m_value34; // nonzero when the texture has a palette
    unsigned char unknown38[4];
    int m_width : 10;
    int m_height : 10;
    int m_mips : 4;
    unsigned int m_flags : 8;
};

/* inferred: the shape's palette record, when its type is known */
static inline unsigned char* GetPalette(Shape* shape) {
    if (!shape->paletteOffset)
        return 0;
    unsigned char* palette = (unsigned char*)shape + shape->paletteOffset;
    if ((unsigned)*palette == ' ' || (unsigned)*palette == '!' || (unsigned char)(*palette - '0') <= 1 || (unsigned)*palette == '2')
        return palette;
    return 0;
}

int CTexture::Load(CDmaPacket&, CRenderList*) {
    if (m_value34 != 0)
        GXLoadTlut(&m_tlutObj, GX_TLUT0);
    GXLoadTexObj(&m_texObj, GX_TEXMAP0);
    return 0;
}

void CTexture::Set(Shape* shape, bool) {
    GXCITexFmt indexFormat;
    GXBool mipmap = false;
    GXTexFmt format;
    int entries;

    m_width = shape->width;
    m_height = shape->height;
    m_mips = shape->mips + 1;
    if (m_mips > 1)
        mipmap = true;
    switch (shape->format) {
    case 20:
        m_value34 = 0;
        format = GX_TF_RGB565;
        break;
    case 22:
        m_value34 = 0;
        format = GX_TF_RGBA8;
        break;
    case 24:
        m_value34 = 1;
        indexFormat = GX_TF_C4;
        entries = 16;
        break;
    case 25:
        m_value34 = 1;
        indexFormat = GX_TF_C8;
        entries = 256;
        break;
    case 30:
        m_value34 = 0;
        format = GX_TF_CMPR;
        break;
    }
    if (m_value34 == 0) {
        GXInitTexObj(&m_texObj, shape->data, shape->width, shape->height, format, GX_CLAMP, GX_CLAMP, mipmap);
    } else {
        unsigned char* palette = GetPalette(shape);
        GXTlutFmt tlutFormat;
        switch (*palette) {
        case '1':
            tlutFormat = GX_TL_RGB565;
            break;
        case '2':
            tlutFormat = GX_TL_RGB5A3;
            break;
        case '0':
            tlutFormat = GX_TL_IA8;
            break;
        }
        GXInitTlutObj(&m_tlutObj, palette + 16, tlutFormat, entries);
        GXInitTexObjCI(&m_texObj, shape->data, shape->width, shape->height, indexFormat, GX_CLAMP, GX_CLAMP, mipmap, 0);
    }
    if (mipmap == true)
        GXInitTexObjLOD(&m_texObj, m_value34 ? GX_LIN_MIP_NEAR : GX_LIN_MIP_LIN, GX_LINEAR, 0.0f, m_mips, 9.0f, GX_TRUE,
                        GX_TRUE, m_value34 == 0 ? GX_ANISO_2 : GX_ANISO_1);
    if (m_mips > 1)
        printf("Mipmaps! - [%d]\n", m_mips);
}

void CTexture::Set(ShapeFile* file, bool) {
    Shape* shape = (Shape*)((char*)file + file->shapeOffset);
    if (strncmp((char*)file, "SHPG", 4)) {
        DebugMsg("Non GAMECUBE shape format!\n");
        return;
    }
    GXTexFmt format;
    GXCITexFmt indexFormat;
    GXTlutFmt tlutFormat;
    int entries;
    GXBool mipmap = false;


    m_width = shape->width;
    m_height = shape->height;
    m_mips = shape->mips + 1;
    if (m_mips > 1)
        mipmap = true;
    switch (shape->format) {
    case 20:
        m_value34 = 0;
        format = GX_TF_RGB565;
        break;
    case 22:
        m_value34 = 0;
        format = GX_TF_RGBA8;
        break;
    case 24:
        m_value34 = 1;
        indexFormat = GX_TF_C4;
        entries = 16;
        break;
    case 25:
        m_value34 = 1;
        indexFormat = GX_TF_C8;
        entries = 256;
        break;
    case 30:
        m_value34 = 0;
        format = GX_TF_CMPR;
        break;
    }
    if (m_value34 == 0) {
        GXInitTexObj(&m_texObj, shape->data, shape->width, shape->height, format, GX_CLAMP, GX_CLAMP, mipmap);
    } else {
        unsigned char* palette = GetPalette(shape);
        switch (*palette) {
        case '1':
            tlutFormat = GX_TL_RGB565;
            break;
        case '2':
            tlutFormat = GX_TL_RGB5A3;
            break;
        case '0':
            tlutFormat = GX_TL_IA8;
            break;
        }
        GXInitTlutObj(&m_tlutObj, palette + 16, tlutFormat, entries);
        GXInitTexObjCI(&m_texObj, shape->data, shape->width, shape->height, indexFormat, GX_CLAMP, GX_CLAMP, mipmap, 0);
    }
    if (mipmap == true)
        GXInitTexObjLOD(&m_texObj, m_value34 ? GX_LIN_MIP_NEAR : GX_LIN_MIP_LIN, GX_LINEAR, 0.0f, m_mips, 9.0f, GX_TRUE,
                        GX_TRUE, m_value34 == 0 ? GX_ANISO_2 : GX_ANISO_1);
    if (m_mips > 1)
        printf("Mipmaps! - [%d]\n", m_mips);
}
