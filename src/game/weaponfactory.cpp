// weaponfactory.cpp: CWeaponFactory::LoadWeaponObjects gives each weapon record
// a mesh, sharing one between records with the same mesh name, allocates the
// meshes with new[] and loads them from the repository; the weak CStaticMesh
// constructor (its table pointer, a cleared word and two flag bits set; the
// class declares its destructor, defined elsewhere, first so its global table
// stays elsewhere); LoadWeaponParameters loads the multiplayer or difficulty's
// parameter file, allocates the weapon property table and fills it from the
// file's weapon records (skipping the bullet records), through the weak
// EndianSwap(WeaponProperties_struct&); DestroyWeapon shuts the weapon down and
// moves it from the active list to the free list; CreateWeaponByCRC looks the
// weapon's properties up by CRC (falling back to the first record for a
// non-zero CRC that has none, returning no weapon for CRC 0), takes a weapon
// from the free list onto the active list and initialises it.
// GetWeaponPropertiesByCRC (inlined there) maps ten weapon CRCs to the CRC
// whose properties they share, then returns the properties record (80 bytes)
// with that CRC and stores its index (-1 and none when there is no such
// record). CreateWeapon takes a weapon from the free list onto the active list
// and initialises it with the type's properties; Shutdown deletes each
// properties record's object at +64 unless an earlier record with the same
// name (+44) shares it, then the mesh array, the properties and the weapons;
// Reset re-creates the 150 weapons (768 bytes each) as one free list and
// allocates the two muzzle-flash static objects; Init loads the parameters,
// creates each record's sprite unless an earlier record with the same name
// shares it (a "data\\" path is built but unused, as the code does), loads
// the weapon objects, then creates the weapons and muzzle flashes as Reset
// does and marks the factory loaded; then the destructor (nothing to release)
// and the constructor (empty lists and a cleared flag). The names come from
// the mangled symbols; the resource, record, weapon, mesh and factory views
// are inferred (the mesh-loading pass reads the record through its own view:
// the mesh name at +20 and the mesh at +40), and the byte-order helpers are
// the inlined ones described in propdat.cpp (inferred; the converted fields
// are named by offset). The lookup is inlined into CreateWeaponByCRC, which
// comes before it, so the unit is compiled with deferred inlining and the
// functions are written last to first.
extern "C" int strcmp(const char*, const char*);
void* TLT_LoadFromRepository(const char*, int*, bool*);
extern "C" void* memcpy(void*, const void*, unsigned long);
void DebugMsg(const char*, ...);
void TLT_CloseFile(void*);

enum TLTResourceID {};
struct LevelFileContentsStruct_;

// Inferred view of the level resource entry and its file names.
struct BulletFileNamesView {
    unsigned char unknown000[36];
    const char* m_easy;
    unsigned char unknown028[4];
    const char* m_normal;
    unsigned char unknown030[4];
    const char* m_hard;
    unsigned char unknown038[468];
    const char* m_multiplayer;
};

struct LevelResourceView {
    unsigned char unknown00[8];
    BulletFileNamesView* m_names;
};

LevelResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

class CShellMenu {
public:
    unsigned char Get_currentDifficulty();

    unsigned char unknown0000[0x19dc];
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

extern "C" {
char* strcpy(char*, const char*);
unsigned long strlen(const char*);
}

void* DWI_alloc(const char*, int, int);

enum EWeaponTypes {};
class ISceneNode;
class CStaticMesh;
class CStaticObject;

enum ERenderPriority {};

/* CSprite as in bulletfactory_sprites.cpp: its table pointer after 28 bytes */
class CSpriteBase {
public:
    unsigned char unknown00[28];
};

class CSprite : public CSpriteBase {
public:
    enum ESpriteLoadType {};

    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }
    CSprite(const char*, ESpriteLoadType, ERenderPriority);
    virtual ~CSprite();

    void* m_texture;
    unsigned char unknown24[64];
};

/* inferred: a weapon's properties (80 bytes), its CRC first */
struct WeaponProperties_struct {
    unsigned long crc;
    unsigned char unknown04[40];
    char name[20];
    CSprite* mesh;
    unsigned char unknown44[12];
};

class CWeapon {
public:
    void Shutdown();
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
    void Init();
    CWeaponFactory();
    ~CWeaponFactory();
    void LoadWeaponParameters();
    void LoadWeaponObjects();
    void DestroyWeapon(CWeapon*);
    CWeapon* CreateWeaponByCRC(int, ISceneNode*, int);
    /* inferred inline helper */
    void LoadMeshes() {
        for (int i = 0; i < m_count; i++) {
            int j;
            m_properties[i].mesh = 0;
            char* name = m_properties[i].name;
            if (*name) {
                for (j = 0; j < i; j++) {
                    if (strcmp(name, m_properties[j].name) == 0) {
                        m_properties[i].mesh = m_properties[j].mesh;
                        break;
                    }
                }
                if (!m_properties[i].mesh) {
                    char path[64];
                    strcpy(path, "data\\");
                    strcpy(path + strlen("data\\"), name);
                    m_properties[i].mesh = new CSprite(name, (CSprite::ESpriteLoadType)1, (ERenderPriority)111);
                }
            }
        }
    }
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
    virtual ~CStaticMesh(); /* defined elsewhere; keeps the table out */
    CStaticMesh();
    void Init(void*, bool);

    int m_unknown04;
    unsigned char m_flag80 : 1;
    unsigned char m_flag40 : 1;
    unsigned char m_flags : 6;
};

// The fields of a weapon record used while loading its mesh (inferred).
struct WeaponPropertiesView {
    unsigned char unknown00[20];
    char meshName[20];
    CStaticMesh* mesh;
    unsigned char unknown2c[36];
};

void EndianSwap(WeaponProperties_struct&);

CWeaponFactory::CWeaponFactory() {
    m_properties = 0;
    m_weapons = 0;
    m_free = 0;
    m_active = 0;
    m_loaded = false;
}

CWeaponFactory::~CWeaponFactory() {
}

void CWeaponFactory::Init() {
    LoadWeaponParameters();
    LoadMeshes();
    LoadWeaponObjects();
    m_weapons = new ("Weapon factory") CWeapon[150];
    m_free = m_weapons;
    m_active = 0;
    for (int k = 0; k < 149; k++)
        m_weapons[k].m_next = &m_weapons[k + 1];
    m_weapons[149].m_next = 0;
    m_loaded = true;
    g_pStaticObjectMuzzleFlashNPC = CStaticObjectFactory::AllocateStaticObject(-1332363775, -1);
    g_pStaticObjectMuzzleFlashPlayer = CStaticObjectFactory::AllocateStaticObject(-1372071268, -1);
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

CWeapon* CWeaponFactory::CreateWeapon(EWeaponTypes type, ISceneNode* owner, int index) {
    CWeapon* weapon = m_free;
    m_free = weapon->m_next;
    weapon->m_next = m_active;
    m_active = weapon;
    weapon->Init(type, &m_properties[type], owner, index);
    return weapon;
}

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

void CWeaponFactory::DestroyWeapon(CWeapon* weapon) {
    weapon->Shutdown();
    CWeapon** link = &m_active;
    CWeapon* current = m_active;
    while (current != weapon) {
        link = &current->m_next;
        current = current->m_next;
    }
    *link = weapon->m_next;
    weapon->m_next = m_free;
    m_free = weapon;
}

__declspec(weak) void EndianSwap(WeaponProperties_struct& record) {
    unsigned char* data = (unsigned char*)&record;
    ChangeEndian(*(int*)(data + 0));
    ChangeEndian(*(short*)(data + 4));
    ChangeEndian(*(short*)(data + 6));
    ChangeEndian(*(short*)(data + 12));
    ChangeEndian(*(short*)(data + 14));
    ChangeEndian(*(short*)(data + 16));
    ChangeEndian(*(short*)(data + 18));
    ChangeEndian(*(int*)(data + 40));
    ChangeEndian(*(int*)(data + 64));
    ChangeEndian(*(short*)(data + 72));
    ChangeEndian(*(short*)(data + 74));
    ChangeEndian(*(int*)(data + 8));
    ChangeEndian(*(int*)(data + 68));
    ChangeEndian(*(int*)(data + 76));
}

void CWeaponFactory::LoadWeaponParameters() {
    short offset;
    char* file;
    LevelResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)11, 0);
    if (g_bInMultiplayerMode) {
        file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_multiplayer, 0);
    } else {
        switch (g_Shell.Get_currentDifficulty()) {
        case 1:
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_easy, 0);
            break;
        case 2:
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_normal, 0);
            break;
        case 3:
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_hard, 0);
            break;
        default:
            DebugMsg("Unknown difficulty level.  Assuming Level TE\n");
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_easy, 0);
            break;
        }
    }
    memcpy(&m_count, file, 4);
    offset = 12;
    ChangeEndian(m_count);
    m_properties = (WeaponProperties_struct*)new char[m_count * 80];
    for (int read = 0; read < m_count;) {
        short type;
        memcpy(&type, file + offset, 2);
        offset += 2;
        ChangeEndian(type);
        switch (type) {
        case 1: {
            short index;
            memcpy(&index, file + offset, 2);
            ChangeEndian(index);
            memcpy(&m_properties[index], file + (short)(offset + 2), 80);
            EndianSwap(m_properties[index]);
            offset += 82;
            read++;
            break;
        }
        case 2:
            offset += 66;
            break;
        case 3:
            offset += 78;
            break;
        }
    }
    TLT_CloseFile(file);
}

__declspec(weak) CStaticMesh::CStaticMesh() : m_unknown04(0) {
    m_flag80 = 1;
    m_flag40 = 1;
}


void CWeaponFactory::LoadWeaponObjects() {
    WeaponPropertiesView*& props = *(WeaponPropertiesView**)&m_properties;
    int unique = 0;
    int i;
    int j;
    for (i = 0; i < m_count; i++) {
        char* name = props[i].meshName;
        if (name[0]) {
            for (j = 0; j < i; j++) {
                if (strcmp(name, props[j].meshName) == 0) {
                    props[i].mesh = (CStaticMesh*)j;
                    break;
                }
            }
            if (j == i) {
                unique++;
                props[i].mesh = (CStaticMesh*)i;
            }
        } else {
            props[i].mesh = (CStaticMesh*)-1;
        }
    }
    m_meshes = new CStaticMesh[unique];
    int next = 0;
    for (i = 0; i < m_count; i++) {
        int index = (int)props[i].mesh;
        if (index == -1) {
            props[i].mesh = 0;
        } else if (index < i) {
            props[i].mesh = props[index].mesh;
        } else if (index == i) {
            bool compressed;
            props[i].mesh = 0;
            void* data = TLT_LoadFromRepository(props[i].meshName, 0, &compressed);
            if (data) {
                m_meshes[next].Init(data, !compressed);
                props[i].mesh = &m_meshes[next];
                next++;
            }
        }
    }
}
