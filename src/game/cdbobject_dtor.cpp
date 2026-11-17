// A fragment of cdbobject.cpp (0x800712e0): the CCDBObject destructor, which is the
// class's key function, so this unit also owns the file's CCDBObject virtual table and
// RTTI records (.data, .sdata, .rodata, .sdata2), as the original file did. The rest
// of cdbobject.cpp stays in its other fragments until they can merge into one unit
// (a stepping stone, accepted 2026-11-13: docs/Remaining.md).
//
// The file name is this project's; the original record is cdbobject.cpp, and the
// destructor follows cdbobject_csg.cpp. IVolume and CCDBObject are named by the mangled
// symbols. IVolume's 17 virtuals are declared in the virtual table's slot order;
// the result types of the TestCollision overloads are inferred (bool, as in the other
// cdbobject fragments). IVolume's destructor is inline (weak in the original), so the
// compiler's copies of it and of IVolume's weak virtual table are weak duplicates,
// linked to the original copies. CCDBObject declares its destructor first, then the
// overrides in IVolume's order, then its own TestCollision(const CCSGVolume&) last.
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

class CCDBObject : public IVolume {
public:
    virtual ~CCDBObject();
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
    virtual void GetExtents(CVector3&, CVector3&) const;
    virtual bool TestCollision(const CCSGVolume&, CCollision&, bool) const;
};

CCDBObject::~CCDBObject() {
}
