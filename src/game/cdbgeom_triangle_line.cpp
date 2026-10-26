// A fragment of cdbgeom.cpp (0x8006cbc4): the two CCDBTriangle::TestCollision
// overloads against a box and against a line.
//
// The box test is a separating-axis test over 13 axes, in order: the box's
// three axes (the triangle's offset from the box centre and its two edges
// from the first vertex projected through CVector3::DotDotDot, then the
// three-projection separate), the triangle's unit normal (box radius from the
// absolute axis projections against the absolute normal distance, an inline
// one-projection separate), then each box axis crossed with the three edges,
// skipped when the cross product is (nearly) degenerate: the first is
// measured with Length() before it is normalised, the others use Normalize's
// returned length. Crosses with the first two edges project the offset and
// the remaining edge (two-projection separate, the projections taken into
// locals first), crosses with the third edge (vertex 2 - vertex 1) use
// DotDotDot again. A separating axis raises the result to 1 when asked and
// returns 1; otherwise the smallest penetration and its axis are tracked from
// FLT_MAX, the result code (1/3/2 against CONTACT_EPSILON) raises the
// collision's result when asked, and a touching contact (2) is added with the
// tracked axis vector as its normal, the penetration as its separation and
// the triangle record.
//
// The line test (Moller-Trumbore): unless the collision is a fiber line, a
// triangle flagged in its record (lowest bit of +14) is skipped (1). A
// determinant within 1e-5, or barycentric/line parameters outside [0, 1],
// miss: the result is raised to 1 when asked and 1 is returned. A hit clears
// the collision's contacts (when asked), stores the time, takes the hit point
// and its signed distance from the line's end along the unit direction (1/3/2
// against CONTACT_EPSILON; a fiber line is always a contact at 0), raises the
// result and adds a contact: the triangle's unit normal (edge cross product),
// the point, the separation and the triangle. Its normalisations are expanded
// inline (Unitize, named here), while the box test calls the file's
// out-of-line CVector3::Normalize.
//
// The file name is this project's; the original record is cdbgeom.cpp. The
// classes and functions are named by the mangled symbols; the members, the
// record flag, the box view (half extents, axes, centre), the vector helpers
// (x, y, z view, by-value assignment), the axis numbering and the result
// codes are inferred; DotDotDot is defined in another unit and the two
// file-local separate overloads, defined after the tests (as in
// cdbgeom_volbox_ctor, which places them), are file duplicates here; fabs is
// the inline double function of the MSL math header. Constants are the file's
// .sdata2 pool entries; CONTACT_EPSILON is defined in another file. The rest
// of the file is in other units.
#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }

union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    CVector3 operator=(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
        return *this;
    }
    CVector3(float x, float y, float z) {
        d.c.x = x;
        d.c.y = y;
        d.c.z = z;
    }
    void Scale(const CVector3& v, float k) {
        d.c.x = k * v.d.c.x;
        d.c.y = k * v.d.c.y;
        d.c.z = k * v.d.c.z;
    }
    CVector3& operator+=(const CVector3& o) {
        d.c.x += o.d.c.x;
        d.c.y += o.d.c.y;
        d.c.z += o.d.c.z;
        return *this;
    }
    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }
    float LengthSq() const { return d.c.x * d.c.x + d.c.y * d.c.y + d.c.z * d.c.z; }
    void Cross(const CVector3& a, const CVector3& b) {
        d.c.x = a.d.c.y * b.d.c.z - a.d.c.z * b.d.c.y;
        d.c.y = a.d.c.z * b.d.c.x - a.d.c.x * b.d.c.z;
        d.c.z = a.d.c.x * b.d.c.y - a.d.c.y * b.d.c.x;
    }
    float Normalize();
    float Length() const;
    void DotDotDot(const CVector3&, const CVector3&, const CVector3&, const CVector3&);
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

class CLine3 {
public:
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 GetDirection() const { return m_dir; }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
};

class CDBTri {
public:
    unsigned char unknown00[14];
    signed char m_flag7 : 1;
    signed char m_flag6 : 1;
    signed char m_flag5 : 1;
    signed char m_flag4 : 1;
    signed char m_flag3 : 1;
    signed char m_flag2 : 1;
    signed char m_flag1 : 1;
    signed char m_noLine : 1;
};
class IVolume;

struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    const CDBTri* m_tri;
    const IVolume* m_object;
} __attribute__((aligned(16)));

class CCollision {
public:
    void AddContact(const SClsnContact&);
    void ClearContacts();

    unsigned char unknown00[12];
    float m_time;
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
};

extern const float CONTACT_EPSILON;

inline int Compare(float separation) {
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

struct VECTOR3VIEW {
    double pair[2];
};

class IVolume {
public:
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    CVector3 GetCenter() const { return *(const CVector3*)&m_center; }
    CVector3 GetAxis(int i) const { return *(const CVector3*)&m_basis[i]; }

    float m_half[3];
    VECTOR3VIEW m_basis[3];
    VECTOR3VIEW m_center;
};

enum EBoxTriSeperatingAxis { kAxisNone };

static bool separate(float, float, float, EBoxTriSeperatingAxis, float&, EBoxTriSeperatingAxis&);
static bool separate(float, float, float, float, EBoxTriSeperatingAxis, float&, EBoxTriSeperatingAxis&);

static inline bool separate(float r, float p, EBoxTriSeperatingAxis axis, float& best, EBoxTriSeperatingAxis& bestAxis) {
    float d = (float)fabs(p) - r;
    if (Compare(d) == 1)
        return true;
    if ((float)fabs(d) < (float)fabs(best)) {
        best = d;
        bestAxis = axis;
    }
    return false;
}

class CCDBTriangle {
public:
    int TestCollision(const CVolBox&, CCollision&, const CDBTri*, bool) const;
    int TestCollision(const CLine3&, CCollision&, const CDBTri*, bool) const;

    CVector3 m_vertices[3];
};

#define FAIL() \
    { \
        if (flag && collision.m_result < 1) \
            collision.m_result = 1; \
        return 1; \
    }

int CCDBTriangle::TestCollision(const CVolBox& box, CCollision& collision, const CDBTri* tri, bool flag) const {
    SClsnContact contact;
    CVector3 e0;
    CVector3 e1;
    CVector3 normal;
    CVector3 diff;
    CVector3 p;
    CVector3 e2;
    float h2;
    float h0;
    float h1;
    float best = 3.4028235e38f;
    EBoxTriSeperatingAxis bestAxis = (EBoxTriSeperatingAxis)0;
    SetDifference(diff, m_vertices[0], box.GetCenter());
    SetDifference(e0, m_vertices[1], m_vertices[0]);
    SetDifference(e1, m_vertices[2], m_vertices[0]);
    SetDifference(e2, m_vertices[2], m_vertices[1]);
    h0 = box.m_half[0];
    CVector3 a0 = box.GetAxis(0);
    p.DotDotDot(diff, e0, e1, a0);
    if (separate(h0, p.d.c.x, p.d.c.y, p.d.c.z, (EBoxTriSeperatingAxis)1, best, bestAxis))
        FAIL();
    h1 = box.m_half[1];
    CVector3 a1 = box.GetAxis(1);
    p.DotDotDot(diff, e0, e1, a1);
    if (separate(h1, p.d.c.x, p.d.c.y, p.d.c.z, (EBoxTriSeperatingAxis)2, best, bestAxis))
        FAIL();
    h2 = box.m_half[2];
    CVector3 a2 = box.GetAxis(2);
    p.DotDotDot(diff, e0, e1, a2);
    if (separate(h2, p.d.c.x, p.d.c.y, p.d.c.z, (EBoxTriSeperatingAxis)3, best, bestAxis))
        FAIL();
    normal.Cross(e0, e1);
    normal.Normalize();
    p.DotDotDot(a0, a1, a2, normal);
    p.d.c.x = fabs(p.d.c.x);
    p.d.c.y = fabs(p.d.c.y);
    p.d.c.z = fabs(p.d.c.z);
    if (separate(p.d.c.x * h0 + p.d.c.y * h1 + p.d.c.z * h2, normal.Dot(diff), (EBoxTriSeperatingAxis)4, best, bestAxis))
        FAIL();
    CVector3 c00;
    c00.Cross(a0, e0);
    if (c00.Length() > 0.001f) {
        c00.Normalize();
        float q0 = c00.Dot(diff);
        float q1 = c00.Dot(e1);
        if (separate(h1 * (float)fabs(a1.Dot(c00)) + h2 * (float)fabs(a2.Dot(c00)), q0, q1, (EBoxTriSeperatingAxis)5, best, bestAxis))
            FAIL();
    }
    CVector3 c01;
    c01.Cross(a0, e1);
    if (c01.Normalize() > 0.001f) {
        float q0 = c01.Dot(diff);
        float q1 = c01.Dot(e0);
        if (separate(h1 * (float)fabs(a1.Dot(c01)) + h2 * (float)fabs(a2.Dot(c01)), q0, q1, (EBoxTriSeperatingAxis)6, best, bestAxis))
            FAIL();
    }
    CVector3 c02;
    c02.Cross(a0, e2);
    if (c02.Normalize() > 0.001f) {
        p.DotDotDot(diff, e0, e1, c02);
        float q0 = p.d.c.x;
        float q1 = p.d.c.y;
        float q2 = p.d.c.z;
        if (separate(h1 * (float)fabs(a1.Dot(c02)) + h2 * (float)fabs(a2.Dot(c02)), q0, q1, q2, (EBoxTriSeperatingAxis)7, best, bestAxis))
            FAIL();
    }
    CVector3 c10;
    c10.Cross(a1, e0);
    if (c10.Normalize() > 0.001f) {
        float q0 = c10.Dot(diff);
        float q1 = c10.Dot(e1);
        if (separate(h0 * (float)fabs(a0.Dot(c10)) + h2 * (float)fabs(a2.Dot(c10)), q0, q1, (EBoxTriSeperatingAxis)8, best, bestAxis))
            FAIL();
    }
    CVector3 c11;
    c11.Cross(a1, e1);
    if (c11.Normalize() > 0.001f) {
        float q0 = c11.Dot(diff);
        float q1 = c11.Dot(e0);
        if (separate(h0 * (float)fabs(a0.Dot(c11)) + h2 * (float)fabs(a2.Dot(c11)), q0, q1, (EBoxTriSeperatingAxis)9, best, bestAxis))
            FAIL();
    }
    CVector3 c12;
    c12.Cross(a1, e2);
    if (c12.Normalize() > 0.001f) {
        p.DotDotDot(diff, e0, e1, c12);
        float q0 = p.d.c.x;
        float q1 = p.d.c.y;
        float q2 = p.d.c.z;
        if (separate(h0 * (float)fabs(a0.Dot(c12)) + h2 * (float)fabs(a2.Dot(c12)), q0, q1, q2, (EBoxTriSeperatingAxis)10, best, bestAxis))
            FAIL();
    }
    CVector3 c20;
    c20.Cross(a2, e0);
    if (c20.Normalize() > 0.001f) {
        float q0 = c20.Dot(diff);
        float q1 = c20.Dot(e1);
        if (separate(h0 * (float)fabs(a0.Dot(c20)) + h1 * (float)fabs(a1.Dot(c20)), q0, q1, (EBoxTriSeperatingAxis)11, best, bestAxis))
            FAIL();
    }
    CVector3 c21;
    c21.Cross(a2, e1);
    if (c21.Normalize() > 0.001f) {
        float q0 = c21.Dot(diff);
        float q1 = c21.Dot(e0);
        if (separate(h0 * (float)fabs(a0.Dot(c21)) + h1 * (float)fabs(a1.Dot(c21)), q0, q1, (EBoxTriSeperatingAxis)12, best, bestAxis))
            FAIL();
    }
    CVector3 c22;
    c22.Cross(a2, e2);
    if (c22.Normalize() > 0.001f) {
        p.DotDotDot(diff, e0, e1, c22);
        float q0 = p.d.c.x;
        float q1 = p.d.c.y;
        float q2 = p.d.c.z;
        if (separate(h0 * (float)fabs(a0.Dot(c22)) + h1 * (float)fabs(a1.Dot(c22)), q0, q1, q2, (EBoxTriSeperatingAxis)13, best, bestAxis))
            FAIL();
    }
    int code = Compare(best);
    if (flag) {
        if (code > collision.m_result)
            collision.m_result = code;
        if (code == 2) {
            contact.m_separation = best;
            switch (bestAxis) {
            case 1: contact.m_vector0 = a0; break;
            case 2: contact.m_vector0 = a1; break;
            case 3: contact.m_vector0 = a2; break;
            case 4: contact.m_vector0 = normal; break;
            case 5: contact.m_vector0 = c00; break;
            case 6: contact.m_vector0 = c01; break;
            case 7: contact.m_vector0 = c02; break;
            case 8: contact.m_vector0 = c10; break;
            case 9: contact.m_vector0 = c11; break;
            case 10: contact.m_vector0 = c12; break;
            case 11: contact.m_vector0 = c20; break;
            case 12: contact.m_vector0 = c21; break;
            case 13: contact.m_vector0 = c22; break;
            }
            contact.m_tri = tri;
            collision.AddContact(contact);
        }
    }
    return code;
}

int CCDBTriangle::TestCollision(const CLine3& line, CCollision& collision, const CDBTri* tri, bool flag) const {
    if (!collision.m_line && tri->m_noLine)
        return 1;
    CVector3 dir;
    CVector3 point;
    CVector3 normal;
    CVector3 e1;
    CVector3 e2;
    SetDifference(e1, m_vertices[1], m_vertices[0]);
    SetDifference(e2, m_vertices[2], m_vertices[0]);
    dir = line.GetDirection();
    CVector3 p;
    p.Cross(dir, e2);
    float det = e1.Dot(p);
    if ((float)fabs(det) < 1e-05f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float inv = 1.0f / det;
    CVector3 s;
    SetDifference(s, line.GetStart(), m_vertices[0]);
    float u = inv * s.Dot(p);
    if (u < 0.0f || u > 1.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    CVector3 q;
    q.Cross(s, e1);
    float v = inv * dir.Dot(q);
    if (v < 0.0f || u + v > 1.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float t = inv * e2.Dot(q);
    if (t < 0.0f || t > 1.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    if (flag)
        collision.ClearContacts();
    point.Scale(dir, t);
    point += line.GetStart();
    collision.m_time = t;
    CVector3 rest;
    SetDifference(rest, point, line.GetEnd());
    dir.Unitize();
    float separation = dir.Dot(rest);
    int result = Compare(separation);
    if (flag) {
        if (collision.m_line) {
            result = 2;
            separation = 0.0f;
        }
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            SClsnContact contact;
            normal.Cross(e1, e2);
            normal.Unitize();
            contact.m_vector1 = point;
            contact.m_vector0 = normal;
            contact.m_separation = separation;
            contact.m_tri = tri;
            collision.AddContact(contact);
        }
    }
    return result;
}

/* Inferred: which side of the contact band a distance lies on. */
static inline int Classify(float d, float eps) {
    if (d > eps)
        return 1;
    if (d < -eps)
        return 3;
    return 2;
}

/* Inferred: the smaller and larger of two values, by reference. */
template <class T> inline const T& Min(const T& a, const T& b) { return a < b ? a : b; }
template <class T> inline const T& Max(const T& a, const T& b) { return a < b ? b : a; }

static bool separate(float r, float p0, float p1, EBoxTriSeperatingAxis axis, float& best, EBoxTriSeperatingAxis& bestAxis) {
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
    if (Classify(d, CONTACT_EPSILON) == 1)
        return true;
    if ((float)fabs(d) < (float)fabs(best)) {
        best = d;
        bestAxis = axis;
    }
    return false;
}

static bool separate(float r, float p0, float p1, float p2, EBoxTriSeperatingAxis axis, float& best, EBoxTriSeperatingAxis& bestAxis) {
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
    if (Classify(d, CONTACT_EPSILON) == 1)
        return true;
    if ((float)fabs(d) < (float)fabs(best)) {
        best = d;
        bestAxis = axis;
    }
    return false;
}
