// A fragment of capsule.cpp (0x800c6ed8): CVolCapsule's sphere test (as the
// point test, with the sphere's centre as the point and its radius taken off
// the separation too), then its double-dispatch collision tests against
// CVolBox and IVolume, which reverse the collision order and let the other
// volume test this one. The file name is this project's; the original record
// is capsule.cpp and the functions around these are not part of this unit.
// The classes come from the mangled symbols; IVolume's virtual functions are
// declared in the order of __vt__7IVolume and CVolCapsule redeclares them
// with its destructor first (defined elsewhere, so its virtual table is not
// emitted here). The capsule members, the line's getters, the vector helpers,
// the collision result at +20, the collision flag, the contact record and the
// int-sized results are inferred views, as in capsule_dispatch.cpp; the
// sphere's centre is read through an inferred inline helper.
#include <math.h>

class CMatrix;
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
class CPlane;
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
class CVolBox : public IVolume {};

class CVolSphere : public IVolume {
public:
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
    float GetRadius() const;
    CVector3 GetCenter() const;
};

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

    unsigned char unknown04[8];
    float m_radius;
    CLine3 m_axis;
};

class CAnimatedVolume : public CVolSphere {
public:
};

/* inferred: the sphere centre returned through an inline, whose returned
   copy is the dead copy at the bottom of the original frame */
inline CVector3 SphereCenter(const CVolSphere& s) { return s.GetCenter(); }

int CVolCapsule::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    float t;
    CVector3 nearest;
    CVector3 point = SphereCenter(other);
    m_axis.GetProjection(point, nearest, t);
    if (t < 0.0f) {
        t = 0.0f;
        nearest = m_axis.GetStart();
    } else if (t > 1.0f) {
        t = 1.0f;
        nearest = m_axis.GetEnd();
    }
    CVector3 direction;
    direction.d.v[0] = point.d.v[0] - nearest.d.v[0];
    direction.d.v[1] = point.d.v[1] - nearest.d.v[1];
    direction.d.v[2] = point.d.v[2] - nearest.d.v[2];
    float lengthSq = direction.LengthSq();
    float separation = sqrtf(lengthSq) - m_radius - other.GetRadius();
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
            direction.Normalize();
            SClsnContact contact;
            contact.m_separation = separation;
            float radius = m_radius;
            float x = radius * direction.d.v[0];
            float y = radius * direction.d.v[1];
            float z = radius * direction.d.v[2];
            contact.m_vector1.d.v[0] = nearest.d.v[0] + x;
            contact.m_vector1.d.v[1] = nearest.d.v[1] + y;
            contact.m_vector1.d.v[2] = nearest.d.v[2] + z;
            contact.m_vector0 = direction;
            contact.m_index = 0;
            contact.m_object = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}

int CVolCapsule::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolCapsule::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}
