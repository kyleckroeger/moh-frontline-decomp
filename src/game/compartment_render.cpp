// A fragment of compartment.cpp (0x80073bd4): CCPTMatBin::Link and
// CCPTMatBin::Render. Link walks the render tags: a command word of 1
// (opaque) or 2 (alpha) sets the pass state (world-to-camera position
// matrix, indexed vertex format, one texture stage, fog, blend/alpha/z modes)
// and is followed by a draw node; each draw node loads the lights of its
// owner (colour, eye-space position through the world-to-camera matrix,
// distance attenuation; a normal matrix per light), the ambient and material
// colours and the channel control, the node material's texture or a
// pass-colour stage when the material changes, the vertex descriptors for 8-
// or 16-bit indices and its display list, then resets the lights' normal
// matrices to identity; a zero word ends a tag. Render: each queued node is
// added to the packet, preceded once by the pending render state, and
// unlinked; then a zero word ends the list; returns 0. The file name is this
// project's; the original record is compartment.cpp. CCPTMatBin, CDmaTag,
// CDmaPacket, CLight, ISceneNode, g_pFog, g_GCFogType, g_NearPlane,
// g_FarPlane, g_WorldToCameraGCMatrix, g_CurrMaterial, g_CurrState and the
// function static prevVertFormat are named by the symbols; the record, draw
// node, light list, display list and fog views, the light's members (light
// object +44, colour +176, radius +196), the bin's members and the node link
// are inferred (ISceneNode's virtuals are flattened in the order of
// __vt__10ISceneNode up to GetPosition). The globals are file-local in the
// original and declared extern to link to them. The packet's templated Add
// takes a const reference (the literal 0 is the image's anonymous .sdata
// object, linked to its pool entry). The bin's destructor (defined
// elsewhere) is declared first so no table is emitted here.
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

class CVector3 {
public:
    CVector3() {}
    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

/* Inferred: the fog colour, range and type. */
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
extern float g_WorldToCameraGCMatrix[3][4];

class CCPTMatBin;
extern CCPTMatBin* g_CurrMaterial;

/* Inferred: the packet's write cursor at +8 and its word append. */
class CDmaPacket {
public:
    template <class T> void Add(const T& value) {
        *(T*)m_cur = value;
        m_cur += sizeof(T);
    }

    unsigned char unknown00[8];
    unsigned char* m_cur;
};

/* Inferred: a queued node and its link at +4. */
struct SPTNode {
    unsigned char unknown00[4];
    SPTNode* m_next;
};

extern int g_CurrState;

/* Inferred: a render tag: the next tag and its command words. */
class CDmaTag {
public:
    CDmaTag* m_next;
    unsigned long* m_data;
};

class CMatrix;
class CDrawContext;
class CCollision;

// The scene-node interface, flattened in the order of __vt__10ISceneNode up
// to GetPosition.
class ISceneNode {
public:
    enum EVolumeType {};

    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual const void* GetLocalBoundingVolume(EVolumeType) const;
    virtual const void* GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
};

/* Inferred: the light object, colour and radius of a light. */
class CLight : public ISceneNode {
public:
    unsigned char unknown04[40];
    GXLightObj m_lightObj;
    unsigned char unknown6c[68];
    float m_color[4];
    unsigned char unknownc0[4];
    float m_radius;
};

/* Inferred: a light list node. */
struct LightNode {
    LightNode* m_next;
    CLight* m_light;
};

/* Inferred: the owner of a draw node and its light list at +92. */
struct CPTLightOwner {
    unsigned char unknown00[92];
    LightNode* m_lights;
};

/* Inferred: a display list (format flag, size, list). */
struct CPTDisplayList {
    unsigned short m_flags;
    unsigned short m_size;
    unsigned char unknown04[16];
    void* m_list;
};

/* Inferred: a draw node: owner, material and display list. */
struct CPTDrawNode {
    CPTLightOwner* m_owner;
    unsigned char unknown04[4];
    CCPTMatBin* m_material;
    CPTDisplayList* m_list;
};

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
};

class CCPTMatBin : public CRenderBin {
public:
    virtual ~CCPTMatBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Link(CDmaTag*, CDmaPacket&);

    unsigned char unknown20[8];
    GXTexObj m_texObj;
    GXTlutObj m_tlutObj;
    int m_hasTlut;
    unsigned char unknown58[8];
    void* m_texture;
    unsigned long m_flags;
    SPTNode* m_nodes;
};


inline GXColor MakeColor(const float* c) { GXColor r; r.r = (int)c[0]; r.g = (int)c[1]; r.b = (int)c[2]; r.a = (int)c[3]; return r; }
inline GXColor MakeColor2(float fr, float fg, float fb, float fa) { GXColor r; r.r = (int)fr; r.g = (int)fg; r.b = (int)fb; r.a = (int)fa; return r; }
void CCPTMatBin::Link(CDmaTag* tag, CDmaPacket&) {
    int index = 0;
    CDmaTag* t = tag;
    while (t) {
        unsigned long next = t->m_data[0];
        index++;
        while (next) {
            int command = next;
            if (command == 1 || command == 2) {
                GXLoadPosMtxImm(g_WorldToCameraGCMatrix, GX_PNMTX0);
                GXClearVtxDesc();
                GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
                GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
                GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
                GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
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
                    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
                } else {
                    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
                    GXSetZCompLoc(GX_TRUE);
                    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
                    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
                }
                g_CurrMaterial = 0;
                next = t->m_data[index++];
            }
            CPTDrawNode* node = (CPTDrawNode*)next;
            int i = 0;
            int mask = 0;
            for (LightNode* l = node->m_owner->m_lights; l; l = l->m_next) {
                CLight* light = l->m_light;
                GXColor color;
                color.r = (u8)light->m_color[0];
                color.g = (u8)light->m_color[1];
                color.b = (u8)light->m_color[2];
                color.a = (u8)light->m_color[3];
                GXInitLightColor(&light->m_lightObj, color);
                CVector3 position;
                light->GetPosition(position);
                CVector3 eye;
                PSMTXMultVec(g_WorldToCameraGCMatrix, (Vec*)&position, (Vec*)&eye);
                GXLoadNrmMtxImm(g_WorldToCameraGCMatrix, i * 3);
                GXInitLightPos(&light->m_lightObj, eye.d.v[0], eye.d.v[1], eye.d.v[2]);
                GXInitLightDistAttn(&light->m_lightObj, 0.5f * light->m_radius, 0.4f, GX_DA_STEEP);
                GXLightID id = (GXLightID)(1 << i);
                GXLoadLightObjImm(&light->m_lightObj, id);
                mask |= id;
                i++;
            }
            GXColor ambient = {128, 128, 128, 255};
            GXSetChanAmbColor(GX_COLOR0A0, ambient);
            GXColor material = {255, 255, 255, 255};
            GXSetChanMatColor(GX_COLOR0A0, material);
            if (mask) {
                GXSetChanCtrl(GX_COLOR0A0, GX_TRUE, GX_SRC_VTX, GX_SRC_REG, mask, GX_DF_CLAMP, GX_AF_SPOT);
            } else {
                GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
            }
            CPTDisplayList* list = node->m_list;
            CCPTMatBin* mat = node->m_material;
            if (list) {
                if (mat != g_CurrMaterial) {
                    g_CurrMaterial = mat;
                    if (mat->m_texture) {
                        GXSetNumTexGens(1);
                        GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
                        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
                        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
                        GXSetCullMode((GXCullMode)((mat->m_flags & 1) ^ 1));
                        if (mat->m_hasTlut)
                            GXLoadTlut(&mat->m_tlutObj, GX_TLUT0);
                        GXLoadTexObj(&mat->m_texObj, GX_TEXMAP0);
                    } else {
                        GXSetNumTexGens(0);
                        GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
                        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
                        GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_DIVIDE_2, GX_TRUE, GX_TEVPREV);
                    }
                }
                static int prevVertFormat = 0;
                int format = list->m_flags & 1;
                if (format != prevVertFormat) {
                    if (format != 1) {
                        GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
                        GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
                        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
                        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
                    } else {
                        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
                        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
                        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
                        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
                    }
                    prevVertFormat = format;
                }
                GXCallDisplayList(list->m_list, list->m_size);
            }
            Mtx identity;
            PSMTXIdentity(identity);
            for (int j = 0; j < i * 3; j += 3)
                GXLoadNrmMtxImm(identity, j);
            next = t->m_data[index++];
        }
        t = t->m_next;
        index = 0;
    }
}

int CCPTMatBin::Render(CDmaPacket& packet, void*) {
    while (m_nodes) {
        if (g_CurrState) {
            packet.Add(g_CurrState);
            g_CurrState = 0;
        }
        packet.Add(m_nodes);
        SPTNode* node = m_nodes;
        m_nodes = node->m_next;
        node->m_next = 0;
    }
    packet.Add(0);
    return 0;
}
