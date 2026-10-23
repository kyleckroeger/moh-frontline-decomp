// A fragment of capsule.cpp (0x800c568c): CVolCapsule's TestVisibility
// (always visible) and triangle test (no collision), then its plane test.
// When both axis end points lie on the same side of the plane beyond the
// radius, the plane is tested against a sphere of the capsule's radius at the
// end point nearer to it; otherwise the capsule straddles the plane: the
// collision's result is raised to 3 when asked, and 3 is returned. Then the
// line test: a degenerate axis tests the line against a sphere at the axis
// start; otherwise the file-local FindIntersection (defined after this
// fragment, declared non-static here so it links to the original) gives up to
// two line parameters, the nearest one in [0, 1] (none: -1) is the hit time.
// A miss raises the result to 1. A hit stores the time and the point on the
// line; a fiber line (the collision's line flag) is a contact (2), otherwise
// the hit point is compared with the line's end (2 within the squared
// contact epsilon, else 3). A contact adds the direction from the nearest axis
// point, the hit point and the negated distance to the end. The file name is
// this project's; the original record is capsule.cpp. The classes and
// functions are named by the mangled symbols; the members, the plane distance
// helper, the vector helpers and copies (one double at a time), the
// squared-distance compare and the result codes are inferred (as in
// capsule_dispatch.cpp).
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
    CVector3() {}
    CVector3(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
    }
    void Scale(const CVector3& v, float k) {
        d.v[0] = k * v.d.v[0];
        d.v[1] = k * v.d.v[1];
        d.v[2] = k * v.d.v[2];
    }
    CVector3& operator+=(const CVector3& o) {
        d.v[0] += o.d.v[0];
        d.v[1] += o.d.v[1];
        d.v[2] += o.d.v[2];
        return *this;
    }
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
    bool IsDegenerate() const;
    void GetProjection(CVector3, CVector3&, float&) const;
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 GetDirection() const { return m_dir; }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
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
extern const float CONTACT_EPSILON_SQRD;
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
    CVolSphere(const CVector3& center, float radius) {
        m_radius = radius;
        m_center = center;
    }
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

    unsigned char unknown04[8];
    float m_radius;
    CLine3 m_axis;
};

class CAnimatedVolume : public CVolSphere {
public:
};

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

int FindIntersection(const CLine3& line, const CVolCapsule& capsule, float* afT);

inline int CompareSqrd(float distSq) {
    if (-distSq > CONTACT_EPSILON_SQRD)
        return 1;
    if (distSq > CONTACT_EPSILON_SQRD)
        return 3;
    return 2;
}

int CVolCapsule::TestCollision(const CLine3& line, CCollision& collision, bool flag) const {
    CVector3 point;
    CVector3 delta;
    CVector3 nearest;
    if (m_axis.IsDegenerate()) {
        CVolSphere sphere(m_axis.GetStart(), m_radius);
        return sphere.TestCollision(line, collision, flag);
    }
    float afT[2];
    float t;
    switch (FindIntersection(line, *this, afT)) {
    case 0:
        t = -1.0f;
        break;
    case 1:
        t = afT[0];
        break;
    case 2:
        if (afT[0] < afT[1] && afT[0] >= 0.0f)
            t = afT[0];
        else
            t = afT[1];
        break;
    }
    if (t < 0.0f || t > 1.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    collision.m_time = t;
    point.Scale(line.GetDirection(), t);
    point += line.GetStart();
    int result;
    if (collision.m_line) {
        t = 0.0f;
        result = 2;
    } else {
        const CVector3& end = line.GetEnd();
        delta.d.v[0] = point.d.v[0] - end.d.v[0];
        delta.d.v[1] = point.d.v[1] - end.d.v[1];
        delta.d.v[2] = point.d.v[2] - end.d.v[2];
        t = delta.LengthSq();
        result = CompareSqrd(t);
        if (result != 2)
            result = 3;
    }
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            float s;
            m_axis.GetProjection(point, nearest, s);
            if (s < 0.0f)
                nearest = m_axis.GetStart();
            else if (s > 1.0f)
                nearest = m_axis.GetEnd();
            delta.d.v[0] = point.d.v[0] - nearest.d.v[0];
            delta.d.v[1] = point.d.v[1] - nearest.d.v[1];
            delta.d.v[2] = point.d.v[2] - nearest.d.v[2];
            delta.Normalize();
            SClsnContact contact;
            contact.m_separation = -sqrtf(t);
            contact.m_vector0 = delta;
            contact.m_vector1 = point;
            contact.m_index = 0;
            contact.m_object = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}
