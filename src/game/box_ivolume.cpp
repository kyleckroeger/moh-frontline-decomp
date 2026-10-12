// First in this unit (0x800c5008): the weak CVector3::Cross (the cross product
// of the arguments into this vector), box.cpp's local Sign (+1, -1 or 0 with
// the argument's sign, built from its bits; the pointer casts keep the
// original's store of the result back to the argument's stack slot) and the
// weak CVector3::Abs (each coordinate of the argument through fabs into this
// vector). The result types and the vector view are inferred.
// A fragment of box.cpp: CVolBox's double-dispatch collision tests (against
// IVolume). Swapped tests reverse the collision order
// and let the other volume test this one; fiber tests set the collision's
// line flag and test the fiber's line. The file name is this project's; the
// original record is box.cpp and the functions around these are not part of
// this unit. The classes come from the mangled symbols; IVolume's virtual
// functions are declared in the order of __vt__7IVolume and CVolBox redeclares
// them with its destructor first (defined elsewhere, so its virtual table is
// not emitted here); CAnimatedVolume derives from CVolSphere (its tests call
// the sphere's), and the collision flag is an inferred bit-field view.
class CMatrix;
class CVector3 {
public:
    void Cross(const CVector3&, const CVector3&);
    void Abs(const CVector3&);

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));
class CTriangle;
class CPlane;
class CLine3;
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
};

__declspec(weak) void CVector3::Cross(const CVector3& a, const CVector3& b) {
    x = a.y * b.z - a.z * b.y;
    y = a.z * b.x - a.x * b.z;
    z = a.x * b.y - a.y * b.x;
}

static float Sign(float x) {
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

__declspec(weak) void CVector3::Abs(const CVector3& v) {
    x = __fabs(v.x);
    y = __fabs(v.y);
    z = __fabs(v.z);
}

bool CVolBox::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}
