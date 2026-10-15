// A fragment of weaponfactory.cpp (0x800d6efc): CWeaponFactory's
// GetWeaponPropertiesByCRC maps ten weapon CRCs to the CRC whose properties
// they share, then returns the properties record (80 bytes) with that CRC and
// stores its index (-1 and none when there is no such record). The class,
// the function and WeaponProperties_struct are named by the symbols; the
// factory view follows weaponfactory_params.cpp and the record's CRC at +0
// is inferred. CreateWeapon takes a weapon from the free list onto the active
// list and initialises it with the type's properties; Shutdown deletes each
// properties record's object at +64 unless an earlier record with the same
// name (+44) shares it, then the mesh array, the properties and the weapons;
// Reset re-creates the 150 weapons (768 bytes each) as one free list and
// allocates the two muzzle-flash static objects. The weapon, mesh-object and
// factory members are inferred (as in weaponfactory_params.cpp). The rest of
// the file is in other units.
extern "C" int strcmp(const char*, const char*);

enum EWeaponTypes {};
class ISceneNode;
class CStaticMesh;
class CStaticObject;

/* inferred: the object a weapon's properties keep at +64 (its table pointer
   after 28 bytes, its destructor in the first slot) */
class WeaponMeshView {
    unsigned char unknown00[28];

public:
    virtual ~WeaponMeshView();
};

/* inferred: a weapon's properties (80 bytes), its CRC first */
struct WeaponProperties_struct {
    unsigned long crc;
    unsigned char unknown04[40];
    char name[20];
    WeaponMeshView* mesh;
    unsigned char unknown44[12];
};

class CWeapon {
public:
    CWeapon();
    ~CWeapon();
    void Init(EWeaponTypes, WeaponProperties_struct*, ISceneNode*, short);
    static void* operator new[](unsigned long, char*);

    unsigned char unknown000[36];
    CWeapon* m_next;
    unsigned char unknown028[728];
};

class CStaticObjectFactory {
public:
    static CStaticObject* AllocateStaticObject(int, int);
};

extern CStaticObject* g_pStaticObjectMuzzleFlashNPC;
extern CStaticObject* g_pStaticObjectMuzzleFlashPlayer;

class CWeaponFactory {
public:
    WeaponProperties_struct* GetWeaponPropertiesByCRC(int, int*);
    CWeapon* CreateWeapon(EWeaponTypes, ISceneNode*, int);
    void Shutdown();
    void Reset();
    /* inferred inline helper: the register numbering needs the loop as one */
    void ReleaseMeshes() {
        for (int i = 0; i < m_count; i++) {
            int j;
            char* name = m_properties[i].name;
            if (*name) {
                for (j = 0; j < i; j++) {
                    if (strcmp(name, m_properties[j].name) == 0) {
                        m_properties[i].mesh = 0;
                        break;
                    }
                }
                if (m_properties[i].mesh) {
                    delete m_properties[i].mesh;
                    m_properties[i].mesh = 0;
                }
            }
        }
    }

    WeaponProperties_struct* m_properties;
    CStaticMesh* m_meshes;
    CWeapon* m_weapons;
    CWeapon* m_free;
    CWeapon* m_active;
    bool m_loaded;
    unsigned char unknown15[3];
    int m_count;
};

class CStaticMesh {
public:
    virtual ~CStaticMesh();
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

CWeapon* CWeaponFactory::CreateWeapon(EWeaponTypes type, ISceneNode* owner, int index) {
    CWeapon* weapon = m_free;
    m_free = weapon->m_next;
    weapon->m_next = m_active;
    m_active = weapon;
    weapon->Init(type, &m_properties[type], owner, index);
    return weapon;
}

void CWeaponFactory::Shutdown() {
    ReleaseMeshes();
    if (m_meshes)
        delete[] m_meshes;
    delete[] m_properties;
    delete[] m_weapons;
    m_properties = 0;
    m_weapons = 0;
    m_loaded = false;
}

void CWeaponFactory::Reset() {
    delete[] m_weapons;
    m_weapons = new ("Weapon factory") CWeapon[150];
    m_free = m_weapons;
    m_active = 0;
    for (int i = 0; i < 149; i++)
        m_weapons[i].m_next = &m_weapons[i + 1];
    m_weapons[149].m_next = 0;
    g_pStaticObjectMuzzleFlashNPC = CStaticObjectFactory::AllocateStaticObject(-1332363775, -1);
    g_pStaticObjectMuzzleFlashPlayer = CStaticObjectFactory::AllocateStaticObject(-1372071268, -1);
}
