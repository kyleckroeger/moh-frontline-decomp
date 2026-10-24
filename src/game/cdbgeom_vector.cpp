// A fragment of cdbgeom.cpp (0x8006c224): weak copies of header inlines
// emitted in this file: CVector3::Normalize (the libc inline sqrtf of the
// squared length; when it is non-zero each component is multiplied by its
// reciprocal; the length is returned), CLine3::SetSD (start and direction stored, the end
// as their sum, the cached length flags cleared), CVector3::DotDotDot (the
// dot products of three vectors with a fourth) and CVector3::Length (the
// square root, through the libc inline sqrtf, of the squared components
// summed). The file name is this project's; the original record is
// cdbgeom.cpp. The classes and functions are named by the mangled symbols;
// CVector3 is the double-pair view (its copies move doubleword pairs) and the
// squares are separate statements (the image does not fuse them), both
// inferred; CLine3's layout follows capsule_create.cpp. Normalize returning
// the length is inferred from the image (the length stays in f1 to the end).
// The constants link to their pool entries.
#include <math.h>

/* Inferred: CVector3 as four floats overlaid with two doubles. */
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
    float LengthSquared() const {
        float xx = d.v[0] * d.v[0];
        float yy = d.v[1] * d.v[1];
        float zz = d.v[2] * d.v[2];
        return xx + yy + zz;
    }
    float Length() const;
    float Normalize();
    static CVector3 DotDotDot(const CVector3&, const CVector3&, const CVector3&, const CVector3&);

    CVector3Data d;
} __attribute__((aligned(8)));

/* The line's points, direction, cached length values and their flags
   (16-aligned). */
class CLine3 {
public:
    void SetSD(CVector3, CVector3);

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_length;
    float m_lengthSq;
    unsigned char m_lengthValid;
    unsigned char m_lengthSqValid;
} __attribute__((aligned(16)));

__declspec(weak) float CVector3::Normalize() {
    float length = sqrtf(LengthSquared());
    if (length != 0.0f) {
        d.v[0] *= 1.0f / length;
        d.v[1] *= 1.0f / length;
        d.v[2] *= 1.0f / length;
    }
    return length;
}

__declspec(weak) void CLine3::SetSD(CVector3 start, CVector3 dir) {
    m_start = start;
    m_dir = dir;
    m_end.d.v[0] = start.d.v[0] + dir.d.v[0];
    m_end.d.v[1] = start.d.v[1] + dir.d.v[1];
    m_end.d.v[2] = start.d.v[2] + dir.d.v[2];
    m_lengthValid = 0;
    m_lengthSqValid = 0;
}

__declspec(weak) CVector3 CVector3::DotDotDot(const CVector3& a, const CVector3& b, const CVector3& c, const CVector3& v) {
    return CVector3(a.d.v[0] * v.d.v[0] + a.d.v[1] * v.d.v[1] + a.d.v[2] * v.d.v[2],
                    b.d.v[0] * v.d.v[0] + b.d.v[1] * v.d.v[1] + b.d.v[2] * v.d.v[2],
                    c.d.v[0] * v.d.v[0] + c.d.v[1] * v.d.v[1] + c.d.v[2] * v.d.v[2]);
}

__declspec(weak) float CVector3::Length() const {
    return sqrtf(LengthSquared());
}
