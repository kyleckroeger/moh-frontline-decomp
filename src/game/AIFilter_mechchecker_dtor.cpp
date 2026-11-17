// A fragment of AIFilter.cpp (0x8005c104): the weak MechanicCollisionChecker destructor (its virtual table
// pointer, the fiber volume at +16 destroyed, then the inline
// dwi::IVisitor<ISceneNode> destructor, and the object freed when asked). The
// file name is this project's; the original record is AIFilter.cpp. The classes,
// the template and the functions are named by the mangled symbols; the
// visitor's table lists its destructor and a pure Visit, the members before
// the fiber are not known, and only the virtuals the destructor needs are
// declared. MechanicCollisionChecker declares its Visit override (defined elsewhere) first so its
// global virtual table is not emitted here. The visitor's destructor is
// inline and its table weak in the original, so the compiler's copies are
// weak duplicates, linked to the original copies.
class ISceneNode;

namespace dwi {
template <class T>
class IVisitor {
public:
    virtual ~IVisitor() {}
    virtual void Visit(T&) = 0;
};
}

#include "game/IVolume.h"


class CVolFiber : public IVolume {
public:
    virtual ~CVolFiber();

    unsigned char unknown04[76];
} __attribute__((aligned(8)));

class MechanicCollisionChecker : public dwi::IVisitor<ISceneNode> {
public:
    virtual void Visit(ISceneNode&);
    virtual ~MechanicCollisionChecker();

    unsigned char unknown04[12];
    CVolFiber m_fiber;
};

__declspec(weak) MechanicCollisionChecker::~MechanicCollisionChecker() {
}
