// CVolCapsule: a collision volume, shared by every file that uses it (PortReadiness step 3.3).
//
// From the original: every member is out of line (capsule.cpp): the destructor (the key function) and the
// 17 other slots of __vt__11CVolCapsule, in the table's order (IVolume's, then CCSGVolume's
// TestCollision). 0x50 bytes: the table pointer, then the radius at +0xc and the axis (a CLine3) at +0x10, as the matched views agree.
//
// Inferred, not in any mangled name: the member names and the result types (IVolume.h's). CVector3
// must be complete before this header.
#ifndef GAME_CVOLCAPSULE_H
#define GAME_CVOLCAPSULE_H

#include "game/IVolume.h"
#include "game/CLine3.h"

class CCSGVolume;

class CVolCapsule : public IVolume {
public:
    virtual ~CVolCapsule();
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

    unsigned char unknown04[8];
    float m_radius;
    CLine3 m_line;
};

#endif
