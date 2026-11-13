#include <math.h>

class CLight;

namespace dwi {
template <class T>
class fast_vec {
public:
    fast_vec(int);
    void push_back(T);
    T* begin();
    T* end();
    int size() const;
    void reserve(T*, int);
    ~fast_vec();

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}



#include "game/CVector3.h"


class CLight {
public:
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual void v3c();
    virtual void GetPosition(CVector3&) const;

    unsigned char unknown004[192];
    float m_radius;
};

/* inferred: a frustum plane (normal, distance at +16) */
struct FrustumPlaneView {
    float n[3];
    unsigned char unknown0c[4];
    float d;
    unsigned char unknown14[12];
};
extern FrustumPlaneView* g_pFrustum;

/* inferred: a drawn item linked into its material's list */
struct CPTMaterialView {
    unsigned char unknown00[104];
    void* m_items;
};
struct CPTItem20 {
    unsigned char unknown00[4];
    void* m_next;
    CPTMaterialView* m_material;
    unsigned char unknown0c[8];
};
struct CPTItem44 {
    unsigned char unknown00[4];
    void* m_next;
    CPTMaterialView* m_material;
    unsigned char unknown0c[32];
};

/* inferred: a node's light link (pooled) */
struct CPTLightLink {
    CPTLightLink* m_next;
    CLight* m_light;
    CPTLightLink* m_poolNext;
};

struct LightPoolView {
    CPTLightLink* Alloc() {
        if (!m_free)
            return 0;
        CPTLightLink* link = m_free;
        m_free = link->m_poolNext;
        link->m_poolNext = m_used;
        m_used = link;
        m_count++;
        return link;
    }

    unsigned char unknown000[3076];
    CPTLightLink* m_used;
    CPTLightLink* m_free;
    unsigned char unknownc08[4];
    int m_count;
};

struct SPRStackView {
    unsigned char unknown00[8];
    char* m_top;

    char* Alloc(int size) {
        char* p = m_top;
        m_top += size;
        return p;
    }
};

/* file statics of compartment.cpp (other units) */
extern LightPoolView g_LightPool;
extern SPRStackView g_SPRStack;
extern bool g_bCPTFogDisabled;
extern bool g_bCPTForceClipped;
extern bool g_bCPTDisableLighting;

class CPTNode {
public:
    float m_axes[3][4];
    CVector3 m_center;
    CPTNode* m_children[2];
    CPTItem20* m_items20;
    int m_count20;
    CPTItem44* m_items44;
    int m_count44;
    short m_flags;
    signed char m_visible : 1;
    signed char m_bits : 7;
    unsigned char unknown5b;
    CPTLightLink* m_lights;
    int m_lightCount;
};

typedef dwi::fast_vec<CLight*> LightVec;

class CCompartment {
public:
    void DrawNode(CPTNode&, unsigned long, dwi::fast_vec<CLight*>&);
};

/* inferred inline: whether the light's sphere reaches the node's box */
static inline bool Touches(CPTNode& node, CLight* light) {
    float radius;
    float dx;
    float dz;
    float dy;
    float e1;
    float dist;
    float e0;
    float e2;
    CVector3 p;
    light->GetPosition(p);
    dist = 0.0f;
    p.x -= node.m_center.x;
    p.y -= node.m_center.y;
    p.z -= node.m_center.z;
    dx = (float)fabs(node.m_axes[0][0] * p.x + node.m_axes[0][1] * p.y + node.m_axes[0][2] * p.z);
    dz = (float)fabs(node.m_axes[2][0] * p.x + node.m_axes[2][1] * p.y + node.m_axes[2][2] * p.z);
    dy = (float)fabs(node.m_axes[1][0] * p.x + node.m_axes[1][1] * p.y + node.m_axes[1][2] * p.z);
    e0 = node.m_axes[0][3];
    e1 = node.m_axes[1][3];
    e2 = node.m_axes[2][3];
    radius = light->m_radius;
    if (dx > e0) {
        float t = dx - e0;
        dist += t * t;
    }
    if (dy > e1) {
        float t = dy - e1;
        dist += t * t;
    }
    if (dz > e2) {
        float t = dz - e2;
        dist += t * t;
    }
    return dist <= radius * radius;
}

/* inferred inline: the node's links to the lights touching it (at most three) */
static inline void LinkLights(CPTNode& node, dwi::fast_vec<CLight*>& lights) {
    if (!lights.size()) {
        node.m_lights = 0;
        node.m_lightCount = 0;
        return;
    }
    node.m_lightCount = lights.size();
    if (node.m_lightCount > 3)
        node.m_lightCount = 3;
    CPTLightLink* link = g_LightPool.Alloc();
    node.m_lights = link;
    CLight** it = lights.begin();
    link->m_light = *it++;
    int n = 1;
    while (it != lights.end()) {
        if (n == 3)
            break;
        CPTLightLink* next = g_LightPool.Alloc();
        link->m_next = next;
        link = next;
        n++;
        link->m_light = *it++;
    }
    link->m_next = 0;
}

/* inferred inline: the node's box against the frustum planes still to test
   (1 when outside one of them); the planes it straddles are added to clipped */
static inline bool IsOutside(CPTNode& node, unsigned long planes, unsigned long& clipped) {
    float nz;
    float nx;
    float d;
    float ny;
    float r;
    if (!planes)
        return false;
    {
        float e2;
        float e1;
        float e0;
        e2 = node.m_axes[2][3];
        e1 = node.m_axes[1][3];
        e0 = node.m_axes[0][3];
        int i;
        for (i = 0; i < 10; i++) {
            FrustumPlaneView* plane;
            unsigned long bit = 1 << i;
            if (planes & bit) {
                plane = &g_pFrustum[i];
                nx = plane->n[0];
                ny = plane->n[1];
                nz = plane->n[2];
                CVector3 center = node.m_center;
                r = (float)fabs(node.m_axes[0][0] * nx + node.m_axes[0][1] * ny + node.m_axes[0][2] * nz) * e0 +
                          (float)fabs(node.m_axes[1][0] * nx + node.m_axes[1][1] * ny + node.m_axes[1][2] * nz) * e1 +
                          (float)fabs(node.m_axes[2][0] * nx + node.m_axes[2][1] * ny + node.m_axes[2][2] * nz) * e2;
                d = nx * center.x + ny * center.y + nz * center.z - plane->d;
                if (d > r)
                    return true;
                if (d >= -r)
                    clipped |= bit;
            }
        }
    }
    return false;
}

void CCompartment::DrawNode(CPTNode& node, unsigned long planes, LightVec& parentLights) {
    unsigned long clipped = 0;
    node.m_flags = 0;
    node.m_lights = 0;
    node.m_lightCount = 0;
    bool outside = IsOutside(node, planes, clipped);
    node.m_visible = !outside;
    if (!node.m_visible)
        return;
    LightVec lights = 2;
    if (parentLights.size()) {
        CLight** buffer = (CLight**)g_SPRStack.Alloc(parentLights.size() * 4);
        lights.reserve(buffer, parentLights.size());
        CLight** it;
        for (it = parentLights.begin(); it != parentLights.end(); it++) {
            if (Touches(node, *it))
                lights.push_back(*it);
        }
    }
    if (lights.size())
        node.m_flags |= 2;
    if (clipped & 0x3c1)
        node.m_flags |= 4;
    node.m_flags |= 8;
    if (g_bCPTForceClipped)
        node.m_flags |= 4;
    if (g_bCPTFogDisabled)
        node.m_flags &= ~8;
    if (g_bCPTDisableLighting)
        node.m_flags &= ~2;
    if (node.m_items44) {
        if (!node.m_lights)
            LinkLights(node, lights);
        int i;
        CPTItem44* item = node.m_items44;
        for (i = 0; i < node.m_count44; i++) {
            CPTMaterialView* material = item->m_material;
            item->m_next = material->m_items;
            material->m_items = item;
            item++;
        }
    }
    if (node.m_items20) {
        if (!node.m_lights)
            LinkLights(node, lights);
        int i;
        CPTItem20* item = node.m_items20;
        for (i = 0; i < node.m_count20; i++) {
            CPTMaterialView* material = item->m_material;
            item->m_next = material->m_items;
            material->m_items = item;
            item++;
        }
    }
    if (node.m_children[0])
        DrawNode(*(node.m_children[0]), clipped, lights);
    if (node.m_children[1])
        DrawNode(*(node.m_children[1]), clipped, lights);
    g_SPRStack.m_top -= parentLights.size() * 4;
}
