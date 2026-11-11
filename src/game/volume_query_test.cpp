// The whole of volume_query_test.cpp as linked: only inline code survives,
// the weak destructors of dwi::IVisitor<ISceneNode> and IVolume, then IVolume's
// Create, TransformedCopy, every TestCollision overload (no collision),
// TestVisibility and GetExtents. They are inline in the original (weak
// symbols), so they are defined __declspec(weak). The names come from the
// mangled symbols; the result types (a volume pointer and bool) are inferred,
// and the visitor's table lists its destructor and a pure Visit. The virtuals
// are declared in table order. Defining IVolume's destructor out of line makes
// it the key function, so the compiler emits a global copy of the weak
// original table: both tables are weak duplicates, linked to the originals.
class ISceneNode;

namespace dwi {
template <class T>
class IVisitor {
public:
    virtual ~IVisitor();
    virtual void Visit(T&) = 0;
};

template <>
__declspec(weak) IVisitor<ISceneNode>::~IVisitor() {
}
}

class CCollision;
class CMatrix;
class CDrawContext;
class CTriangle;
class CPlane;
class CLine3;
class CWorldVolume;
class CAnimatedVolume;
class CVolFiber;
class CCDBObject;
class CVolCapsule;
class CVolSphere;
class CVolBox;

#include "game/CVector3.h"

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
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
};

__declspec(weak) IVolume::~IVolume() {
}

__declspec(weak) IVolume* IVolume::Create() const {
    return 0;
}

__declspec(weak) void IVolume::TransformedCopy(const IVolume&, const CMatrix&) {
}

__declspec(weak) bool IVolume::TestCollision(const CTriangle&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(const CPlane&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(const CLine3&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(CVector3, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(const CWorldVolume&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(const CAnimatedVolume&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(const CVolFiber&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(const CCDBObject&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(const CVolCapsule&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(const CVolSphere&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(const CVolBox&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestCollision(const IVolume&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool IVolume::TestVisibility(CDrawContext&) const {
    return false;
}

__declspec(weak) void IVolume::GetExtents(CVector3&, CVector3&) const {
}
