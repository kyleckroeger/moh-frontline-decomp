// CAnimatedVolume collision tests: against triangles, planes, points,
// capsules, spheres and boxes it tests as its CVolSphere base; against CDB
// objects and generic volumes it swaps the collision's order and lets the
// other volume test against it. The names come from the mangled symbols.
// IVolume's virtual functions are declared in the order of __vt__7IVolume,
// CVolSphere's tests are a non-virtual view of the base, and the result type
// (bool) is inferred. The fibre test before this run and the rest of the file
// are not part of this unit.
class CMatrix;
class CDrawContext;
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

#include "game/CVector3.h"

class CCollision {
public:
    void SwapOrder();
};

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual bool TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual bool TestCollision(CVector3, CCollision&, bool) const;
    virtual bool TestCollision(const CLine3&, CCollision&, bool) const;
    virtual bool TestCollision(const CPlane&, CCollision&, bool) const;
    virtual bool TestCollision(const CTriangle&, CCollision&, bool) const;
};

class CCDBObject : public IVolume {
};

class CVolSphere {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
    bool TestCollision(const CPlane&, CCollision&, bool) const;
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CVolCapsule&, CCollision&, bool) const;
    bool TestCollision(const CVolSphere&, CCollision&, bool) const;
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
};

class CAnimatedVolume : public CVolSphere {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
    bool TestCollision(const CPlane&, CCollision&, bool) const;
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    bool TestCollision(const CVolCapsule&, CCollision&, bool) const;
    bool TestCollision(const CVolSphere&, CCollision&, bool) const;
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
    bool TestCollision(const IVolume&, CCollision&, bool) const;
};

bool CAnimatedVolume::TestCollision(const CTriangle& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(other, collision, flag);
}

bool CAnimatedVolume::TestCollision(const CPlane& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(other, collision, flag);
}

bool CAnimatedVolume::TestCollision(CVector3 point, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(point, collision, flag);
}

bool CAnimatedVolume::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

bool CAnimatedVolume::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(other, collision, flag);
}

bool CAnimatedVolume::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(other, collision, flag);
}

bool CAnimatedVolume::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision(other, collision, flag);
}

bool CAnimatedVolume::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}
