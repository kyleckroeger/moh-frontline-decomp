// A fragment of player.cpp (0x800a61bc): the CPlayerObject destructor (its
// virtual table pointer; each of the 45 weapon objects at +1120 shut down and
// released to the animated-object factory; then the members in reverse
// order: the user interface at +2672, the camera at +2000, the hearing volume
// at +704, the collision list at +680 (dwi::fast_vec<CClsnInfo>, freed when
// it owns its storage), two capsules at +512 and +432, two boxes at +352 and
// +272 and the AI doodad at +36; then the inline IMovingSceneNode and
// ISceneNode destructors and IObserver's; the object freed when asked). The
// file name is this project's; the original record is player.cpp, before
// player_init.cpp. The classes and functions are named by the mangled symbols;
// the member positions come from the code, while their names, the class sizes
// and the space between members are inferred, and only the virtuals the
// destructor needs are declared. CPlayerObject declares Destroy (its own
// override, defined elsewhere) first so its global virtual table is not
// emitted here. IMovingSceneNode's and ISceneNode's destructors are inline and
// their tables weak in the original, so the compiler's copies are weak
// duplicates, linked to the original copies.
extern "C" void MEM_free(void*);

#include "game/ISceneNode.h"

#include "game/DwiContainers.h"

class CAnimObject;
class CPlayerWeaponObject {
public:
    void Shutdown();
};

class CAnimObjectFactory {
public:
    void ReleaseObject(CAnimObject*);

    unsigned char data[16];
};

extern CAnimObjectFactory g_AnimObjFactory;

class CAIPlayerDoodad {
public:
    ~CAIPlayerDoodad();

    unsigned char unknown00[236];
};

class CVolBox {
public:
    virtual ~CVolBox();

    unsigned char unknown04[76];
} __attribute__((aligned(8)));

class CVolCapsule {
public:
    virtual ~CVolCapsule();

    unsigned char unknown04[76];
} __attribute__((aligned(8)));

class CHearingVolume {
public:
    virtual ~CHearingVolume();

    unsigned char unknown004[188];
} __attribute__((aligned(8)));

#include "game/CCamera.h"

class UserInterface {
public:
    ~UserInterface();

    unsigned char unknown00[4];
};

class CPlayerObject : public IMovingSceneNode {
public:
    struct CClsnInfo;

    virtual void Destroy();
    virtual ~CPlayerObject();

    CAIPlayerDoodad m_doodad;
    CVolBox m_boxA;
    CVolBox m_boxB;
    CVolCapsule m_capsuleA;
    CVolCapsule m_capsuleB;
    unsigned char unknown240[88];
    dwi::fast_vec<CClsnInfo> m_collisions;
    CHearingVolume m_hearing;
    unsigned char unknown380[224];
    CPlayerWeaponObject* m_weapons[45];
    unsigned char unknown0514[700];
    CCamera m_camera;
    UserInterface m_ui;
};

CPlayerObject::~CPlayerObject() {
    for (int i = 0; i < 45; i++) {
        CPlayerWeaponObject* weapon = m_weapons[i];
        if (weapon) {
            weapon->Shutdown();
            g_AnimObjFactory.ReleaseObject((CAnimObject*)weapon);
        }
    }
}
