// A fragment of cdbgeom.cpp (0x8006f3d4): CCDBVolBox's collision test against
// fibers, which sets the collision's line flag and tests the fiber's line
// (virtual, in the order of __vt__7IVolume). The plane test after it is not
// reconstructed. The classes come from the mangled symbols; CCDBVolBox
// redeclares IVolume's virtual functions with its destructor first (defined
// elsewhere, so its virtual table is not emitted here) and the collision flag
// is an inferred bit-field view.
class CMatrix;
class CVector3;
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

    unsigned char unknown00[16];
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
};

#include "game/IVolume.h"


class CCSGVolume : public IVolume {};

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();
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

class CVolCapsule : public IVolume {
public:
};

class CVolFiber {
public:
    const CLine3& GetLine() const;
};

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();
    bool TestCollision(const CVolCapsule&, CCollision&, bool) const;
};

class CCDBVolBox : public CVolBox {
public:
    virtual ~CCDBVolBox();
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
    bool TestCollision(const CCSGVolume&, CCollision&, bool) const;
};

class CAnimatedVolume : public CVolSphere {
public:
};

bool CCDBVolBox::TestCollision(const CVolFiber& fiber, CCollision& collision, bool flag) const {
    collision.m_line = 1;
    return TestCollision(fiber.GetLine(), collision, flag);
}
