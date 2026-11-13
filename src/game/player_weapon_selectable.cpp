// A fragment of player_weapon_object.cpp (0x800a8074):
// CPlayerWeaponObject::IsSelectable (weapon types 33 and 34 while they have
// ammunition loaded or in reserve, otherwise the selectable flag at +12754).
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

bool CPlayerWeaponObject::IsSelectable() const {
    int type = m_weapon->m_type;
    if (type == 33 || type == 34)
        return m_weapon->m_loaded + m_weapon->m_reserve != 0;
    return m_selectable;
}
