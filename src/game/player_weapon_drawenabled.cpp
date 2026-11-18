// A fragment of player_weapon_object.cpp (0x800a7f80):
// CPlayerWeaponObject::IsDrawEnabled (the draw flag at +12754 is set and the
// player's flag at +919 is clear) and AddAmo (adds to the weapon's reserve and
// re-enables pickup collisions of the weapon's type while it can take more).
// AddAmo passes the flag and the type through an inferred inline wrapper
// (flag first, type through the GetType accessor): CW computes value-returning
// inline arguments as temporaries, last argument first, so the type is loaded
// before the flag's compare as in the original.
// CPlayerWeaponObject, CWeapon, CPlayerObject and EDumpType are named by the
// mangled symbols; the members, the weapon property record and the flag bits
// are inferred views. The rest of the file is not part of this unit.
enum EDumpType {};

void EnableDumpCollisionsByType(EDumpType, int, bool);

struct WeaponPropertiesView {
    unsigned char unknown00[18];
    short maxReserve;
};

class CWeapon {
public:
    void AddAmo(short);
    bool CanAddAmo() { return (unsigned int)m_properties->maxReserve > (unsigned int)m_reserve; }
    int GetType() { return m_type; }
    void DoneReloading();

    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    short m_reserve;
    short m_loaded;
    unsigned char unknown288[16];
    int m_type;
};

#include "game/CPlayerObject.h"


class CPlayerWeaponObject {
public:
    bool IsDrawEnabled() const;
    void AddAmo(int);
    void DoneReloading();
    bool IsSelectable() const;

    unsigned char unknown0000[12512];
    CPlayerObject* m_player;
    CWeapon* m_weapon;
    unsigned char unknown30e8[234];
    unsigned char m_flag0 : 1;
    unsigned char m_drawEnabled : 1;
    bool m_selectable : 1;
    unsigned char m_flags : 5;
};

bool CPlayerWeaponObject::IsDrawEnabled() const {
    bool enabled = false;
    if (m_drawEnabled && !m_player->m_weaponHidden)
        enabled = true;
    return enabled;
}

inline void EnablePickupCollisions(bool enable, int type) {
    EnableDumpCollisionsByType((EDumpType)1, type, enable);
}

void CPlayerWeaponObject::AddAmo(int count) {
    m_weapon->AddAmo(count);
    EnablePickupCollisions(m_weapon->CanAddAmo(), m_weapon->GetType());
}
