// A fragment of particlesystem.cpp (0x8008076c..0x800807e0): the weak
// CParticleSystem::IsMoving and IsBoundingBoxValid (header inlines emitted in
// this file, so defined __declspec(weak)), then Terminate (clears the first
// two flag bits) and DeActivate (clears the second and records the particle
// system manager's current time). CParticleSystem and CPSManager are named by
// the mangled symbols (the manager by g_ParticleSystemManager, a file-local
// object of the original declared extern here); both are non-virtual views,
// and the flag bits at +136 (named after the accessors), the deactivation
// time at +124, the manager's time at +68 and its inline getter (as in Rising
// Sun) are inferred. The rest of the file is not part of this unit.
class CPSManager {
public:
    float GetCurrentTicks() const { return m_currentTicks; }

    unsigned char unknown00[68];
    float m_currentTicks;
};

extern CPSManager g_ParticleSystemManager;

class CParticleSystem {
public:
    void Terminate();
    void DeActivate();
    bool HasRotation() const;
    bool IsActive() const;
    void ValidateBoundingBox(bool);
    bool IsMoving() const;
    bool IsBoundingBoxValid() const;

    unsigned char unknown000[124];
    float m_deactivationTicks;
    unsigned char unknown080[8];
    unsigned char m_active : 1;
    unsigned char m_emitting : 1;
    unsigned char m_boundingBoxValid : 1;
    unsigned char m_moving : 1;
    unsigned char m_hasRotation : 1;
    unsigned char m_flags : 3;
};

__declspec(weak) bool CParticleSystem::IsMoving() const {
    return m_moving;
}

__declspec(weak) bool CParticleSystem::IsBoundingBoxValid() const {
    return m_boundingBoxValid;
}

void CParticleSystem::Terminate() {
    m_active = 0;
    m_emitting = 0;
}

void CParticleSystem::DeActivate() {
    m_emitting = 0;
    m_deactivationTicks = g_ParticleSystemManager.GetCurrentTicks();
}
