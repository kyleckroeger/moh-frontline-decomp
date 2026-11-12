// A fragment of AISplinePath.cpp (0x80051ad4): CAISplinePathSegment's
// ExpandDerivative (the cubic's derivative at t, (1.5t * a + b) * 2t + c; the
// constants are .sdata2 pool entries), Expand,
// which evaluates the segment's cubic at t by Horner's rule into the output
// vector (coefficients at +8, +24, +40 and +56), and the segment's empty
// destructor and constructor. The classes are named by the mangled symbols;
// the coefficient layout and the in-place vector operators are inferred.
class CVector3 {
public:
    void operator+=(const CVector3& o) {
        x += o.x;
        y += o.y;
        z += o.z;
    }
    void operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
    }

    float x;
    float y;
    float z;
    float w;
};

class CAIFilterRealVector3 {
public:
    float x;
    float y;
    float z;
    float w;
};

class CAISplinePathSegment {
public:
    float GetClosestParameter(float, const CAIFilterRealVector3&);
    CAISplinePathSegment();
    ~CAISplinePathSegment();
    void ExpandDerivative(float, CVector3&);
    void Expand(float, CVector3&);

    unsigned char unknown00[4];
    float m_step;
    CVector3 m_coefficients[4];
};

/* inferred: the cubic, its derivative and its second derivative at t, as
   Expand and ExpandDerivative (in-place vector operations) */
inline void Position(const CVector3* c, float t, CVector3& out) {
    out.x = t * c[0].x;
    out.y = t * c[0].y;
    out.z = t * c[0].z;
    out += c[1];
    out *= t;
    out += c[2];
    out *= t;
    out += c[3];
}

inline void Derivative(const CVector3* c, float t, CVector3& out) {
    out.x = 1.5f * t * c[0].x;
    out.y = 1.5f * t * c[0].y;
    out.z = 1.5f * t * c[0].z;
    out += c[1];
    out *= 2.0f * t;
    out += c[2];
}

inline void SecondDerivative(const CVector3* c, float t, CVector3& out) {
    out.x = 3.0f * t * c[0].x;
    out.y = 3.0f * t * c[0].y;
    out.z = 3.0f * t * c[0].z;
    out += c[1];
    out *= 2.0f;
}

// Inferred: the dwi library's reference-returning min (the scan loop's bound
// selects between the two candidates' stack slots).
namespace dwi {
template <class T>
inline const T& min(const T& a, const T& b) {
    return a < b ? a : b;
}
}

float CAISplinePathSegment::GetClosestParameter(float start, const CAIFilterRealVector3& target) {
    CVector3 p;
    CVector3 d;
    CVector3 dd;
    float t;
    float num;
    float den;
    float next;
    float best;
    float bestDistance;
    float distance;
    t = start;
    for (int i = 8; i != 0; i--) {
        Position(m_coefficients, t, p);
        p.x -= target.x;
        p.y -= target.y;
        p.z -= target.z;
        Derivative(m_coefficients, t, d);
        SecondDerivative(m_coefficients, t, dd);
        num = p.x * d.x + p.y * d.y + p.z * d.z;
        den = d.x * d.x + d.y * d.y + d.z * d.z + (p.x * dd.x + p.y * dd.y + p.z * dd.z);
        if (!((float)__fabs(den) > (float)__fabs(0.0001f * num)))
            break;
        next = t - num / den;
        if (__fabs(next - t) < 0.125f * m_step) {
            if (next >= start && next < 1.0f)
                return next;
            break;
        }
        t = next;
    }
    best = start;
    Position(m_coefficients, start, p);
    p.x -= target.x;
    p.y -= target.y;
    p.z -= target.z;
    bestDistance = p.x * p.x + p.y * p.y + p.z * p.z;
    float a = 1.0f + m_step;
    float b = 16.0f * m_step + start;
    float end = dwi::min(a, b);
    for (float u = start + m_step; u < end; u += m_step) {
        Position(m_coefficients, u, p);
        p.x -= target.x;
        p.y -= target.y;
        p.z -= target.z;
        distance = p.x * p.x + p.y * p.y + p.z * p.z;
        if (distance < bestDistance) {
            best = u;
            bestDistance = distance;
        }
    }
    return best;
}

void CAISplinePathSegment::ExpandDerivative(float t, CVector3& out) {
    out.x = 1.5f * t * m_coefficients[0].x;
    out.y = 1.5f * t * m_coefficients[0].y;
    out.z = 1.5f * t * m_coefficients[0].z;
    out += m_coefficients[1];
    out *= 2.0f * t;
    out += m_coefficients[2];
}

void CAISplinePathSegment::Expand(float t, CVector3& out) {
    out.x = t * m_coefficients[0].x;
    out.y = t * m_coefficients[0].y;
    out.z = t * m_coefficients[0].z;
    out += m_coefficients[1];
    out *= t;
    out += m_coefficients[2];
    out *= t;
    out += m_coefficients[3];
}

CAISplinePathSegment::~CAISplinePathSegment() {
}

CAISplinePathSegment::CAISplinePathSegment() {
}
