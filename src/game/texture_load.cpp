// A fragment of texture.cpp (0x80082a60): CTexture::Load loads the texture's
// palette (when it has one) into TLUT 0 and its texture object into map 0,
// and returns 0. The packet and render list are unused. CTexture, CDmaPacket
// and CRenderList are named by the mangled symbols; the members (a GX texture
// object at +8, a TLUT object at +0x28, a word at +0x34 that is nonzero when
// there is a palette) are inferred from the offsets, as is the int result.
// The rest of the file is in other units.
#include <dolphin/gx.h>

class CDmaPacket;
class CRenderList;

class CTexture {
public:
    int Load(CDmaPacket&, CRenderList*);

    unsigned char unknown00[8];
    GXTexObj m_texObj;
    GXTlutObj m_tlutObj;
    int m_value34; // nonzero when the texture has a palette
};

int CTexture::Load(CDmaPacket&, CRenderList*) {
    if (m_value34 != 0)
        GXLoadTlut(&m_tlutObj, GX_TLUT0);
    GXLoadTexObj(&m_texObj, GX_TEXMAP0);
    return 0;
}
