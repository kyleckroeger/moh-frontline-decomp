// A fragment of sphere.cpp (0x800cb31c): CVolSphere's point test (as the
// sphere-sphere test below, with the point as a sphere of radius 0), then its
// double-dispatch collision tests. Against CDB objects, capsules, world volumes and CSG
// volumes the collision order is swapped and the other volume tests against
// the sphere; against a fiber the collision's line flag is set and the sphere
// tests the fiber's line; an animated volume is tested as a sphere. The
// sphere-sphere test follows: the direction between the centres is
// normalised (the SDK's inline sqrtf), the separation is the distance less
// both radii, and the result is 1 beyond CONTACT_EPSILON, 3 within
// -CONTACT_EPSILON and 2 between, when a contact (the direction, the point on
// this sphere's surface, the separation, index 0) is added. The file name is
// this project's; the original record is sphere.cpp (sphere.cpp holds the
// box and generic tests onward). The classes come from the mangled symbols; IVolume's
// virtual functions are declared in the order of __vt__7IVolume, CVolSphere
// redeclares them (its destructor first, defined elsewhere, so its virtual
// table is not emitted here), CAnimatedVolume is taken to derive from
// CVolSphere (the call it makes implies it), and the collision flag is an
// inferred bit-field view. The sphere members (radius at +12, centre at +16),
// the collision result at +20, the contact record and the int-sized results
// are inferred.
#include <math.h>

class CMatrix;

// Inferred: CVector3 as four floats overlaid with two doubles (its copies
// move doubleword pairs).


#include "game/CVector3.h"

// Inferred contact record view: two vectors, the separation and an index.
struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
} __attribute__((aligned(16)));

extern const float CONTACT_EPSILON;
class CTriangle;
class CPlane;
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

class CCDBObject : public IVolume {};
class CVolCapsule : public IVolume {};
class CWorldVolume : public IVolume {};
class CCSGVolume : public IVolume {};

class CVolFiber : public IVolume {
public:
    const CLine3& GetLine() const;
};

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
    int TestCollision(const CCSGVolume&, CCollision&, bool) const;

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
};

class CAnimatedVolume : public CVolSphere {};

int CVolSphere::TestCollision(CVector3 point, CCollision& collision, bool flag) const {
    CVector3 direction;
    direction.x = point.x - m_center.x;
    direction.y = point.y - m_center.y;
    direction.z = point.z - m_center.z;
    float distance = sqrtf(direction.x * direction.x + direction.y * direction.y +
                           direction.z * direction.z);
    if (distance != 0.0f) {
        direction.x *= 1.0f / distance;
        direction.y *= 1.0f / distance;
        direction.z *= 1.0f / distance;
    }
    float separation = distance - m_radius;
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
            float radius = m_radius;
            contact.m_vector0 = direction;
            direction.x *= radius;
            direction.y *= radius;
            direction.z *= radius;
            contact.m_vector1.x = m_center.x + direction.x;
            contact.m_vector1.y = m_center.y + direction.y;
            contact.m_vector1.z = m_center.z + direction.z;
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}

int CVolSphere::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    collision.m_line = 1;
    return TestCollision(other.GetLine(), collision, flag);
}

int CVolSphere::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CWorldVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    return TestCollision((const CVolSphere&)other, collision, flag);
}

int CVolSphere::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    CVector3 direction;
    direction.x = other.m_center.x - m_center.x;
    direction.y = other.m_center.y - m_center.y;
    direction.z = other.m_center.z - m_center.z;
    float distance = sqrtf(direction.x * direction.x + direction.y * direction.y +
                           direction.z * direction.z);
    if (distance != 0.0f) {
        direction.x *= 1.0f / distance;
        direction.y *= 1.0f / distance;
        direction.z *= 1.0f / distance;
    }
    float separation = distance - (m_radius + other.m_radius);
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
            float radius = m_radius;
            contact.m_vector0 = direction;
            direction.x *= radius;
            direction.y *= radius;
            direction.z *= radius;
            contact.m_vector1.x = m_center.x + direction.x;
            contact.m_vector1.y = m_center.y + direction.y;
            contact.m_vector1.z = m_center.z + direction.z;
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}
