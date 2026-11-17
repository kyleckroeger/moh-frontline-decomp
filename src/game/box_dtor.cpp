// A fragment of box.cpp (0x800c5458..0x800c555c): the CVolBox destructor (its virtual
// table pointer, then IVolume's through the inline base destructor, and the
// object freed when asked). The destructor is the class's key function, so this unit
// compiles with RTTI on and also owns box.cpp's CVolBox virtual table and RTTI records,
// as the original file did, while the rest of the file stays in its other fragments (a
// stepping stone, accepted 2026-11-13: docs/Remaining.md). The file name is this
// project's; the original record is box.cpp, and the destructor follows box_create.cpp.
// IVolume and CVolBox are named by the mangled symbols; IVolume's 17 virtuals and
// CVolBox's 18 (its own TestCollision(const CCSGVolume&) last) are declared in the
// virtual tables' slot order, with CVolBox's destructor first; the result types of the
// TestCollision overloads are inferred (bool). IVolume's destructor is inline (weak in
// the original).
// BoxBoxTest (static), a separating-axis step of the box-box test: the overlap along
// one axis is classified against CONTACT_EPSILON (a global .sdata2 constant
// of the original, declared extern: its file is unknown), a separated axis fails, and a shallower overlap records
// its depth, the sign of the projection and the axis. EAxis is named by the
// mangled symbol; its enumerators are unknown. The parameter names and both
// inline helpers (Classify, Sign) are inferred; Sign's pointer casts keep the
// original's store of the result back to its argument's stack slot.
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

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();
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

CVolBox::~CVolBox() {
}

enum EAxis {};

extern const float CONTACT_EPSILON;

inline int Classify(float d) {
    if (d > CONTACT_EPSILON)
        return 1;
    if (d < -CONTACT_EPSILON)
        return 3;
    return 2;
}

inline float Sign(float x) {
    unsigned int r = *(unsigned int*)&x;
    if (r & 0x7f800000) {
        r &= 0x80000000;
        r |= 0x3f800000;
    } else {
        r = 0;
    }
    x = *(float*)&r;
    return x;
}
static bool BoxBoxTest(float extent, float projection, EAxis axis, float& depth, float& sign, EAxis& bestAxis) {
    float d = (float)__fabs(projection) - extent;
    if (Classify(d) != 1) {
        if (d < depth) {
            depth = d;
            sign = Sign(projection);
            bestAxis = axis;
        }
        return true;
    }
    return false;
}
