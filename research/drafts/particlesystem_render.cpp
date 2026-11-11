// particlesystem.cpp: draws a particle system as camera-facing quads, as partial
// source. Particles aren't stored: each generation replays a seeded random sequence,
// so a particle's start values are the same every frame and its state follows from
// its age. Virtual functions are called by table offset; the names are inferred.
extern "C" {
void GXSetBlendMode(int, int, int, int);
void GXSetCullMode(int);
void GXLoadPosMtxImm(const float (*)[4], int);
void GXSetCurrentMtx(unsigned long);
void GXBegin(int, int, unsigned short);
double sin(double);
double cos(double);
}

union CVector3Data {
    double pair[2];
    float v[4];
};
class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { d = other.d; }
    CVector3Data d;
} __attribute__((aligned(8)));
class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix(const CMatrix& other) { *this = other; }
    static void InitClass();
    static void Multiply(CMatrix&, const CMatrix&, const CMatrix&);
    void GetGAMECUBEMatrix34(float (&)[3][4]) const;
    CMatrix& operator=(const CMatrix&);
    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));
extern CMatrix g_WorldToCamera;
extern CVector3 g_XAxis;
extern CVector3 g_YAxis;
extern CVector3 g_ZAxis;
struct ParticleManagerView {
    unsigned char unknown00[0x44];
    float m_time;
};
extern ParticleManagerView g_ParticleSystemManager;
struct FogView {
    unsigned char unknown00[0x48];
    float m_end;
    int m_type;
};
extern FogView* g_pFog;
extern bool g_bAllowRotation;
extern bool g_bInMultiplayerMode;
extern int g_RenderTypeCache;
extern int g_CurrentSeed;
extern int g_ParticleCounter;
class CShellMenu {
public:
    int Get_currentMission();
    unsigned char unknown[0x19dc]; // g_Shell's size: addressed absolutely, not through r13
};
extern CShellMenu g_Shell;

struct GXFifo {
    union {
        volatile float f;
        volatile unsigned char b;
    };
};
#define GX_FIFO (*(GXFifo*)0xcc008000)

static inline float Random01() {
    g_CurrentSeed = g_CurrentSeed * 0x41c64e6d + 0x3039;
    return (float)(g_CurrentSeed & 0x7fff) * 3.051851e-05f;
}
static inline float Lerp(float low, float range) { return range * Random01() + low; }

struct Color8 {
    unsigned char r, g, b, a;
};
struct Vec3 {
    float x, y, z;
    float pad;
};
class CDmaPacket;
class CParticleSystem {
public:
    bool RenderSystem(CDmaPacket&);
    unsigned char unknown00[0x1c];
    void** m_table;     // +0x1c
    unsigned char unknown20[0x58];
    float m_startTime;  // +0x78
    float m_maxTime;    // +0x7c
    float m_time;       // +0x80
    float m_endTime;    // +0x84
    unsigned char m_flags; // +0x88: bit 6 ignores m_maxTime
};
typedef bool (*GetBoolFn)(CParticleSystem*);
typedef int (*GetIntFn)(CParticleSystem*);
typedef float (*GetFloatFn)(CParticleSystem*);
typedef void (*GetMatrixFn)(CParticleSystem*, CMatrix*);
typedef void (*GetVecFn)(CParticleSystem*, Vec3*);
typedef void (*GetPairFn)(CParticleSystem*, Vec3*, Vec3*);
typedef void (*GetSizeFn)(CParticleSystem*, Vec3*, Vec3*, float*);
typedef void (*GetColorsFn)(CParticleSystem*, Color8*, Color8*, Color8*, Color8*);
typedef void (*GetRotationFn)(CParticleSystem*, float*, float*);
typedef void (*GetAlphaFn)(CParticleSystem*, float*, float*, float*);

bool CParticleSystem::RenderSystem(CDmaPacket&) {
    bool rotate = ((GetBoolFn)m_table[0x34 / 4])(this) & g_bAllowRotation;
    int renderType = ((GetIntFn)m_table[0x3c / 4])(this);
    if (renderType != g_RenderTypeCache) {
        g_RenderTypeCache = renderType;
        if (renderType == 1)
            GXSetBlendMode(3, 1, 1, 5);
        else if (renderType == 0)
            GXSetBlendMode(1, 4, 1, 5);
        else
            GXSetBlendMode(1, 4, 5, 5);
    }
    GXSetCullMode(0);

    CMatrix toWorld;
    ((GetMatrixFn)m_table[0x54 / 4])(this, &toWorld);
    float now = g_ParticleSystemManager.m_time;
    int period = ((GetIntFn)m_table[0x48 / 4])(this);
    int count = ((GetIntFn)m_table[0x44 / 4])(this);
    float rate = ((GetFloatFn)m_table[0x50 / 4])(this);
    float endTime = m_endTime;
    float life = 60.0f * rate;
    if (!((m_flags >> 6) & 1) && m_maxTime < endTime)
        endTime = m_maxTime;
    if (g_Shell.Get_currentMission() == 6 && !g_bInMultiplayerMode)
        count >>= 1;

    Color8 color0;
    color0.r = 0;
    color0.g = 0;
    color0.b = 0;
    color0.a = 0x80;
    Color8 color1;
    color1.r = 0;
    color1.g = 0;
    color1.b = 0;
    color1.a = 0x80;
    Color8 color2;
    color2.r = 0;
    color2.g = 0;
    color2.b = 0;
    color2.a = 0x80;
    Color8 color3;
    color3.r = 0;
    color3.g = 0;
    color3.b = 0;
    color3.a = 0x80;
    int seed = ((GetIntFn)m_table[0x40 / 4])(this);
    Vec3 sizeLow, sizeHigh, spawnLow, spawnHigh, velocityLow, velocityHigh, pull, acceleration, drift;
    float growth, rotationLow, rotationHigh, alphaStart, alphaMid, alphaEnd;
    ((GetSizeFn)m_table[0x64 / 4])(this, &sizeLow, &sizeHigh, &growth);
    ((GetPairFn)m_table[0x68 / 4])(this, &spawnLow, &spawnHigh);
    ((GetPairFn)m_table[0x6c / 4])(this, &velocityLow, &velocityHigh);
    ((GetVecFn)m_table[0x58 / 4])(this, &pull);
    ((GetVecFn)m_table[0x5c / 4])(this, &acceleration);
    ((GetVecFn)m_table[0x70 / 4])(this, &drift);
    ((GetColorsFn)m_table[0x60 / 4])(this, &color0, &color1, &color2, &color3);
    ((GetRotationFn)m_table[0x74 / 4])(this, &rotationLow, &rotationHigh);
    ((GetAlphaFn)m_table[0x78 / 4])(this, &alphaStart, &alphaMid, &alphaEnd);

    // Fade with distance from the camera, near the far end of the fog.
    if (g_pFog->m_type) {
        float fadeEnd = 2.0f * g_pFog->m_end;
        float fadeStart = (9.0f * fadeEnd) / 10.0f;
        fadeStart *= fadeStart;
        const float (*w)[4] = g_WorldToCamera.m;
        const float (*t)[4] = toWorld.m;
        float depth = (w[3][2] + t[3][2] * w[2][2] + t[3][0] * w[0][2] + t[3][1] * w[1][2]) * -1.0f;
        float side = (w[3][0] + t[3][2] * w[2][0] + t[3][0] * w[0][0] + t[3][1] * w[1][0]) * -1.0f;
        float distance = side * side + depth * depth;
        if (fadeStart < distance) {
            float fade = 0.0f;
            if (distance < fadeEnd * fadeEnd)
                fade = 1.0f - (distance - fadeStart) / (fadeEnd * fadeEnd - fadeStart);
            if (fade == 0.0f)
                return true;
            alphaStart *= fade;
            alphaMid *= fade;
            alphaEnd *= fade;
        }
    }

    float spin = 3.1415927f * (2.0f * rate);
    rotationLow *= spin;
    rotationHigh *= spin;
    float rotationRange = rotationHigh - rotationLow;
    sizeHigh.y *= 0.7f;
    sizeLow.y *= 0.7f;
    float sizeRangeX = sizeHigh.x - sizeLow.x;
    float sizeRangeY = sizeHigh.y - sizeLow.y;
    growth -= 1.0f;
    Vec3 spawnRange, velocityRange;
    spawnRange.x = spawnHigh.x - spawnLow.x;
    spawnRange.y = spawnHigh.y - spawnLow.y;
    spawnRange.z = spawnHigh.z - spawnLow.z;
    velocityRange.x = velocityHigh.x - velocityLow.x;
    velocityRange.y = velocityHigh.y - velocityLow.y;
    velocityRange.z = velocityHigh.z - velocityLow.z;

    // Red, green, blue and alpha, each random between two colours or alphas, at
    // birth and at death.
    float birthR = color0.b, birthG = color0.g, birthB = color0.r;
    float birthRange[4], deathBase[4], deathRange[4];
    birthRange[0] = (float)color1.b - birthR;
    birthRange[1] = (float)color1.g - birthG;
    birthRange[2] = (float)color1.r - birthB;
    birthRange[3] = alphaMid - alphaStart;
    deathBase[0] = color2.b;
    deathBase[1] = color2.g;
    deathBase[2] = color2.r;
    deathBase[3] = alphaEnd;
    deathRange[0] = (float)color3.b - deathBase[0];
    deathRange[1] = (float)color3.g - deathBase[1];
    deathRange[2] = (float)color3.r - deathBase[2];
    deathRange[3] = alphaEnd - alphaEnd;

    float elapsed = now - m_time;
    float age = elapsed / life;
    float seconds = elapsed / 60.0f;
    float ageStep = -(float)period / life;
    float secondsStep = -(float)period / 60.0f;
    float emitTime = (m_time - m_startTime) / 60.0f;
    float emitStep = (float)period / 60.0f;

    CMatrix toCamera(g_WorldToCamera);
    CMatrix::Multiply(toCamera, toWorld, toCamera);
    float gcMatrix[3][4];
    toCamera.GetGAMECUBEMatrix34(gcMatrix);
    GXLoadPosMtxImm(gcMatrix, 3);
    GXSetCurrentMtx(3);

    // The camera axes, in the system's space.
    CVector3 xAxis = g_XAxis, yAxis = g_YAxis, zAxis = g_ZAxis;
    const float (*t)[4] = toWorld.m;
    float rightX = xAxis.d.v[2] * t[0][2] + xAxis.d.v[0] * t[0][0] + xAxis.d.v[1] * t[0][1];
    float rightY = xAxis.d.v[2] * t[1][2] + xAxis.d.v[0] * t[1][0] + xAxis.d.v[1] * t[1][1];
    float rightZ = xAxis.d.v[2] * t[2][2] + xAxis.d.v[0] * t[2][0] + xAxis.d.v[1] * t[2][1];
    float upX = yAxis.d.v[2] * t[0][2] + yAxis.d.v[0] * t[0][0] + yAxis.d.v[1] * t[0][1];
    float upY = yAxis.d.v[2] * t[1][2] + yAxis.d.v[0] * t[1][0] + yAxis.d.v[1] * t[1][1];
    float upZ = yAxis.d.v[2] * t[2][2] + yAxis.d.v[0] * t[2][0] + yAxis.d.v[1] * t[2][1];
    float axisX = zAxis.d.v[2] * t[0][2] + zAxis.d.v[0] * t[0][0] + zAxis.d.v[1] * t[0][1];
    float axisY = zAxis.d.v[2] * t[1][2] + zAxis.d.v[0] * t[1][0] + zAxis.d.v[1] * t[1][1];
    float axisZ = zAxis.d.v[2] * t[2][2] + zAxis.d.v[0] * t[2][0] + zAxis.d.v[1] * t[2][1];

    g_CurrentSeed = seed;
    for (int generation = (int)m_time; (float)generation <= endTime; generation += period) {
        float scale = 0.5f * (growth * age + 1.0f);
        float emit2 = emitTime * emitTime;
        float seconds2 = seconds * seconds;
        Vec3 offset, force;
        force.x = seconds2 * drift.x;
        force.y = seconds2 * drift.y;
        force.z = seconds2 * drift.z;
        offset.x = emitTime * pull.x + emit2 * acceleration.x;
        offset.y = emitTime * pull.y + emit2 * acceleration.y;
        offset.z = emitTime * pull.z + emit2 * acceleration.z;
        GXBegin(0x80, 0, (count & 0x3fff) << 2);
        float youth = 1.0f - age;
        for (int i = 0; i < count; i++) {
            float spawnX = Lerp(spawnLow.x, spawnRange.x);
            float spawnY = Lerp(spawnLow.y, spawnRange.y);
            float spawnZ = Lerp(spawnLow.z, spawnRange.z);
            float velocityX = Lerp(velocityLow.x, velocityRange.x);
            float velocityY = Lerp(velocityLow.y, velocityRange.y);
            float velocityZ = Lerp(velocityLow.z, velocityRange.z);
            float mixA = Random01();
            float mixB = Random01();
            float red = youth * (birthRange[0] * mixA + birthR) + age * (deathRange[0] * mixB + deathBase[0]);
            float green = youth * (birthRange[1] * mixA + birthG) + age * (deathRange[1] * mixB + deathBase[1]);
            float blue = youth * (birthRange[2] * mixA + birthB) + age * (deathRange[2] * mixB + deathBase[2]);
            float alpha = youth * (birthRange[3] * mixA + alphaStart) + age * (deathRange[3] * mixB + deathBase[3]);
            float centreX = offset.x + (spawnX + (velocityX * seconds + force.x));
            float centreY = offset.y + (spawnY + (velocityY * seconds + force.y));
            float centreZ = offset.z + (spawnZ + (velocityZ * seconds + force.z));
            float width = Lerp(sizeLow.x, sizeRangeX);
            float height = Lerp(sizeLow.y, sizeRangeY);
            float angle = age * Lerp(rotationLow, rotationRange);

            float rx, ry, rz, ux, uy, uz;
            if (!rotate) {
                float w = scale * width;
                float h = scale * height;
                rx = rightX * w;
                ry = rightY * w;
                rz = rightZ * w;
                ux = upX * h;
                uy = upY * h;
                uz = upZ * h;
            } else {
                // Rotate about the view axis.
                float s = (float)sin(angle);
                float c = (float)cos(angle);
                float k = 1.0f - c;
                float kx = k * axisX;
                float m00 = kx * axisX + c;
                float m01 = kx * axisY - s * axisZ;
                float m02 = kx * axisZ + s * axisY;
                float m10 = kx * axisY + s * axisZ;
                float m11 = axisY * (k * axisY) + c;
                float kyz = axisZ * (k * axisY);
                float m12 = kyz - s * axisX;
                float m20 = kx * axisZ - s * axisY;
                float m21 = s * axisX + kyz;
                float m22 = axisZ * (k * axisZ) + c;
                float w = scale * width;
                rx = w * (rightZ * m02 + rightX * m00 + rightY * m01);
                ry = w * (rightZ * m12 + rightX * m10 + rightY * m11);
                rz = w * (rightZ * m22 + rightX * m20 + rightY * m21);
                float h = scale * height;
                ux = h * (upZ * m02 + upX * m00 + upY * m01);
                uy = h * (upZ * m12 + upX * m10 + upY * m11);
                uz = h * (upZ * m22 + upX * m20 + upY * m21);
            }
            int ir = (int)red, ig = (int)green, ib = (int)blue, ia = (int)alpha;
            float ax = centreX - rx, ay = centreY - ry, az = centreZ - rz;
            float bx = centreX + rx, by = centreY + ry, bz = centreZ + rz;
#define VERTEX(x, y, z, s, t) \
    GX_FIFO.f = (x); GX_FIFO.f = (y); GX_FIFO.f = (z); \
    GX_FIFO.b = ib; GX_FIFO.b = ig; GX_FIFO.b = ir; GX_FIFO.b = ia; \
    GX_FIFO.f = (s); GX_FIFO.f = (t)
            VERTEX(ax + ux, ay + uy, az + uz, 0.0f, 0.0f);
            VERTEX(bx + ux, by + uy, bz + uz, 1.0f, 0.0f);
            VERTEX(bx - ux, by - uy, bz - uz, 1.0f, 1.0f);
            VERTEX(ax - ux, ay - uy, az - uz, 0.0f, 1.0f);
#undef VERTEX
            g_ParticleCounter++;
        }
        age += ageStep;
        seconds += secondsStep;
        emitTime += emitStep;
    }
    GXSetCurrentMtx(0);
    return true;
}
