// A fragment of fiber.cpp (0x800c9b64): CVolFiber::TestCollision against
// another fiber sets the collision's top flag bit (+16) and tests this volume
// against the other fiber's line (at +16) through the virtual line test. The
// classes and functions are named by the symbols; the collision view, the
// line's position in the fiber and the placeholder virtuals before the line
// test (declared in IVolume's table order, the destructor first and defined
// elsewhere so the table is not emitted here) are inferred. The rest of the
// file is in other units.
class CMatrix;
class CVector3;
class CTriangle;
class CPlane;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

/* Inferred: the collision's flag bit at the top of +16. */
class CCollision {
public:
    unsigned char unknown00[16];
    unsigned char m_flag : 1;
    unsigned char m_flags : 7;
};

/* Inferred: an opaque line record. */
class CLine3 {
public:
    unsigned char unknown00[16];
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
    virtual bool TestCollision(const class CVolFiber&, CCollision&, bool) const;
    virtual bool TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual bool TestCollision(CVector3, CCollision&, bool) const;
    virtual bool TestCollision(const CLine3&, CCollision&, bool) const;
};

class CVolFiber : public IVolume {
public:
    virtual ~CVolFiber();
    virtual bool TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual bool TestCollision(const CLine3&, CCollision&, bool) const;

    unsigned char unknown04[12];
    CLine3 m_line;
};

bool CVolFiber::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    collision.m_flag = 1;
    return TestCollision(other.m_line, collision, flag);
}
