// AnimObjFactory.cpp: CAnimObjectFactory, the pool the animated objects (soldiers, players,
// player weapons) are allocated from, the whole file in one unit. SetLargestObjectSize (rounded
// up to 16 bytes), ReleaseObject and AllocateObject (pops the free list, finds the object
// definition with bsearch and constructs the object class in place, then binds the
// definition's skeleton, meshes, collision and the soldier and weapon values);
// InitPlayerWeaponFromLevelContentsStruct and InitAnimObjectFromLevelContentsStruct fill a
// definition from a level resource (meshes, textures, skeleton, static mesh, collision data for
// the difficulty); the collision data's endian conversion (inline in the original, weak here);
// Shutdown, Reset and Init (texture pack, definitions sorted by id, the object pool and its
// free list); CAnimObjDef's weak constructor, the factory constructor and the qsort/bsearch
// comparison. The file is compiled with RTTI off and '-inline auto', the definitions in image
// order.
//
// The classes, functions, LevelFileContentsStruct_, g_Shell, g_cheats and g_staticObjCount
// (file-local) are named by the symbols. Inferred: the members and their names; the
// definition record CAnimObjDef (80 bytes); the level-contents records, reached either as
// {tag, value} fields or by name (an anonymous union of the two pointer views); the factory's
// texture pack and object count words (anonymous unions, as the functions use them); the mesh,
// sprite, cheat and shell views; the CVector3 view (four floats overlaid with two doubles,
// indexed by the definitions' value triple) and the endian helpers (as propdat.cpp's).
// Header-defined hash-table primes: every unit that includes the header carries a copy (this
// file's .rodata starts with it, before the strings).
#include "game/DwiHashMap.h"

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

inline void EndianSwap(float& value, bool) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}


extern "C" void* bsearch(const void*, const void*, unsigned long, unsigned long, int (*)(const void*, const void*));
extern "C" void qsort(void*, unsigned long, unsigned long, int (*)(const void*, const void*));
extern "C" void MEM_free(void*);
extern "C" unsigned long strlen(const char*);
extern "C" int stricmp(const char*, const char*);
extern "C" int printf(const char*, ...);
int CompareObjectDefinitions(const void*, const void*);

enum EObjectClass {};
enum ERenderPriority {};
enum TLTResourceID {};
struct CharSkel_t;
struct StaticMesh;
struct LevelFileContentsStruct_;
struct CObjLocationCollisionData;

LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFromRepository(const char*, int*, bool*);
void CharRelocateSkeleton(CharSkel_t*);
long long MathFunRandomI64(long long, long long);
void DebugMsg(const char*, ...);
void* DWI_alloc(const char*, int, int);

class CVector3 {
public:
    CVector3() {}
    void Set(float ax, float ay, float az) {
        d.v[0] = ax;
        d.v[1] = ay;
        d.v[2] = az;
    }
    CVector3(float ax, float ay, float az) {
        d.v[0] = ax;
        d.v[1] = ay;
        d.v[2] = az;
    }
    float& operator[](int i) { return d.v[i]; }
    // inferred: always inlined in the original (no symbol)
    void EndianSwap() {
        ::EndianSwap(d.v[0], true);
        ::EndianSwap(d.v[1], true);
        ::EndianSwap(d.v[2], true);
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CVolBox {
public:
    void EndianSwap();

    unsigned char unknown00[4];
    float m_width;
    float m_depth;
    float m_height;
    CVector3 m_rows[3];
    CVector3 m_center;
    unsigned char unknown50[32];
};

struct CObjLocationCollisionBox {
    CVolBox box;
    int field70;
    unsigned char unknown74[12];
};

struct CObjLocationCollisionData {
    CObjLocationCollisionBox boxes[24];
    int count;
    int field0c04;
};

void EndianSwap(CObjLocationCollisionData&);
void EndianSwap(CObjLocationCollisionBox&);

class CObjLocationCollisionMap {
public:
    CObjLocationCollisionMap(const CObjLocationCollisionData&);

    unsigned char unknown000[0x124];
};

class CTexturePack {
public:
    CTexturePack();
    void* FindTexture(char*);
};

class CStaticMesh {
public:
    CStaticMesh(StaticMesh*, bool);

    unsigned char unknown00[12];
};

class CDMesh {
public:
    void Init(bool);
    void BindTextures(CTexturePack*, char*);
    void BindSkeleton(CharSkel_t*);

    unsigned char unknown00[4];
    unsigned char m_major;
    unsigned char m_minor;
    unsigned short m_revision;
    unsigned char unknown08[32];
    unsigned int m_partCount;
    unsigned char* m_parts;
    unsigned int m_nameCount;
    char (*m_names)[16];
};

class CSprite {
public:
    enum ESpriteLoadType {};
    CSprite(const char*, ESpriteLoadType, ERenderPriority);

    unsigned char unknown00[100];
};

class CShellMenu {
public:
    unsigned char Get_currentDifficulty();

    unsigned char unknown0000[6620];
};

extern CShellMenu g_Shell;

/* Inferred: the cheat flags (the one at +9 picks random static meshes). */
struct CheatsView {
    unsigned char unknown00[9];
    unsigned char m_randomStaticMeshes;
    unsigned char unknown0a[10];
};

extern CheatsView g_cheats;

static int g_staticObjCount;

/* Inferred: a level contents field (a tag word, then the value). */
struct LevelField {
    int tag;
    union {
        int i;
        float f;
        const char* s;
    };
};

/* Inferred: a level resource's record as {tag, value} fields. */
struct LevelFileInfo {
    LevelField fields[27];
};

/* Inferred: the same record by name: file names and values (the value words of the fields),
   and the texture pack's name at +228. */
struct LevelObjectInfo {
    unsigned char unknown00[4];
    const char* m_skeletonName;
    unsigned char unknown08[4];
    const char* m_meshName;
    unsigned char unknown10[4];
    char* m_textureName;
    unsigned char unknown18[4];
    const char* m_mesh2Name;
    unsigned char unknown20[4];
    char* m_texture2Name;
    unsigned char unknown28[4];
    int m_value2c;
    unsigned char unknown30[4];
    int m_value34;
    unsigned char unknown38[4];
    const char* m_staticMeshName;
    unsigned char unknown40[4];
    float m_value44;
    unsigned char unknown48[4];
    float m_value4c;
    unsigned char unknown50[4];
    float m_value54;
    unsigned char unknown58[4];
    const char* m_collisionHard;
    unsigned char unknown60[4];
    const char* m_collisionMedium;
    unsigned char unknown68[4];
    const char* m_collisionEasy;
    unsigned char unknown70[4];
    int m_value74;
    unsigned char unknown78[108];
    const char* m_texturePack;
};

/* Inferred: a player weapon's level contents info. */
struct LevelWeaponInfo {
    unsigned char unknown00[4];
    const char* m_skeletonName;
    unsigned char unknown08[4];
    const char* m_meshName;
    unsigned char unknown10[44];
    int m_value3c;
};

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    union {
        LevelFileInfo* m_info;
        LevelObjectInfo* m_object;
    };
};

/* Inferred: a weapon's contents carry a weapon info record. */
inline LevelWeaponInfo* WeaponInfo(LevelFileContentsStruct_* contents) {
    return (LevelWeaponInfo*)contents->m_info;
}


/* Inferred: an object definition (80 bytes): the resources, the soldier values, the collision
   data (with a map it owns unless cached), the id the definitions are sorted by, and the
   level contents. */
class CAnimObjDef {
public:
    CAnimObjDef() {}

    CharSkel_t* m_skeleton;
    CDMesh* m_mesh;
    CDMesh* m_mesh2;
    void* m_texture;
    unsigned char m_bodyPart;
    unsigned char unknown11[3];
    void* m_texture2;
    CStaticMesh* m_staticMesh;
    unsigned char unknown1c[4];
    CVector3 m_helmetOffset;
    int m_value30;
    int m_value34;
    CObjLocationCollisionData* m_collisionData;
    CObjLocationCollisionMap* m_collisionMap;
    bool m_collisionCached;
    unsigned char unknown41[3];
    int m_id;
    LevelFileContentsStruct_* m_contents;
    unsigned char unknown4c[4];
};

class CAnimated {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void SetSkeleton(CharSkel_t*);
    void BindMesh(CDMesh*);
};

class CAnimObject {
public:
    CAnimObject();
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual void vf0cc();
    virtual void vf0d0();
    virtual void vf0d4();
    virtual void vf0d8();
    virtual void vf0dc();
    virtual void vf0e0();
    virtual void vf0e4();
    virtual void vf0e8();
    virtual void vf0ec();
    virtual void vf0f0();
    virtual void vf0f4();
    virtual void vf0f8();
    virtual void vf0fc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual void vf120();
    virtual void Init();
    void BindLocationCollision(CObjLocationCollisionData*, CObjLocationCollisionMap*);
    void SetNodeState(long, long);

    unsigned char unknown004[40];
    CAnimObject* m_next;
    CAnimated m_animated;
};

class CSoldierObject : public CAnimObject {
public:
    CSoldierObject();
    void AttachMesh(CStaticMesh*, char*, int);
    void SetHelmetOffset(CVector3&);

    unsigned char unknown034[12656 - 52];
    CAnimated m_head;
};

class CAnimatedPlayer : public CSoldierObject {
public:
    CAnimatedPlayer();
};

class CPlayerWeaponObject : public CAnimObject {
public:
    CPlayerWeaponObject();
    void SetWeaponName(const char*);
};

class CAnimObjectFactory {
public:
    CAnimObjectFactory();
    void Init(unsigned long, char*);
    void Shutdown();
    void Reset();
    void SetLargestObjectSize(int size);
    void ReleaseObject(CAnimObject* object);
    CAnimObject* AllocateObject(unsigned long, EObjectClass);
    void InitPlayerWeaponFromLevelContentsStruct(LevelFileContentsStruct_*, int);
    void InitAnimObjectFromLevelContentsStruct(LevelFileContentsStruct_*, int);

    CAnimObjDef* m_entries;
    char* m_pool;
    CAnimObject* m_free;
    union {
        void* m_textures;
        CTexturePack* m_texturePack;
    };
    int m_objectSize;
    union {
        int m_count;
        unsigned int m_numDefs;
    };
    int m_maxObjects;
};

inline void* operator new(unsigned long, void* place) {
    return place;
}

#define FIELD(o, n) ((LevelField*)((char*)(o) + 0))
#define AT(type, o, off) (*(type*)((char*)(o) + (off)))

#define INIT_SOLDIER(object_, entry) \
    CSoldierObject* object = (CSoldierObject*)object_; \
    object->m_head.BindMesh(entry->m_mesh2); \
    object->m_head.SetSkeleton(entry->m_skeleton); \
    AT(int, object, 17104) = entry->m_value30; \
    object->SetNodeState(1, entry->m_value34); \
    object->AttachMesh(entry->m_staticMesh, "head", 0); \
    object->SetHelmetOffset(entry->m_helmetOffset); \
    AT(void*, object, 12596) = entry->m_texture; \
    AT(void*, object, 12600) = entry->m_texture2; \
    AT(unsigned char, object, 12604) = entry->m_bodyPart; \
    if (entry->m_contents->m_info->fields[15].i) { \
        AT(float, object, 17324) = (float)entry->m_contents->m_info->fields[17].i / 100.0f; \
        AT(int, object, 17320) = entry->m_contents->m_info->fields[18].i; \
        AT(int, object, 17316) = entry->m_contents->m_info->fields[16].i;


void CAnimObjectFactory::SetLargestObjectSize(int size) {
    if (size & 15)
        size += 16 - (size & 15);
    m_objectSize = size;
}

void CAnimObjectFactory::ReleaseObject(CAnimObject* object) {
    object->m_next = m_free;
    m_free = object;
}

CAnimObject* CAnimObjectFactory::AllocateObject(unsigned long id, EObjectClass type) {
    CAnimObject* object = 0;
    if (m_free) {
    CAnimObject* memory = m_free;
    m_free = m_free->m_next;
    object = memory;
    CAnimObjDef key;
    key.m_id = id;
    CAnimObjDef* entry = (CAnimObjDef*)bsearch(&key, m_entries, m_count, sizeof(CAnimObjDef), CompareObjectDefinitions);
    switch (type) {
    case 0:
        object = new (memory) CAnimObject;
        break;
    case 1:
        object = new (memory) CSoldierObject;
        break;
    case 4:
        object = new (memory) CAnimatedPlayer;
        break;
    case 2:
        break;
    case 3: {
        object = new (memory) CPlayerWeaponObject;
        CPlayerWeaponObject* weapon = (CPlayerWeaponObject*)object;
        LevelFileContentsStruct_* contents = entry->m_contents;
        CVector3 scale;
        scale.Set(contents->m_info->fields[9].f, contents->m_info->fields[10].f, contents->m_info->fields[11].f);
        AT(CVector3, weapon, 12536) = scale;
        scale.Set(contents->m_info->fields[12].f, contents->m_info->fields[13].f, contents->m_info->fields[14].f);
        AT(CVector3, weapon, 12552) = scale;
        scale.Set(contents->m_info->fields[15].f, contents->m_info->fields[16].f, contents->m_info->fields[17].f);
        if (!scale.d.v[0])
            scale.d.v[0] = 1.0f;
        if (!scale.d.v[1])
            scale.d.v[1] = 1.0f;
        if (!scale.d.v[2])
            scale.d.v[2] = 1.0f;
        AT(CVector3, weapon, 12568) = scale;
        float range = contents->m_info->fields[19].f;
        AT(float, weapon, 12672) = range > 0.0f ? range : 1.0f;
        char* sprites = (char*)object + 12768;
        AT(int, weapon, 12708) = contents->m_info->fields[18].i;
        AT(int, weapon, 12716) = contents->m_info->fields[20].i;
        AT(CSprite*, weapon, 12520) = 0;
        if (contents->m_info->fields[3].s) {
            AT(CSprite*, weapon, 12520) = new (sprites) CSprite(contents->m_info->fields[3].s, (CSprite::ESpriteLoadType)1, (ERenderPriority)10);
            sprites += 100;
        }
        AT(CSprite*, weapon, 12524) = 0;
        if (contents->m_info->fields[4].s) {
            AT(CSprite*, weapon, 12524) = new (sprites) CSprite(contents->m_info->fields[4].s, (CSprite::ESpriteLoadType)1, (ERenderPriority)7);
            sprites += 100;
        }
        AT(CSprite*, weapon, 12528) = 0;
        if (contents->m_info->fields[5].s)
            AT(CSprite*, weapon, 12528) = new (sprites) CSprite(contents->m_info->fields[5].s, (CSprite::ESpriteLoadType)1, (ERenderPriority)10);
        float x, y, z;
        z = contents->m_info->fields[24].f;
        y = contents->m_info->fields[22].f;
        x = contents->m_info->fields[23].f;
        if (z == 0.0f)
            z = 1.0f;
        AT(float, weapon, 12684) = x;
        AT(float, weapon, 12688) = y;
        AT(float, weapon, 12692) = z;
        AT(float, weapon, 12696) = contents->m_info->fields[21].f;
        AT(float, weapon, 12676) = contents->m_info->fields[25].f;
        AT(float, weapon, 12680) = contents->m_info->fields[26].f;
        AT(int, weapon, 12704) = contents->m_info->fields[8].i;
        weapon->SetWeaponName(contents->m_info->fields[6].s);
        break;
    }
    }
    CAnimObject* object0 = object;
    object->Init();
    object->m_animated.SetSkeleton(entry->m_skeleton);
    object->m_animated.BindMesh(entry->m_mesh);
    object->BindLocationCollision(entry->m_collisionData, entry->m_collisionMap);
    if (type == 1) {
        INIT_SOLDIER(object0, entry)
        }
        AT(bool, object, 17456) = entry->m_contents->m_info->fields[19].i == 0;
    } else if (type == 4) {
        INIT_SOLDIER(object0, entry)
        }
    }
    }
    return object;
}

void CAnimObjectFactory::InitPlayerWeaponFromLevelContentsStruct(LevelFileContentsStruct_* contents, int index) {
    bool cached = false;
    int size;
    m_entries[index].m_contents = contents;
    m_entries[index].m_id = WeaponInfo(contents)->m_value3c;
    m_entries[index].m_value34 = -1;
    const char* name = WeaponInfo(contents)->m_meshName;
    m_entries[index].m_mesh = (CDMesh*)TLT_LoadFromRepository(name, &size, &cached);
    CDMesh* mesh = m_entries[index].m_mesh;
    if (mesh && (mesh->m_major != 6 || mesh->m_minor != 1 || mesh->m_revision != 3)) {
        printf("WARNING! \"%s\" has an incompatible format for this executable. File ignored.\n", name);
        m_entries[index].m_mesh = 0;
    }
    if (!cached && m_entries[index].m_mesh) {
        m_entries[index].m_mesh->Init(true);
        m_entries[index].m_skeleton = (CharSkel_t*)TLT_LoadFromRepository(WeaponInfo(contents)->m_skeletonName, &size, &cached);
        if (!cached)
            CharRelocateSkeleton(m_entries[index].m_skeleton);
        m_entries[index].m_mesh->BindTextures(m_texturePack, 0);
        m_entries[index].m_mesh->BindSkeleton(m_entries[index].m_skeleton);
    } else {
        m_entries[index].m_skeleton = (CharSkel_t*)TLT_LoadFromRepository(WeaponInfo(contents)->m_skeletonName, &size, &cached);
        if (!m_entries[index].m_mesh && !cached)
            CharRelocateSkeleton(m_entries[index].m_skeleton);
    }
    m_entries[index].m_value30 = -1;
    m_entries[index].m_mesh2 = 0;
    m_entries[index].m_collisionData = 0;
    m_entries[index].m_collisionMap = 0;
}

void CAnimObjectFactory::InitAnimObjectFromLevelContentsStruct(LevelFileContentsStruct_* contents, int index) {
    bool cached = false;
    int size;
    m_entries[index].m_contents = contents;
    m_entries[index].m_id = contents->m_object->m_value74;
    m_entries[index].m_value34 = contents->m_object->m_value34;
    m_entries[index].m_mesh = (CDMesh*)TLT_LoadFromRepository(contents->m_object->m_meshName, &size, &cached);
    if (m_entries[index].m_mesh) {
        if (m_texturePack && contents->m_object->m_textureName)
            m_entries[index].m_texture = m_texturePack->FindTexture(contents->m_object->m_textureName);
        else
            m_entries[index].m_texture = 0;
        if (m_texturePack && contents->m_object->m_texture2Name)
            m_entries[index].m_texture2 = m_texturePack->FindTexture(contents->m_object->m_texture2Name);
        else
            m_entries[index].m_texture2 = 0;
        CDMesh* mesh = m_entries[index].m_mesh;
        if (mesh->m_major != 6 || mesh->m_minor != 1 || mesh->m_revision != 3) {
            printf("WARNING! \"%s\" has an incompatible format for this executable. File ignored.\n", contents->m_object->m_meshName);
            m_entries[index].m_mesh = 0;
            m_entries[index].m_texture = 0;
            m_entries[index].m_texture2 = 0;
        }
    } else {
        m_entries[index].m_texture = 0;
        m_entries[index].m_texture2 = 0;
    }
    if (!cached && m_entries[index].m_mesh) {
        m_entries[index].m_mesh->Init(false);
        m_entries[index].m_skeleton = (CharSkel_t*)TLT_LoadFromRepository(contents->m_object->m_skeletonName, &size, &cached);
        if (!cached)
            CharRelocateSkeleton(m_entries[index].m_skeleton);
        m_entries[index].m_mesh->BindTextures(m_texturePack, contents->m_object->m_textureName);
        m_entries[index].m_mesh->BindSkeleton(m_entries[index].m_skeleton);
    } else {
        m_entries[index].m_skeleton = (CharSkel_t*)TLT_LoadFromRepository(contents->m_object->m_skeletonName, &size, &cached);
        if (!m_entries[index].m_mesh && !cached)
            CharRelocateSkeleton(m_entries[index].m_skeleton);
    }
    if (contents->m_object->m_mesh2Name) {
        m_entries[index].m_value30 = contents->m_object->m_value2c;
        m_entries[index].m_mesh2 = (CDMesh*)TLT_LoadFromRepository(contents->m_object->m_mesh2Name, &size, &cached);
        CDMesh* mesh2 = m_entries[index].m_mesh2;
        if (mesh2->m_major != 6 || mesh2->m_minor != 1 || mesh2->m_revision != 3) {
            printf("WARNING! \"%s\" has an incompatible format for this executable. File ignored.\n", contents->m_object->m_mesh2Name);
            m_entries[index].m_value30 = -1;
            m_entries[index].m_mesh2 = 0;
        }
        if (!cached && m_entries[index].m_mesh2) {
            m_entries[index].m_mesh2->Init(false);
            m_entries[index].m_mesh2->BindTextures(m_texturePack, contents->m_object->m_texture2Name);
            m_entries[index].m_mesh2->BindSkeleton(m_entries[index].m_skeleton);
        }
    } else {
        m_entries[index].m_value30 = -1;
        m_entries[index].m_mesh2 = 0;
    }
    if (contents->m_object->m_staticMeshName) {
        bool found = false;
        if (g_cheats.m_randomStaticMeshes && g_staticObjCount > 0) {
            int n = 1;
            int pick = (int)MathFunRandomI64(1, g_staticObjCount);
            LevelFileContentsStruct_* resource;
            for (resource = TLT_FindNextResourceByType((TLTResourceID)10, 0); resource;
                 resource = TLT_FindNextResourceByType((TLTResourceID)10, resource)) {
                const char* name = resource->m_object->m_skeletonName;
                if (name) {
                    if (!stricmp(&name[strlen(name) - 3], "msh")) {
                    if (n >= pick)
                        break;
                    n++;
                    }
                }
            }
            if (resource) {
                StaticMesh* data = (StaticMesh*)TLT_LoadFromRepository(resource->m_object->m_skeletonName, &size, &cached);
                m_entries[index].m_staticMesh = new CStaticMesh(data, !cached);
                found = true;
            }
        }
        if (!found) {
            StaticMesh* data = (StaticMesh*)TLT_LoadFromRepository(contents->m_object->m_staticMeshName, &size, &cached);
            m_entries[index].m_staticMesh = new CStaticMesh(data, !cached);
        }
    } else {
        m_entries[index].m_staticMesh = 0;
    }
    m_entries[index].m_helmetOffset[0] = contents->m_object->m_value44;
    m_entries[index].m_helmetOffset[1] = contents->m_object->m_value4c;
    m_entries[index].m_helmetOffset[2] = contents->m_object->m_value54;
    if (m_entries[index].m_mesh) {
        for (unsigned int i = 0; i < m_entries[index].m_mesh->m_partCount; i++) {
            CDMesh* mesh = m_entries[index].m_mesh;
            unsigned int name = *(unsigned int*)(mesh->m_parts + i * 0x38 + 0x28);
            if (name < mesh->m_nameCount && !stricmp(mesh->m_names[name], "mohf_body")) {
                m_entries[index].m_bodyPart = i;
                i = m_entries[index].m_mesh->m_partCount;
            }
        }
    }
    if (contents->m_object->m_collisionHard) {
        const char* collision;
        switch (g_Shell.Get_currentDifficulty()) {
        case 1:
            collision = contents->m_object->m_collisionMedium;
            break;
        case 2:
            collision = contents->m_object->m_collisionEasy;
            break;
        case 3:
            collision = contents->m_object->m_collisionHard;
            break;
        default:
            DebugMsg("Unknown difficulty level.  Assuming Level Hard\n");
            collision = contents->m_object->m_collisionHard;
            break;
        }
        if (!collision)
            collision = contents->m_object->m_collisionHard;
        CObjLocationCollisionData* data = (CObjLocationCollisionData*)TLT_LoadFromRepository(collision, &size, &cached);
        m_entries[index].m_collisionData = data;
        if (!cached) {
            EndianSwap(*data);
            m_entries[index].m_collisionMap = new CObjLocationCollisionMap(*data);
        } else {
            m_entries[index].m_collisionMap = 0;
            for (int j = 0; j < index; j++) {
                if (m_entries[j].m_collisionData == data) {
                    m_entries[index].m_collisionMap = m_entries[j].m_collisionMap;
                    break;
                }
            }
        }
            m_entries[index].m_collisionCached = cached;
    } else {
        m_entries[index].m_collisionData = 0;
        m_entries[index].m_collisionMap = 0;
        m_entries[index].m_collisionCached = false;
    }
}

__declspec(weak) void EndianSwap(CObjLocationCollisionData& data) {
    ChangeEndian(data.count);
    ChangeEndian(data.field0c04);
    for (int i = 0; i < data.count; i++)
        EndianSwap(data.boxes[i]);
}

__declspec(weak) void EndianSwap(CObjLocationCollisionBox& box) {
    box.box.EndianSwap();
    ChangeEndian(box.field70);
}

__declspec(weak) void CVolBox::EndianSwap() {
    ::EndianSwap(m_width, true);
    ::EndianSwap(m_depth, true);
    ::EndianSwap(m_height, true);
    m_rows[0].EndianSwap();
    m_rows[1].EndianSwap();
    m_rows[2].EndianSwap();
    m_center.EndianSwap();
}

void CAnimObjectFactory::Shutdown() {
    for (unsigned int i = 0; i < m_numDefs; i++) {
        if (m_entries[i].m_collisionMap && !m_entries[i].m_collisionCached)
            delete (char*)m_entries[i].m_collisionMap;
    }
    MEM_free(m_pool);
    delete[] m_entries;
}

void CAnimObjectFactory::Reset() {
    m_free = (CAnimObject*)m_pool;
    CAnimObject* cur;
    CAnimObject* obj = (CAnimObject*)m_pool;
    CAnimObject* last = 0;
    for (int i = 0; i < m_maxObjects - 1; i++) {
        cur = obj;
        obj = (CAnimObject*)((char*)obj + m_objectSize);
        cur->m_next = obj;
        last = obj;
    }
    last->m_next = 0;
}

void CAnimObjectFactory::Init(unsigned long maxObjects, char*) {
    LevelFileContentsStruct_* entry;
    const char* name;
    LevelFileContentsStruct_* item;
    LevelFileContentsStruct_* weapon;
    LevelFileContentsStruct_* mesh;
    int count;
    int index;
    int size;
    bool cached;
    m_maxObjects = maxObjects;
    entry = TLT_FindNextResourceByType((TLTResourceID)13, 0);
    m_textures = TLT_LoadFromRepository(entry->m_object->m_texturePack, &size, &cached);
    m_textures = new (m_textures) CTexturePack;
    count = 0;
    for (entry = TLT_FindNextResourceByType((TLTResourceID)9, 0); entry; entry = TLT_FindNextResourceByType((TLTResourceID)9, entry))
        count++;
    for (entry = TLT_FindNextResourceByType((TLTResourceID)19, 0); entry; entry = TLT_FindNextResourceByType((TLTResourceID)19, entry))
        count++;
    for (mesh = TLT_FindNextResourceByType((TLTResourceID)10, 0); mesh; mesh = TLT_FindNextResourceByType((TLTResourceID)10, mesh)) {
        name = mesh->m_object->m_skeletonName;
        if (name && !stricmp(&name[strlen(name) - 3], "msh"))
            g_staticObjCount++;
    }
    m_numDefs = count;
    if (count > 0) {
        m_entries = new CAnimObjDef[count];
        index = 0;
        item = TLT_FindNextResourceByType((TLTResourceID)9, 0);
        while (item) {
            InitAnimObjectFromLevelContentsStruct(item, index);
            item = TLT_FindNextResourceByType((TLTResourceID)9, item);
            index++;
        }
        weapon = TLT_FindNextResourceByType((TLTResourceID)19, 0);
        while (weapon) {
            InitPlayerWeaponFromLevelContentsStruct(weapon, index);
            weapon = TLT_FindNextResourceByType((TLTResourceID)19, weapon);
            index++;
        }
    }
    qsort(m_entries, m_numDefs, sizeof(CAnimObjDef), CompareObjectDefinitions);
    m_free = (CAnimObject*)(m_pool = (char*)DWI_alloc("Anim object factory", m_objectSize * m_maxObjects, 1024));
    CAnimObject* cur;
    CAnimObject* obj = (CAnimObject*)m_pool;
    CAnimObject* last = 0;
    for (int i = 0; i < m_maxObjects - 1; i++) {
        cur = obj;
        obj = (CAnimObject*)((char*)obj + m_objectSize);
        cur->m_next = obj;
        last = obj;
    }
    last->m_next = 0;
}

CAnimObjectFactory::CAnimObjectFactory() {
    m_objectSize = 17600;
}

int CompareObjectDefinitions(const void* a, const void* b) {
    int ka = ((const CAnimObjDef*)a)->m_id;
    int kb = ((const CAnimObjDef*)b)->m_id;
    if (ka < kb)
        return -1;
    return ka > kb;
}
