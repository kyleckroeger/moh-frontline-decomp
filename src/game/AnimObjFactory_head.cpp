// The start of AnimObjFactory.cpp (0x80095ba0): CAnimObjectFactory::SetLargestObjectSize
// (rounded up to 16 bytes), ReleaseObject (the object is pushed on the free
// list through m_next) and AllocateObject (pops the free list, finds the
// definition with bsearch and constructs the object class in place). The classes
// are views; members and their names are inferred from offsets. The rest of the
// file (the weapon code onwards) is not part of this unit.
inline void* operator new(unsigned long, void* place) {
    return place;
}

extern "C" void* bsearch(const void*, const void*, unsigned long, unsigned long, int (*)(const void*, const void*));
int CompareObjectDefinitions(const void*, const void*);

enum EObjectClass {};
enum ERenderPriority {};
struct CharSkel_t;
class CDMesh;
class CStaticMesh;
class CObjLocationCollisionData;
class CObjLocationCollisionMap;

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

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CSprite {
public:
    enum ESpriteLoadType {};
    CSprite(const char*, ESpriteLoadType, ERenderPriority);

    unsigned char unknown00[100];
};

/* Inferred: a level contents field (a tag word, then the value). */
struct LevelField {
    int tag;
    union {
        int i;
        float f;
        const char* s;
    };
};

struct LevelFileInfo {
    LevelField fields[27];
};

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    LevelFileInfo* m_info;
};

/* Inferred: a factory entry (80 bytes), as in AnimObjFactory_weapon.cpp. */
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
    void SetLargestObjectSize(int size);
    void ReleaseObject(CAnimObject* object);
    CAnimObject* AllocateObject(unsigned long, EObjectClass);

    AnimObjectEntry* m_entries;
    char* m_pool;
    CAnimObject* m_free;
    void* m_textures;
    int m_objectSize;
    int m_count;
};

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
    AnimObjectEntry key;
    key.m_id = id;
    AnimObjectEntry* entry = (AnimObjectEntry*)bsearch(&key, m_entries, m_count, sizeof(AnimObjectEntry), CompareObjectDefinitions);
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
