// A fragment of particlesystem.cpp (0x8007ea84): CParticleSystem::IsActive,
// then CParticleSystemManager's IsDrawEnabled (unless the configuration flag
// at +6 turns particles off) and IsVisible (always).
// CParticleSystem is named by the mangled symbols; it is a non-virtual view
// and its flag bits at +136 are inferred (named after the accessors). The
// function is a weak copy of a header inline emitted in this file, so it is
// defined __declspec(weak). The configuration view and the manager's
// declaration (only these two members) are inferred. The rest of the file is
// not part of this unit.
class CParticleSystem {
public:
    bool HasRotation() const;
    bool IsActive() const;
    void ValidateBoundingBox(bool);
    bool IsMoving() const;
    bool IsBoundingBoxValid() const;

    unsigned char unknown000[136];
    unsigned char m_active : 1;
    unsigned char m_emitting : 1;
    unsigned char m_boundingBoxValid : 1;
    unsigned char m_moving : 1;
    unsigned char m_hasRotation : 1;
    unsigned char m_flags : 3;
};

__declspec(weak) bool CParticleSystem::IsActive() const {
    return m_active;
}

class CDrawContext;

/* inferred: the configuration flag at +6 that turns particles off */
struct ConfigView {
    unsigned char unknown00[6];
    bool noParticles;
    unsigned char unknown07[13];
};

extern ConfigView g_config;

class CParticleSystemManager {
public:
    bool IsDrawEnabled() const;
    bool IsVisible(CDrawContext&) const;
};

bool CParticleSystemManager::IsDrawEnabled() const {
    return !g_config.noParticles;
}

bool CParticleSystemManager::IsVisible(CDrawContext&) const {
    return true;
}
