// A fragment of weaponfactory.cpp (0x800d65f8): CWeaponFactory::
// LoadWeaponObjects gives each weapon record a mesh, sharing one between
// records with the same mesh name, allocates the meshes with new[] and loads
// them from the repository (the record's mesh fields are inferred).
// Then the weak CStaticMesh constructor (its table
// pointer, a cleared word and two flag bits set; the members are inferred and
// the class declares its destructor, defined elsewhere, first so its global
// table stays elsewhere), CWeaponFactory::
// LoadWeaponParameters, which loads the multiplayer or difficulty's parameter
// file, allocates the weapon property table and fills it from the file's
// weapon records (skipping the bullet records), the weak
// EndianSwap(WeaponProperties_struct&) it calls, and DestroyWeapon (shuts the
// weapon down and moves it from the active list to the free list). The names come from the
// mangled symbols; the resource, record and factory views are inferred, and
// the byte-order helpers are the inlined ones described in propdat.cpp
// (inferred; the converted fields are named by offset).
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


enum ERenderPriority {};

class CWeapon {
public:
    void Shutdown();

    unsigned char unknown00[36];
    CWeapon* m_next;
};

struct WeaponProperties_struct {
    unsigned char data[80];
};

class CStaticMesh;

// The fields of a weapon record used while loading its mesh (inferred).
struct WeaponPropertiesView {
    unsigned char unknown00[20];
    char meshName[20];
    CStaticMesh* mesh;
    unsigned char unknown2c[36];
};

void EndianSwap(WeaponProperties_struct&);

class CWeaponFactory {
public:
    void LoadWeaponObjects();
    void LoadWeaponParameters();
    void DestroyWeapon(CWeapon*);

    WeaponProperties_struct* m_properties;
    CStaticMesh* m_meshes;
    unsigned char unknown08[4];
    CWeapon* m_free;
    CWeapon* m_active;
    unsigned char unknown14[4];
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

__declspec(weak) CStaticMesh::CStaticMesh() : m_unknown04(0) {
    m_flag80 = 1;
    m_flag40 = 1;
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

__declspec(weak) void EndianSwap(WeaponProperties_struct& p) {
    ChangeEndian(*(int*)(p.data + 0));
    ChangeEndian(*(short*)(p.data + 4));
    ChangeEndian(*(short*)(p.data + 6));
    ChangeEndian(*(short*)(p.data + 12));
    ChangeEndian(*(short*)(p.data + 14));
    ChangeEndian(*(short*)(p.data + 16));
    ChangeEndian(*(short*)(p.data + 18));
    ChangeEndian(*(int*)(p.data + 40));
    ChangeEndian(*(int*)(p.data + 64));
    ChangeEndian(*(short*)(p.data + 72));
    ChangeEndian(*(short*)(p.data + 74));
    ChangeEndian(*(int*)(p.data + 8));
    ChangeEndian(*(int*)(p.data + 68));
    ChangeEndian(*(int*)(p.data + 76));
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
