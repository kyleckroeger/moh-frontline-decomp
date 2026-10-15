// A fragment of player.cpp (0x800a7478): CPlayerObject::SetWeaponEnable sets the enable flag (a bit of the byte at
// +12754) of the weapon of the given type (the table at +1120), returning
// whether the player has it (the views are inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
enum EWeaponTypes {};

/* inferred: the weapon's enable bit */
struct WeaponView {
    unsigned char unknown0000[12754];
    unsigned char m_flag80 : 1;
    unsigned char m_flag40 : 1;
    unsigned char m_enabled : 1;
    unsigned char m_flags : 5;
};

class CPlayerObject {
public:
    bool SetWeaponEnable(EWeaponTypes, bool);

    unsigned char unknown0000[1120];
    WeaponView* m_weapons[45];
};

bool CPlayerObject::SetWeaponEnable(EWeaponTypes type, bool enable) {
    WeaponView* weapon = m_weapons[type];
    if (weapon) {
        weapon->m_enabled = enable;
        return true;
    }
    return false;
}
