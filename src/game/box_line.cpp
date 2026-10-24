// A fragment of box.cpp (0x800c1ee0): CVolBox::TestCollision against a
// line, a slab test over the box's three axes that also tracks the entry and
// exit face normals. An empty range, or one outside [0, 1], raises the result
// to 1 when asked and returns 1. Otherwise the time is the entry parameter
// (0 when the line starts inside, with the exit normal); a fiber line is a
// contact (2), else the rest of the line is compared with the squared contact
// epsilon (1/2/3). A contact (when asked) adds the face normal, the hit point
// and the remaining distance. The file name is this project's; the original
// record is box.cpp. The classes and functions are named by the mangled
// symbols; the members, the inline clip helper with by-value vectors, the
// squared-distance compare and the result codes are inferred. // The vector helpers use the x, y, z view of CVector3 (its copies move
// doubleword pairs) and fabs is the inline double function of the MSL math
// header (the SDK macro form gives other register numbers). 
// Constants are the file's .sdata2 pool entries. The rest of the file is in
// other units.
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

extern const float CONTACT_EPSILON_SQRD;

inline int CompareSqrd(float distSq) {
    if (-distSq > CONTACT_EPSILON_SQRD)
        return 1;
    if (distSq > CONTACT_EPSILON_SQRD)
        return 3;
    return 2;
}

inline bool Clip(CVector3 axis, CVector3 offset, CVector3 dir, float half, float& tNear, float& tFar, CVector3& nearNormal,
                 CVector3& farNormal) {
    float d; float e; float t1; float inv; float t2;
    d = dir.Dot(axis);
    e = offset.Dot(axis);
    if ((float)fabs(d) > 1e-06f) {
        inv = 1.0f / d;
        t2 = inv * (-half - e);
        t1 = inv * (half - e);
        if (t2 < t1) {
            if (t2 > tNear) {
                tNear = t2;
                nearNormal.Negate(axis);
            }
            if (t1 < tFar) {
                tFar = t1;
                farNormal = axis;
            }
        } else {
            if (t1 > tNear) {
                tNear = t1;
                nearNormal = axis;
            }
            if (t2 < tFar) {
                tFar = t2;
                farNormal.Negate(axis);
            }
        }
    } else if ((float)fabs(e) > half) {
        return false;
    }
    return tNear <= tFar && tFar >= 0.0f;
}

class IVolume {
public:
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    int TestCollision(const CLine3&, CCollision&, bool) const;
    CVector3 GetAxis(int i) const { return m_axes[i]; }

    float m_half[3];
    CVector3 m_axes[3];
    CVector3 m_center;
};

int CVolBox::TestCollision(const CLine3& line, CCollision& collision, bool flag) const {
    CVector3 nearNormal;
    CVector3 farNormal;
    CVector3 offset;
    CVector3 dir;
    CVector3 normal;
    CVector3 point;
    const CVector3& start = line.GetStart();
    offset.d.c.x = start.d.c.x - m_center.d.c.x;
    offset.d.c.y = start.d.c.y - m_center.d.c.y;
    offset.d.c.z = start.d.c.z - m_center.d.c.z;
    dir = line.GetDirection();
    const CVector3* rdir = &dir;
    const CVector3* roff = &offset;
    float tNear = -3.4028235e+38f;
    float tFar = 3.4028235e+38f;
    if (!Clip(GetAxis(0), *roff, *rdir, m_half[0], tNear, tFar, nearNormal, farNormal)) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    if (!Clip(GetAxis(1), *roff, *rdir, m_half[1], tNear, tFar, nearNormal, farNormal)) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    if (!Clip(GetAxis(2), *roff, *rdir, m_half[2], tNear, tFar, nearNormal, farNormal)) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    if (tNear > 1.0f || tFar < 0.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float t;
    if (tNear >= 0.0f) {
        normal = nearNormal;
        t = tNear;
    } else {
        normal = farNormal;
        t = 0.0f;
    }
    collision.m_time = t;
    float distSq;
    int result;
    if (collision.m_line) {
        distSq = 0.0f;
        result = 2;
    } else {
        CVector3 rest;
        float s = 1.0f - t;
        rest.Scale(dir, s);
        distSq = rest.LengthSq();
        result = CompareSqrd(distSq);
    }
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            point.Scale(dir, t);
            point += line.GetStart();
            SClsnContact contact;
            contact.m_separation = sqrtf(distSq);
            contact.m_vector1 = point;
            contact.m_vector0 = normal;
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}
