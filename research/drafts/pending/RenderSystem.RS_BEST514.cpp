// PENDING: not landable until the maintainer answers State question 10 (vector locals kept in memory):
// the member `float& operator[](int)` pin plus `float* operator[](int)` on an inferred 3x3 view class
// (CMatrix33).
// Function: CParticleSystem::RenderSystem, whole-file particlesystem.cpp fragment, structural 514.
// Source: scratch/loop3/vec3/RS_BEST514.cpp (copied 2026-11-12 by loop agent land2 so the work is
// pushed).
// Not in research/drafts/index.tsv; the index draft for the function stays the landable form.
// See research/plans/three-day-loop.md, State, "Waiting on the maintainer".
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

extern "C" double sin(double);
extern "C" double cos(double);

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3(const CVector3& o) { *(double*)&x = *(const double*)&o.x; *(double*)&z = *(const double*)&o.z; }
    float& operator[](int i) { return ((float*)this)[i]; }
    void Scale(const CVector3& o, float k) { x = k * o.x; y = k * o.y; z = k * o.z; }
    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; }
    CVector3& operator+=(const CVector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    float x, y, z, w;
} __attribute__((aligned(8)));

struct CMatrix33 {
    float* operator[](int i) { return ((float*)this) + 3 * i; }
    float e[3][3];
};

class CColor {
public:
    CColor() : r(0), g(0), b(0), a(0x80) {}
    unsigned char r, g, b, a;
};

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix(const CMatrix& other) { *this = other; }
    static void InitClass();
    void Multiply(const CMatrix&, const CMatrix&);
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;
    CMatrix& operator=(const CMatrix&);
    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CDmaTag;
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
struct CRenderBinData {
    CRenderBin* data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order. */
class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

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
    virtual bool HasRotation() const;
    virtual void GetTexture() const;
    virtual int GetRenderType() const;
    virtual int GetSeed() const;
    virtual int GetEmmisionRate() const;
    virtual int GetEmmisionDelay() const;
    virtual void GetSystemLifetime() const;
    virtual float GetParticleLifetime() const;
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
    virtual bool RenderSystem(CDmaPacket&);

    unsigned char unknown20[84];
    ShapeFile* m_shape;
    float m_startTime;
    float m_maxTime;
    float m_time;
    float m_endTime;
    unsigned char m_active : 1;
    unsigned char m_emitting : 1;
    unsigned char m_flags : 6;
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

/* the file's .bss block, in original order (size views: no constructors) */
struct MatrixView {
    float m[4][4];
} __attribute__((aligned(16)));
struct VectorView {
    float x, y, z, w;
} __attribute__((aligned(8)));
struct DestructorChainView {
    int data[3];
};
class CPSManager {
public:
    float GetCurrentTicks() const { return m_currentTicks; }
    unsigned char unknown00[68];
    float m_currentTicks;
    unsigned char unknown48[0x7c - 0x48];
};
static MatrixView g_ModelToScreenMatrix;
static VectorView g_XAxis;
static VectorView g_YAxis;
static VectorView g_ZAxis;
static MatrixView g_WorldToCamera;
static DestructorChainView g_chain503;
static CPSManager g_ParticleSystemManager;

struct FogView {
    unsigned char unknown00[0x48];
    float m_end;
    int m_type;
};
extern FogView* g_pFog;
extern bool g_bAllowRotation;
extern bool g_bInMultiplayerMode;
extern int g_CurrentSeed;
class CShellMenu {
public:
    unsigned int Get_currentMission();
    unsigned char unknown[0x19dc];
};
extern CShellMenu g_Shell;

static inline int Random() {
    g_CurrentSeed = g_CurrentSeed * 1103515245 + 12345;
    return g_CurrentSeed & 0x7fff;
}
static inline float RandomFloat() { return (float)Random() * (1.0f / 32767.0f); }

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
    if (data00 && m_shape == ((CPropertyParticleSystem*)data00)->m_shape)
        load = false;
    if (load)
        m_shape->m_texture.Load(packet, 0);
    RenderSystem(packet);
}

int CPropertyParticleSystem::Render(CDmaPacket&, void*) {
    return 0;
}

static inline void RotateByRows(CVector3& r, const CMatrix& t, CVector3 v) {
    r.x = v.x * t.m[0][0] + v.y * t.m[0][1] + v.z * t.m[0][2];
    r.y = v.x * t.m[1][0] + v.y * t.m[1][1] + v.z * t.m[1][2];
    r.z = v.x * t.m[2][0] + v.y * t.m[2][1] + v.z * t.m[2][2];
}

bool CParticleSystem::RenderSystem(CDmaPacket&) {
    CVector3 sizeLow, sizeHigh, sizeRange;
    CVector3 spawnLow, spawnHigh, spawnRange;
    CVector3 velLow, velHigh, velRange;
    CVector3 pull, accel, drift;
    CVector3 birth, deathBase, birthRange, deathRange;
    CVector3 right, up;
    CVector3 offset, force, spawn, vel, color, size, centre, a, b;
    float growth;
    bool rotate = HasRotation(); rotate &= g_bAllowRotation;
    int renderType = GetRenderType();
    if (renderType != g_RenderTypeCache) {
        g_RenderTypeCache = renderType;
        switch (renderType) {
        default:
            GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)5, (GXLogicOp)5);
            break;
        case 0:
            GXSetBlendMode((GXBlendMode)1, (GXBlendFactor)4, (GXBlendFactor)1, (GXLogicOp)5);
            break;
        case 1:
            GXSetBlendMode((GXBlendMode)3, (GXBlendFactor)1, (GXBlendFactor)1, (GXLogicOp)5);
            break;
        }
    }
    GXSetCullMode(GX_CULL_NONE);
    CMatrix toWorld;

    GetLocalToWorld(toWorld);
    float now = g_ParticleSystemManager.GetCurrentTicks();
    int delay = GetEmmisionDelay();
    int count = GetEmmisionRate();
    float lifetime = GetParticleLifetime();
    float life = 60.0f * lifetime;
    float endTime = m_endTime;
    if (!m_emitting && m_endTime > m_maxTime)
        endTime = m_maxTime;
    if (g_Shell.Get_currentMission() == 6 && !g_bInMultiplayerMode)
        count >>= 1;
    CColor c0, c1, c2, c3;

    int seed = GetSeed();
    GetParticleSize(sizeLow, sizeHigh, growth);
    GetParticlePosition(spawnLow, spawnHigh);
    GetParticleVelocity(velLow, velHigh);
    GetSystemInitialVelocity(pull);
    GetSystemAcceleration(accel);
    GetParticleAcceleration(drift);
    GetParticleColor(c0, c1, c2, c3);
    float rotLow, rotHigh;
    GetParticleRotation(rotLow, rotHigh);
    float a0, a1, a2;
    GetParticleAlpha(a0, a1, a2);

    if (g_pFog->m_type) {
        float fadeEnd = 2.0f * g_pFog->m_end;
        float fadeStart = (9.0f * fadeEnd) / 10.0f;
        fadeStart *= fadeStart;
        fadeEnd *= fadeEnd;
        const float (*w)[4] = g_WorldToCamera.m;
        const float (*t)[4] = toWorld.m;
        float z = (w[3][2] + (t[3][0] * w[0][2] + t[3][1] * w[1][2] + t[3][2] * w[2][2])) * -1.0f;
        float x = (w[3][0] + (t[3][0] * w[0][0] + t[3][1] * w[1][0] + t[3][2] * w[2][0])) * -1.0f;
        float distance = x * x + z * z;
        if (distance > fadeStart) {
            float fade;
            if (distance < fadeEnd)
                fade = 1.0f - (distance - fadeStart) / (fadeEnd - fadeStart);
            else
                fade = 0.0f;
            if (fade == 0.0f)
                return true;
            a0 *= fade;
            a1 *= fade;
            a2 *= fade;
        }
    }

    float spin = 3.1415927f * (2.0f * lifetime);
    rotLow *= spin;
    rotHigh *= spin;
    sizeLow.y *= 0.7f;
    float rotRange = rotHigh - rotLow;
    sizeHigh.y *= 0.7f;
    sizeRange.Sub(sizeHigh, sizeLow);
    spawnRange.Sub(spawnHigh, spawnLow);
    velRange.Sub(velHigh, velLow);
    birth[3] = a0;
    birth[0] = c0.r;
    birth[1] = c0.g;
    birth[2] = c0.b;
    birthRange.Sub(CVector3(c1.r, c1.g, c1.b), birth);
    deathBase[0] = c2.r;
    growth -= 1.0f;
    birthRange[3] = a1 - a0;
    deathBase[1] = c2.g;
    deathBase[2] = c2.b;
    deathBase[3] = a2;
    float emitStep = (float)delay / 60.0f;
    deathRange.Sub(CVector3(c3.r, c3.g, c3.b), deathBase);
    deathRange[3] = a2 - a2;
    float elapsed = now - m_time;
    float age = elapsed / life;
    float seconds = elapsed / 60.0f;
    float emitTime = (m_time - m_startTime) / 60.0f;
    float secondsStep = -(float)delay / 60.0f;
    float ageStep = -(float)delay / life;
    CMatrix toCamera(*(CMatrix*)&g_WorldToCamera);
    toCamera.Multiply(toWorld, toCamera);
    float gc[3][4];
    toCamera.GetGAMECUBEMatrix34(gc);
    GXLoadPosMtxImm(gc, 3);
    GXSetCurrentMtx(3);

    RotateByRows(right, toWorld, *(CVector3*)&g_XAxis);
    RotateByRows(up, toWorld, *(CVector3*)&g_YAxis);
    CVector3 axis;
    RotateByRows(axis, toWorld, *(CVector3*)&g_ZAxis);

    g_CurrentSeed = seed;
    for (int gen = (int)m_time; (float)gen <= endTime; gen += delay) {
        float emit2 = emitTime * emitTime;
        float seconds2 = seconds * seconds;
        offset.Scale(pull, emitTime);
        CVector3 acc;
        acc.Scale(accel, emit2);
        force.Scale(drift, seconds2);
        offset += acc;
        float scale = 0.5f * (growth * age + 1.0f);
        GXBegin(GX_QUADS, GX_VTXFMT0, count * 4);
        float youth = 1.0f - age;
        for (int i = 0; i < count; i++) {
            spawn.x = spawnLow.x + spawnRange.x * RandomFloat();
            spawn.y = spawnLow.y + spawnRange[1] * RandomFloat();
            spawn.z = spawnLow.z + spawnRange.z * RandomFloat();
            vel[0] = velLow.x + velRange[0] * RandomFloat();
            vel[1] = velLow[1] + velRange[1] * RandomFloat();
            vel[2] = velLow.z + velRange.z * RandomFloat();
            float mixA = RandomFloat();
            centre[0] = offset[0] + (spawn[0] + (vel.x * seconds + force.x));
            centre[1] = offset[1] + (spawn[1] + (vel.y * seconds + force.y));
            float mixB = RandomFloat();
            color[1] = youth * (birth.y + birthRange.y * mixA) + age * (deathBase[1] + deathRange[1] * mixB);
            color[2] = youth * (birth.z + birthRange[2] * mixA) + age * (deathBase[2] + deathRange.z * mixB);
            color[3] = youth * (birth.w + birthRange[3] * mixA) + age * (deathBase[3] + deathRange[3] * mixB);
            color[0] = youth * (birth[0] + birthRange.x * mixA) + age * (deathBase[0] + deathRange[0] * mixB);
            size.x = sizeLow.x + sizeRange[0] * RandomFloat();
            size.y = sizeLow.y + sizeRange.y * RandomFloat();
            centre[2] = offset.z + (spawn.z + (vel[2] * seconds + force[2]));
            float angle = age * (rotLow + rotRange * RandomFloat());
            if (!rotate) {
                float w = scale * size[0];
                a[0] = right[0] * w;
                a[1] = right[1] * w;
                a[2] = right[2] * w;
                float h = scale * size[1];
                b[0] = up[0] * h;
                b[1] = up[1] * h;
                b[2] = up[2] * h;
            } else {
                float s = sin(angle);
                float c = cos(angle);
                float k = 1.0f - c;
                CMatrix33 m;
                m[0][0] = k * axis.x * axis.x + c;
                m[0][1] = k * axis.x * axis.y - s * axis.z;
                m[0][2] = s * axis.y + k * axis.x * axis.z;
                m[1][0] = s * axis.z + k * axis.x * axis.y;
                m[1][1] = axis.y * (k * axis.y) + c;
                m[1][2] = axis.z * (k * axis.y) - s * axis.x;
                m[2][0] = k * axis.x * axis.z - s * axis.y;
                m[2][1] = s * axis.x + axis.z * (k * axis.y);
                m[2][2] = axis.z * (k * axis.z) + c;
                float w = scale * size[0];
                a[0] = w * (right[0] * m[0][0] + right[1] * m[0][1] + right[2] * m[0][2]);
                a[1] = w * (right[0] * m[1][0] + right[1] * m[1][1] + right[2] * m[1][2]);
                a[2] = w * (right[0] * m[2][0] + right[1] * m[2][1] + right[2] * m[2][2]);
                float h = scale * size[1];
                b[0] = h * (up[0] * m[0][0] + up[1] * m[0][1] + up[2] * m[0][2]);
                b[1] = h * (up[0] * m[1][0] + up[1] * m[1][1] + up[2] * m[1][2]);
                b[2] = h * (up[0] * m[2][0] + up[1] * m[2][1] + up[2] * m[2][2]);
            }
            GXPosition3f32(centre[0] - a[0] + b[0], centre[1] - a[1] + b[1], centre[2] - a[2] + b[2]);
            GXColor4u8(color[0], color.y, color[2], color[3]);
            GXTexCoord2f32(0.0f, 0.0f);
            GXPosition3f32(centre[0] + a[0] + b[0], centre[1] + a[1] + b[1], centre[2] + a[2] + b[2]);
            GXColor4u8(color[0], color[1], color[2], color.w);
            GXTexCoord2f32(1.0f, 0.0f);
            GXPosition3f32(centre[0] + a[0] - b[0], centre[1] + a[1] - b[1], centre[2] + a[2] - b[2]);
            GXColor4u8(color[0], color[1], color[2], color[3]);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3f32(centre[0] - a[0] - b[0], centre[1] - a[1] - b[1], centre[2] - a[2] - b[2]);
            GXColor4u8(color[0], color[1], color[2], color.w);
            GXTexCoord2f32(0.0f, 1.0f);
            g_ParticleCounter++;
        }
        seconds += secondsStep;
        age += ageStep;
        emitTime += emitStep;
    }
    GXSetCurrentMtx(0);
    return true;
}
