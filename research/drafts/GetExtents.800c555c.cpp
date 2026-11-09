// A fragment of capsule.cpp (0x800c568c): CVolCapsule's TestVisibility
// (always visible) and triangle test (no collision), then its plane test.
// When both axis end points lie on the same side of the plane beyond the
// radius, the plane is tested against a sphere of the capsule's radius at the
// end point nearer to it; otherwise the capsule straddles the plane: the
// collision's result is raised to 3 when asked, and 3 is returned. The file
// name is this project's; the original record is capsule.cpp. The classes and
// functions are named by the mangled symbols; the members, the plane distance
// helper and the result codes are inferred (as in capsule_dispatch.cpp).
#include <math.h>

class CMatrix;
class CDrawContext;
class IVolume;

// Inferred: CVector3 as four floats overlaid with two doubles (its copies
// move doubleword pairs).
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    float LengthSq() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }
    void Normalize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }

    CVector3Data d;
} __attribute__((aligned(8)));

// Inferred: the line's start and end (as in fiber_ivolume.cpp) with the
// projection of a point.
class CLine3 {
public:
    void GetProjection(CVector3, CVector3&, float&) const;
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }

    CVector3 m_start;
    CVector3 m_end;
};

// Inferred contact record view: two vectors, the separation, an index and
// an object.
struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
    const IVolume* m_object;
} __attribute__((aligned(16)));

extern const float CONTACT_EPSILON;
class CTriangle;

class CPlane {
public:
    float Dist(const CVector3& v) const {
        return v.d.v[0] * m_normal.d.v[0] + v.d.v[1] * m_normal.d.v[1] + v.d.v[2] * m_normal.d.v[2] - m_d;
    }

    CVector3 m_normal;
    float m_d;
};
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

class CCollision {
public:
    void SwapOrder();
    void AddContact(const SClsnContact&);

    unsigned char unknown00[16];
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
};

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
};

class CCSGVolume : public IVolume {};

class CVolSphere : public IVolume {
public:
    CVolSphere(const CVector3& center, float radius) : m_radius(radius), m_center(center) {}
    virtual ~CVolSphere();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
} __attribute__((aligned(16)));

class CCDBObject : public IVolume {
public:
};

class CVolFiber : public IVolume {
public:
    const CLine3& GetLine() const;
};

class CWorldVolume : public IVolume {
public:
};

class CVolCapsule : public IVolume {
public:
    virtual ~CVolCapsule();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
    int TestCollision(const CCSGVolume&, CCollision&, bool) const;
    bool TestVisibility(CDrawContext&) const;
    void GetExtents(CVector3&, CVector3&) const;

    unsigned char unknown04[8];
    float m_radius;
    CLine3 m_axis;
};

class CAnimatedVolume : public CVolSphere {
public:
};

static inline void MinOf(float& a, float b) {
    if (b < a) a = b;
}

static inline void MaxOf(float& a, float b) {
    if (b > a) a = b;
}

void CVolCapsule::GetExtents(CVector3& min, CVector3& max) const {
    float radius = m_radius + CONTACT_EPSILON;
    {
        CVector3 a = m_axis.GetStart();
        CVector3 b = m_axis.GetEnd();
        float ax = a.d.v[0] - radius;
        float ay = a.d.v[1] - radius;
        float az = a.d.v[2] - radius;
        float bx = b.d.v[0] - radius;
        float by = b.d.v[1] - radius;
        float bz = b.d.v[2] - radius;
        if (ax < b.d.v[0] - radius) ax = a.d.v[0] - radius; else ax = bx;
        min.d.v[0] = ax;
        if (ay < by) ay = a.d.v[1] - radius; else ay = by;
        min.d.v[1] = ay;
        if (az < bz) az = a.d.v[2] - radius; else az = bz;
        min.d.v[2] = az;
    }
    {
        CVector3 a = m_axis.GetStart();
        CVector3 b = m_axis.GetEnd();
        float ax = a.d.v[0] + radius;
        float ay = a.d.v[1] + radius;
        float az = a.d.v[2] + radius;
        float bx = b.d.v[0] + radius;
        float by = b.d.v[1] + radius;
        float bz = b.d.v[2] + radius;
        if (ax > bx) ax = a.d.v[0] + radius; else ax = bx;
        max.d.v[0] = ax;
        if (ay > by) ay = a.d.v[1] + radius; else ay = by;
        max.d.v[1] = ay;
        if (az > bz) az = a.d.v[2] + radius; else az = bz;
        max.d.v[2] = az;
    }
}

bool CVolCapsule::TestVisibility(CDrawContext&) const {
    return true;
}

int CVolCapsule::TestCollision(const CTriangle&, CCollision&, bool) const {
    return 0;
}

int CVolCapsule::TestCollision(const CPlane& plane, CCollision& collision, bool flag) const {
    CVector3 start = m_axis.m_start;
    CVector3 end = m_axis.m_end;
    float startDist = plane.Dist(start);
    float endDist = plane.Dist(end);
    if (startDist * endDist > m_radius * m_radius) {
        float absEnd = fabs(endDist);
        float absStart = fabs(startDist);
        if (absStart <= absEnd) {
            CVolSphere sphere(start, m_radius);
            return sphere.TestCollision(plane, collision, flag);
        }
        CVolSphere sphere(end, m_radius);
        return sphere.TestCollision(plane, collision, flag);
    }
    if (flag && collision.m_result < 3)
        collision.m_result = 3;
    return 3;
}
