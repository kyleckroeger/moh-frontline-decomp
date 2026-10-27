// Endian conversion of the animated-object collision data: the collision data
// (box count and a second word, then each 128-byte box), each box (its volume,
// then one word at +112) and CVolBox (width, depth and height, the three basis
// rows and the centre, 16-byte rows). The names come from the mangled symbols;
// the members are inferred from offsets and the classes are non-virtual views.
// The conversion helpers are the inlined ones described in propdat.cpp
// (inferred). These functions are inline in the original (weak symbols), so
// they are defined __declspec(weak). They are followed by
// CAnimObjectFactory::Shutdown and Reset. The factory, its object definitions
// (80 bytes each) and CAnimObject are views: their members are inferred from
// offsets (+44 links free objects). CAnimObjectFactory::Init follows: it loads
// the level's texture pack in place, counts the animated objects (resource 9)
// and player weapons (19), counts the static meshes (resource 10 named *.msh)
// in g_staticObjCount, builds and sorts the object definitions and allocates
// the object pool (its free list built as Reset does). The level-contents
// records are inferred views; CAnimObjDef's constructor is the weak copy
// placed by AnimObjFactory_animobjdef.cpp. The rest of the file is not part
// of this unit.
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

// The game's 16-byte vector; its EndianSwap is always inlined (no symbol), so
// the method here is inferred.
struct CVector3 {
    void EndianSwap() {
        ::EndianSwap(x, true);
        ::EndianSwap(y, true);
        ::EndianSwap(z, true);
    }

    float x;
    float y;
    float z;
    float w;
};

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

extern "C" void MEM_free(void*);

enum TLTResourceID {};

/* inferred: a level-contents entry's record (names at +4 and +228) */
struct LevelFileInfo {
    unsigned char unknown00[4];
    const char* m_name;
    unsigned char unknown08[220];
    const char* m_texturePack;
};

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    LevelFileInfo* m_info;
};

LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
char* TLT_LoadFromRepository(const char*, int*, bool*);
void* DWI_alloc(const char*, int, int);
extern "C" unsigned long strlen(const char*);
extern "C" int stricmp(const char*, const char*);
extern "C" void qsort(void*, unsigned long, unsigned long, int (*)(const void*, const void*));
int CompareObjectDefinitions(const void*, const void*);

inline void* operator new(unsigned long, void* p) {
    return p;
}

class CTexturePack {
public:
    CTexturePack();
};

extern int g_staticObjCount;

class CAnimObject {
public:
    unsigned char unknown00[44];
    CAnimObject* m_next;
};

class CAnimObjDef {
public:
    CAnimObjDef() {}

    unsigned char unknown00[60];
    void* m_data;
    bool m_shared;
    unsigned char unknown41[3];
    int m_key;
    unsigned char unknown48[8];
};

class CAnimObjectFactory {
public:
    CAnimObjectFactory();
    void Init(unsigned long, char*);
    void InitAnimObjectFromLevelContentsStruct(LevelFileContentsStruct_*, int);
    void InitPlayerWeaponFromLevelContentsStruct(LevelFileContentsStruct_*, int);
    void Shutdown();
    void Reset();

    CAnimObjDef* m_defs;
    char* m_pool;
    CAnimObject* m_free;
    void* m_textures;
    int m_objectSize;
    unsigned int m_numDefs;
    int m_maxObjects;
};

void EndianSwap(CObjLocationCollisionBox&);

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
        if (m_defs[i].m_data && !m_defs[i].m_shared)
            delete (char*)m_defs[i].m_data;
    }
    MEM_free(m_pool);
    delete[] m_defs;
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
    m_textures = TLT_LoadFromRepository(entry->m_info->m_texturePack, &size, &cached);
    m_textures = new (m_textures) CTexturePack;
    count = 0;
    for (entry = TLT_FindNextResourceByType((TLTResourceID)9, 0); entry; entry = TLT_FindNextResourceByType((TLTResourceID)9, entry))
        count++;
    for (entry = TLT_FindNextResourceByType((TLTResourceID)19, 0); entry; entry = TLT_FindNextResourceByType((TLTResourceID)19, entry))
        count++;
    for (mesh = TLT_FindNextResourceByType((TLTResourceID)10, 0); mesh; mesh = TLT_FindNextResourceByType((TLTResourceID)10, mesh)) {
        name = mesh->m_info->m_name;
        if (name && !stricmp(&name[strlen(name) - 3], "msh"))
            g_staticObjCount++;
    }
    m_numDefs = count;
    if (count > 0) {
        m_defs = new CAnimObjDef[count];
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
    qsort(m_defs, m_numDefs, sizeof(CAnimObjDef), CompareObjectDefinitions);
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
