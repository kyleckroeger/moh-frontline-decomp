// CVolSphere: a sphere collision volume, shared by every file that uses it (PortReadiness step 3.3).
//
// From the original: every member is out of line (sphere.cpp): the destructor (the key function),
// Create, TransformedCopy, the 14 TestCollision overrides (IVolume's 13, then CCSGVolume's, which
// IVolume doesn't declare), TestVisibility, GetExtents, GetCenter, GetRadius, SetCenter, SetRadius
// and operator=. 0x20 bytes: the table pointer, 8 unknown bytes, the radius at +0xc and the centre
// (a CVector3, 16-byte aligned) at +0x10, as the matched views agree.
//
// Inferred, not in any mangled name: the member names and the result types (IVolume.h's). CVector3
// must be complete before this header.
#ifndef GAME_CVOLSPHERE_H
#define GAME_CVOLSPHERE_H

#include "game/IVolume.h"

class CCSGVolume;

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
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
    virtual bool TestCollision(const CCSGVolume&, CCollision&, bool) const;

    CVolSphere& operator=(const CVolSphere&);
    CVector3 GetCenter() const;
    float GetRadius() const;
    void SetCenter(CVector3);
    void SetRadius(float);

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
};

#endif
