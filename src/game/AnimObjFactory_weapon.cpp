// A fragment of AnimObjFactory.cpp (0x80096120): CAnimObjectFactory::
// InitPlayerWeaponFromLevelContentsStruct and InitAnimObjectFromLevelContentsStruct
// fill the factory's entry for a level resource. For a player weapon: the
// contents and its info's value at +0x3c are kept, the mesh is loaded from
// the repository (and dropped with a warning when its format is not 6.1.3),
// then the skeleton; a freshly loaded mesh is initialised and has the
// factory's texture pack and the skeleton bound to it, a freshly loaded
// skeleton is relocated, and the entry's remaining values are reset. For an
// animated object the same is done with the object's two textures, then an
// optional second mesh, a static mesh (with the random-static-mesh cheat, a
// random "msh" resource of the level instead), three values, the index of
// the mesh's "mohf_body" part, and the collision data for the current
// difficulty (a fresh one gets a collision map; a cached one shares an
// earlier entry's map). The classes, functions, LevelFileContentsStruct_,
// g_Shell, g_cheats and g_staticObjCount (file-local in the original,
// declared extern here) are named by the symbols; the entry, contents, info,
// mesh-header and cheat views (members at their offsets, names not
// original) are inferred. The strings and the prime table are entries of the
// file's .rodata pool, "msh" of its .sdata2 pool. The rest of the file is in
// other units.
// Header-defined hash-table primes; every unit that includes the header carries a copy
// (this file's .rodata starts with it, before the strings).
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

struct CharSkel_t;
struct StaticMesh;
struct CObjLocationCollisionData;

class CTexturePack {
public:
    void* FindTexture(char*);
};

class CStaticMesh {
public:
    CStaticMesh(StaticMesh*, bool);

    unsigned char unknown00[12];
};

class CObjLocationCollisionMap {
public:
    CObjLocationCollisionMap(const CObjLocationCollisionData&);

    unsigned char unknown000[0x124];
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
extern int g_staticObjCount; // file-local in the original

enum TLTResourceID {};
struct LevelFileContentsStruct_;
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
long long MathFunRandomI64(long long, long long);
void EndianSwap(CObjLocationCollisionData&);
void DebugMsg(const char*, ...);
extern "C" unsigned long strlen(const char*);
extern "C" int stricmp(const char*, const char*);

void* TLT_LoadFromRepository(const char*, int*, bool*);
void CharRelocateSkeleton(CharSkel_t*);
extern "C" int printf(const char*, ...);

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

/* Inferred: the level contents' file names and value. */
struct LevelFileInfo {
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

struct LevelFileContentsStruct_;
inline LevelWeaponInfo* WeaponInfo(LevelFileContentsStruct_* contents);

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    LevelFileInfo* m_info;
};

/* Inferred: three floats with indexed access. */
struct LevelVector {
    float& operator[](int i) { return (&x)[i]; }
    float x;
    float y;
    float z;
};

/* Inferred: a factory entry (80 bytes). */
struct AnimObjectEntry {
    CharSkel_t* m_skeleton;
    CDMesh* m_mesh;
    CDMesh* m_mesh2;
    void* m_texture;
    unsigned char m_bodyPart;
    unsigned char unknown11[3];
    void* m_texture2;
    CStaticMesh* m_staticMesh;
    unsigned char unknown1c[4];
    LevelVector m_values20;
    unsigned char unknown2c[4];
    int m_value30;
    int m_value34;
    CObjLocationCollisionData* m_collisionData;
    CObjLocationCollisionMap* m_collisionMap;
    bool m_collisionCached;
    unsigned char unknown41[3];
    int m_value44;
    LevelFileContentsStruct_* m_contents;
    unsigned char unknown4c[4];
};

/* Inferred: a weapon's contents carry a weapon info record. */
inline LevelWeaponInfo* WeaponInfo(LevelFileContentsStruct_* contents) {
    return (LevelWeaponInfo*)contents->m_info;
}

class CAnimObjectFactory {
public:
    void InitPlayerWeaponFromLevelContentsStruct(LevelFileContentsStruct_*, int);
    void InitAnimObjectFromLevelContentsStruct(LevelFileContentsStruct_*, int);

    AnimObjectEntry* m_entries;
    unsigned char unknown04[8];
    CTexturePack* m_texturePack;
};

void CAnimObjectFactory::InitPlayerWeaponFromLevelContentsStruct(LevelFileContentsStruct_* contents, int index) {
    bool cached = false;
    int size;
    m_entries[index].m_contents = contents;
    m_entries[index].m_value44 = WeaponInfo(contents)->m_value3c;
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
    m_entries[index].m_value44 = contents->m_info->m_value74;
    m_entries[index].m_value34 = contents->m_info->m_value34;
    m_entries[index].m_mesh = (CDMesh*)TLT_LoadFromRepository(contents->m_info->m_meshName, &size, &cached);
    if (m_entries[index].m_mesh) {
        if (m_texturePack && contents->m_info->m_textureName)
            m_entries[index].m_texture = m_texturePack->FindTexture(contents->m_info->m_textureName);
        else
            m_entries[index].m_texture = 0;
        if (m_texturePack && contents->m_info->m_texture2Name)
            m_entries[index].m_texture2 = m_texturePack->FindTexture(contents->m_info->m_texture2Name);
        else
            m_entries[index].m_texture2 = 0;
        CDMesh* mesh = m_entries[index].m_mesh;
        if (mesh->m_major != 6 || mesh->m_minor != 1 || mesh->m_revision != 3) {
            printf("WARNING! \"%s\" has an incompatible format for this executable. File ignored.\n", contents->m_info->m_meshName);
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
        m_entries[index].m_skeleton = (CharSkel_t*)TLT_LoadFromRepository(contents->m_info->m_skeletonName, &size, &cached);
        if (!cached)
            CharRelocateSkeleton(m_entries[index].m_skeleton);
        m_entries[index].m_mesh->BindTextures(m_texturePack, contents->m_info->m_textureName);
        m_entries[index].m_mesh->BindSkeleton(m_entries[index].m_skeleton);
    } else {
        m_entries[index].m_skeleton = (CharSkel_t*)TLT_LoadFromRepository(contents->m_info->m_skeletonName, &size, &cached);
        if (!m_entries[index].m_mesh && !cached)
            CharRelocateSkeleton(m_entries[index].m_skeleton);
    }
    if (contents->m_info->m_mesh2Name) {
        m_entries[index].m_value30 = contents->m_info->m_value2c;
        m_entries[index].m_mesh2 = (CDMesh*)TLT_LoadFromRepository(contents->m_info->m_mesh2Name, &size, &cached);
        CDMesh* mesh2 = m_entries[index].m_mesh2;
        if (mesh2->m_major != 6 || mesh2->m_minor != 1 || mesh2->m_revision != 3) {
            printf("WARNING! \"%s\" has an incompatible format for this executable. File ignored.\n", contents->m_info->m_mesh2Name);
            m_entries[index].m_value30 = -1;
            m_entries[index].m_mesh2 = 0;
        }
        if (!cached && m_entries[index].m_mesh2) {
            m_entries[index].m_mesh2->Init(false);
            m_entries[index].m_mesh2->BindTextures(m_texturePack, contents->m_info->m_texture2Name);
            m_entries[index].m_mesh2->BindSkeleton(m_entries[index].m_skeleton);
        }
    } else {
        m_entries[index].m_value30 = -1;
        m_entries[index].m_mesh2 = 0;
    }
    if (contents->m_info->m_staticMeshName) {
        bool found = false;
        if (g_cheats.m_randomStaticMeshes && g_staticObjCount > 0) {
            int n = 1;
            int pick = (int)MathFunRandomI64(1, g_staticObjCount);
            LevelFileContentsStruct_* resource;
            for (resource = TLT_FindNextResourceByType((TLTResourceID)10, 0); resource;
                 resource = TLT_FindNextResourceByType((TLTResourceID)10, resource)) {
                const char* name = resource->m_info->m_skeletonName;
                if (name) {
                    if (!stricmp(&name[strlen(name) - 3], "msh")) {
                    if (n >= pick)
                        break;
                    n++;
                    }
                }
            }
            if (resource) {
                StaticMesh* data = (StaticMesh*)TLT_LoadFromRepository(resource->m_info->m_skeletonName, &size, &cached);
                m_entries[index].m_staticMesh = new CStaticMesh(data, !cached);
                found = true;
            }
        }
        if (!found) {
            StaticMesh* data = (StaticMesh*)TLT_LoadFromRepository(contents->m_info->m_staticMeshName, &size, &cached);
            m_entries[index].m_staticMesh = new CStaticMesh(data, !cached);
        }
    } else {
        m_entries[index].m_staticMesh = 0;
    }
    m_entries[index].m_values20[0] = contents->m_info->m_value44;
    m_entries[index].m_values20[1] = contents->m_info->m_value4c;
    m_entries[index].m_values20[2] = contents->m_info->m_value54;
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
    if (contents->m_info->m_collisionHard) {
        const char* collision;
        switch (g_Shell.Get_currentDifficulty()) {
        case 1:
            collision = contents->m_info->m_collisionMedium;
            break;
        case 2:
            collision = contents->m_info->m_collisionEasy;
            break;
        case 3:
            collision = contents->m_info->m_collisionHard;
            break;
        default:
            DebugMsg("Unknown difficulty level.  Assuming Level Hard\n");
            collision = contents->m_info->m_collisionHard;
            break;
        }
        if (!collision)
            collision = contents->m_info->m_collisionHard;
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
