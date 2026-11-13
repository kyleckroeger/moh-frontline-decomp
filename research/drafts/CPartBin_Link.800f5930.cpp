// A fragment of dmesh.cpp (0x800f5f04): CDMRenderBin::IsUsed (its used flag)
// and Link (the flag cleared; priority-8 bins mark the weapon draw finished,
// the others the scene draw, and a priority-8 bin then clears the depth
// buffer: depth writes on, colour writes off, a copy of the display with a
// black clear colour and the far depth), CPart::Render (returns 0),
// CRemapBin::Link (the target bin's link data is saved, replaced by this
// bin's, the target linked, and the data swapped back), CRemapBin::Render
// (the target bin renders; returns 0) and CPartBin::Init (link words and the
// remap cursor cleared). The file name is this project's; the original record
// is dmesh.cpp. The classes, functions and globals are named by the mangled
// symbols (CRenderBinData by CRenderBin's RTTI record); the members are
// inferred. The flags and cursor are file-local in the original and declared
// extern to link to them. Each class declares its destructor (defined
// elsewhere) first so its table is not emitted here; CRenderBin's Init and
// IsUsed are declared without a body, which keeps its weak table out of this
// unit (the saved copy refers to it).
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <dolphin/mtx.h>
extern "C" void PPCSync(void);

class CDmaPacket;

/* Inferred: a render tag: the next tag and the tagged object. */
class CDmaTag {
public:
    CDmaTag* m_next;
    struct CDMeshLinkView* m_mesh;
};

class CRenderList;

class CTexture {
public:
    void Load(CDmaPacket&, CRenderList*);
};

/* Inferred: a part: its display list, size in 32-byte units and the indices
   of its matrices. */
#pragma pack(2)
struct CPartView {
    void* m_list;
    unsigned short m_size;
    unsigned short m_numMatrices;
    unsigned char m_matrices[10];
};
#pragma pack()

#include "game/CVector3.h"


/* Inferred: the lighting block at +64 of a mesh. */
struct DMeshLightingView {
    unsigned char unknown00[128];
    float (*m_matrices)[3][4];
    unsigned char unknown84[20];
    CVector3 m_lightPos[3];
    float m_lightColor[3][4];
    float m_ambient[3];
    float m_alpha;

    CVector3 GetLightPos(int i) const { return m_lightPos[i]; }
};

struct DMeshGeometryView {
    unsigned char unknown00[96];
    void* m_positions;
    unsigned char unknown64[4];
    void* m_normals;
    unsigned char unknown6c[4];
    void* m_texCoords;
};

/* Inferred: the mesh fields the part bin uses. */
struct CDMeshLinkView {
    unsigned char unknown000[12];
    DMeshGeometryView* m_geometry;
    unsigned char unknown010[48];
    DMeshLightingView m_lighting;
    unsigned char unknown148[14];
    unsigned short m_skinSize;
    void* m_skin;
    unsigned char unknown15c[248];
    int* m_matrixCount;
    unsigned char unknown258[8];
    float m_projection[4][4];
};

struct FogView {
    unsigned char unknown00[64];
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char unknown43;
    float start;
    float end;
    int type;
};

extern FogView* g_pFog;
extern GXFogType g_GCFogType[];
extern float g_NearPlane;
extern float g_FarPlane;


class CRenderBin;

/* CRenderBin's non-polymorphic base (see dmesh_cremapbin_ctor.cpp). */
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

/* CRenderBin's virtuals in table order; the destructor, Render and Link are
   inline (weak in the original). */
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
    virtual ~CUcodeRenderBin();
};

class CDMRenderBin : public CUcodeRenderBin {
public:
    virtual ~CDMRenderBin();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    bool m_used;
    unsigned char unknown21[7];
    int m_remapData;
};

class CPart : public CRenderBin {
public:
    virtual ~CPart();
    virtual int Render(CDmaPacket&, void*);
};

class CRemapBin : public CRenderBin {
public:
    virtual ~CRemapBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);

    CDMRenderBin* m_bin;
    int m_remapData;
};

class CPartBin : public CRenderBin {
public:
    virtual ~CPartBin();
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);

    int m_value20;
    unsigned int m_flags;
    CTexture* m_texture;
    unsigned int m_count;
    CPartView* m_parts;
};

/* Inferred: only the display copy's frame buffer at +248. */
struct CScreenView {
    unsigned char unknown00[248];
    void* m_frameBuffer;
};

extern CScreenView g_screen;
extern bool _finish_draw;
extern bool _finish_draw_weapon;
extern int _remap_next;

/* inferred */
static inline void InitLightPos(GXLightObj* light, CVector3 position) {
    GXInitLightPos(light, position.x, position.y, position.z);
}

void CPartBin::Link(CDmaTag* tag, CDmaPacket& packet) {
    GXColor fogColor;
    DMeshLightingView* lighting;
    int slot;
    CDMeshLinkView* mesh;
    int mask = 0;
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_S16, 14);
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_NRM, GX_NRM_XYZ, GX_S16, 15);
    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
    GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_S16, 14);
    GXSetNumChans(1);
    GXSetNumTevStages(1);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    if (this->m_flags & 0x10) {
        GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
    } else {
        fogColor.a = 255;
        fogColor.r = g_pFog->r;
        fogColor.g = g_pFog->g;
        fogColor.b = g_pFog->b;
        GXSetFog(g_GCFogType[g_pFog->type], g_pFog->start, g_pFog->end, g_NearPlane, g_FarPlane, fogColor);
    }
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZCompLoc(GX_TRUE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetCullMode(GX_CULL_NONE);
    while (tag) {
        f32 projection[7];
        GXLightObj lights[3];
        Mtx normal;
        GXGetProjectionv(projection);
        mesh = tag->m_mesh;
        lighting = &mesh->m_lighting;
        DMeshGeometryView* geometry = mesh->m_geometry;
        int* count = mesh->m_matrixCount;
        float (*matrices)[3][4] = lighting->m_matrices;
        GXSetProjection(mesh->m_projection, GX_PERSPECTIVE);
        bool skinned = mesh->m_skin != 0;
        DCStoreRangeNoSync(matrices, *count * 48);
        if (skinned)
            DCStoreRangeNoSync(mesh->m_skin, mesh->m_skinSize);
        if (skinned) {
            GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
            GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        } else {
            GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
            GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        }
        GXSetArray(GX_VA_POS, geometry->m_positions, 6);
        GXSetArray(GX_VA_NRM, geometry->m_normals, 6);
        GXSetArray(GX_VA_TEX0, geometry->m_texCoords, 4);
        GXSetArray(GX_POS_MTX_ARRAY, matrices, 48);
        GXInvalidateVtxCache();
        mask = 0;
        int i;
        for (i = 0; i < 3; i++) {
            float red = lighting->m_lightColor[i][0];
            if (red != 0.0f || lighting->m_lightColor[i][1] != 0.0f || lighting->m_lightColor[i][2] != 0.0f) {
                GXColor color;
                CVector3 position;
                CVector3 lightPos;
                color.r = (u8)red;
                lightPos = lighting->m_lightPos[i];
                color.g = (u8)lighting->m_lightColor[i][1];
                color.b = (u8)lighting->m_lightColor[i][2];
                color.a = (u8)lighting->m_alpha;
                CVector3 tmp = lightPos;
                position = tmp;
                GXInitLightColor(&lights[i], color);
                GXInitLightPos(&lights[i], position.x, position.y, position.z);
                GXLightID id = (GXLightID)(1 << i);
                GXLoadLightObjImm(&lights[i], id);
                mask |= id;
            }
        }
        GXColor ambient;
        ambient.r = (u8)lighting->m_ambient[0];
        ambient.g = (u8)lighting->m_ambient[1];
        ambient.b = (u8)lighting->m_ambient[2];
        ambient.a = (u8)lighting->m_alpha;
        GXSetChanAmbColor(GX_COLOR0A0, ambient);
        GXColor material = {255, 255, 255, 0};
        material.a = (u8)lighting->m_alpha;
        GXSetChanMatColor(GX_COLOR0A0, material);
        if (mask) {
            GXSetChanCtrl(GX_COLOR0A0, GX_TRUE, GX_SRC_REG, GX_SRC_REG, mask, GX_DF_CLAMP, GX_AF_NONE);
        } else {
            GXSetChanMatColor(GX_COLOR0A0, ambient);
            GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
        }
        if (this->m_texture)
            this->m_texture->Load(packet, 0);
        PPCSync();
        u32 p;
        for (p = 0; p < this->m_count; p++) {
            CPartView* part = &this->m_parts[p];
            slot = 0;
            int j;
            for (j = 0; j < part->m_numMatrices; j++) {
                u8 index = part->m_matrices[j];
                GXLoadPosMtxIndx(index, slot);
                PSMTXInvXpose(matrices[index], normal);
                GXLoadNrmMtxImm(normal, slot);
                slot += 3;
            }
            void* list;
            u32 size;
            if (skinned) {
                list = mesh->m_skin;
                size = mesh->m_skinSize;
            } else {
                list = part->m_list;
                size = part->m_size << 5;
            }
            GXCallDisplayList(list, size);
        }
        tag = tag->m_next;
        GXSetProjectionv(projection);
    }
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, mask, GX_DF_NONE, GX_AF_NONE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
    Mtx identity;
    PSMTXIdentity(identity);
    int k;
    for (k = 0; k < 30; k += 3)
        GXLoadNrmMtxImm(identity, k);
}

bool CDMRenderBin::IsUsed() {
    return m_used;
}

void CDMRenderBin::Link(CDmaTag*, CDmaPacket&) {
    m_used = false;
    if (m_priority == 8)
        _finish_draw_weapon = true;
    if (m_priority != 8)
        _finish_draw = true;
    if (m_priority == 8) {
        GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        GXSetColorUpdate(GX_FALSE);
        GXColor clear = {0, 0, 0, 0};
        GXSetCopyClear(clear, 0xFFFFFF);
        GXCopyDisp(g_screen.m_frameBuffer, GX_TRUE);
        GXSetColorUpdate(GX_TRUE);
    }
}

int CPart::Render(CDmaPacket&, void*) {
    return 0;
}

void CRemapBin::Link(CDmaTag* tag, CDmaPacket& packet) {
    CRenderBin saved;
    m_bin->m_remapData = m_remapData;
    CRenderBin& bin = *m_bin;
    saved = bin;
    bin = *this;
    m_bin->Link(tag, packet);
    *(CRenderBin*)this = bin;
    bin = saved;
}

int CRemapBin::Render(CDmaPacket& packet, void* data) {
    m_bin->Render(packet, data);
    return 0;
}

void CPartBin::Init() {
    data0c = 0;
    data10 = 0;
    _remap_next = 0;
    m_children = 0;
}
