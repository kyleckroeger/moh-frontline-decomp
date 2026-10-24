// A fragment of cdbgeom.cpp (0x8006f44c): CCDBVolBox's collision tests
// against planes (the plane distance of the centre less the box's projected
// half extents: 1 beyond the contact epsilon, 3 behind it, else 2), capsules
// (CVolBox's test, called directly), CSG volumes (no collision), animated
// volumes (tested as spheres) and spheres (the squared distance from the
// sphere's centre, in box coordinates, to the box, against the radius: same
// 1/3/2 codes). The box-space vectors are built by the three-float
// constructor (its arguments are evaluated right to left, z first). The file
// name is this project's; the original record is cdbgeom.cpp. The classes come
// from the mangled symbols; CCDBVolBox derives from CVolBox (its capsule test
// calls CVolBox's), IVolume's virtual functions are declared in the order of
// __vt__7IVolume and CCDBVolBox redeclares them with its destructor first
// (defined elsewhere, so its virtual table is not emitted here), and
// CAnimatedVolume derives from CVolSphere. The vector view (x, y, z over two
// doubles), the plane view and the result codes are inferred; fabs is the
// inline double function of the MSL math header. Constants are the file's
// .sdata2 pool entries.
#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }

class CMatrix;

union CVector3Data {
    double pair[2];
    float v[4];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) { d.c.x = x; d.c.y = y; d.c.z = z; }
    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }
    CVector3Data d;
} __attribute__((aligned(8)));
class CTriangle;
// Inferred plane view: the normal and the distance from the origin.
class CPlane {
public:
    float GetDistance(CVector3 point) const {
        return m_normal.d.c.x * point.d.c.x + m_normal.d.c.y * point.d.c.y + m_normal.d.c.z * point.d.c.z - m_distance;
    }

    CVector3 m_normal;
    float m_distance;
};
class CLine3;
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

    unsigned char unknown00[16];
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
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
    CVector3 GetCenter() const;
    float GetRadius() const;
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
};

class CVolCapsule : public IVolume {
public:
};

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();
    int TestCollision(const CVolCapsule&, CCollision&, bool) const;
};

extern const float CONTACT_EPSILON;

class CCDBVolBox : public CVolBox {
public:
    virtual ~CCDBVolBox();
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

    float m_half[3];
    CVector3 m_axes[3];
    CVector3 m_center;
};

class CAnimatedVolume : public CVolSphere {
public:
};

int CCDBVolBox::TestCollision(const CPlane& plane, CCollision&, bool) const {
    CVector3 proj(m_axes[0].Dot(plane.m_normal), m_axes[1].Dot(plane.m_normal), m_axes[2].Dot(plane.m_normal));
    float width = (float)fabs(proj.d.c.x);
    float depth = (float)fabs(proj.d.c.y);
    float height = (float)fabs(proj.d.c.z);
    float separation = plane.GetDistance(m_center) - (width * m_half[0] + depth * m_half[1] + height * m_half[2]);
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

int CCDBVolBox::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(other, collision, flag);
}

int CCDBVolBox::TestCollision(const CCSGVolume&, CCollision&, bool) const {
    return false;
}

int CCDBVolBox::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    return TestCollision((const CVolSphere&)other, collision, flag);
}

int CCDBVolBox::TestCollision(const CVolSphere& sphere, CCollision&, bool) const {
    const CVector3& center = sphere.GetCenter();
    CVector3 diff;
    diff.d.c.x = center.d.c.x - m_center.d.c.x;
    diff.d.c.y = center.d.c.y - m_center.d.c.y;
    diff.d.c.z = center.d.c.z - m_center.d.c.z;
    CVector3 local(m_axes[0].Dot(diff), m_axes[1].Dot(diff), m_axes[2].Dot(diff));
    float a0 = (float)fabs(local.d.c.x);
    float a1 = (float)fabs(local.d.c.y);
    float a2 = (float)fabs(local.d.c.z);
    float distSq;
    float separation;
    distSq = 0.0f;
    if (a0 > m_half[0]) {
        float d = a0 - m_half[0];
        distSq += d * d;
    }
    if (a1 > m_half[1]) {
        float d = a1 - m_half[1];
        distSq += d * d;
    }
    if (a2 > m_half[2]) {
        float d = a2 - m_half[2];
        distSq += d * d;
    }
    separation = sqrtf(distSq) - sphere.GetRadius();
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}
