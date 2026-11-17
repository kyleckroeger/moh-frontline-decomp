// IVolume: the base of every collision volume (boxes, spheres, capsules, fibers, CDB objects,
// world volumes), shared by every file that uses it (PortReadiness step 3.3).
//
// From the original: __vt__7IVolume (weak) holds the destructor, then the 16 slots declared below in
// this order. Every one of them is inline in the original: weak copies at 0x8001c078..0x8001c138,
// placed by src/game/volume_query_test.cpp, the matched unit these bodies follow. Each TestCollision
// overload reports no collision, Create returns no copy, TestVisibility says not visible, and
// TransformedCopy and GetExtents do nothing. IVolume has no data: its destructor stores only the
// vtable pointer.
//
// Inferred, not in any mangled name: the result types (bool for the tests and TestVisibility, a
// volume pointer for Create), as volume_query_test.cpp and the other matched views have them.
#ifndef GAME_IVOLUME_H
#define GAME_IVOLUME_H

#include "game/CVector3.h"

class CCollision;
class CMatrix;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CVolFiber;
class CAnimatedVolume;
class CWorldVolume;
class CLine3;
class CPlane;
class CTriangle;
class CDrawContext;

class IVolume {
public:
    virtual ~IVolume() {}
    virtual IVolume* Create() const { return 0; }
    virtual void TransformedCopy(const IVolume&, const CMatrix&) {}
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const { return false; }
    virtual bool TestCollision(const CVolBox&, CCollision&, bool) const { return false; }
    virtual bool TestCollision(const CVolSphere&, CCollision&, bool) const { return false; }
    virtual bool TestCollision(const CVolCapsule&, CCollision&, bool) const { return false; }
    virtual bool TestCollision(const CCDBObject&, CCollision&, bool) const { return false; }
    virtual bool TestCollision(const CVolFiber&, CCollision&, bool) const { return false; }
    virtual bool TestCollision(const CAnimatedVolume&, CCollision&, bool) const { return false; }
    virtual bool TestCollision(const CWorldVolume&, CCollision&, bool) const { return false; }
    virtual bool TestCollision(CVector3, CCollision&, bool) const { return false; }
    virtual bool TestCollision(const CLine3&, CCollision&, bool) const { return false; }
    virtual bool TestCollision(const CPlane&, CCollision&, bool) const { return false; }
    virtual bool TestCollision(const CTriangle&, CCollision&, bool) const { return false; }
    virtual bool TestVisibility(CDrawContext&) const { return false; }
    virtual void GetExtents(CVector3&, CVector3&) const {}
};

#endif
