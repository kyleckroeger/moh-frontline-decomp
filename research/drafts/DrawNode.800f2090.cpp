#include <math.h>



#include "game/CVector3.h"


#include "game/CMatrix.h"


/* inferred: a frustum plane (normal, distance at +16) */
class CPlane {
public:
    float GetDistance(CVector3 p) const { return m_normal.Dot(p) - m_d; }

    CVector3 m_normal;
    float m_d;
    unsigned char unknown14[12];
};

/* inferred: a material (flags at +100, its draw list at +104) */
struct MSHMaterialView {
    unsigned char unknown00[100];
    unsigned int m_flags;
    struct MSHItemView* m_items;
    unsigned char unknown6c[4];
};

/* inferred: a drawn item linked into its material's list */
struct MSHItemView {
    unsigned char unknown00[4];
    MSHMaterialView* m_material;
    unsigned char unknown08[8];
    MSHItemView* m_next;
    unsigned char unknown14[12];
};

/* inferred: the mesh's two material arrays */
struct StaticMesh {
    unsigned char unknown00[8];
    MSHMaterialView* m_materials;
    int m_materialCount;
    MSHMaterialView* m_alphaMaterials;
    int m_alphaMaterialCount;
};

struct MSHNode {
    MSHNode(const MSHNode&);

    MSHItemView* m_items;
    int m_itemCount;
    MSHNode* m_child0;
    MSHNode* m_child1;
    CVector3 vectors[4];
    unsigned int m_flags;
    int word54;
    int word58;
    int word5c;
};

/* inferred: the configuration's switches */
struct ConfigView {
    unsigned char m_noDraw;
    unsigned char unknown01[63];
};

extern CMatrix* g_pModelToWorld;
extern CPlane* g_pFrustum;
extern ConfigView g_config;

class CStaticMesh {
public:
    void DrawNode(MSHNode&, unsigned long);
    static void SetLightVolume(struct BPDLightVolume* volume);

    void* vtable;
    StaticMesh* m_mesh;
    unsigned char m_lighting : 1;
    unsigned char m_fogging : 1;
    unsigned char m_flags : 6;
};

static inline float Abs(float x) { return fabs(x); }
/* inferred inline: the node's world box against the frustum planes still to
   test (true when outside one of them); the planes it straddles are added to
   clipped */
static inline bool IsOutside(MSHNode& world, unsigned long planes, unsigned long& clipped) {
    if (!planes)
        return false;
    clipped = 0;
    CPlane* plane;
    for (int i = 0; i < 10; i++) {
        unsigned long bit = 1 << i;
        if (planes & bit) {
            plane = &g_pFrustum[i];
            const CVector3& n = plane->m_normal;
            double f2 = fabs(world.vectors[2].Dot(n));
            double f1 = fabs(world.vectors[1].Dot(n));
            double f0 = fabs(world.vectors[0].Dot(n));
            float r = (float)f0 + (float)f1 + (float)f2;
            float d = plane->GetDistance(world.vectors[3]);
            if (d > r)
                return true;
            if (d >= -r)
                clipped |= bit;
        }
    }
    return false;
}

void CStaticMesh::DrawNode(MSHNode& node, unsigned long planes) {
    int j;
    int i;
    unsigned long clipped;
    MSHMaterialView* material;
    MSHMaterialView* m2;
    MSHItemView* item;
    MSHNode world = node;
    g_pModelToWorld->TransformVector(world.vectors[0], node.vectors[0]);
    g_pModelToWorld->TransformVector(world.vectors[1], node.vectors[1]);
    g_pModelToWorld->TransformVector(world.vectors[2], node.vectors[2]);
    g_pModelToWorld->TransformPoint(world.vectors[3], node.vectors[3]);
    if (IsOutside(world, planes, clipped))
        return;
    node.m_flags = 0;
    if (clipped & 0x3c1)
        node.m_flags |= 4;
    if (m_fogging)
        node.m_flags |= 8;
    if (m_lighting)
        node.m_flags |= 2;
    if (!m_lighting) {
        material = m_mesh->m_materials;
        for (i = 0; i < m_mesh->m_materialCount; i++) {
            material->m_flags |= 0x10;
            material++;
        }
        material = m_mesh->m_alphaMaterials;
        for (i = 0; i < m_mesh->m_alphaMaterialCount; i++) {
            material->m_flags |= 0x10;
            material++;
        }
    }
    item = node.m_items;
    if (item) {
        if (!g_config.m_noDraw) {
            for (j = 0; j < node.m_itemCount; j++) {
                m2 = item->m_material;
                item->m_next = m2->m_items;
                m2->m_items = item;
                item++;
            }
        }
    } else {
        if (node.m_child0)
            DrawNode(*node.m_child0, clipped);
        if (node.m_child1)
            DrawNode(*node.m_child1, clipped);
    }
}

__declspec(weak) MSHNode::MSHNode(const MSHNode& other)
    : m_items(other.m_items), m_itemCount(other.m_itemCount), m_child0(other.m_child0), m_child1(other.m_child1) {
    vectors[0] = other.vectors[0];
    vectors[1] = other.vectors[1];
    vectors[2] = other.vectors[2];
    vectors[3] = other.vectors[3];
    m_flags = other.m_flags;
    word54 = other.word54;
    word58 = other.word58;
    word5c = other.word5c;
}

struct BPDLightVolume;

struct LightParamView {
    unsigned char unknown00[96];
};

extern "C" void* memset(void*, int, unsigned long);

extern BPDLightVolume* g_pLightVolume;
extern bool g_bCacheLightParam;
extern LightParamView g_LightParam;


void CStaticMesh::SetLightVolume(BPDLightVolume* volume) {
    g_pLightVolume = volume;
    g_bCacheLightParam = false;
    if (volume)
        memset(&g_LightParam, 0, sizeof(g_LightParam));
}
