// A fragment of projectilebullet.cpp (0x800d063c):
// CProjectileBulletRenderBin::Link (GX state for textured, vertex-coloured,
// blended quads with depth test but no depth writes and an unlit channel;
// the bin's texture is loaded and each linked tracer is drawn as a white quad
// from its four corners in camera space; depth writes are turned back on and
// the camera axes marked cached) and the weak STracer::GetVerts emitted after
// it (the segment's ends moved either way along a side vector: the camera's
// Y axis minus, or negated minus, its X axis as the segment runs, normalised
// and scaled by the tracer's width times a mode factor). The file name is
// this project's; the original record is projectilebullet.cpp. The classes,
// functions and globals are named by the mangled symbols; the members and
// the inline vector helpers are inferred. g_bCached, g_WorldToCamera and the
// axes are file-local in the original and declared extern to link to them.
// CProjectileBulletRenderBin declares its destructor (defined elsewhere)
// first so its table is not emitted here. Render after these reads the axes
// relative to g_WorldToCamera's address (one base register into the file's
// .bss block), which a fragment cannot reproduce; it is not part of this
// unit. The constants link to their pool entries.
#include <dolphin/gx.h>
#include <math.h>

class CDmaPacket;
class CRenderList;

/* Inferred: CVector3 as four floats (copies move doubleword pairs). */
class CVector3 {
public:
    float Dot(const CVector3& o) const { return x * o.x + y * o.y + z * o.z; }
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            x *= scale;
            y *= scale;
            z *= scale;
        }
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

inline CVector3 operator*(float s, const CVector3& v) {
    return CVector3(s * v.x, s * v.y, s * v.z);
}

/* Only the GX export of the matrix is part of this view. */
class CMatrix {
public:
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;

    float m[4][4];
} __attribute__((aligned(16)));

/* Inferred: the packet's write cursor at +8. */
class CDmaPacket {
public:
    unsigned char unknown00[8];
    unsigned char* m_cur;
};

/* Only Load and the 64-byte size are part of this view. */
class CTexture {
public:
    void Load(CDmaPacket&, CRenderList*);

    unsigned char unknown00[64];
};

/* Inferred: a tracer's segment and width (40 bytes, added to the packet). */
struct STracer {
    void GetVerts(CVector3*);

    CVector3 m_start;
    float m_width;
    CVector3 m_end;
};

/* Inferred: a render tag's link and its tracer. */
class CDmaTag {
public:
    CDmaTag* m_next;
    STracer* m_tracer;
};

class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CProjectileBulletRenderBin : public CRenderBin {
public:
    virtual ~CProjectileBulletRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);

    int unknown20;
    CTexture m_texture;
};

extern bool g_bCached;
extern bool g_bInMultiplayerMode;
extern CMatrix g_WorldToCamera;
extern CVector3 g_XAxis;
extern CVector3 g_YAxis;

void CProjectileBulletRenderBin::Link(CDmaTag* tag, CDmaPacket& packet) {
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
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
    GXSetCullMode(GX_CULL_NONE);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_4, GX_TRUE, GX_TEVPREV);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    m_texture.Load(packet, 0);
    for (CDmaTag* t = tag; t; t = t->m_next) {
        CVector3 verts[4];
        float matrix[3][4];
        t->m_tracer->GetVerts(verts);
        g_WorldToCamera.GetGAMECUBEMatrix34(matrix);
        GXLoadPosMtxImm(matrix, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXPosition3f32(verts[0].x, verts[0].y, verts[0].z);
        GXColor4u8(255, 255, 255, 255);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(verts[1].x, verts[1].y, verts[1].z);
        GXColor4u8(255, 255, 255, 255);
        GXTexCoord2f32(0.0f, 1.0f);
        GXPosition3f32(verts[2].x, verts[2].y, verts[2].z);
        GXColor4u8(255, 255, 255, 255);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(verts[3].x, verts[3].y, verts[3].z);
        GXColor4u8(255, 255, 255, 255);
        GXTexCoord2f32(1.0f, 0.0f);
    }
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    g_bCached = true;
}

inline void STracer::GetVerts(CVector3* verts) {
    CVector3 side;
    CVector3 dir;
    dir.x = m_end.x - m_start.x;
    dir.y = m_end.y - m_start.y;
    dir.z = m_end.z - m_start.z;
    if (dir.Dot(g_XAxis) * dir.Dot(g_YAxis) > 0.0f) {
        side.x = g_YAxis.x - g_XAxis.x;
        side.y = g_YAxis.y - g_XAxis.y;
        side.z = g_YAxis.z - g_XAxis.z;
    } else {
        CVector3 flipped = -1.0f * g_YAxis;
        side.x = flipped.x - g_XAxis.x;
        side.y = flipped.y - g_XAxis.y;
        side.z = flipped.z - g_XAxis.z;
    }
    float width = m_width * (g_bInMultiplayerMode ? 0.05f : 0.025f);
    side.Normalize();
    side.x *= width;
    side.y *= width;
    side.z *= width;
    verts[0].x = m_start.x + side.x;
    verts[0].y = m_start.y + side.y;
    verts[0].z = m_start.z + side.z;
    verts[1].x = m_start.x - side.x;
    verts[1].y = m_start.y - side.y;
    verts[1].z = m_start.z - side.z;
    verts[2].x = m_end.x - side.x;
    verts[2].y = m_end.y - side.y;
    verts[2].z = m_end.z - side.z;
    verts[3].x = m_end.x + side.x;
    verts[3].y = m_end.y + side.y;
    verts[3].z = m_end.z + side.z;
}
