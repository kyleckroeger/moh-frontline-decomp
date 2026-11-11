// A fragment of light.cpp (0x80078250): CLight::IsVisible tests a temporary
// sphere at the light's position (from its virtual GetPosition) with the
// radius at +196 for visibility, and the empty CommitUpdate, AttemptUpdate and
// BeginUpdate. CLight, CVolSphere, IVolume, CDrawContext and CVector3 are
// named by the mangled symbols; IVolume's virtual functions are declared in
// the order of __vt__7IVolume, CVolSphere and CLight declare their destructors
// first (defined elsewhere, so no virtual tables are emitted here), CLight's
// virtual slots before GetPosition are placeholders, the sphere's inline
// constructor and members are inferred, and the sphere view is 16-byte
// aligned because the target realigns the stack for it. The rest of the file
// is not part of this unit.
#include "game/CVector3.h"

class CMatrix;
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
class CCollision;
class CDrawContext;

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

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();
    bool TestVisibility(CDrawContext&) const;

    CVolSphere(const CVector3& center, float radius) {
        m_radius = radius;
        m_center = center;
    }

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
} __attribute__((aligned(16)));

class CLight {
public:
    virtual ~CLight();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual void unknown34();
    virtual void unknown38();
    virtual void unknown3c();
    virtual void GetPosition(CVector3&) const;
    bool IsVisible(CDrawContext&) const;
    void CommitUpdate();
    void AttemptUpdate(float);
    void BeginUpdate(float);

    unsigned char unknown004[192];
    float m_radius;
};

bool CLight::IsVisible(CDrawContext& context) const {
    CVector3 position;

    GetPosition(position);
    CVolSphere sphere(position, m_radius);
    return sphere.TestVisibility(context);
}

void CLight::CommitUpdate() {
}

void CLight::AttemptUpdate(float) {
}

void CLight::BeginUpdate(float) {
}
