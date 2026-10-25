// A fragment of animated.cpp (0x80060e0c): CAnimated::Draw. With a mesh it
// updates the render parts, synthesizes the cluster matrices under the
// local-to-camera matrix, sets the render object's view projection, 2D
// reject, local-to-camera matrix, scale and cluster matrices, then builds the
// lighting block from the owner's light volume (the default volume when it
// has none): up to three directional lights and the ambient light, colours
// scaled by 255 and the intensity, plus the strongest of the context's point
// lights in range (weight 1 - d^2/r^2, direction normalised, colour scaled by
// the weight) in the next directional slot, which it remembers in the
// context. It updates the lighting, scales the alpha by +2620/127 when flag
// 0x100 of +2616 is set, and draws the mesh. The file name is this project's;
// the original record is animated.cpp. CAnimated, CLight, CDMesh, the DM
// render functions and g_matStack are named by the symbols; the members, the
// lighting block, light record and list views, the owner's light-volume
// getter (table slot +288) and the squared-length helper are inferred.
// CAnimated is a non-virtual view here. The hoisted float declarations follow
// the original's register use. The rest of the file is in other units.
#include <math.h>
extern "C" void* memset(void*, int, unsigned long);

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix(const CMatrix& other) { *this = other; }
    static void InitClass();
    CMatrix& operator=(const CMatrix&);
    void Multiply(const CMatrix&, const CMatrix&);
    float m[3][4];
    CVector3 m_pos;
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CMatrixStack {
public:
    void Push();
    void Pop();
    void Load(const CMatrix*);
};
extern CMatrixStack* g_matStack;

struct DMClusterObject_T {
    unsigned char unknown00[4];
    float (*m_matrices)[3][4];
    unsigned char unknown08[8];
};
struct DMRenderObject_T {
    unsigned char unknown00[20];
};
struct DMGeometryState_T {
    unsigned char unknown00[588];
};
struct CharOrient3_t;
class CDrawContext;

class CDMesh {
public:
    void Draw(CDrawContext&, DMRenderObject_T*);
};

/* inferred: the lighting block passed to DMLightingUpdate */
struct DMLighting_T {
    float colors[3][4];
    float ambient[4];
    float dirs[3][3];
};

void DMRenderUpdateParts(DMRenderObject_T*);
void DMClusterSynthesizeMatrices(unsigned long, DMClusterObject_T*, CDMesh*, CharOrient3_t*);
void DMRenderResetState(DMRenderObject_T*);
void DMRenderSetViewProj(DMRenderObject_T*, CMatrix*);
void DMRenderSet2dReject(DMRenderObject_T*, unsigned long);
void DMRenderSetLocalToCamera(DMRenderObject_T*, CMatrix*, CMatrix*);
void DMRenderSetScale(DMRenderObject_T*, float, float, float);
void DMRenderSetClusterMatrices(DMRenderObject_T*, float (*)[3][4]);
void DMLightingUpdate(DMRenderObject_T*, DMLighting_T*);
void DMLightingScaleAlpha(DMRenderObject_T*, float);

/* inferred: a light record of a light volume */
struct LightRecordView {
    int type;
    unsigned char unknown04[12];
    float dir[3];
    float color[3];
    float intensity;
    unsigned char unknown2c[4];
};

struct CLightVolume {
    unsigned char unknown00[32];
    int m_count;
    LightRecordView* m_lights;
};

class CLight {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual void unknown34();
    virtual void unknown38();
    virtual void unknown3c();
    virtual void GetPosition(CVector3&) const;
    static CLightVolume* GetDefaultLightVolume();

    unsigned char unknown004[172];
    float m_color[3];
    unsigned char unknown0bc[8];
    float m_radius;
};

struct LightListView {
    CLight** m_data;
    int m_unknown4;
    int m_size;
};

class CDrawContext {
public:
    unsigned char unknown000[192];
    CMatrix* m_localToWorld;
    CMatrix* m_worldToView;
    CMatrix* m_viewToScreen;
    unsigned char unknown0cc[20];
    CVector3 m_scale;
    LightListView* m_lights;
    unsigned char unknown0f4[4];
    CLightVolume* m_lightVolume;
    CLight* m_light;
    unsigned long m_reject;
};

struct AnimObjectView {
    unsigned char unknown00[56];
    CharOrient3_t* m_orient;
};

/* inferred: the owner's light volume getter (table slot +288) */
class LightSourceView {
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
    virtual void v40();
    virtual void v44();
    virtual void v48();
    virtual void v4c();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5c();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6c();
    virtual void v70();
    virtual void v74();
    virtual void v78();
    virtual void v7c();
    virtual void v80();
    virtual void v84();
    virtual void v88();
    virtual void v8c();
    virtual void v90();
    virtual void v94();
    virtual void v98();
    virtual void v9c();
    virtual void va0();
    virtual void va4();
    virtual void va8();
    virtual void vac();
    virtual void vb0();
    virtual void vb4();
    virtual void vb8();
    virtual void vbc();
    virtual void vc0();
    virtual void vc4();
    virtual void vc8();
    virtual void vcc();
    virtual void vd0();
    virtual void vd4();
    virtual void vd8();
    virtual void vdc();
    virtual void ve0();
    virtual void ve4();
    virtual void ve8();
    virtual void vec();
    virtual void vf0();
    virtual void vf4();
    virtual void vf8();
    virtual void vfc();
    virtual void v100();
    virtual void v104();
    virtual void v108();
    virtual void v10c();
    virtual void v110();
    virtual void v114();
    virtual void v118();
    virtual void v11c();
    virtual CLightVolume* GetLightVolume();
};

class CAnimated {
public:
    void Draw(CDrawContext&);

    unsigned char unknown0000[2616];
    unsigned int m_flags;
    unsigned char m_alpha;
    unsigned char unknownA3D[3];
    DMRenderObject_T m_render;
    DMGeometryState_T m_geometry;
    DMClusterObject_T m_cluster;
    CDMesh* m_mesh;
    unsigned char unknownCB4[4];
    LightSourceView* m_owner;
    AnimObjectView* m_object;
};

static inline float LengthSq(const CVector3& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }

void CAnimated::Draw(CDrawContext& context) {
    if (!m_mesh)
        return;
    DMRenderUpdateParts(&m_render);
    CMatrix localToCamera;
    localToCamera.Multiply(*context.m_localToWorld, *context.m_worldToView);
    g_matStack->Push();
    g_matStack->Load(&localToCamera);
    DMClusterSynthesizeMatrices(0, &m_cluster, m_mesh, m_object->m_orient);
    g_matStack->Pop();
    DMRenderResetState(&m_render);
    CMatrix viewProj(*context.m_viewToScreen);
    DMRenderSetViewProj(&m_render, &viewProj);
    DMRenderSet2dReject(&m_render, context.m_reject);
    DMRenderSetLocalToCamera(&m_render, &localToCamera, 0);
    DMRenderSetScale(&m_render, context.m_scale.x, context.m_scale.y, context.m_scale.z);
    DMRenderSetClusterMatrices(&m_render, m_cluster.m_matrices);
    g_matStack->Push();
    g_matStack->Load(context.m_worldToView);
    CLightVolume* volume = m_owner->GetLightVolume();
    if (!volume)
        volume = CLight::GetDefaultLightVolume();
    context.m_lightVolume = volume;
    LightRecordView* light = volume->m_lights;
    DMLighting_T lighting;
    memset(&lighting, 0, sizeof(lighting));
    lighting.colors[0][3] = 255.0f;
    lighting.colors[1][3] = 255.0f;
    lighting.colors[2][3] = 255.0f;
    lighting.ambient[3] = 255.0f;
    int count = 0;
    for (int i = 0; i < volume->m_count; i++, light++) {
        float intensity = light->intensity;
        if (light->type == 2) {
            lighting.dirs[count][0] = light->dir[0];
            lighting.dirs[count][1] = light->dir[1];
            lighting.dirs[count][2] = light->dir[2];
            lighting.colors[count][0] = 255.0f * light->color[0] * intensity;
            lighting.colors[count][1] = 255.0f * light->color[1] * intensity;
            lighting.colors[count][2] = 255.0f * light->color[2] * intensity;
            count++;
        } else {
            lighting.ambient[0] = 255.0f * light->color[0] * intensity;
            lighting.ambient[1] = 255.0f * light->color[1] * intensity;
            lighting.ambient[2] = 255.0f * light->color[2] * intensity;
        }
    }
    LightListView* lights = context.m_lights;
    if (lights && lights->m_size) {
    float radius;
    float py;
    float weight;
    float dist;
    float pz;
    float bestWeight;
        CLight* best = 0;
        bestWeight = 0.0f;
        CVector3 pos = context.m_localToWorld->m_pos;
        py = pos.y;
        pz = pos.z;
        for (CLight** it = lights->m_data; it != context.m_lights->m_data + context.m_lights->m_size; it++) {
            CVector3 d;
            (*it)->GetPosition(d);
            d.x -= pos.x;
            d.y -= py;
            d.z -= pz;
            dist = LengthSq(d);
            radius = (*it)->m_radius * (*it)->m_radius;
            if (dist < radius) {
                weight = 1.0f - dist / radius;
                if (weight > bestWeight) {
                    bestWeight = weight;
                    best = *it;
                }
            }
        }
        if (best) {
            if (count >= 3)
                count = 2;
            CVector3 d;
            best->GetPosition(d);
            d.x -= pos.x;
            d.y -= py;
            d.z -= pz;
            float length = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
            if (length != 0.0f) {
                float scale = 1.0f / length;
                d.x *= scale;
                d.y *= scale;
                d.z *= scale;
            }
            float r = best->m_color[0];
            float g = best->m_color[1];
            float b = best->m_color[2];
            lighting.dirs[count][0] = d.x;
            lighting.dirs[count][1] = d.y;
            lighting.dirs[count][2] = d.z;
            lighting.colors[count][0] = bestWeight * r;
            lighting.colors[count][1] = bestWeight * g;
            lighting.colors[count][2] = bestWeight * b;
        }
        context.m_light = best;
    }
    DMLightingUpdate(&m_render, &lighting);
    if (m_flags & 0x100)
        DMLightingScaleAlpha(&m_render, m_alpha / 127.0f);
    g_matStack->Pop();
    m_mesh->Draw(context, &m_render);
}
