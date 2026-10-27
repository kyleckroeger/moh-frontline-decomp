// A fragment of plane.cpp (0x800910ac): CPlane::GetIntersection (the line
// parameter of the plane crossing, with a cached-length parallel test and a
// near-plane fallback), CPlane::Transform (the normal scaled by the distance is
// transformed as a point, the normal as a vector, and the distance is
// recomputed as their dot product) and the CPlane constructor from a normal
// (copied as two doubles) and a distance. The file name is this project's; the
// original record is plane.cpp. CPlane, CLine3, CVector3 and CMatrix are named
// by the mangled symbols; the member layouts, the vector view (four floats,
// copied through a double pair), the cached CLine3 length and the helper
// inlines are inferred.
#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }
inline float Abs(float x) { return fabs(x); }

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3 operator=(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
        return *this;
    }
    CVector3& operator+=(const CVector3& o) {
        d.v[0] += o.d.v[0];
        d.v[1] += o.d.v[1];
        d.v[2] += o.d.v[2];
        return *this;
    }
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    CVector3Data d;
} __attribute__((aligned(8)));

inline float Dot(const CVector3& a, const CVector3& b) {
    return a.d.v[0] * b.d.v[0] + a.d.v[1] * b.d.v[1] + a.d.v[2] * b.d.v[2];
}

class CMatrix {
public:
    void TransformPoint(CVector3&, CVector3) const;
    void TransformVector(CVector3&, CVector3) const;

    float m[4][4];
} __attribute__((aligned(16)));

class CLine3 {
public:
    CVector3 GetStart() const { return m_start; }
    CVector3 GetDir() const { return m_dir; }
    float GetLengthSq() const {
        if (!m_lengthSqValid) {
            m_lengthSq = m_dir.d.v[0] * m_dir.d.v[0] + m_dir.d.v[1] * m_dir.d.v[1] + m_dir.d.v[2] * m_dir.d.v[2];
            m_lengthSqValid = 1;
        }
        return m_lengthSq;
    }
    float GetLength() const {
        if (!m_lengthValid) {
            m_length = sqrtf(GetLengthSq());
            m_lengthValid = 1;
        }
        return m_length;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    mutable float m_length;
    mutable float m_lengthSq;
    mutable unsigned char m_lengthValid;
    mutable unsigned char m_lengthSqValid;
} __attribute__((aligned(16)));

class CPlane {
public:
    CPlane(CVector3, float);
    unsigned char GetIntersection(const CLine3&, float&, CVector3*) const;
    void Transform(const CMatrix&);

    CVector3 m_normal;
    float m_d;
} __attribute__((aligned(16)));

static const float kEpsilon = 0.001745328f;

unsigned char CPlane::GetIntersection(const CLine3& line, float& t, CVector3* point) const {
    CVector3 dir;
    dir = line.GetDir();
    float denominator = Dot(m_normal, dir);
    float numerator = Dot(m_normal, line.GetStart());
    if (Abs(denominator) > kEpsilon * line.GetLength()) {
        t = (m_d - numerator) / denominator;
    } else {
        if (Abs(numerator) > kEpsilon * line.GetStart().Length())
            t = -1.0f;
        else
            t = 1.0f;
    }
    bool hit = t >= 0.0f && t <= 1.0f;
    if (hit && point) {
        float x = t * dir.d.v[0];
        float y = t * dir.d.v[1];
        float z = t * dir.d.v[2];
        point->d.v[0] = x;
        point->d.v[1] = y;
        point->d.v[2] = z;
        *point += line.GetStart();
    }
    return hit;
}

void CPlane::Transform(const CMatrix& matrix) {
    CVector3 scaled;
    CVector3 point;
    scaled.d.v[0] = m_d * m_normal.d.v[0];
    scaled.d.v[1] = m_d * m_normal.d.v[1];
    scaled.d.v[2] = m_d * m_normal.d.v[2];
    matrix.TransformPoint(point, scaled);
    matrix.TransformVector(m_normal, m_normal);
    m_d = m_normal.d.v[0] * point.d.v[0] + m_normal.d.v[1] * point.d.v[1] + m_normal.d.v[2] * point.d.v[2];
}

CPlane::CPlane(CVector3 normal, float d) : m_normal(normal), m_d(d) {
}
