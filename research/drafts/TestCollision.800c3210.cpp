// real2 (2026-11-12): 13 real lines, unchanged: diff.Sub, normal.Add, contact.m_vector1.Add and abs.Abs(local) (the original's CVector3 Sub/Add/Abs members) replace per-component code and the += operator.
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
    CVector3(float x, float y, float z) {
        d.c.x = x;
        d.c.y = y;
        d.c.z = z;
    }
    void Normalize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            d.c.x *= 1.0f / length;
            d.c.y *= 1.0f / length;
            d.c.z *= 1.0f / length;
        }
    }
    void Scale(const CVector3& v, float k) {
        d.c.x = k * v.d.c.x;
        d.c.y = k * v.d.c.y;
        d.c.z = k * v.d.c.z;
    }
    void Sub(const CVector3& a, const CVector3& b) { d.c.x = a.d.c.x - b.d.c.x; d.c.y = a.d.c.y - b.d.c.y; d.c.z = a.d.c.z - b.d.c.z; }
    void Add(const CVector3& o) { d.c.x += o.d.c.x; d.c.y += o.d.c.y; d.c.z += o.d.c.z; }
    void Abs(const CVector3& o) { d.c.x = (float)fabs(o.d.c.x); d.c.y = (float)fabs(o.d.c.y); d.c.z = (float)fabs(o.d.c.z); }
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

extern "C" unsigned long OSGetTick();
extern const float CONTACT_EPSILON;

inline float Sign(float x) {
    float r;
    *(int*)&r = *(int*)&x;
    if (*(int*)&x & 0x7f800000) {
        *(int*)&r = *(int*)&x & 0x80000000;
        *(int*)&r |= 0x3f800000;
    } else {
        *(int*)&r = 0;
    }
    x = r;
    return x;
}

class IVolume {
public:
    virtual ~IVolume();
};

class CVolSphere : public IVolume {
public:
    CVector3 GetCenter() const;
    float GetRadius() const;
};

class CVolBox : public IVolume {
public:
    int TestCollision(const CVolSphere&, CCollision&, bool) const;
    void Rotate(CVector3& r, const CVector3& v) const {
        r.d.c.x = m_axes[0].d.c.x * v.d.c.x + m_axes[1].d.c.x * v.d.c.y + m_axes[2].d.c.x * v.d.c.z;
        r.d.c.y = m_axes[0].d.c.y * v.d.c.x + m_axes[1].d.c.y * v.d.c.y + m_axes[2].d.c.y * v.d.c.z;
        r.d.c.z = m_axes[0].d.c.z * v.d.c.x + m_axes[1].d.c.z * v.d.c.y + m_axes[2].d.c.z * v.d.c.z;
    }

    float m_half[3];
    CVector3 m_axes[3];
    CVector3 m_center;
};

int CVolBox::TestCollision(const CVolSphere& sphere, CCollision& collision, bool flag) const {
    OSGetTick();
    const CVector3& center = sphere.GetCenter();
    CVector3 diff;
    diff.Sub(center, m_center);
    CVector3 local;
    local.d.c.z = m_axes[2].Dot(diff);
    local.d.c.y = m_axes[1].Dot(diff);
    local.d.c.x = m_axes[0].Dot(diff);
    CVector3 normal;
    bool outside;
    float maxDist;
    CVector3 closest;
    float distSq;
    CVector3 abs;
    abs.Abs(local);
    outside = false;
    maxDist = -3.4028235e+38f;
    distSq = 0.0f;
    if (flag) {
        closest = local;
        normal.d.c.z = 0.0f;
        normal.d.c.y = 0.0f;
        normal.d.c.x = 0.0f;
        float d = abs.d.c.x - m_half[0];
        if (d > 0.0f) {
            distSq += d * d;
            outside = true;
            float s = Sign(local.d.c.x);
            normal.Add(CVector3(s * d, 0.0f, 0.0f));
            closest.d.c.x = s * m_half[0];
        } else {
            maxDist = d;
        }
        d = abs.d.c.y - m_half[1];
        if (d > 0.0f) {
            distSq += d * d;
            outside = true;
            float s = Sign(local.d.c.y);
            normal.Add(CVector3(0.0f, s * d, 0.0f));
            closest.d.c.y = s * m_half[1];
        } else if (d > maxDist) {
            maxDist = d;
        }
        d = abs.d.c.z - m_half[2];
        if (d > 0.0f) {
            distSq += d * d;
            outside = true;
            float s = Sign(local.d.c.z);
            normal.Add(CVector3(0.0f, 0.0f, s * d));
            closest.d.c.z = s * m_half[2];
        } else if (d > maxDist) {
            maxDist = d;
        }
    } else {
        float d = abs.d.c.x - m_half[0];
        if (d > 0.0f) {
            distSq += d * d;
            outside = true;
        } else {
            maxDist = d;
        }
        d = abs.d.c.y - m_half[1];
        if (d > 0.0f) {
            distSq += d * d;
            outside = true;
        } else if (d > maxDist) {
            maxDist = d;
        }
        d = abs.d.c.z - m_half[2];
        if (d > 0.0f) {
            distSq += d * d;
            outside = true;
        } else if (d > maxDist) {
            maxDist = d;
        }
    }
    float separation;
    if (outside)
        separation = sqrtf(distSq) - sphere.GetRadius();
    else
        separation = maxDist - sphere.GetRadius();
    int result;
    if (separation > CONTACT_EPSILON)
        result = 1;
    else if (separation < -CONTACT_EPSILON)
        result = 3;
    else
        result = 2;
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            SClsnContact contact;
            contact.m_separation = separation;
            float zx = m_axes[2].d.c.x;
            float yx = m_axes[1].d.c.x;
            float xx = m_axes[0].d.c.x;
            float zy = m_axes[2].d.c.y;
            float yy = m_axes[1].d.c.y;
            float xy = m_axes[0].d.c.y;
            float zz = m_axes[2].d.c.z;
            float yz = m_axes[1].d.c.z;
            float xz = m_axes[0].d.c.z;
            contact.m_vector0.d.c.x = xx * normal.d.c.x + yx * normal.d.c.y + zx * normal.d.c.z;
            contact.m_vector0.d.c.y = xy * normal.d.c.x + yy * normal.d.c.y + zy * normal.d.c.z;
            contact.m_vector0.d.c.z = xz * normal.d.c.x + yz * normal.d.c.y + zz * normal.d.c.z;
            contact.m_vector0.Normalize();
            contact.m_vector1.d.c.z = xz * closest.d.c.x + yz * closest.d.c.y + zz * closest.d.c.z;
            contact.m_vector1.d.c.x = xx * closest.d.c.x + yx * closest.d.c.y + zx * closest.d.c.z;
            contact.m_vector1.d.c.y = xy * closest.d.c.x + yy * closest.d.c.y + zy * closest.d.c.z;
            contact.m_vector1.Add(m_center);
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    OSGetTick();
    return result;
}
