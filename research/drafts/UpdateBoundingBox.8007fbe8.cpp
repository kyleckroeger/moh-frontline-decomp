// real2 (2026-11-12): 101 real lines, unchanged: start.Scale, aa.Scale, m_center.Sub/Add (the original's CVector3 members). The Mul inline stays load-bearing (acc.Scale + start.Add: 103); vel as a vector 735.

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

    void Scale(const CVector3& v, float k) {
        d.v[0] = k * v.d.v[0];
        d.v[1] = k * v.d.v[1];
        d.v[2] = k * v.d.v[2];
    }
    void Add(const CVector3& v) { d.v[0] += v.d.v[0]; d.v[1] += v.d.v[1]; d.v[2] += v.d.v[2]; }
    void Sub(const CVector3& a, const CVector3& b) { d.v[0] = a.d.v[0] - b.d.v[0]; d.v[1] = a.d.v[1] - b.d.v[1]; d.v[2] = a.d.v[2] - b.d.v[2]; }
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
static inline void Turn(float v, float a, const float& limit, float& lo, float& hi) {
    float t = -0.5f * v / a;
    if (t > 0.0f && t < limit) {
        float p = v * t + t * (a * t);
        if (p < lo)
            lo = p;
        else if (p > hi)
            hi = p;
    }
}

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
        CVector3* plo = &lo;
        CVector3* phi = &hi;
        if ((float)__fabs(a.d.v[0]) > 1e-05f) {
            Turn(v0.d.v[0], a.d.v[0], life, plo->d.v[0], phi->d.v[0]);
            Turn(v1.d.v[0], a.d.v[0], life, plo->d.v[0], phi->d.v[0]);
        }
        if ((float)__fabs(a.d.v[1]) > 1e-05f) {
            Turn(v0.d.v[1], a.d.v[1], life, plo->d.v[1], phi->d.v[1]);
            Turn(v1.d.v[1], a.d.v[1], life, plo->d.v[1], phi->d.v[1]);
        }
        if ((float)__fabs(a.d.v[2]) > 1e-05f) {
            Turn(v0.d.v[2], a.d.v[2], life, plo->d.v[2], phi->d.v[2]);
            Turn(v1.d.v[2], a.d.v[2], life, plo->d.v[2], phi->d.v[2]);
        }
        m_min.d.v[0] = Min(m_min.d.v[0], lo.d.v[0]);
        m_min.d.v[1] = Min(m_min.d.v[1], lo.d.v[1]);
        m_min.d.v[2] = Min(m_min.d.v[2], lo.d.v[2]);
        m_max.d.v[0] = Max(m_max.d.v[0], hi.d.v[0]);
        m_max.d.v[1] = Max(m_max.d.v[1], hi.d.v[1]);
        m_max.d.v[2] = Max(m_max.d.v[2], hi.d.v[2]);
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
        m_center.Sub(m_max, m_min);
        m_center.d.v[0] *= 0.5f;
        m_center.d.v[1] *= 0.5f;
        m_center.d.v[2] *= 0.5f;
        m_center.Add(m_min);
        ValidateBoundingBox(true);
    }
    if (IsMoving()) {
        CVector3 sv;
        GetSystemInitialVelocity(sv);
        CVector3 sa;
        GetSystemAcceleration(sa);
        float ez;
        float vz;
        float t;
        float k;
        float t2;
        float a2;
        float sx;
        float sy;
        float sz;
        float vy;
        float age;
        float vx;
        float ex;
        float ey;
        float ax;
        float ay;
        float az;
        t = (m_updateTicks - m_startTicks) / 60.0f;
        age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
        k = 2.0f * t;
        t2 = t * t;
        a2 = age * age;
        CVector3 start;
        start.Scale(sv, t);
        sx = Mul(t2, sa.d.v, 0);
        sy = Mul(t2, sa.d.v, 1);
        sz = Mul(t2, sa.d.v, 2);
        start.d.v[0] += sx;
        start.d.v[1] += sy;
        start.d.v[2] += sz;
        vx = k * sa.d.v[0];
        vy = k * sa.d.v[1];
        vz = k * sa.d.v[2];
        vx += sv.d.v[0];
        vy += sv.d.v[1];
        vz += sv.d.v[2];
        ex = age * vx;
        ey = age * vy;
        ez = age * vz;
        ex += start.d.v[0];
        ey += start.d.v[1];
        ez += start.d.v[2];
        CVector3 aa;
        aa.Scale(sa, a2);
        ex += aa.d.v[0];
        ey += aa.d.v[1];
        ez += aa.d.v[2];
        CVector3 lo = start;
        CVector3 hi = start;
        CVector3* plo = &lo;
        CVector3* phi = &hi;
        lo.d.v[0] = Min(lo.d.v[0], ex);
        lo.d.v[1] = Min(lo.d.v[1], ey);
        lo.d.v[2] = Min(lo.d.v[2], ez);
        hi.d.v[0] = Max(hi.d.v[0], ex);
        hi.d.v[1] = Max(hi.d.v[1], ey);
        hi.d.v[2] = Max(hi.d.v[2], ez);
        CVector3 tlo = lo;
        CVector3 thi = hi;
        CVector3* ptlo = &tlo;
        CVector3* pthi = &thi;
        if ((float)__fabs(sa.d.v[0]) > 1e-05f)
            Turn(vx, sa.d.v[0], age, tlo.d.v[0], thi.d.v[0]);
        if ((float)__fabs(sa.d.v[1]) > 1e-05f)
            Turn(vy, sa.d.v[1], age, tlo.d.v[1], thi.d.v[1]);
        if ((float)__fabs(sa.d.v[2]) > 1e-05f)
            Turn(vz, sa.d.v[2], age, tlo.d.v[2], thi.d.v[2]);
        lo.d.v[0] = lo.d.v[0] < tlo.d.v[0] ? lo.d.v[0] : tlo.d.v[0];
        lo.d.v[1] = lo.d.v[1] < tlo.d.v[1] ? lo.d.v[1] : tlo.d.v[1];
        lo.d.v[2] = lo.d.v[2] < tlo.d.v[2] ? lo.d.v[2] : tlo.d.v[2];
        hi.d.v[0] = hi.d.v[0] > thi.d.v[0] ? hi.d.v[0] : thi.d.v[0];
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
