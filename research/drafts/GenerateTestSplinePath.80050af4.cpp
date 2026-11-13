// AISplinePath.cpp: builds a natural cubic spline through a list of points, as
// partial source. It solves the usual tridiagonal system for the tangents D (gamma
// and delta forward, D backward), then stores each segment's polynomial
// a t^3 + b t^2 + c t + d. A segment's arc-length estimate gives its parameter step
// (0.25 / length); a segment too short for that is stored as a straight line.
// The segment's next pointer, its Set and the local names are inferred; the
// coefficient layout follows the landed segment functions. The stack layout
// (zero, then a, b, c, d declared d first) and the copy loops' temporary are read
// from the original's frame. Left: one callee FPR too many in the segment loop.
// CVector3 and sqrtf: copies of include/game/CVector3.h and msl_sqrtf.h (the unit would include them).
extern inline float sqrtf(float x) {
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f) {
        double guess = __frsqrte((double)x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        y = (float)(x * guess);
        return y;
    }
    return x;
}

struct CVector3XYZW {
    float x, y, z, w;
};

union CVector3Data {
    double pair[2];
    float v[4];
    CVector3XYZW c;
};

// Some views cast to this to copy a vector as two doubles.
struct CVector3Pair {
    double a;
    double b;
};

// The fields live in an 8-byte-aligned plain struct so that the copy constructor can copy
// them as one plain object (see the copy constructor). The base is this project's spelling:
// the original may have used a member or a typedef'd C struct to the same effect; it
// changes neither the layout nor any mangled name.
struct CVector3Base {
    float x, y, z, w;
} __attribute__((aligned(8)));

class CVector3 : public CVector3Base {
public:
    // Bodies marked "original" are read from the weak copies the original image keeps
    // (inline functions the compiler didn't inline somewhere): Moh2RelGC.elf has
    // __as__, both constructors, Length, Normalize, Scale, Add, Sub, Abs, Cross and
    // DotDotDot of CVector3.
    CVector3() {}
    CVector3(float ax, float ay, float az) { x = ax; y = ay; z = az; }          // original
    // operator= returns a reference: with a by-value return the inlined assignment keeps
    // a dead copy on the stack that matched code doesn't have (e.g. CCamera::GetPosition).
    // The original's one out-of-line __as__ does return by value; the unit that emits it
    // keeps its own view (see research/layout/header_moves/CVector3.tsv).
    // The copy constructor copies the plain base: the compiler moves it as two doubles (lfd/stfd
    // pairs, as the weak __ct__ at 80074bd8 does) but schedules and colours the inlined copy
    // like an implicit one. A cast copy through CVector3Pair (the earlier form) or no copy constructor at all
    // each break a different group of matched units (research/notes/cvector3-header-variants.md).
    CVector3(const CVector3& other) : CVector3Base(other) {}                     // original
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    void Set(float ax, float ay, float az) { x = ax; y = ay; z = az; }
    float LengthSq() const { return x * x + y * y + z * z; }
    float LengthSquared() const { return x * x + y * y + z * z; }
    float Length() const { return sqrtf(x * x + y * y + z * z); }               // original
    // original: returns the length; one divide, then three multiplies.
    float Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            x *= scale;
            y *= scale;
            z *= scale;
        }
        return length;
    }
    float Dot(const CVector3& o) const { return x * o.x + y * o.y + z * o.z; }
    void Cross(const CVector3& a, const CVector3& b) {                         // original
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    // original: each component of this is one vector's dot product with v.
    void DotDotDot(const CVector3& a, const CVector3& b, const CVector3& c, const CVector3& v) {
        x = a.x * v.x + a.y * v.y + a.z * v.z;
        y = b.x * v.x + b.y * v.y + b.z * v.z;
        z = c.x * v.x + c.y * v.y + c.z * v.z;
    }
    void Abs(const CVector3& o) { x = __fabs(o.x); y = __fabs(o.y); z = __fabs(o.z); } // original
    CVector3& operator+=(const CVector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    CVector3& operator-=(const CVector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    CVector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    void Scale(const CVector3& o, float k) { x = k * o.x; y = k * o.y; z = k * o.z; } // original
    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; } // original
    void Add(const CVector3& o) { x += o.x; y += o.y; z += o.z; }              // original
    // Not in the original's weak copies; cdbgeom_triangle_line's form.
    void Unitize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    void Constrain(const CVector3&, float); // a real function (Constrain__8CVector3FRC8CVector3f)
} __attribute__((aligned(8)));


struct _aifilter_simple_v3 {
    float x, y, z;
};

class CAISplinePathSegment {
public:
    CAISplinePathSegment();
    ~CAISplinePathSegment();
    CAISplinePathSegment* m_next;
    float m_step;
    // Inferred setter. The original keeps a dead stack copy of the step at each of its three
    // uses (one slot per inlined call), which the cast read reproduces.
    void Set(float step, const CVector3& a, const CVector3& b, const CVector3& c, const CVector3& d) {
        m_step = *(float*)&step;
        m_coefficients[0] = a;
        m_coefficients[1] = b;
        m_coefficients[2] = c;
        m_coefficients[3] = d;
    }
    CVector3 m_coefficients[4];
};

class CAISplinePath;

// Work buffers shared by every path built at level load (names from AISplinePath.cpp,
// whose AllocateGenerateBuffers names them).
class CAISplinePathManager {
public:
    unsigned long m_count;
    CAISplinePath* m_paths;
    CVector3* m_points;
    float* m_diagonals;
    CVector3* m_deltas;
    CVector3* m_derivatives;
};

class CAISplinePath {
public:
    void GenerateTestSplinePath(const CAISplinePathManager&, unsigned long, const _aifilter_simple_v3*, bool);
    CAISplinePathSegment* m_segments;
    unsigned long m_lastIndex;
};

void CAISplinePath::GenerateTestSplinePath(const CAISplinePathManager& manager, unsigned long count,
                                           const _aifilter_simple_v3* source, bool reversed) {
    CVector3* points = manager.m_points;
    float* gamma = manager.m_diagonals;
    CVector3* delta = manager.m_deltas;
    CVector3* tangents = manager.m_derivatives;
    unsigned long last = count - 1;
    m_lastIndex = last;
    m_segments = new CAISplinePathSegment[count];

    unsigned long i;
    if (reversed) {
        for (i = 0; i < count; i++) {
            CVector3 v;
            v.x = source[count - i - 1].x;
            points[i].x = v.x;
            v.y = source[count - i - 1].y;
            points[i].y = v.y;
            v.z = source[count - i - 1].z;
            points[i].z = v.z;
        }
    } else {
        for (i = 0; i < count; i++) {
            CVector3 v;
            v.x = source[i].x;
            points[i].x = v.x;
            v.y = source[i].y;
            points[i].y = v.y;
            v.z = source[i].z;
            points[i].z = v.z;
        }
    }

    // Forward sweep.
    gamma[0] = 0.5f;
    for (i = 1; i < last; i++)
        gamma[i] = 1.0f / (4.0f - gamma[i - 1]);
    gamma[last] = 1.0f / (2.0f - gamma[last - 1]);

    delta[0].Sub(points[1], points[0]);
    delta[0] *= 3.0f * gamma[0];
    for (i = 1; i < last; i++) {
        delta[i].Sub(points[i + 1], points[i - 1]);
        delta[i] *= 3.0f;
        delta[i] -= delta[i - 1];
        delta[i] *= gamma[i];
    }
    delta[last].Sub(points[last], points[last - 1]);
    delta[last] *= 3.0f;
    delta[last] -= delta[last - 1];
    delta[last] *= gamma[last];

    // Back substitution.
    tangents[last] = delta[last];
    for (i = count - 2; i != -1; i--) {
        tangents[i].Scale(tangents[i + 1], -gamma[i]);
        tangents[i].Add(delta[i]);
    }

    // One polynomial per segment. A short one is stored as a straight line.
    CVector3 d;
    CVector3 c;
    CVector3 b;
    CVector3 a;
    CVector3 zero;
    zero.x = zero.y = zero.z = 0.0f;
    for (i = 0; i < last; i++) {
        CVector3& p0 = points[i];
        CVector3& p1 = (&p0)[1];
        CVector3& d0 = tangents[i];
        CVector3& d1 = (&d0)[1];
        c = d0;
        a.Sub(p0, p1);
        a *= 2.0f;
        a += d0;
        a += d1;
        b.Sub(p1, p0);
        b *= 3.0f;
        CVector3 t;
        t.Scale(d0, 2.0f);
        t += d1;
        b -= t;
        d = p0;
        float estimate = c.Dot(c) + 2.0f * b.Dot(c) + 1.3333332f * b.Dot(b) + 2.0f * a.Dot(c)
                       + 1.8f * a.Dot(a) + 3.0f * a.Dot(b);
        CAISplinePathSegment& segment = m_segments[i];
        if (estimate < 1.0f) {
            c.Sub(p1, p0);
            segment.Set(0.25f, zero, zero, c, d);
        } else {
            float step = 0.25f / sqrtf(estimate);
            segment.Set(step, a, b, c, d);
        }
    }

    CAISplinePathSegment& end = m_segments[last];
    end.Set(0.25f, zero, zero, tangents[last], points[last]);

    for (i = 0; i < count; i++)
        m_segments[i].m_next = &m_segments[i + 1];
    m_segments[last].m_next = 0;
}
