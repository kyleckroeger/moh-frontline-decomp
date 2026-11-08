#include <math.h>

union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }
    float LengthSq() const { return d.c.x * d.c.x + d.c.y * d.c.y + d.c.z * d.c.z; }
    void Cross(const CVector3& a, const CVector3& b) {
        d.c.x = a.d.c.y * b.d.c.z - a.d.c.z * b.d.c.y;
        d.c.y = a.d.c.z * b.d.c.x - a.d.c.x * b.d.c.z;
        d.c.z = a.d.c.x * b.d.c.y - a.d.c.y * b.d.c.x;
    }
    void Unitize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            d.c.x *= 1.0f / length;
            d.c.y *= 1.0f / length;
            d.c.z *= 1.0f / length;
        }
    }

    CVector3Data d;
} __attribute__((aligned(8)));

inline void SetDifference(CVector3& r, const CVector3& a, const CVector3& b) {
    r.d.c.x = a.d.c.x - b.d.c.x;
    r.d.c.y = a.d.c.y - b.d.c.y;
    r.d.c.z = a.d.c.z - b.d.c.z;
}

class CCollision;
extern const float CONTACT_EPSILON;

inline int Compare(float separation) {
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

template <class T> inline const T& Min(const T& a, const T& b) { return a < b ? a : b; }
template <class T> inline const T& Max(const T& a, const T& b) { return a < b ? b : a; }

inline double ifabs(double x) { return __fabs(x); }
enum EBoxTriSeperatingAxis { kAxisNone };

inline bool Separate(float r, float p, EBoxTriSeperatingAxis axis, float& best, EBoxTriSeperatingAxis& bestAxis) {
    float d = (float)fabs(p) - r;
    if (Compare(d) == 1)
        return true;
    if ((float)ifabs(d) < (float)ifabs(best)) {
        best = d;
        bestAxis = axis;
    }
    return false;
}

inline bool Separate(float r, float p0, float p1, EBoxTriSeperatingAxis axis, float& best, EBoxTriSeperatingAxis& bestAxis) {
    float d;
    if (p0 >= 0.0f) {
        if (p1 > 0.0f)
            p1 = 0.0f;
        d = p0 + p1 - r;
    } else {
        if (p1 < 0.0f)
            p1 = 0.0f;
        d = -(p0 + p1) - r;
    }
    if (Compare(d) == 1)
        return true;
    if ((float)ifabs(d) < (float)ifabs(best)) {
        best = d;
        bestAxis = axis;
    }
    return false;
}

inline bool Separate(float r, float p0, float p1, float p2, EBoxTriSeperatingAxis axis, float& best, EBoxTriSeperatingAxis& bestAxis) {
    float d;
    if (p0 >= 0.0f) {
        float m = Min(p1, p2);
        if (m > 0.0f)
            d = p0 - r;
        else
            d = p0 + m - r;
    } else {
        float m = Max(p1, p2);
        if (m < 0.0f)
            d = -p0 - r;
        else
            d = -(p0 + m) - r;
    }
    if (Compare(d) == 1)
        return true;
    if ((float)ifabs(d) < (float)ifabs(best)) {
        best = d;
        bestAxis = axis;
    }
    return false;
}

class CTriangle {
public:
    CVector3 m_vertices[3];
};

class IVolume {
public:
    virtual ~IVolume();
};

class CCDBVolBox : public IVolume {
public:
    int TestCollision(const CTriangle&, CCollision&, bool) const;

    float m_half[3];
    CVector3 m_axes[3];
    CVector3 m_center;
};

int CCDBVolBox::TestCollision(const CTriangle& tri, CCollision&, bool) const {
    float b2e1;
    float a0e2;
    float b1e0;
    float b0e0;
    float a2e0;
    float nd;
    float b2e0;
    float gx;
    float dx;
    float b1e1;
    float c21;
    float a1e1;
    float gz;
    float fy;
    float a1n;
    float c10;
    float fz;
    float a1e2;
    float a2e1;
    CVector3 diff;
    CVector3 e0;
    CVector3 e1;
    CVector3 normal;
    CVector3 c;
    float best = 3.4028235e38f;
    EBoxTriSeperatingAxis bestAxis;
    dx = tri.m_vertices[0].d.c.x - m_center.d.c.x;
    float dy = tri.m_vertices[0].d.c.y - m_center.d.c.y;
    float dz = tri.m_vertices[0].d.c.z - m_center.d.c.z;
    float fx = tri.m_vertices[1].d.c.x - tri.m_vertices[0].d.c.x;
    fy = tri.m_vertices[1].d.c.y - tri.m_vertices[0].d.c.y;
    fz = tri.m_vertices[1].d.c.z - tri.m_vertices[0].d.c.z;
    gx = tri.m_vertices[2].d.c.x - tri.m_vertices[0].d.c.x;
    float gy = tri.m_vertices[2].d.c.y - tri.m_vertices[0].d.c.y;
    gz = tri.m_vertices[2].d.c.z - tri.m_vertices[0].d.c.z;
    diff.d.c.x = dx;
    diff.d.c.y = dy;
    diff.d.c.z = dz;
    e0.d.c.x = fx;
    e0.d.c.y = fy;
    e0.d.c.z = fz;
    e1.d.c.x = gx;
    e1.d.c.y = gy;
    e1.d.c.z = gz;
    float a0d = diff.Dot(m_axes[0]);
    float a0e0 = e0.Dot(m_axes[0]);
    float a0e1 = e1.Dot(m_axes[0]);
    if (Separate(m_half[0], a0d, a0e0, a0e1, (EBoxTriSeperatingAxis)1, best, bestAxis))
        return 1;
    float a1d = diff.Dot(m_axes[1]);
    a1e1 = e1.Dot(m_axes[1]);
    float a1e0 = e0.Dot(m_axes[1]);
    if (Separate(m_half[1], a1d, a1e0, a1e1, (EBoxTriSeperatingAxis)2, best, bestAxis))
        return 1;
    float a2d = diff.Dot(m_axes[2]);
    a2e1 = e1.Dot(m_axes[2]);
    a2e0 = e0.Dot(m_axes[2]);
    if (Separate(m_half[2], a2d, a2e0, a2e1, (EBoxTriSeperatingAxis)3, best, bestAxis))
        return 1;
    normal.Cross(e0, e1);
    normal.Unitize();
    nd = normal.Dot(diff);
    a1n = m_axes[1].Dot(normal);
    float a2n = m_axes[2].Dot(normal);
    float a0n = m_axes[0].Dot(normal);
    if (Separate((float)fabs(a0n) * m_half[0] + (float)fabs(a1n) * m_half[1] + (float)fabs(a2n) * m_half[2], nd,
                 (EBoxTriSeperatingAxis)4, best, bestAxis))
        return 1;
    float c00;
float c01;
float c02;
    b1e0 = (float)fabs(a1e0);
    b2e0 = (float)fabs(a2e0);
    c.Cross(m_axes[0], e0);
    c00 = c.Dot(diff);
    if (Separate(m_half[1] * b2e0 + m_half[2] * b1e0, c00, a0n, (EBoxTriSeperatingAxis)5, best, bestAxis))
        return 1;
    b1e1 = (float)fabs(a1e1);
    b2e1 = (float)fabs(a2e1);
    c.Cross(m_axes[0], e1);
    c01 = c.Dot(diff);
    if (Separate(m_half[1] * b2e1 + m_half[2] * b1e1, c01, -a0n, (EBoxTriSeperatingAxis)6, best, bestAxis))
        return 1;
    a1e2 = a1e1 - a1e0;
    float a2e2 = a2e1 - a2e0;
    float b1e2 = (float)fabs(a1e2);
    float b2e2 = (float)fabs(a2e2);
    c02 = c01 - c00;
    if (Separate(m_half[1] * b2e2 + m_half[2] * b1e2, c02, -a0n, (EBoxTriSeperatingAxis)7, best, bestAxis))
        return 1;
    b0e0 = (float)fabs(a0e0);
    c.Cross(m_axes[1], e0);
    c10 = c.Dot(diff);
    if (Separate(m_half[0] * b2e0 + m_half[2] * b0e0, c10, a1n, (EBoxTriSeperatingAxis)8, best, bestAxis))
        return 1;
    float b0e1 = (float)fabs(a0e1);
    c.Cross(m_axes[1], e1);
    float c11 = c.Dot(diff);
    if (Separate(m_half[0] * b2e1 + m_half[2] * b0e1, c11, -a1n, (EBoxTriSeperatingAxis)9, best, bestAxis))
        return 1;
    a0e2 = a0e1 - a0e0;
    float b0e2 = (float)fabs(a0e2);
    if (Separate(m_half[0] * b2e2 + m_half[2] * b0e2, c11 - c10, -a1n, (EBoxTriSeperatingAxis)10, best, bestAxis))
        return 1;
    c.Cross(m_axes[2], e0);
    float c20 = c.Dot(diff);
    if (Separate(m_half[0] * b1e0 + m_half[1] * b0e0, c20, a2n, (EBoxTriSeperatingAxis)11, best, bestAxis))
        return 1;
    c.Cross(m_axes[2], e1);
    c21 = c.Dot(diff);
    if (Separate(m_half[0] * b1e1 + m_half[1] * b0e1, c21, -a2n, (EBoxTriSeperatingAxis)12, best, bestAxis))
        return 1;
    if (Separate(m_half[0] * b1e2 + m_half[1] * b0e2, c21 - c20, -a2n, (EBoxTriSeperatingAxis)13, best, bestAxis))
        return 1;
    return Compare(best);
}
