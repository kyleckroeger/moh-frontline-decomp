// A fragment of particlesystem.cpp (0x800807e0): CParticleSystem::UpdateSystem
// advances the system clock, emits the particles due since the last emission
// (each one advances the random generator eleven steps from the system's seed,
// which is then stored back), deactivates an emitting system whose lifetime is
// over and terminates an inactive one once its last particles are dead, then
// updates the bounding box; CParticleSystem::GetTexture returns the texture
// (+96) of the system's texture bin (+116). The class and functions are named
// by the symbols; the virtual-table slots, members, views and the result type
// are inferred. The rest of the file is in other units.
class CTexture;

/* inferred: the texture bin's texture at +96 */
struct ParticleTextureBinView {
    unsigned char unknown00[96];
    CTexture* m_texture;
};

extern int g_CurrentSeed;

class CPSManager {
public:
    float GetCurrentTicks() const { return m_currentTicks; }

    unsigned char unknown00[68];
    float m_currentTicks;
};

extern CPSManager g_ParticleSystemManager;

/* inferred: the C library generator kept in g_CurrentSeed */
static inline int Random() {
    g_CurrentSeed = g_CurrentSeed * 1103515245 + 12345;
    return g_CurrentSeed & 0x7fff;
}

class CParticleSystem {
    unsigned char unknown00[28];

public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void DeActivate();
    virtual void Terminate();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual int GetSeed() const;
    virtual int GetEmmisionRate() const;
    virtual int GetEmmisionDelay() const;
    virtual float GetSystemLifetime() const;
    virtual float GetParticleLifetime() const;
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void SetSeed(int);
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void UpdateBoundingBox();

    void UpdateSystem(float);
    CTexture* GetTexture() const;

    unsigned char unknown020[84];
    ParticleTextureBinView* m_bin;
    float m_activationTicks;
    float m_deactivationTicks;
    float m_emitTicks;
    float m_ticks;
    unsigned char m_active : 1;
    unsigned char m_emitting : 1;
    unsigned char m_boundingBoxValid : 1;
    unsigned char m_moving : 1;
    unsigned char m_hasRotation : 1;
    unsigned char m_flags : 3;
};

static inline void Emit(void* p, float dt, float lifetime, float& last) {
    CParticleSystem* ps = (CParticleSystem*)p;
    if (!(ps->m_ticks - (last = ps->m_emitTicks) > lifetime)) return;
    int delay = ps->GetEmmisionDelay();
    ps->m_emitTicks += (float)(delay * ((int)dt / delay + 1));
    int count = (int)(ps->m_emitTicks - last) / delay;
    count *= ps->GetEmmisionRate();
    int value = ps->GetSeed();
    g_CurrentSeed = value;
    for (int i = 0; i < count; i++) {
        value = Random();
        value = Random();
        value = Random();
        value = Random();
        value = Random();
        value = Random();
        value = Random();
        value = Random();
        value = Random();
        value = Random();
        value = Random();
    }
    ps->SetSeed(value);
}

void CParticleSystem::UpdateSystem(float dt) {
    float lifetime = 60.0f * GetParticleLifetime();
    m_ticks += dt;
    float last;
    Emit(this, dt, lifetime, last);
    if (m_emitting) {
        float life = GetSystemLifetime();
        if (life != 0.0f && g_ParticleSystemManager.GetCurrentTicks() >= m_activationTicks + (float)(60.0f * life))
            DeActivate();
    } else if (g_ParticleSystemManager.GetCurrentTicks() > m_deactivationTicks + lifetime) {
        Terminate();
    }
    UpdateBoundingBox();
}

CTexture* CParticleSystem::GetTexture() const {
    return m_bin->m_texture;
}
