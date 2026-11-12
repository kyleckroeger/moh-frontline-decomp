// A fragment of light.cpp (0x8007890c): the weak CAnimLightManager destructor
// (its virtual table pointer, then the inline ISceneNode destructor and
// IObserver's, and the object freed when asked). The file name is this
// project's; the original record is light.cpp. The classes and functions are
// named by the mangled symbols; only the virtuals the destructor needs are
// declared. CAnimLightManager declares Destroy (its own override, defined
// elsewhere) first so its global virtual table is not emitted here.
// ISceneNode's destructor is inline and its table weak in the original, so
// the compiler's copies are weak duplicates, linked to the original copies.






#include "game/ISceneNode.h"


class IMovingSceneNode : public ISceneNode {
public:
    IMovingSceneNode() {}
    virtual ~IMovingSceneNode() {}
};

class CAnimLightManager : public ISceneNode {
public:
    virtual void Destroy();
    virtual ~CAnimLightManager();
};

__declspec(weak) CAnimLightManager::~CAnimLightManager() {
}
