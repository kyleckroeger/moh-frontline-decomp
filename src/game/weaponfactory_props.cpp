// A fragment of weaponfactory.cpp (0x800d6efc): CWeaponFactory's
// GetWeaponPropertiesByCRC maps ten weapon CRCs to the CRC whose properties
// they share, then returns the properties record (80 bytes) with that CRC and
// stores its index (-1 and none when there is no such record). The class,
// the function and WeaponProperties_struct are named by the symbols; the
// factory view follows weaponfactory_params.cpp and the record's CRC at +0
// is inferred. The rest of the file is in other units.
/* inferred: a weapon's properties (80 bytes), its CRC first */
struct WeaponProperties_struct {
    unsigned long crc;
    unsigned char unknown04[76];
};

class CWeaponFactory {
public:
    WeaponProperties_struct* GetWeaponPropertiesByCRC(int, int*);

    WeaponProperties_struct* m_properties;
    unsigned char unknown04[20];
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
