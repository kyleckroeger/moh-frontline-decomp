// A fragment of decal.cpp (0x8008304c): CBulletDecalManager::Link (the GX
// state shared by the decal bins: position, colour and texture coordinates
// in direct format, alpha compare with depth compare after texturing, one
// textured stage modulated by the vertex colour, alpha blending, no culling),
// CBulletDecalTextureBin::IsUsed (a decal is listed), Link (the bin's texture
// is loaded and its decals are drawn as quads, four vertices each, without
// depth update; depth update is turned back on) and Render (returns 0), and
// CBulletDecalManager's Draw (each of the 13 bins holding decals is rendered
// through the file-local render list, the context kept), IsUsed (decals are
// active), IsDrawEnabled and IsVisible (always). The file name is this
// project's; the original record is decal.cpp. The classes, functions and
// globals are named by the mangled symbols (CRenderBinData by CRenderBin's
// RTTI record); the members are inferred, as in decal_dtor.cpp. Each class
// declares its destructor (defined in decal_dtor.cpp) first so its table is
// not emitted here; the render list and context are file-local in the
// original and declared extern to link to them.
#include <dolphin/gx.h>

class CDmaTag;
class CDmaPacket;
class CRenderList;
class CBullet;
class CCollision;
class CDrawContext;
class CLight;
class CPlayerObject;
class CStaticObject;
class CMatrix;
class CVector3;
struct AIDoodadView;







/* ISceneNode's virtuals in table order (slots 20 to 216). They are the weak
   inline defaults of Moh2.cpp in the original; their bodies are not repeated
   here, so they are declared without one. */
#include "game/ISceneNode.h"


class CRenderBin;

/* CRenderBin's non-polymorphic base, named by CRenderBin's RTTI record; the
   members are inferred (CRenderBin::Init walks children from +8 through the
   siblings at +4, IsUsed tests +16, +20 is the render priority, 3 by
   default). */
struct CRenderBinData {
    CRenderBinData() : data00(0), m_next(0), m_children(0), data0c(0), data10(0), m_priority(3) {}

    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order; all are weak (inline) in the
   original. Init's and IsUsed's bodies are not part of this view, so they
   are declared without one, which also keeps CRenderBin's table out of this
   unit. */
class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed();
};

class CUcodeRenderBin : public CRenderBin {
public:
    CUcodeRenderBin() {}
    virtual ~CUcodeRenderBin() {}
    virtual void Link(CDmaTag*, CDmaPacket&);
};


/* Only Load and the 64-byte size are part of this view. */
class CTexture {
public:
    void Load(CDmaPacket&, CRenderList*);

    unsigned char unknown00[64];
};

class CRenderList {
public:
    void Render(CRenderBin&, void*);
};

/* Inferred: a decal's four vertices (position, colour, texture coordinates)
   and the next decal of its bin. */
struct SDecalVertex {
    float x, y, z;
    unsigned char r, g, b, a;
    float u, v;
};

/* Inferred inline helper (the name is this project's). */
inline void SendVertex(const SDecalVertex& v) {
    GXPosition3f32(v.x, v.y, v.z);
    GXColor4u8(v.r, v.g, v.b, v.a);
    GXTexCoord2f32(v.u, v.v);
}

struct BulletDecal {
    SDecalVertex m_vertices[4];
    BulletDecal* m_next;
};

class CBulletDecalTextureBin : public CUcodeRenderBin {
public:
    virtual ~CBulletDecalTextureBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    int m_count;
    BulletDecal* m_decals;
    CTexture m_texture;
    int unknown68;
};

class CBulletDecalManager : public ISceneNode, public CUcodeRenderBin {
public:
    virtual ~CBulletDecalManager();
    virtual void Draw(CDrawContext&);
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    CBulletDecalTextureBin m_bins[13];
    int unknown5c0;
    int m_active;
};

extern CRenderList* g_pRenderList;
extern CDrawContext* g_pContext;

void CBulletDecalManager::Link(CDmaTag*, CDmaPacket&) {
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_GEQUAL, 0, GX_AOP_AND, GX_LEQUAL, 255);
    GXSetNumChans(1);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 60, GX_FALSE, 125);
    GXSetNumTevStages(1);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    GXSetCullMode(GX_CULL_NONE);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
}

bool CBulletDecalTextureBin::IsUsed() {
    return m_decals != 0;
}

void CBulletDecalTextureBin::Link(CDmaTag*, CDmaPacket& packet) {
    m_texture.Load(packet, 0);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    BulletDecal* decal = m_decals;
    int count = 0;
    for (BulletDecal* d = decal; d; d = d->m_next)
        count++;
    GXBegin(GX_QUADS, GX_VTXFMT0, count * 4);
    for (; decal; decal = decal->m_next) {
        SendVertex(decal->m_vertices[0]);
        SendVertex(decal->m_vertices[1]);
        SendVertex(decal->m_vertices[2]);
        SendVertex(decal->m_vertices[3]);
    }
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
}

int CBulletDecalTextureBin::Render(CDmaPacket&, void*) {
    return 0;
}

void CBulletDecalManager::Draw(CDrawContext& context) {
    g_pContext = &context;
    for (int i = 0; i < 13; i++) {
        if (m_bins[i].m_count > 0)
            g_pRenderList->Render(m_bins[i], 0);
    }
}

bool CBulletDecalManager::IsUsed() {
    return m_active > 0;
}

int CBulletDecalManager::IsDrawEnabled() const {
    return 1;
}

int CBulletDecalManager::IsVisible(CDrawContext&) const {
    return 1;
}
