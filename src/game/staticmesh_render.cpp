// A fragment of staticmesh.cpp (0x800f1690): CMSHMatBin::Link and
// CMSHMatBin::Render. Link walks the render tags: per record (two command
// words, a 3x4 position matrix, the lighting parameters and a chunk pointer)
// it sets the pass state for commands 1 (opaque) and 2 (alpha), loads the
// matrix, the three GX lights with a non-zero colour and the ambient colour
// (or plain vertex colours when the material is unlit), the chunk's texture
// when the material changes, the vertex descriptors for 8- or 16-bit indices,
// and calls the display list; a zero command word ends a tag. Render: for each
// queued mesh node: the pending render state once, the world-to-camera
// matrix in GX 3x4 form copied element by element, the lighting parameters
// and the node, which is then unlinked; a zero word ends the list; returns
// 0. The file name is this project's; the original record is staticmesh.cpp.
// CMSHMatBin, CRenderBin, CDmaTag, CDmaPacket, CMatrix, CVector3, g_pFog,
// g_GCFogType, g_NearPlane, g_FarPlane, g_ActualWorldToCameraMatrix,
// g_CurrMaterial, g_CurrState, g_WorldToCameraMatrix, UpdateLightingParams
// and the function statics CurIndexType and light are named by the symbols;
// the record layout, the fog, lighting, chunk and display-list views, the
// bin's members and the node link are inferred (the record words are read
// through '*(T*)&tag->m_data[index + n]' and the light position through two
// vector copies, as the original's stack slots show). The globals are
// file-local in the original and declared extern to link to them. The bin's
// destructor (defined elsewhere) is declared first so no table is emitted
// here.
#include <dolphin/gx.h>

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
    }
    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;

    float m[4][4];
} __attribute__((aligned(16)));

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
extern CMatrix g_ActualWorldToCameraMatrix;

/* Inferred: a queued node and its link at +16. */
struct SMSHNode {
    unsigned char unknown00[16];
    SMSHNode* m_next;
};

extern int g_CurrState;
extern CMatrix g_WorldToCameraMatrix;

class CMSHMatBin;
extern CMSHMatBin* g_CurrMaterial;

/* Inferred: the packet's write cursor at +8 and its templated append. */
class CDmaPacket {
public:
    template <class T> void Add(const T& value) {
        *(T*)m_cur = value;
        m_cur += sizeof(T);
    }

    unsigned char unknown00[8];
    unsigned char* m_cur;
};

void UpdateLightingParams(CDmaPacket&);

/* Inferred: a render tag: the next tag and its command words. */
class CDmaTag {
public:
    CDmaTag* m_next;
    unsigned long* m_data;
};

/* Inferred: a chunk's display list (flags, size at +12, list at +32). */
struct MSHDisplayListView {
    unsigned long m_flags;
    unsigned char unknown04[8];
    unsigned long m_size;
    unsigned char unknown10[16];
    void* m_list;
};

/* Inferred: a drawn chunk: its material and display list. */
struct MSHChunkView {
    unsigned char unknown00[4];
    CMSHMatBin* m_material;
    MSHDisplayListView* m_list;
};

/* Inferred: the lighting part of a record: three light positions, their
   colours and the ambient colour. */
struct MSHLightingView {
    float m_pos[3][3];
    float m_color[3][3];
    float m_ambient[3];

    CVector3 GetLightPos(int i) const { return *(const CVector3*)m_pos[i]; }
};

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
};

class CMSHMatBin : public CRenderBin {
public:
    virtual ~CMSHMatBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);

    unsigned char unknown20[8];
    GXTexObj m_texObj;
    GXTlutObj m_tlutObj;
    int m_hasTlut;
    unsigned char unknown58[8];
    void* m_texture;
    unsigned long m_flags;
    SMSHNode* m_nodes;
};

void CMSHMatBin::Link(CDmaTag* tag, CDmaPacket&) {
    static int CurIndexType = 0;
    static GXLightObj light[3];
    int index = 0;
    CDmaTag* t = tag;
    while (t) {
        unsigned long next = t->m_data[0];
        while (next) {
            int command = next;
            if (command == 1 || command == 2) {
                GXClearVtxDesc();
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 0);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
                GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
                GXSetNumChans(1);
                GXSetNumTexGens(1);
                GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
                GXSetColorUpdate(GX_TRUE);
                GXSetNumTevStages(1);
                GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
                GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
                GXColor fogColor;
                fogColor.a = 255;
                fogColor.r = g_pFog->r;
                fogColor.g = g_pFog->g;
                fogColor.b = g_pFog->b;
                GXSetFog(g_GCFogType[g_pFog->type], g_pFog->start, g_pFog->end, g_NearPlane, g_FarPlane, fogColor);
                if (command == 1) {
                    GXSetBlendMode(GX_BM_BLEND, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);
                    GXSetZCompLoc(GX_FALSE);
                    GXSetAlphaCompare(GX_GREATER, 160, GX_AOP_AND, GX_ALWAYS, 0);
                } else {
                    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
                    GXSetZCompLoc(GX_TRUE);
                    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
                }
                GXSetZMode(GX_TRUE, GX_LEQUAL, command == 1 ? GX_TRUE : GX_FALSE);
                index++;
                g_CurrMaterial = 0;
                CurIndexType = 0;
            }
            GXLoadPosMtxImm((float(*)[4])&t->m_data[index], GX_PNMTX0);
            MSHLightingView* lighting = (MSHLightingView*)&t->m_data[index + 12];
            if (!(m_flags & 0x10)) {
                int mask = 0;
                for (int i = 0; i < 3; i++) {
                    float red = lighting->m_color[i][0];
                    if (red != 0.0f || lighting->m_color[i][1] != 0.0f || lighting->m_color[i][2] != 0.0f) {
                        GXColor color;
                        CVector3 lightPos = lighting->GetLightPos(i);
                        color.r = (int)red;
                        color.g = (int)lighting->m_color[i][1];
                        color.b = (int)lighting->m_color[i][2];
                        color.a = 255;
                        CVector3 position;
                        position = lightPos;
                        GXInitLightColor(&light[i], color);
                        GXInitLightPos(&light[i], position.d.v[0], position.d.v[1], position.d.v[2]);
                        GXLightID id = (GXLightID)(1 << i);
                        GXLoadLightObjImm(&light[i], id);
                        mask |= id;
                    }
                }
                GXColor ambient;
                ambient.a = 255;
                ambient.r = (int)lighting->m_ambient[0];
                ambient.g = (int)lighting->m_ambient[1];
                ambient.b = (int)lighting->m_ambient[2];
                GXSetChanAmbColor(GX_COLOR0A0, ambient);
                GXColor material = {172, 172, 172, 255};
                GXSetChanMatColor(GX_COLOR0A0, material);
                if (mask) {
                    GXSetChanCtrl(GX_COLOR0A0, GX_TRUE, GX_SRC_REG, GX_SRC_REG, mask, GX_DF_CLAMP, GX_AF_NONE);
                } else {
                    GXSetChanMatColor(GX_COLOR0A0, ambient);
                    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
                }
            } else {
                GXColor ambient = {255, 255, 255, 255};
                GXSetChanAmbColor(GX_COLOR0A0, ambient);
                GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
            }
            MSHChunkView* chunk = *(MSHChunkView**)&t->m_data[index + 36];
            CMSHMatBin* mat = chunk->m_material;
            MSHDisplayListView* list = chunk->m_list;
            if (list) {
                if (mat->m_texture && mat != g_CurrMaterial) {
                    g_CurrMaterial = mat;
                    GXSetCullMode((GXCullMode)((mat->m_flags & 1) ^ 1));
                    if (mat->m_hasTlut)
                        GXLoadTlut(&mat->m_tlutObj, GX_TLUT0);
                    GXLoadTexObj(&mat->m_texObj, GX_TEXMAP0);
                }
                if (list->m_flags & 1) {
                    if (CurIndexType != 1) {
                        GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
                        GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
                        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
                        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
                        CurIndexType = 1;
                    }
                } else if (CurIndexType != 2) {
                    GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
                    GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
                    GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
                    GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
                    CurIndexType = 2;
                }
                GXCallDisplayList(list->m_list, list->m_size);
            }
            next = *(unsigned long*)&t->m_data[index + 37];
            index += 38;
        }
        t = t->m_next;
        index = 0;
    }
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    float matrix[3][4];
    g_ActualWorldToCameraMatrix.GetGAMECUBEMatrix34(matrix);
    GXLoadPosMtxImm(matrix, GX_PNMTX0);
}

int CMSHMatBin::Render(CDmaPacket& packet, void*) {
    while (m_nodes) {
        if (g_CurrState) {
            packet.Add(g_CurrState);
            g_CurrState = 0;
        }
        float matrix[3][4];
        g_WorldToCameraMatrix.GetGAMECUBEMatrix34(matrix);
        float (*dst)[4] = (float (*)[4])packet.m_cur;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 4; j++)
                dst[i][j] = matrix[i][j];
        packet.m_cur += sizeof(matrix);
        UpdateLightingParams(packet);
        packet.Add(m_nodes);
        SMSHNode* node = m_nodes;
        m_nodes = node->m_next;
        node->m_next = 0;
    }
    packet.Add(0);
    return 0;
}
