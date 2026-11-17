// A fragment of particlesystem.cpp (0x80081210): the CInstancedParticleSystem destructor (its virtual table
// pointer, which follows 28 bytes of members, then each base's through the
// inline destructors of CRenderBin, CParticleSystem, CPropertyParticleSystem, and the object freed when asked). The file
// name is this project's; the original record is particlesystem.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CInstancedParticleSystem declares another of its virtual functions (defined
// elsewhere; the result type is not known) before its destructor, so its own
// global virtual table is not emitted here.
// CRenderBin: weak table (no key function) and inline destructor, so
// the compiler's copies are weak duplicates, linked to the original copies.
// The destructors of CPropertyParticleSystem, CParticleSystem are global in the original (out of line,
// elsewhere in the same file) yet inlined here; how the original made them available
// inline is not known, so the view declares them inline, after another of
// the class's own virtual functions so that its global table is not emitted.
class CDmaPacket;

#include "game/CRenderBin.h"


class CParticleSystem : public CRenderBin {
public:
    virtual void DeActivate(); /* result type not known */
    virtual ~CParticleSystem() {}
};

class CPropertyParticleSystem : public CParticleSystem {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CPropertyParticleSystem() {}
};

class CInstancedParticleSystem : public CPropertyParticleSystem {
public:
    virtual void GetSeed() const; /* result type not known */
    virtual ~CInstancedParticleSystem();
};

__declspec(weak) CInstancedParticleSystem::~CInstancedParticleSystem() {
}
