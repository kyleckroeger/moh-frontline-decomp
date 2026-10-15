// A fragment of particlesystem.cpp (0x80080d9c): CParticleSystem::Register keeps a registration pointer and value in the
// particle system manager (+72, +76; the manager view and the parameter
// meanings are inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
/* inferred: the manager's registration at +72 */
struct ParticleSystemManagerView {
    unsigned char unknown00[72];
    void* m_registered;
    int m_value;
};

extern ParticleSystemManagerView g_ParticleSystemManager;

class CParticleSystem {
public:
    static void Register(void*, int);
};

void CParticleSystem::Register(void* data, int value) {
    g_ParticleSystemManager.m_registered = data;
    g_ParticleSystemManager.m_value = value;
}
