// A fragment of AISplinePath.cpp (0x8004fcbc):
// CAISplinePathTraversal::GetFinalPointForward expands the path's segment
// at the index at +4 of the path (+16) at t = 0 into the output position.
// The classes and functions are named by the symbols; the segment view and
// Expand's body follow AISplinePath_segment.cpp. Expand is defined later in
// the original file but inlined here (the file's inlining is deferred);
// this unit marks it inline so that it is inlined without being emitted.
// The path view, the output's CVector3 view of the position and the index's
// meaning are inferred. The rest of the file is in other units.
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

class CAIFilterRealPosition {
public:
    float x;
    float y;
    float z;
};

class CAISplinePathSegment {
public:
    void Expand(float, CVector3&);

    unsigned char unknown00[8];
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

/* inferred: the path's segments and the final segment's index */
class CAISplinePath {
public:
    CAISplinePathSegment* m_segments;
    int m_last;
};

class CAISplinePathTraversal {
public:
    void GetFinalPointForward(CAIFilterRealPosition*);

    unsigned char unknown00[16];
    CAISplinePath* m_path;
};

void CAISplinePathTraversal::GetFinalPointForward(CAIFilterRealPosition* point) {
    m_path->m_segments[m_path->m_last].Expand(0.0f, *(CVector3*)point);
}
