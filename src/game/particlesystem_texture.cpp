// A fragment of particlesystem.cpp (0x80080b24): CParticleSystem::GetTexture
// returns the texture (+96) of the system's texture bin (+116). The class and
// function are named by the symbols; the views and the result type are
// inferred. The rest of the file is in other units.
class CTexture;

/* inferred: the texture bin's texture at +96 */
struct ParticleTextureBinView {
    unsigned char unknown00[96];
    CTexture* m_texture;
};

class CParticleSystem {
public:
    CTexture* GetTexture() const;

    unsigned char unknown00[116];
    ParticleTextureBinView* m_bin;
};

CTexture* CParticleSystem::GetTexture() const {
    return m_bin->m_texture;
}
