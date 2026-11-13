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
    bool CanAddAmo() { return (unsigned int)m_properties->maxReserve > (unsigned int)m_reserve; }
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

void CPlayerWeaponObject::AddAmo(int count){ m_weapon->AddAmo(count); EnableDumpCollisionsByType((EDumpType)1, m_weapon->m_type, m_weapon->CanAddAmo()); }
