// A fragment of player_weapon_object.cpp (0x800a800c):
// CPlayerWeaponObject::DoneReloading forwards to the weapon, and Shoot fires
// it with the count as a float and -1.0f (its constants are entries of the
// file's .sdata2 pool; the meaning of the second value is unknown).
// CPlayerWeaponObject, CWeapon, CPlayerObject and EDumpType are named by the
// mangled symbols; the members, the weapon property record and the flag bits
// are inferred views. The rest of the file is not part of this unit.
enum EDumpType {};
enum EWeaponShootType {};

void EnableDumpCollisionsByType(EDumpType, int, bool);

struct WeaponPropertiesView {
    unsigned char unknown00[18];
    short maxReserve;
};

class CWeapon {
public:
    void AddAmo(short);
    void DoneReloading();
    void Shoot(EWeaponShootType, float, float);

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
    void Shoot(EWeaponShootType, int);
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

void CPlayerWeaponObject::DoneReloading() {
    m_weapon->DoneReloading();
}

void CPlayerWeaponObject::Shoot(EWeaponShootType type, int count) {
    m_weapon->Shoot(type, count, -1.0f);
}
