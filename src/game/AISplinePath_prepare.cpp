// A fragment of AISplinePath.cpp (0x8005041c):
// CAISplinePathTraversal::PrepareForTraversalForward starts a traversal at
// the path's first segment: the current point is the segment expanded at
// t = 0 and, for a patrol, the AI object (+8 of the filter object) is asked
// to walk an A* path to it first (state 2) unless it cannot (state 1, as
// without a patrol). The classes and functions are named by the symbols; the
// segment view and Expand's body follow AISplinePath_segment.cpp (Expand is
// later in the original file and marked inline here so that it is inlined
// without being emitted). SetupNextSegmentForward moves on to the first
// segment whose closest parameter to the position is within 0..1 and expands
// it there, then makes the modified next point. MakeModifiedNextPoint moves
// the parameter on by four steps of the segment: within the segment the point
// is the segment there; past its end the parameter is clamped to 1 and the
// end point is pushed a quarter unit along the normalised end tangent
// (ExpandDerivative, marked inline here for the same reason as Expand; the
// segment's step at +4 is inferred). The traversal and path views follow
// AISplinePath_finalpoint.cpp and the states' meanings are inferred. The
// rest of the file is in other units.
#include <math.h>

class CVector3 {
public:
    void Normalize() {
        float length = sqrtf(x * x + y * y + z * z);
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
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

class CAIFilterRealVector3;

class CAISplinePathSegment {
public:
    void Expand(float, CVector3&);
    void ExpandDerivative(float, CVector3&);
    float GetClosestParameter(float, const CAIFilterRealVector3&);

    unsigned char unknown00[4];
    float m_step;
    CVector3 m_coefficients[4];
};

inline void CAISplinePathSegment::Expand(float t, CVector3& out) {
    out.x = t * m_coefficients[0].x;
    out.y = t * m_coefficients[0].y;
    out.z = t * m_coefficients[0].z;
    out += m_coefficients[1];
    out *= t;
    out += m_coefficients[2];
    out *= t;
    out += m_coefficients[3];
}

inline void CAISplinePathSegment::ExpandDerivative(float t, CVector3& out) {
    out.x = 1.5f * t * m_coefficients[0].x;
    out.y = 1.5f * t * m_coefficients[0].y;
    out.z = 1.5f * t * m_coefficients[0].z;
    out += m_coefficients[1];
    out *= 2.0f * t;
    out += m_coefficients[2];
}

class CAIFilterRealPosition {
public:
    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 {
public:
    float x;
    float y;
    float z;
    float w;
};

class CAIObject {
public:
    bool SetupAStarPathWalkForPatrol(const CAIFilterRealVector3&);
};

/* inferred: the filter object's AI object at +8 */
class CAIFilterObject {
public:
    unsigned char unknown00[8];
    CAIObject* m_object;
};

/* inferred: the path's segments */
class CAISplinePath {
public:
    CAISplinePathSegment* m_segments;
};

class CAISplinePathTraversal {
public:
    void PrepareForTraversalForward(CAISplinePath*, bool, CAIFilterObject*);
    void SetupNextSegmentForward(const CAIFilterRealPosition&);
    void MakeModifiedNextPoint();

    int m_state;
    float m_t;
    float m_value08;
    int m_index;
    CAISplinePath* m_path;
    CAISplinePathSegment* m_segment;
    CVector3 m_point;
};

void CAISplinePathTraversal::PrepareForTraversalForward(CAISplinePath* path, bool patrol, CAIFilterObject* filter) {
    m_path = path;
    m_index = 0;
    m_segment = &path->m_segments[m_index];
    m_t = 0.0f;
    m_segment->Expand(0.0f, m_point);
    if (patrol) {
        if (filter->m_object->SetupAStarPathWalkForPatrol((CAIFilterRealVector3&)m_point))
            m_state = 2;
        else
            m_state = 1;
    } else {
        m_state = 1;
    }
    m_value08 = 0.0f;
}

void CAISplinePathTraversal::SetupNextSegmentForward(const CAIFilterRealPosition& position) {
    float t = m_segment->GetClosestParameter(0.0f, (const CAIFilterRealVector3&)position);
    while (t < 0.0f || t > 1.0f) {
        m_index++;
        m_segment++;
        t = m_segment->GetClosestParameter(0.0f, (const CAIFilterRealVector3&)position);
    }
    m_t = t;
    m_segment->Expand(t, m_point);
    MakeModifiedNextPoint();
}

void CAISplinePathTraversal::MakeModifiedNextPoint() {
    float t = 4.0f * m_segment->m_step + m_t;
    if (t > 1.0f) {
        m_t = 1.0f;
        m_segment->Expand(1.0f, m_point);
        CVector3 tangent;
        m_segment->ExpandDerivative(1.0f, tangent);
        tangent.Normalize();
        tangent *= 0.25f;
        m_point += tangent;
    } else {
        m_segment->Expand(t, m_point);
    }
}
