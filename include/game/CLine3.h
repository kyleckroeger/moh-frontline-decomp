// CLine3: a line segment (start, end, direction start->end), shared by every file that uses it
// (PortReadiness step 3.3).
//
// From the original: out of line (cdbgeom.cpp): GetProjection, GetSquaredDistanceBetweenSegments,
// IsDegenerate. Weak (inline) copies: the constructor from two points (placed by player_vec_sub.cpp),
// SetSD (cdbgeom_vector.cpp) and the destructor (debug_picker.cpp), whose bodies these follow. The
// constructor sets the end points through SetSE, inline everywhere. 0x40 bytes, 16-byte aligned.
//
// Inferred, not in any mangled name: the member names (m_start, m_end, m_dir as 26 matched views
// spell them; the cached length and squared length with their valid flags as cdbgeom_vector.cpp
// names them), SetSE and the getters. CVector3 must be complete before this header.
#ifndef GAME_CLINE3_H
#define GAME_CLINE3_H

class CLine3 {
public:
    CLine3() {}
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    ~CLine3() {}
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.x = m_end.x - m_start.x;
        m_dir.y = m_end.y - m_start.y;
        m_dir.z = m_end.z - m_start.z;
        m_lengthValid = 0;
        m_lengthSqValid = 0;
    }
    void SetSD(CVector3 start, CVector3 dir) {
        m_start = start;
        m_dir = dir;
        m_end.x = start.x + dir.x;
        m_end.y = start.y + dir.y;
        m_end.z = start.z + dir.z;
        m_lengthValid = 0;
        m_lengthSqValid = 0;
    }
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 GetDirection() const { return m_dir; }
    void GetProjection(CVector3, CVector3&, float&) const;
    float GetSquaredDistanceBetweenSegments(const CLine3&, float&, float&, float*) const;
    bool IsDegenerate() const;

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_length;
    float m_lengthSq;
    unsigned char m_lengthValid;
    unsigned char m_lengthSqValid;
} __attribute__((aligned(16)));

#endif
