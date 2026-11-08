#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }

class CMatrix;
class CTriangle;
class CPlane;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
    }
    void Scale(const CVector3& v, float k) {
        d.c.x = k * v.d.c.x;
        d.c.y = k * v.d.c.y;
        d.c.z = k * v.d.c.z;
    }
    CVector3& operator-=(const CVector3& o) {
        d.c.x -= o.d.c.x;
        d.c.y -= o.d.c.y;
        d.c.z -= o.d.c.z;
        return *this;
    }
    CVector3& operator+=(const CVector3& o) {
        d.c.x += o.d.c.x;
        d.c.y += o.d.c.y;
        d.c.z += o.d.c.z;
        return *this;
    }
    void Negate(const CVector3& v) {
        d.c.x = -v.d.c.x;
        d.c.y = -v.d.c.y;
        d.c.z = -v.d.c.z;
    }
    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }
    float LengthSq() const { return d.c.x * d.c.x + d.c.y * d.c.y + d.c.z * d.c.z; }

    CVector3Data d;
} __attribute__((aligned(8)));

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

extern const float CONTACT_EPSILON;

inline int Compare(float separation) {
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

class CPlane {
public:
    float GetDistance(CVector3 r) const {
        return m_normal.d.c.x * r.d.c.x + m_normal.d.c.y * r.d.c.y + m_normal.d.c.z * r.d.c.z - m_distance;
    }
    static CVector3 Id(CVector3 v) {
        CVector3 q = v;
        return q;
    }
    float Project2(CVector3 p, CVector3& out) const {
        float distance = GetDistance(Id(p));
        out.Scale(m_normal, -distance);
        out += p;
        return distance;
    }
    CVector3 m_normal;
    float m_distance;
};

class IVolume {
public:
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    int TestCollision(const CPlane&, CCollision&, bool) const;

    float m_half[3];
    CVector3 m_axes[3];
    CVector3 m_center;
};

int CVolBox::TestCollision(const CPlane& plane, CCollision& collision, bool flag) const {
    CVector3 a;
    CVector3 b;
    CVector3 c;
    a.Scale(m_axes[0], m_half[0]);
    b.Scale(m_axes[1], m_half[1]);
    c.Scale(m_axes[2], m_half[2]);
    CVector3 point;
    CVector3 corner = m_center;
    corner -= a;
    corner -= b;
    corner -= c;
    SClsnContact contact;
    contact.m_vector0.Negate(plane.m_normal);
    contact.m_separation = plane.Project2(corner, contact.m_vector1);
#define STEP(v, op) \
    corner op v; \
    corner op v; \
    { \
        float distance = plane.Project2(corner, point); \
        if (distance < contact.m_separation) { \
            contact.m_separation = distance; \
            contact.m_vector1 = point; \
        } \
    }
    STEP(a, +=)
    STEP(b, +=)
    STEP(a, -=)
    STEP(c, +=)
    STEP(b, -=)
    STEP(a, +=)
    STEP(b, +=)
    int result = Compare(contact.m_separation);
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}
