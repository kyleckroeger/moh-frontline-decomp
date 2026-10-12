// A fragment of player.cpp (0x800a74a8): CPlayerObject::CanPlayerSwitchWeapon,
// whether any of the 45 weapon slots after the current one (wrapping) holds a
// selectable weapon. The file name is this project's; the original record is
// player.cpp (player_ctor.cpp holds the constructor after it). CPlayerObject
// and CPlayerWeaponObject are named by the mangled symbols; the player is a
// non-virtual view with only the current-weapon index (+1116) and the weapon
// slots (+1120) placed (as in player_ctor.cpp).
class CPlayerWeaponObject {
public:
    bool IsSelectable() const;
};

class CPlayerObject {
public:
    bool CanPlayerSwitchWeapon();

    unsigned char unknown000[1116];
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[45];
};

bool CPlayerObject::CanPlayerSwitchWeapon() {
    for (int i = 1; i < 45; i++) {
        int index = m_currentWeapon + i;
        if (index >= 45)
            index -= 45;
        if (m_weapons[index] && m_weapons[index]->IsSelectable())
            return true;
    }
    return false;
}
