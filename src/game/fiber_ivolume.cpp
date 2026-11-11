// A fragment of fiber.cpp: CVolFiber's double-dispatch collision tests
// (against CWorldVolume, CCSGVolume, CAnimatedVolume, CCDBObject,
// CVolCapsule, CVolSphere and CVolBox, which swap the order, set the line
// flag and let the other volume test this fiber, and against IVolume, which
// only swaps), then GetLine (the line at +16) and Set (the line's
// start and end through the inferred CLine3 setter). Swapped tests reverse
// the collision order and let the other volume test this one; fiber tests
// set the collision's line flag and test the fiber's line. The file name is
// this project's; the original record is fiber.cpp and the functions around
// these are not part of this unit. The classes come from the mangled
// symbols; IVolume's virtual functions are declared in the order of
// __vt__7IVolume and CVolFiber redeclares them with its destructor first
// (defined elsewhere, so its virtual table is not emitted here);
// CAnimatedVolume derives from CVolSphere (its tests call the sphere's), and
// the collision flag is an inferred bit-field view.
class CMatrix;
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles (the
// doubleword copies; inferred, as in player_vec_add.cpp).


#include "game/CVector3.h"

class CTriangle;
class CPlane;
// CLine3 view with the inferred start-and-end setter (as in player_vec_add.cpp).
class CLine3 {
public:
    void SetSE(CVector3 start, CVector3 end) {
        *(CVector3Data*)&m_start = *(const CVector3Data*)&start;
        *(CVector3Data*)&m_end = *(const CVector3Data*)&end;
        m_dir.x = m_end.x - m_start.x;
        m_dir.y = m_end.y - m_start.y;
        m_dir.z = m_end.z - m_start.z;
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
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

    unsigned char unknown00[16];
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
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

class CCSGVolume : public IVolume {};
class CWorldVolume : public IVolume {};
class CCDBObject : public IVolume {};
class CVolCapsule : public IVolume {};
class CVolSphere : public IVolume {};
class CAnimatedVolume : public CVolSphere {};
class CVolBox : public IVolume {};

class CVolFiber : public IVolume {
public:
    virtual ~CVolFiber();
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

    const CLine3& GetLine() const;
    void Set(CVector3, CVector3);

    unsigned char data004[12];
    CLine3 m_line;
};

bool CVolFiber::TestCollision(const CWorldVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

bool CVolFiber::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

bool CVolFiber::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

bool CVolFiber::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

bool CVolFiber::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

bool CVolFiber::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

bool CVolFiber::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

bool CVolFiber::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

const CLine3& CVolFiber::GetLine() const {
    return m_line;
}

void CVolFiber::Set(CVector3 start, CVector3 end) {
    m_line.SetSE(start, end);
}
