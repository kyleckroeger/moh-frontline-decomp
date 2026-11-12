// A fragment of bsbifunc.cpp (0x80027280): the script built-in that returns
// ammunition information for a weapon slot of the calling node's player
// (otherwise the first player): the low nine bits of the request select the
// slot, and flag 0x200 asks for the weapon's property value, 0x400 whether
// the weapon is selectable, 0x800 the clip ammunition, 0x1000 whether the
// clip is full, and no flag the clip plus reserve ammunition (-1 without a
// weapon). It reads its argument below the script stack top, pops the
// built-in's arguments and writes the result to the new top through its
// address. BIFunc_SetGrenadeTimer after it sets a grenade object's timer
// (+144, inferred) from an integer argument. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around it
// are not reconstructed. The functions, classes and globals are named by the
// mangled symbols; ISceneNode is declared with its virtual functions in the
// order of __vt__10ISceneNode (AsPlayerObject at +156), and the
// player-weapon (its weapon at +12516), weapon, weapon-property, scene and
// built-in record views are inferred (members at their offsets, names not
// original).
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;

#include "game/ISceneNode.h"



struct WeaponPropertiesView {
    unsigned char unknown00[4];
    short maxAmmo;
    unsigned char unknown06[12];
    short info;
};

class CWeapon {
public:
    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    short m_reserveAmmo;
    short m_clipAmmo;
};

class CPlayerWeaponObject {
public:
    bool IsSelectable() const;

    unsigned char unknown0000[12516];
    CWeapon* m_weapon;
};

class CPlayerObject {
public:
    CPlayerWeaponObject* GetWeapon(int) const;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

struct GrenadeTimerView {
    unsigned char unknown00[144];
    float timer;
};

void BIFunc_GetPlayerAmmo(int** stack, void* object) {
    int request = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int value = 0;
    CPlayerObject* player;
    if (object && ((ISceneNode*)object)->AsPlayerObject())
        player = ((ISceneNode*)object)->AsPlayerObject();
    else
        player = g_scene.GetPlayer(0);
    CPlayerWeaponObject* weapon = player->GetWeapon(request & 0x1ff);
    if (weapon) {
        if (request & 0x200)
            value = weapon->m_weapon->m_properties->info;
        else if (request & 0x400)
            value = weapon->IsSelectable() != 0;
        else if (request & 0x800)
            value = weapon->m_weapon->m_clipAmmo;
        else if (request & 0x1000)
            value = weapon->m_weapon->m_clipAmmo == weapon->m_weapon->m_properties->maxAmmo;
        else
            value = weapon->m_weapon->m_reserveAmmo + weapon->m_weapon->m_clipAmmo;
    } else {
        value = -1;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&value;
}

void BIFunc_SetGrenadeTimer(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    GrenadeTimerView* grenade = (GrenadeTimerView*)*(*stack - (count - 1));
    int time = *(*stack - (count - 2));
    grenade->timer = time;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
