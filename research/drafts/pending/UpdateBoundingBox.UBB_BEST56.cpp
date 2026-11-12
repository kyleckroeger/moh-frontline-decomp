// PENDING: not landable until the maintainer answers State question 10 (vector locals kept in memory):
// uses the member `float& operator[](int)` pin on the vector locals, the alias-free way vec1 found to
// keep 18 vector locals and a matrix in memory.
// Function: UpdateBoundingBox (CParticleSystem), 56 real lines (index draft 101).
// Source: scratch/loop3/vec5/UBB_BEST56.cpp (copied 2026-11-12 by loop agent land2 so the work is
// pushed).
// Not in research/drafts/index.tsv; the index draft for the function stays the landable form.
// See research/plans/three-day-loop.md, State, "Waiting on the maintainer".

class CPSManager {
public:
    float GetCurrentTicks() const { return m_currentTicks; }

    unsigned char unknown00[68];
    float m_currentTicks;
};

extern CPSManager g_ParticleSystemManager;

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }

    float& operator[](int i) { return ((float*)this)[i]; }
    void Scale(const CVector3& v, float k) {
        d.v[0] = k * v.d.v[0];
        d.v[1] = k * v.d.v[1];
        d.v[2] = k * v.d.v[2];
    }
    CVector3& operator+=(const CVector3& v) {
        d.v[0] += v.d.v[0];
        d.v[1] += v.d.v[1];
        d.v[2] += v.d.v[2];
        return *this;
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix;
class CColor;
class CDmaPacket;
class CDmaTag;
class CDrawContext;
struct ShapeFile;

struct CRenderBinData {
    int data00;
    int m_next;
    int m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CParticleSystem : public CRenderBin {
public:
    virtual ~CParticleSystem();
    virtual void DeActivate();
    virtual void Terminate();
    virtual bool IsActive() const;
    virtual bool IsEmitting() const;
    virtual bool IsBoundingBoxValid() const;
    virtual bool IsMoving() const;
    virtual bool HasRotation() const;
    virtual void GetTexture() const;
    virtual void GetRenderType() const;
    virtual void GetSeed() const;
    virtual void GetEmmisionRate() const;
    virtual void GetEmmisionDelay() const;
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
    virtual void RenderSystem(CDmaPacket&);
    virtual void UpdateBoundingBox();
    virtual bool IsVisible(CDrawContext&);
    virtual void ValidateBoundingBox(bool);

    CVector3 m_min;
    CVector3 m_max;
    CVector3 m_localMin;
    CVector3 m_localMax;
    CVector3 m_center;
    unsigned char unknown70[8];
    float m_startTicks;
    float m_deactivationTicks;
    float m_updateTicks;
};

static inline float Min(float a, float b) {
    return a < b ? a : b;
}

static inline float Max(float a, float b) {
    return a > b ? a : b;
}

/* inferred: widens [lo, hi] by the turning point of p(t) = v t + a t^2 inside (0, limit) */
static inline void Turn(float a, float v, const float& limit, float& lo, float& hi) {
    float t = -0.5f * v / a;
    if (t > 0.0f && t < limit) {
        float p = v * t + t * (a * t);
        if (p < lo)
            lo = p;
        else if (p > hi)
            hi = p;
    }
}

static inline float Mul2(float a, float b) { float r = a; r *= b; return r; }
static inline float SubV(float a, float b) { return a - b; }
static inline float Mul(float a, float* v, int i) {
    return a * v[i];
}

void CParticleSystem::UpdateBoundingBox() {
    if (!IsBoundingBoxValid()) {
        m_min = CVector3(3.4028235e38f, 3.4028235e38f, 3.4028235e38f);
        m_max = CVector3(-3.4028235e38f, -3.4028235e38f, -3.4028235e38f);
        float life = GetParticleLifetime();
        CVector3 v0;
        CVector3 v1;
        GetParticleVelocity(v0, v1);
        CVector3 a;
        GetParticleAcceleration(a);
        float t2 = life * life;
        CVector3 at;
        at.Scale(a, t2);
        CVector3 p0;
        p0.Scale(v0, life);
        CVector3 p1;
        p1.Scale(v1, life);
        p0 += at;
        p1 += at;
        m_min.d.v[0] = m_min.d.v[0] < p0.d.v[0] ? m_min.d.v[0] : p0.d.v[0];
        m_min.d.v[1] = m_min.d.v[1] < p0.d.v[1] ? m_min.d.v[1] : p0.d.v[1];
        m_min.d.v[2] = m_min.d.v[2] < p0.d.v[2] ? m_min.d.v[2] : p0.d.v[2];
        m_max.d.v[0] = m_max.d.v[0] > p1.d.v[0] ? m_max.d.v[0] : p1.d.v[0];
        m_max.d.v[1] = m_max.d.v[1] > p1.d.v[1] ? m_max.d.v[1] : p1.d.v[1];
        m_max.d.v[2] = m_max.d.v[2] > p1.d.v[2] ? m_max.d.v[2] : p1.d.v[2];
        CVector3 lo(0.0f, 0.0f, 0.0f);
        CVector3 hi(0.0f, 0.0f, 0.0f);
        if ((float)__fabs(a.d.v[0]) > 1e-05f) {
            Turn(a.d.v[0], v0.d.v[0], life, lo[0], hi[0]);
            Turn(a.d.v[0], v1.d.v[0], life, lo[0], hi[0]);
        }
        if ((float)__fabs(a.d.v[1]) > 1e-05f) {
            Turn(a.d.v[1], v0.d.v[1], life, lo[1], hi[1]);
            Turn(a.d.v[1], v1.d.v[1], life, lo[1], hi[1]);
        }
        if ((float)__fabs(a.d.v[2]) > 1e-05f) {
            Turn(a.d.v[2], v0.d.v[2], life, lo[2], hi[2]);
            Turn(a.d.v[2], v1.d.v[2], life, lo[2], hi[2]);
        }
        m_min.d.v[0] = m_min.d.v[0] < lo.d.v[0] ? m_min.d.v[0] : lo.d.v[0];
        m_min.d.v[1] = m_min.d.v[1] < lo.d.v[1] ? m_min.d.v[1] : lo.d.v[1];
        m_min.d.v[2] = m_min.d.v[2] < lo.d.v[2] ? m_min.d.v[2] : lo.d.v[2];
        m_max.d.v[0] = m_max.d.v[0] > hi.d.v[0] ? m_max.d.v[0] : hi.d.v[0];
        m_max.d.v[1] = m_max.d.v[1] > hi.d.v[1] ? m_max.d.v[1] : hi.d.v[1];
        m_max.d.v[2] = m_max.d.v[2] > hi.d.v[2] ? m_max.d.v[2] : hi.d.v[2];
        CVector3 q0;
        CVector3 q1;
        GetParticlePosition(q0, q1);
        m_min.d.v[0] += q0.d.v[0];
        m_min.d.v[1] += q0.d.v[1];
        m_min.d.v[2] += q0.d.v[2];
        m_max.d.v[0] += q1.d.v[0];
        m_max.d.v[1] += q1.d.v[1];
        m_max.d.v[2] += q1.d.v[2];
        CVector3 s0;
        CVector3 s1;
        float scale;
        GetParticleSize(s0, s1, scale);
        float radius;
        if (s1.d.v[0] > s1.d.v[1])
            radius = 0.5f * s1.d.v[0];
        else
            radius = 0.5f * s1.d.v[1];
        if (scale > 1.0f)
            radius *= scale;
        m_min.d.v[0] -= radius;
        m_min.d.v[1] -= radius;
        m_min.d.v[2] -= radius;
        m_max.d.v[0] += radius;
        m_max.d.v[1] += radius;
        m_max.d.v[2] += radius;
        m_localMin = m_min;
        m_localMax = m_max;
        m_center.d.v[0] = m_max.d.v[0] - m_min.d.v[0];
        m_center.d.v[1] = m_max.d.v[1] - m_min.d.v[1];
        m_center.d.v[2] = m_max.d.v[2] - m_min.d.v[2];
        m_center.d.v[0] *= 0.5f;
        m_center.d.v[1] *= 0.5f;
        m_center.d.v[2] *= 0.5f;
        m_center.d.v[0] += m_min.d.v[0];
        m_center.d.v[1] += m_min.d.v[1];
        m_center.d.v[2] += m_min.d.v[2];
        ValidateBoundingBox(true);
    }
    if (IsMoving()) {
        float t;
        float saz;
        float age;
        float sy;
        float ez;
        float t2;
        float svz;
        float ay;
        float sz;
        float sx;
        float vx;
        float k;
        float a2;
        float az;
        float ey;
        CVector3 sv;
        float ax;
        float ex;
        float vy;
        float vz;
        GetSystemInitialVelocity(sv);
        CVector3 sa;
        GetSystemAcceleration(sa);
        svz = sv.d.v[2];
        saz = sa.d.v[2];
        t = (m_updateTicks - m_startTicks) / 60.0f;
        age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
        t2 = t * t;
        a2 = age * age;
        k = 2.0f * t;
        CVector3 start;
        sx = Mul(t2, sa.d.v, 0);
        start.d.v[0] = t * sv.d.v[0];
        start.d.v[2] = svz * t;
        vx = sa.d.v[0] * k;
        sz = Mul(t2, sa.d.v, 2);
        start.d.v[1] = Mul2(t, sv.d.v[1]);
        sy = Mul(t2, sa.d.v, 1);
        start.d.v[0] += sx;
        start.d.v[2] = start.d.v[2] + sz;
        ax = sa.d.v[0] * a2;
        vy = k * sa.d.v[1];
        vz = saz * k;
        vy += sv.d.v[1];
        vz += svz;
        vx += sv.d.v[0];
        ex = vx * age;
        ez = age * vz;
        ey = vy * age;
        start.d.v[1] = start.d.v[1] + sy;
        ex += start.d.v[0];
        ey = ey + start.d.v[1];
        ez += start.d.v[2];
        ay = sa.d.v[1] * a2;
        az = Mul2(a2, saz);
        ex = ax + ex;
        CVector3 lo = start;
        ez += az;
        ey = ay + ey;
        CVector3 hi = start;
        lo.d.v[0] = lo.d.v[0] < ex ? lo.d.v[0] : ex;
        lo.d.v[1] = lo.d.v[1] < ey ? lo.d.v[1] : ey;
        lo.d.v[2] = Min(lo.d.v[2], ez);
        hi.d.v[0] = Max(hi.d.v[0], ex);
        hi.d.v[1] = hi.d.v[1] > ey ? hi.d.v[1] : ey;
        hi.d.v[2] = hi.d.v[2] > ez ? hi.d.v[2] : ez;
        CVector3 tlo = lo;
        CVector3 thi = hi;
        if ((float)__fabs(sa.d.v[0]) > 1e-05f)
            Turn(sa.d.v[0], vx, age, tlo.d.v[0], thi.d.v[0]);
        if ((float)__fabs(sa.d.v[1]) > 1e-05f)
            Turn(sa.d.v[1], vy, age, tlo.d.v[1], thi.d.v[1]);
        if ((float)__fabs(sa.d.v[2]) > 1e-05f)
            Turn(sa.d.v[2], vz, age, tlo.d.v[2], thi.d.v[2]);
        lo.d.v[0] = lo.d.v[0] < tlo.d.v[0] ? lo.d.v[0] : tlo.d.v[0];
        lo.d.v[1] = lo.d.v[1] < tlo.d.v[1] ? lo.d.v[1] : tlo.d.v[1];
        lo.d.v[2] = lo.d.v[2] < tlo.d.v[2] ? lo.d.v[2] : tlo.d.v[2];
        hi.d.v[0] = Max(hi.d.v[0], thi.d.v[0]);
        hi.d.v[1] = hi.d.v[1] > thi.d.v[1] ? hi.d.v[1] : thi.d.v[1];
        hi.d.v[2] = hi.d.v[2] > thi.d.v[2] ? hi.d.v[2] : thi.d.v[2];
        m_min.d.v[0] = lo.d.v[0] + m_localMin.d.v[0];
        m_min.d.v[1] = lo.d.v[1] + m_localMin.d.v[1];
        m_min.d.v[2] = lo.d.v[2] + m_localMin.d.v[2];
        m_max.d.v[0] = hi.d.v[0] + m_localMax.d.v[0];
        m_max.d.v[1] = hi.d.v[1] + m_localMax.d.v[1];
        m_max.d.v[2] = hi.d.v[2] + m_localMax.d.v[2];
        m_center.d.v[0] = m_max.d.v[0] - m_min.d.v[0];
        m_center.d.v[1] = m_max.d.v[1] - m_min.d.v[1];
        m_center.d.v[2] = m_max.d.v[2] - m_min.d.v[2];
        m_center.d.v[0] *= 0.5f;
        m_center.d.v[1] *= 0.5f;
        m_center.d.v[2] *= 0.5f;
        m_center.d.v[0] += m_min.d.v[0];
        m_center.d.v[1] += m_min.d.v[1];
        m_center.d.v[2] += m_min.d.v[2];
    }
}
