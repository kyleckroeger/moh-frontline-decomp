// CCDBVolBox's weak destructor (0x8006a620: its table pointer, then CVolBox's
// destructor, called; the object freed when asked) and collision tests
// emitted in cdbgeom.cpp: no collision with world volumes, and the point, CDB-object, box and generic-volume tests made as its
// CVolBox base. The names come from the mangled symbols; CVolBox's tests are a
// non-virtual view of the base and the result type (bool) is inferred. They
// are inline in the original (weak symbols), so they are defined
// __declspec(weak). CVolBox and CCDBVolBox are views here: CCDBVolBox
// declares Create (defined elsewhere) first so its global table stays
// elsewhere. The rest of the file is not part of this unit.
class CCollision;
class CWorldVolume;
class CCDBObject;
class IVolume;

#include "game/CVector3.h"

class CVolBox {
public:
    virtual ~CVolBox();
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
    bool TestCollision(const IVolume&, CCollision&, bool) const;
};

class CCDBVolBox : public CVolBox {
public:
    virtual IVolume* Create() const; /* defined elsewhere; keeps the table out */
    virtual ~CCDBVolBox();
    bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
    bool TestCollision(const IVolume&, CCollision&, bool) const;
};

__declspec(weak) CCDBVolBox::~CCDBVolBox() {
}

__declspec(weak) bool CCDBVolBox::TestCollision(const CWorldVolume&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool CCDBVolBox::TestCollision(CVector3 point, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(point, collision, flag);
}

__declspec(weak) bool CCDBVolBox::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(other, collision, flag);
}

__declspec(weak) bool CCDBVolBox::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(other, collision, flag);
}

__declspec(weak) bool CCDBVolBox::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(other, collision, flag);
}
