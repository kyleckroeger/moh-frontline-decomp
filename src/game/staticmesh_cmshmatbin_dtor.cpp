// A fragment of staticmesh.cpp (0x800f2ce4): UpdateLightingParams and the
// CMSHMatBin destructor. UpdateLightingParams writes the static-mesh
// lighting block into the DMA packet: when a light volume is current and
// lighting is allowed it rebuilds the cached block once (directional lights
// turned into model space by the inverse model matrix, colours scaled by 255
// and the intensity, the ambient light doubled, then the strongest of the
// context's dynamic lights in range - weight 1 - d^2/r^2 - in the next
// directional slot), then copies the 96-byte block; otherwise it writes
// zeros. The destructor stores its virtual table pointer, which follows 28
// bytes of members, then each base's through the inline destructors of
// CRenderBin and MSHMaterial, and frees the object when asked. The file name
// is this project's; the original record is staticmesh.cpp. The classes and
// functions are named by the mangled symbols; the lighting block, light
// record, light volume and draw-context views and the squared-length helper
// are inferred (g_pLightVolume, g_bAllowLighting, g_pModelToWorld and
// g_pContext are file statics of the original, declared extern here). Only
// the virtuals the destructor needs are declared. CMSHMatBin declares an
// override defined elsewhere before its destructor, so its own (global)
// virtual table is not emitted here; the bases' tables are weak in the
// original (no key function), so the compiler's copies of them and of the
// inline base destructors are weak duplicates, linked to the original copies.
class CDmaTag;

#include <math.h>

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3(const CVector3& other) : d(other.d) {}
    float LengthSquared() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;
    void FastInverse(const CMatrix&);
    void TransformVector(CVector3&, CVector3) const;

    float m[3][4];
    CVector3 pos;
} __attribute__((aligned(16)));

/* inferred: the DMA packet's write pointer (+8) */
class CDmaPacket {
public:
    unsigned char unknown00[8];
    unsigned char* m_cur;
};

/* inferred: the lighting block sent with each mesh (three directions, their
   colours, the ambient colour) */
struct LightParamView {
    float dir[3][3];
    float color[3][3];
    float ambient[3];
    unsigned char unknown54[12];
};

/* inferred: a light of the light volume (48 bytes) */
struct LightView {
    int type;
    unsigned char unknown04[12];
    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
    float intensity;
    unsigned char unknown2c[4];
};

struct BPDLightVolume {
    unsigned char unknown00[32];
    int count;
    LightView* lights;
};

/* inferred: a dynamic light object (position through slot +64) */
class DynamicLightView {
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

    unsigned char unknown04[172];
    float r;
    float g;
    float b;
    unsigned char unknown0bc[8];
    float radius;
};

struct DynamicLightListView {
    DynamicLightView** begin;
    unsigned char unknown04[4];
    int count;
};

/* inferred: the draw context's model matrix (+192) and dynamic lights (+240) */
struct DrawContextView {
    unsigned char unknown000[192];
    CMatrix* m_localToWorld;
    unsigned char unknown0c4[44];
    DynamicLightListView* m_lights;
};

extern BPDLightVolume* g_pLightVolume;
extern bool g_bAllowLighting;
extern bool g_bCacheLightParam;
extern LightParamView g_LightParam;
extern CMatrix* g_pModelToWorld;
extern DrawContextView* g_pContext;

void UpdateLightingParams(CDmaPacket& packet) {
    if (g_pLightVolume && g_bAllowLighting) {
        if (!g_bCacheLightParam) {
            g_bCacheLightParam = true;
            CMatrix inverse;
            inverse.FastInverse(*g_pModelToWorld);
            float* dir;
            int count;
            LightView* light;
            int i;
            LightParamView* param;
            light = g_pLightVolume->lights;
            param = &g_LightParam;
            count = 0;
            dir = g_LightParam.dir[0];
            for (i = 0; i < g_pLightVolume->count; i++, light++) {
                switch (light->type) {
                case 0:
                    break;
                case 1:
                    param->ambient[0] = 255.0f * (2.0f * light->r) * light->intensity;
                    param->ambient[1] = 255.0f * (2.0f * light->g) * light->intensity;
                    param->ambient[2] = 255.0f * (2.0f * light->b) * light->intensity;
                    break;
                case 2: {
                    CVector3 in(light->x, light->y, light->z);
                    CVector3 out;
                    inverse.TransformVector(out, in);
                    count++;
                    dir[0] = out.d.v[0];
                    dir[1] = out.d.v[1];
                    dir[2] = out.d.v[2];
                    dir[9] = 255.0f * light->r * light->intensity;
                    dir[10] = 255.0f * light->g * light->intensity;
                    dir[11] = 255.0f * light->b * light->intensity;
                    dir += 3;
                    break;
                }
                }
            }
            DynamicLightListView* lights = g_pContext->m_lights;
            if (lights && lights->count) {
                DynamicLightView* best = 0;
                float bestValue = 0.0f;
                CVector3 eye(g_pContext->m_localToWorld->pos);
                DynamicLightView** it;
                for (it = lights->begin; it != g_pContext->m_lights->begin + g_pContext->m_lights->count; it++) {
                    CVector3 p;
                    (*it)->GetPosition(p);
                    p.d.v[0] -= eye.d.v[0];
                    p.d.v[1] -= eye.d.v[1];
                    p.d.v[2] -= eye.d.v[2];
                    float distance = p.LengthSquared();
                    float radius = (*it)->radius * (*it)->radius;
                    if (distance < radius) {
                        float value = 1.0f - distance / radius;
                        if (value > bestValue) {
                            bestValue = value;
                            best = *it;
                        }
                    }
                }
                if (best) {
                    if (count >= 3)
                        count = 2;
                    CVector3 d;
                    best->GetPosition(d);
                    d.d.v[0] -= eye.d.v[0];
                    d.d.v[1] -= eye.d.v[1];
                    d.d.v[2] -= eye.d.v[2];
                    float length = sqrtf(d.d.v[0] * d.d.v[0] + d.d.v[1] * d.d.v[1] + d.d.v[2] * d.d.v[2]);
                    if (length != 0.0f) {
                        float scale = 1.0f / length;
                        d.d.v[0] *= scale;
                        d.d.v[1] *= scale;
                        d.d.v[2] *= scale;
                    }
                    float r = best->r;
                    float g = best->g;
                    float b = best->b;
                    g_LightParam.dir[count][0] = d.d.v[0];
                    g_LightParam.dir[count][1] = d.d.v[1];
                    g_LightParam.dir[count][2] = d.d.v[2];
                    g_LightParam.color[count][0] = bestValue * r;
                    g_LightParam.color[count][1] = bestValue * g;
                    g_LightParam.color[count][2] = bestValue * b;
                }
            }
        }
        long* dst = (long*)packet.m_cur;
        long* src = (long*)&g_LightParam;
        int i = 24;
        while (i--)
            *dst++ = *src++;
        packet.m_cur += sizeof(LightParamView);
    } else {
        long* p = (long*)packet.m_cur;
        int i = 24;
        while (i--)
            *p++ = 0;
        packet.m_cur += sizeof(LightParamView);
    }
}

class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin() {}
};

class MSHMaterial : public CRenderBin {
public:
    virtual ~MSHMaterial() {}
};

class CMSHMatBin : public MSHMaterial {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CMSHMatBin();
};

__declspec(weak) CMSHMatBin::~CMSHMatBin() {
}
