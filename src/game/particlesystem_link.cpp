// A fragment of particlesystem.cpp (0x8007d438): CParticleSystemManager::Link
// (the GX state shared by the particle systems: position, colour and
// texture coordinates in direct format, one textured stage modulating the
// vertex colour, depth test without update and an unlit channel; the render
// type cache and particle counter are reset, every child bin is linked, the
// count is reported every tenth call through DebugMsg and its maximum kept,
// and the children are cleared), CPropertyParticleSystem::Link (the system's
// texture is loaded unless the previous system used the same shape file,
// then the system is rendered through its virtual RenderSystem) and
// CPropertyParticleSystem::Render (nothing to add; returns 0). The file name
// is this project's; the original record is particlesystem.cpp. The classes,
// functions and globals are named by the mangled symbols; the members are
// inferred, and the virtual functions are declared in the order of
// __vt__23CPropertyParticleSystem (result types not known). The counter and
// maximum are file-local in the original and declared extern to link to
// them. Each class declares its destructor (defined elsewhere) first so its
// table is not emitted here. The report's string links to its pool entry.
#include <dolphin/gx.h>

class CDmaTag;
class CMatrix;
class CVector3;
class CColor;
class CDrawContext;
class CRenderList;
struct ShapeFile;

void DebugMsg(const char*, ...);

/* Inferred: the packet's interface is not used directly here. */
class CDmaPacket;

/* Only Load and the 64-byte layout are part of this view. */
class CTexture {
public:
    void Load(CDmaPacket&, CRenderList*);
};

/* Inferred: the shape file's texture at +32. */
struct ShapeFile {
    unsigned char unknown00[32];
    CTexture m_texture;
};

class CRenderBin;

/* CRenderBin's non-polymorphic base (see dmesh_cremapbin_ctor.cpp). */


/* CRenderBin's virtuals in table order. */
#include "game/CRenderBin.h"


class CUcodeRenderBin : public CRenderBin {
public:
    virtual ~CUcodeRenderBin();
};

class CParticleSystemManager : public CUcodeRenderBin {
public:
    virtual ~CParticleSystemManager();
    virtual void Link(CDmaTag*, CDmaPacket&);
};

class CParticleSystem : public CRenderBin {
public:
    virtual ~CParticleSystem();
    virtual void DeActivate();
    virtual void Terminate();
    virtual void IsActive() const;
    virtual void IsEmitting() const;
    virtual void IsBoundingBoxValid() const;
    virtual void IsMoving() const;
    virtual void HasRotation() const;
    virtual void GetTexture() const;
    virtual void GetRenderType() const;
    virtual void GetSeed() const;
    virtual void GetEmmisionRate() const;
    virtual void GetEmmisionDelay() const;
    virtual void GetSystemLifetime() const;
    virtual void GetParticleLifetime() const;
    virtual void GetLocalToWorld(CMatrix&) const;
    virtual void GetSystemInitialVelocity(CVector3&) const;
    virtual void GetSystemAcceleration(CVector3&) const;
    virtual void GetParticleColor(CColor&, CColor&, CColor&, CColor&) const;
    virtual void GetParticleSize(CVector3&, CVector3&, float&) const;
    virtual void GetParticlePosition(CVector3&, CVector3&) const;
    virtual void GetParticleVelocity(CVector3&, CVector3&) const;
    virtual void GetParticleAcceleration(CVector3&) const;
    virtual void GetParticleRotation(float&, float&) const;
    virtual void GetParticleAlpha(float&, float&, float&) const;
    virtual void GetFogEnable() const;
    virtual void SetSeed(int);
    virtual void SetRenderType(int);
    virtual void SetTexture(ShapeFile*);
    virtual void SetLocalToWorld(const CMatrix&);
    virtual void SetEmmisionRate(float);
    virtual void SetEmmisionDelay(float);
    virtual void SetSystemLifetime(float);
    virtual void SetParticleLifetime(float);
    virtual void SetSystemInitialVelocity(CVector3);
    virtual void SetSystemAcceleration(CVector3);
    virtual void SetParticleColor(CColor, CColor);
    virtual void SetParticleSize(CVector3, CVector3, float);
    virtual void SetParticlePosition(CVector3, CVector3);
    virtual void SetParticleVelocity(CVector3, CVector3);
    virtual void SetParticleAcceleration(CVector3);
    virtual void SetFogEnable(bool);
    virtual void UpdateSystem(float);
    virtual void RenderSystem(CDmaPacket&);

    unsigned char unknown20[84];
    ShapeFile* m_shape;
};

class CPropertyParticleSystem : public CParticleSystem {
public:
    virtual ~CPropertyParticleSystem();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);
};

extern int g_RenderTypeCache;
extern int g_ParticleCounter;
extern int g_ParticleCounterMax;

void CParticleSystemManager::Link(CDmaTag*, CDmaPacket& packet) {
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetNumChans(1);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 60, GX_FALSE, 125);
    GXSetNumTevStages(1);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_4, GX_TRUE, GX_TEVPREV);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    g_RenderTypeCache = -1;
    g_ParticleCounter = 0;
    for (CRenderBin* child = m_children; child; child = child->m_next)
        child->Link(0, packet);
    static int counter = 10;
    if (counter-- <= 0) {
        counter = 10;
        DebugMsg("# Particles = %d (MAX %d)\n", g_ParticleCounter, g_ParticleCounterMax);
    }
    if (g_ParticleCounter > g_ParticleCounterMax)
        g_ParticleCounterMax = g_ParticleCounter;
    m_children = 0;
}

void CPropertyParticleSystem::Link(CDmaTag*, CDmaPacket& packet) {
    bool load = true;
    if (m_prev && m_shape == ((CPropertyParticleSystem*)m_prev)->m_shape)
        load = false;
    if (load)
        m_shape->m_texture.Load(packet, 0);
    RenderSystem(packet);
}

int CPropertyParticleSystem::Render(CDmaPacket&, void*) {
    return 0;
}
