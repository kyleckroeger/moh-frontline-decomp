// A fragment of fiber.cpp (0x800c9668): CVolFiber::TestCollision against a
// line, the closest points of two segments (Bourke's line-line method). The
// collision's line flag (top bit of +16) is set. With p21 the fiber's
// segment, p43 the line's and p13 the start offset, a degenerate segment
// (every component within 1e-12) or parallel segments (denominator within
// 1e-12) miss, as do parameters outside [0, 1] and closest points more than
// 0.001 apart in any axis: the result is raised to 1 when asked and 1 is
// returned. A hit raises the result to 2 and adds a contact: the distance of
// the fiber's closest point from its start as separation, the time set to the
// fiber parameter, the cross product of the two directions and the closest
// point. The file name is this project's; the original record is fiber.cpp.
// The classes and functions are named by the mangled symbols; the members,
// the difference helper, the by-value assignment (whose dead return copies
// the original keeps on the stack) and the result codes are inferred.
// The vector helpers use the x, y, z view of CVector3 (its copies move
// doubleword pairs) and fabs is the inline double function of the MSL math
// header (the SDK macro form gives other register numbers). Constants
// are the file's .sdata2 pool entries. The rest of the file is in other units.
#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }

class CMatrix;
class CTriangle;
class CPlane;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;
class CVolFiber;

union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    CVector3 operator=(const CVector3& o) { d.pair[0] = o.d.pair[0]; d.pair[1] = o.d.pair[1]; return *this; }
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
    CVector3& operator-=(const CVector3& o) {
        d.c.x -= o.d.c.x;
        d.c.y -= o.d.c.y;
        d.c.z -= o.d.c.z;
        return *this;
    }
    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }
    float LengthSq() const { return d.c.x * d.c.x + d.c.y * d.c.y + d.c.z * d.c.z; }
    float Length() const { return sqrtf(LengthSq()); }

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

class IVolume;

struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
    const IVolume* m_object;
} __attribute__((aligned(16)));

class CCollision {
public:
    void AddContact(const SClsnContact&);

    unsigned char unknown00[12];
    float m_time;
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
};

class IVolume {
public:
    virtual ~IVolume();
};

class CVolFiber : public IVolume {
public:
    int TestCollision(const CLine3&, CCollision&, bool) const;

    unsigned char unknown04[12];
    CLine3 m_line;
};

int CVolFiber::TestCollision(const CLine3& line, CCollision& collision, bool flag) const {
    CVector3 pa;
    CVector3 n;
    CVector3 l;
    collision.m_line = 1;
    CVector3 p21;
    CVector3 p43;
    CVector3 p13;
    SetDifference(p21, m_line.GetEnd(), m_line.GetStart());
    SetDifference(p43, line.GetEnd(), line.GetStart());
    SetDifference(p13, m_line.GetStart(), line.GetStart());
    if (fabs(p21.d.c.x) < 1e-12f && fabs(p21.d.c.y) < 1e-12f && fabs(p21.d.c.z) < 1e-12f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    if (fabs(p43.d.c.x) < 1e-12f && fabs(p43.d.c.y) < 1e-12f && fabs(p43.d.c.z) < 1e-12f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float d4321 = p43.Dot(p21);
    float d4343 = p43.LengthSq();
    float d2121 = p21.LengthSq();
    float denom = d4343 * d2121 - d4321 * d4321;
    if (fabs(denom) < 1e-12f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float inv = 1.0f / denom;
    float d1321 = p13.Dot(p21);
    float d1343 = p13.Dot(p43);
    float mua = inv * (d1343 * d4321 - d4343 * d1321);
    if (mua < 0.0f || mua > 1.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float mub = inv * (d4321 * mua + d1343);
    if (mub < 0.0f || mub > 1.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    pa.Scale(p21, mua);
    pa += m_line.GetStart();
    CVector3 pb;
    pb.Scale(p43, mub);
    pb += line.GetStart();
    pa -= pb;
    if (fabs(pa.d.c.x) > 0.001f || fabs(pa.d.c.y) > 0.001f || fabs(pa.d.c.z) > 0.001f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    if (flag) {
        if (collision.m_result < 2)
            collision.m_result = 2;
        SClsnContact contact;
        SetDifference(n, pa, m_line.GetStart());
        contact.m_separation = n.Length();
        collision.m_time = mua;
        n = m_line.GetDirection();
        l = line.GetDirection();
        contact.m_vector0.d.c.x = n.d.c.y * l.d.c.z - n.d.c.z * l.d.c.y;
        contact.m_vector0.d.c.y = n.d.c.z * l.d.c.x - n.d.c.x * l.d.c.z;
        contact.m_vector0.d.c.z = n.d.c.x * l.d.c.y - n.d.c.y * l.d.c.x;
        contact.m_vector1 = pa;
        contact.m_index = 0;
        collision.AddContact(contact);
    }
    return 2;
}
