// A fragment of weaponfactory.cpp (0x800d6cf4): CWeaponFactory::CreateWeaponByCRC
// looks the weapon's properties up by CRC (falling back to the first record for a
// non-zero CRC that has none, returning no weapon for CRC 0), takes a weapon from
// the free list onto the active list and initialises it. The lookup is
// GetWeaponPropertiesByCRC inlined: its out-of-line copy follows in the original
// (weaponfactory_props.cpp places it); it is compiled here again as a file
// duplicate (Rising Sun keeps the call out of line, which shows the helper
// boundary). The class, the functions and WeaponProperties_struct are named by
// the symbols; the factory, weapon and record views follow weaponfactory_props.cpp
// (inferred).
enum EWeaponTypes {};
class ISceneNode;

/* inferred: a weapon's properties (80 bytes), its CRC first */
struct WeaponProperties_struct {
    unsigned long crc;
    unsigned char unknown04[76];
};

class CWeapon {
public:
    void Init(EWeaponTypes, WeaponProperties_struct*, ISceneNode*, short);

    unsigned char unknown000[36];
    CWeapon* m_next;
};

class CWeaponFactory {
public:
    WeaponProperties_struct* GetWeaponPropertiesByCRC(int, int*);
    CWeapon* CreateWeaponByCRC(int, ISceneNode*, int);

    WeaponProperties_struct* m_properties;
    void* m_meshes;
    CWeapon* m_weapons;
    CWeapon* m_free;
    CWeapon* m_active;
    bool m_loaded;
    unsigned char unknown15[3];
    int m_count;
};

WeaponProperties_struct* CWeaponFactory::GetWeaponPropertiesByCRC(int crc, int* index) {
    switch (crc) {
    case -1021255877:
        crc = -1896116718;
        break;
    case -665576871:
        crc = 958924262;
        break;
    case -871513808:
        crc = -195145515;
        break;
    case 1208277298:
        crc = -1379654328;
        break;
    case -154155496:
        crc = 1851280536;
        break;
    case -2001962608:
        crc = -1048131012;
        break;
    case -1909284242:
        crc = -966596322;
        break;
    case -2029810030:
        crc = 917370864;
        break;
    case 1482684816:
        crc = -2090384249;
        break;
    case -772383655:
        crc = 333140481;
        break;
    }
    for (int i = 0; i < m_count; i++) {
        if ((unsigned long)crc == m_properties[i].crc) {
            *index = i;
            return &m_properties[i];
        }
    }
    *index = -1;
    return 0;
}

CWeapon* CWeaponFactory::CreateWeaponByCRC(int crc, ISceneNode* owner, int extra) {
    int index;
    WeaponProperties_struct* properties = GetWeaponPropertiesByCRC(crc, &index);
    if (!properties) {
        if (crc) {
            properties = m_properties;
            index = 0;
        } else {
            return 0;
        }
    }
    CWeapon* weapon = m_free;
    m_free = weapon->m_next;
    weapon->m_next = m_active;
    m_active = weapon;
    weapon->Init((EWeaponTypes)index, properties, owner, extra);
    return weapon;
}
