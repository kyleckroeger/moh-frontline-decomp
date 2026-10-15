// A fragment of AnimObjFactory.cpp (0x80096120): CAnimObjectFactory::
// InitPlayerWeaponFromLevelContentsStruct fills the factory's entry for a
// player weapon from the level contents: the contents and its info's value
// at +0x3c are kept, the mesh is loaded from the repository (and dropped
// with a warning when its format is not 6.1.3), then the skeleton; a freshly
// loaded mesh is initialised and has the factory's texture pack and the
// skeleton bound to it, a freshly loaded skeleton is relocated, and the
// entry's remaining values are reset. The class, functions and
// LevelFileContentsStruct_ are named by the mangled symbols; the entry,
// contents and mesh-header views (members at their offsets, names not
// original) are inferred. The string is an entry of the file's .rodata pool.
// The rest of the file is in other units.
struct CharSkel_t;
class CTexturePack;

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
};

/* Inferred: the level contents' file names and value. */
struct LevelFileInfo {
    unsigned char unknown00[4];
    const char* m_skeletonName;
    unsigned char unknown08[4];
    const char* m_meshName;
    unsigned char unknown10[44];
    int m_value3c;
};

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    LevelFileInfo* m_info;
};

/* Inferred: a factory entry (80 bytes). */
struct AnimObjectEntry {
    CharSkel_t* m_skeleton;
    CDMesh* m_mesh;
    int m_value08;
    unsigned char unknown0c[36];
    int m_value30;
    int m_value34;
    int m_value38;
    int m_value3c;
    unsigned char unknown40[4];
    int m_value44;
    LevelFileContentsStruct_* m_contents;
    unsigned char unknown4c[4];
};

class CAnimObjectFactory {
public:
    void InitPlayerWeaponFromLevelContentsStruct(LevelFileContentsStruct_*, int);

    AnimObjectEntry* m_entries;
    unsigned char unknown04[8];
    CTexturePack* m_texturePack;
};

void CAnimObjectFactory::InitPlayerWeaponFromLevelContentsStruct(LevelFileContentsStruct_* contents, int index) {
    bool cached = false;
    int size;
    m_entries[index].m_contents = contents;
    m_entries[index].m_value44 = contents->m_info->m_value3c;
    m_entries[index].m_value34 = -1;
    const char* name = contents->m_info->m_meshName;
    m_entries[index].m_mesh = (CDMesh*)TLT_LoadFromRepository(name, &size, &cached);
    CDMesh* mesh = m_entries[index].m_mesh;
    if (mesh && (mesh->m_major != 6 || mesh->m_minor != 1 || mesh->m_revision != 3)) {
        printf("WARNING! \"%s\" has an incompatible format for this executable. File ignored.\n", name);
        m_entries[index].m_mesh = 0;
    }
    if (!cached && m_entries[index].m_mesh) {
        m_entries[index].m_mesh->Init(true);
        m_entries[index].m_skeleton = (CharSkel_t*)TLT_LoadFromRepository(contents->m_info->m_skeletonName, &size, &cached);
        if (!cached)
            CharRelocateSkeleton(m_entries[index].m_skeleton);
        m_entries[index].m_mesh->BindTextures(m_texturePack, 0);
        m_entries[index].m_mesh->BindSkeleton(m_entries[index].m_skeleton);
    } else {
        m_entries[index].m_skeleton = (CharSkel_t*)TLT_LoadFromRepository(contents->m_info->m_skeletonName, &size, &cached);
        if (!m_entries[index].m_mesh && !cached)
            CharRelocateSkeleton(m_entries[index].m_skeleton);
    }
    m_entries[index].m_value30 = -1;
    m_entries[index].m_value08 = 0;
    m_entries[index].m_value38 = 0;
    m_entries[index].m_value3c = 0;
}
