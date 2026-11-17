// CVolFiber: a collision volume, shared by every file that uses it (PortReadiness step 3.3).
//
// From the original: every member is out of line (fiber.cpp): the destructor (the key function) and the
// 17 other slots of __vt__9CVolFiber, in the table's order (IVolume's, then CCSGVolume's
// TestCollision). 0x50 bytes: the table pointer, then 12 unknown bytes and the fiber (a CLine3) at +0x10, as the matched views agree.
//
// Inferred, not in any mangled name: the member names and the result types (IVolume.h's). CVector3
// must be complete before this header.
#ifndef GAME_CVOLFIBER_H
#define GAME_CVOLFIBER_H

#include "game/IVolume.h"
#include "game/CLine3.h"

class CCSGVolume;

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
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
    virtual bool TestCollision(const CCSGVolume&, CCollision&, bool) const;
    const CLine3& GetLine() const;

    unsigned char unknown04[12];
    CLine3 m_line;
};

#endif
