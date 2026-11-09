// AISplinePath.cpp: builds a natural cubic spline through a list of points, as
// partial source. It solves the usual tridiagonal system for the tangents D (gamma
// and delta forward, D backward), then stores each segment's polynomial
// a t^3 + b t^2 + c t + d. A segment's arc-length estimate gives invLength; a
// segment too short for that is stored as a straight line.
//
// Checker: flagged only for one extra load of 0.0f; the original keeps 0.0f in a
// register across the segment loop. The logic is unaffected. Size also differs
// (824 against 736 instructions), since the original saves all of f16-f31 here.
extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

struct _aifilter_simple_v3 {
    float x, y, z;
};

class CAIFilterRealVector3 {
public:
    float x, y, z;
    float pad;
};

class CAISplinePathSegment {
public:
    CAISplinePathSegment();
    ~CAISplinePathSegment();
    CAISplinePathSegment* m_next;
    float m_invLength;
    CAIFilterRealVector3 m_a;
    CAIFilterRealVector3 m_b;
    CAIFilterRealVector3 m_c;
    CAIFilterRealVector3 m_d;
};

// Work buffers shared by every path built at level load.
class CAISplinePathManager {
public:
    unsigned char unknown00[8];
    CAIFilterRealVector3* m_points;
    float* m_gamma;
    CAIFilterRealVector3* m_delta;
    CAIFilterRealVector3* m_tangents;
};

class CAISplinePath {
public:
    void GenerateTestSplinePath(const CAISplinePathManager&, unsigned long, const _aifilter_simple_v3*, bool);
    CAISplinePathSegment* m_segments;
    unsigned long m_lastIndex;
};

void CAISplinePath::GenerateTestSplinePath(const CAISplinePathManager& manager, unsigned long count,
                                           const _aifilter_simple_v3* source, bool reversed) {
    CAIFilterRealVector3* points = manager.m_points;
    float* gamma = manager.m_gamma;
    CAIFilterRealVector3* delta = manager.m_delta;
    CAIFilterRealVector3* tangents = manager.m_tangents;
    unsigned long last = count - 1;
    m_lastIndex = last;
    m_segments = new CAISplinePathSegment[count];

    unsigned long i;
    if (!reversed) {
        for (i = 0; i < count; i++) {
            points[i].x = source[i].x;
            points[i].y = source[i].y;
            points[i].z = source[i].z;
        }
    } else {
        for (i = 0; i < count; i++) {
            points[i].x = source[count - i - 1].x;
            points[i].y = source[count - i - 1].y;
            points[i].z = source[count - i - 1].z;
        }
    }

    // Forward sweep.
    gamma[0] = 0.5f;
    for (i = 1; i < last; i++)
        gamma[i] = 1.0f / (4.0f - gamma[i - 1]);
    gamma[last] = 1.0f / (2.0f - gamma[last - 1]);

    delta[0].x = points[1].x - points[0].x;
    delta[0].y = points[1].y - points[0].y;
    delta[0].z = points[1].z - points[0].z;
    float scale = 3.0f * gamma[0];
    delta[0].x *= scale;
    delta[0].y *= scale;
    delta[0].z *= scale;
    for (i = 1; i < last; i++) {
        delta[i].x = points[i + 1].x - points[i - 1].x;
        delta[i].y = points[i + 1].y - points[i - 1].y;
        delta[i].z = points[i + 1].z - points[i - 1].z;
        delta[i].x *= 3.0f;
        delta[i].y *= 3.0f;
        delta[i].z *= 3.0f;
        delta[i].x -= delta[i - 1].x;
        delta[i].y -= delta[i - 1].y;
        delta[i].z -= delta[i - 1].z;
        delta[i].x *= gamma[i];
        delta[i].y *= gamma[i];
        delta[i].z *= gamma[i];
    }
    delta[last].x = points[last].x - points[last - 1].x;
    delta[last].y = points[last].y - points[last - 1].y;
    delta[last].z = points[last].z - points[last - 1].z;
    delta[last].x *= 3.0f;
    delta[last].y *= 3.0f;
    delta[last].z *= 3.0f;
    delta[last].x -= delta[last - 1].x;
    delta[last].y -= delta[last - 1].y;
    delta[last].z -= delta[last - 1].z;
    delta[last].x *= gamma[last];
    delta[last].y *= gamma[last];
    delta[last].z *= gamma[last];

    // Back substitution.
    tangents[last] = delta[last];
    for (int k = (int)count - 2; k != -1; k--) {
        float g = -gamma[k];
        tangents[k].x = g * tangents[k + 1].x;
        tangents[k].y = g * tangents[k + 1].y;
        tangents[k].z = g * tangents[k + 1].z;
        tangents[k].x += delta[k].x;
        tangents[k].y += delta[k].y;
        tangents[k].z += delta[k].z;
    }

    // One polynomial per segment. A short one is stored as a straight line.
    float zero = 0.0f;
    for (i = 0; i < last; i++) {
        CAIFilterRealVector3& p0 = points[i];
        CAIFilterRealVector3& p1 = points[i + 1];
        CAIFilterRealVector3& d0 = tangents[i];
        CAIFilterRealVector3& d1 = tangents[i + 1];
        float dy = p1.y - p0.y;
        float dx = p1.x - p0.x;
        float dz = p1.z - p0.z;
        float ax = (p0.x - p1.x) * 2.0f + d0.x + d1.x;
        float ay = (p0.y - p1.y) * 2.0f + d0.y + d1.y;
        float az = (p0.z - p1.z) * 2.0f + d0.z + d1.z;
        float bx = dx * 3.0f - (2.0f * d0.x + d1.x);
        float by = dy * 3.0f - (2.0f * d0.y + d1.y);
        float bz = dz * 3.0f - (2.0f * d0.z + d1.z);
        float estimate = d0.z * d0.z + d0.x * d0.x + d0.y * d0.y
                       + 2.0f * (bz * d0.z + bx * d0.x + by * d0.y)
                       + 1.3333332f * (bz * bz + bx * bx + by * by)
                       + 2.0f * (az * d0.z + ax * d0.x + ay * d0.y)
                       + 1.8f * (az * az + ax * ax + ay * ay)
                       + 3.0f * (az * bz + ax * bx + ay * by);
        CAISplinePathSegment& segment = m_segments[i];
        if (estimate >= 1.0f) {
            segment.m_invLength = 0.25f / sqrtf(estimate);
            segment.m_a.x = ax;
            segment.m_a.y = ay;
            segment.m_a.z = az;
            segment.m_b.x = bx;
            segment.m_b.y = by;
            segment.m_b.z = bz;
            segment.m_c = d0;
        } else {
            segment.m_invLength = 0.25f;
            segment.m_a.x = segment.m_a.y = segment.m_a.z = zero;
            segment.m_b.x = segment.m_b.y = segment.m_b.z = zero;
            segment.m_c.x = dx;
            segment.m_c.y = dy;
            segment.m_c.z = dz;
        }
        segment.m_d = p0;
    }

    CAISplinePathSegment& end = m_segments[last];
    end.m_invLength = 0.25f;
    end.m_a.x = end.m_a.y = end.m_a.z = zero;
    end.m_b.x = end.m_b.y = end.m_b.z = zero;
    end.m_c = tangents[last];
    end.m_d = points[last];

    for (i = 0; i < count; i++)
        m_segments[i].m_next = &m_segments[i + 1];
    m_segments[last].m_next = 0;
}
