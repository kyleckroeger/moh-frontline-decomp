// A fragment of capsule.cpp (0x800c746c): the CVolCapsule destructor (its virtual table
// pointer, then IVolume's through the inline base destructor, and the object freed when
// asked). The destructor is the class's key function, so this unit also owns capsule.cpp's
// CVolCapsule virtual table and RTTI records, as the original file did, while the rest of
// the file stays in its other fragments (a stepping stone, accepted 2026-11-13:
// docs/Remaining.md). The file name is this project's; the original record is
// capsule.cpp, and the destructor follows capsule_create.cpp.
//
// IVolume and CVolCapsule are named by the mangled symbols; IVolume's 17 virtuals and
// CVolCapsule's overrides are declared in the virtual table's slot order (the destructor
// first, CVolCapsule's own TestCollision(const CCSGVolume&) last); the result types of the
// TestCollision overloads are inferred (bool). IVolume's destructor is inline and inlined
// here; IVolume's weak table is referenced, not compiled.
class CCollision;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CAnimatedVolume;
class CWorldVolume;
class CLine3;
class CPlane;
class CTriangle;
class CDrawContext;
class CMatrix;
class CCSGVolume;
class CCDBObject;

#include "game/CVector3.h"

class IVolume {
public:
    virtual ~IVolume() {}
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
};

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
};

CVolCapsule::~CVolCapsule() {
}
