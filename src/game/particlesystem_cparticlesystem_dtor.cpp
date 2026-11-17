// A fragment of particlesystem.cpp (0x800811b4): the CParticleSystem destructor (its virtual table
// pointer, which follows 28 bytes of members, then each base's through the
// inline destructors of CRenderBin, and the object freed when asked). The file
// name is this project's; the original record is particlesystem.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CParticleSystem declares another of its virtual functions (defined
// elsewhere; the result type is not known) before its destructor, so its own
// global virtual table is not emitted here; the bases' tables are weak in the
// original (no key function), so the compiler's copies of them and of the
// inline base destructors are weak duplicates, linked to the original copies.
#include "game/CRenderBin.h"


class CParticleSystem : public CRenderBin {
public:
    virtual void DeActivate(); /* result type not known */
    virtual ~CParticleSystem();
};

CParticleSystem::~CParticleSystem() {
}
