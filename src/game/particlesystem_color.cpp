// A fragment of particlesystem.cpp (0x8007f198): CPropertyParticleSystem's
// GetParticleColor (four colours copied from the particle-property record at
// +0x71, +0x7a, +0x75 and +0x7d; the last overlaps the second's alpha, and
// what the colours mean is unknown), GetFogEnable (fog unless the record's
// low flag bit is set) and GetParticleAlpha (the alpha bytes of the first,
// fourth and third colours as floats; the conversion constant is an entry of
// the file's .sdata2 pool). The file name is this project's; the original
// record is particlesystem.cpp. The class names come from the mangled
// symbols; the members and the property-record view are inferred, and the
// class is a non-virtual view.
class CMatrix;

#include "game/CVector3.h"

class CColor {
public:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct ParticleVectorView {
    float x;
    float y;
    float z;
};

struct ParticlePropertiesView {
    unsigned char unknown00[112];
    unsigned char fogFlags;
    CColor color71;
    CColor color75;
    unsigned char unknown79;
    CColor color7a;
    unsigned char unknown7e[14];
    int seed;
    ParticleVectorView position;
    ParticleVectorView positionRange;
    ParticleVectorView velocity;
    ParticleVectorView velocityRange;
    ParticleVectorView acceleration;
};

class CPropertyParticleSystem {
public:
    void SetLocalToWorld(const CMatrix&);
    void SetSeed(int);
    void GetParticleAcceleration(CVector3&) const;
    void GetParticleVelocity(CVector3&, CVector3&) const;
    void GetParticlePosition(CVector3&, CVector3&) const;
    void GetParticleColor(CColor&, CColor&, CColor&, CColor&) const;
    bool GetFogEnable() const;
    void GetParticleAlpha(float&, float&, float&) const;

    unsigned char unknown000[240];
    ParticlePropertiesView* m_properties;
};

void CPropertyParticleSystem::GetParticleColor(CColor& color0, CColor& color1, CColor& color2, CColor& color3) const {
    color0 = m_properties->color71;
    color1 = m_properties->color7a;
    color2 = m_properties->color75;
    color3 = *(const CColor*)((const unsigned char*)&m_properties->color7a + 3);
}

bool CPropertyParticleSystem::GetFogEnable() const {
    return !(m_properties->fogFlags & 1);
}

void CPropertyParticleSystem::GetParticleAlpha(float& alpha0, float& alpha1, float& alpha2) const {
    alpha0 = m_properties->color71.a;
    alpha1 = ((const CColor*)((const unsigned char*)&m_properties->color7a + 3))->a;
    alpha2 = m_properties->color75.a;
}
