// A fragment of cdbgeom.cpp (0x8006dac8): CCDBTriangle::TestCollision
// against a line (Moller-Trumbore). Unless the collision is a fiber line, a
// triangle flagged in its record (lowest bit of +14) is skipped (1). A
// determinant within 1e-5, or barycentric/line parameters outside [0, 1],
// miss: the result is raised to 1 when asked and 1 is returned. A hit clears
// the collision's contacts (when asked), stores the time, takes the hit point
// and its signed distance from the line's end along the unit direction
// (1/3/2 against CONTACT_EPSILON; a fiber line is always a contact at 0),
// raises the result and adds a contact: the triangle's unit normal (edge
// cross product), the point, the separation and the triangle. The file name
// is this project's; the original record is cdbgeom.cpp. The classes and
// functions are named by the mangled symbols; the members, the record flag,
// the vector helpers (x, y, z view, by-value assignment) and the result codes
// are inferred; fabs is the inline double function of the MSL math header.
// Constants are the file's .sdata2 pool entries. The rest of the file is in
// other units.
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
    void Normalize() {
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

class CCDBTriangle {
public:
    int TestCollision(const CLine3&, CCollision&, const CDBTri*, bool) const;

    CVector3 m_vertices[3];
};

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
    dir.Normalize();
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
            normal.Normalize();
            contact.m_vector1 = point;
            contact.m_vector0 = normal;
            contact.m_separation = separation;
            contact.m_tri = tri;
            collision.AddContact(contact);
        }
    }
    return result;
}
