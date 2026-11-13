// bulletfactory.cpp: CBulletFactory::GetBulletWeight (the projectile or thrown
// property table's first value) and DeleteBulletGUISprites, which deletes each
// property's GUI sprite unless an earlier property with the same sprite name
// already owns it, LoadBulletGUISprites, which shares or creates them
// (dropping projectile sprites whose texture failed to load), and
// LoadBulletParameters, which loads the multiplayer or difficulty's parameter
// file, allocates both property tables and fills them from its typed records,
// converting each record's byte order (the byte-order helpers are the inlined
// ones described in propdat.cpp, inferred; the converted fields are named by
// offset); then whether a thrown bullet type can be cooked, a bullet type's
// sprite, destruction through the projectile or thrown helper and creation
// through the helper for the bullet's kind; Shutdown, Reset, Init, the
// destructor and the constructor; last the weak CBulletFactoryHelper members
// for the thrown and the projectile bullets: the constructor (everything
// cleared), empty destructor, Init (keeps the property table, allocates 64
// bullets through the bullet's array new, chains them into the free list and
// sets the flag), Reset (deletes and reallocates the array and chains it
// again), Shutdown (deletes the bullet array), CreateBullet (takes a free
// bullet onto the used list and initialises it with its type's properties) and
// DestroyBullet (releases it and moves it back to the free list). The class,
// template and enum names come from the mangled symbols; the property tables,
// the bullets' list link at +160, the virtual called before a bullet is
// released (slot +296, name unknown), the bullet sizes, the sprite's virtual
// table position (+28), its size and allocator and the factory's members are
// inferred. The helper members are defined out of the class at the end of the
// file; the factory's calls instantiate them (weak, thrown then projectile).
extern "C" int strcmp(const char*, const char*);
void* DWI_alloc(const char*, int, int);
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

#include "game/CVector3.h"

class ISceneNode;
enum EClsnId {};
enum EThrownBulletTypes {};
enum EProjectileBulletTypes {};

class CBullet;

/* inferred: a property record's weight, GUI sprite name and sprite (the
   thrown record's cook flag last) */
struct ProjectileBulletProperties_struct {
    float weight;
    unsigned char unknown04[20];
    char spriteName[20];
    CSprite* sprite;
    unsigned char unknown30[16];
};

struct ThrownBulletProperties_struct {
    float weight;
    unsigned char unknown04[20];
    char spriteName[20];
    CSprite* sprite;
    unsigned char unknown30[27];
    signed char canCook;
};

typedef ProjectileBulletProperties_struct ProjectilePropertiesView;
typedef ThrownBulletProperties_struct ThrownPropertiesView;

/* inferred: the bullet's kind (1: projectile) */
struct BulletKindView {
    unsigned char unknown00[40];
    int kind;
};

class CThrownBullet {
public:
    virtual ~CThrownBullet();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    void Init(EThrownBulletTypes, ThrownBulletProperties_struct*, const CVector3&, const CVector3&, float, EClsnId, float,
              ISceneNode*);

    static void* operator new[](unsigned long);
    CThrownBullet();

    unsigned char unknown004[156];
    CThrownBullet* m_next;
    unsigned char padding0a4[348];
};

class CProjectileBullet {
public:
    virtual ~CProjectileBullet();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    void Init(EProjectileBulletTypes, ProjectileBulletProperties_struct*, const CVector3&, const CVector3&, float, EClsnId,
              float, ISceneNode*);

    static void* operator new[](unsigned long);
    CProjectileBullet();

    unsigned char unknown004[156];
    CProjectileBullet* m_next;
    unsigned char padding0a4[204];
};

template <class Bullet, class Properties, class Types>
class CBulletFactoryHelper {
public:
    CBulletFactoryHelper();
    ~CBulletFactoryHelper();
    void Init(Properties*);
    void Reset();
    void Shutdown();
    Bullet* CreateBullet(Types, CVector3, CVector3, float, EClsnId, float, ISceneNode*);
    void DestroyBullet(Bullet*);

    Bullet* m_bullets;
    Bullet* m_free;
    Bullet* m_used;
    Properties* m_properties;
    bool m_initialised;
};

class CBulletFactory {
public:
    CBulletFactory();
    ~CBulletFactory();
    float GetBulletWeight(bool, int);
    void DeleteBulletGUISprites();
    void LoadBulletGUISprites();
    void LoadBulletParameters();
    bool CanBulletTypeCook(bool, int);
    void* GetBulletSprite(bool, int);
    void DestroyBullet(CBullet*);
    CBullet* CreateBullet(int, bool, CVector3, CVector3, float, EClsnId, float, ISceneNode*);
    void Shutdown();
    void Reset();
    void Init();

    ProjectileBulletProperties_struct* m_projectileProperties;
    ThrownBulletProperties_struct* m_thrownProperties;
    CBulletFactoryHelper<CProjectileBullet, ProjectileBulletProperties_struct, EProjectileBulletTypes> m_projectiles;
    CBulletFactoryHelper<CThrownBullet, ThrownBulletProperties_struct, EThrownBulletTypes> m_thrown;
    bool m_initialised;
};

float CBulletFactory::GetBulletWeight(bool projectile, int type) {
    if (projectile)
        return m_projectileProperties[type].weight;
    return m_thrownProperties[type].weight;
}

void CBulletFactory::DeleteBulletGUISprites() {
    int i;
    int j;
    for (i = 0; i < 31; i++) {
        char* name = m_projectileProperties[i].spriteName;
        if (name[0]) {
            for (j = 0; j < i; j++) {
                if (strcmp(name, m_projectileProperties[j].spriteName) == 0) {
                    m_projectileProperties[i].sprite = 0;
                    break;
                }
            }
            if (m_projectileProperties[i].sprite) {
                delete m_projectileProperties[i].sprite;
                m_projectileProperties[i].sprite = 0;
            }
        }
    }
    for (i = 0; i < 11; i++) {
        char* name = m_thrownProperties[i].spriteName;
        if (name[0]) {
            for (j = 0; j < i; j++) {
                if (strcmp(name, m_thrownProperties[j].spriteName) == 0) {
                    m_thrownProperties[i].sprite = 0;
                    break;
                }
            }
            if (m_thrownProperties[i].sprite) {
                delete m_thrownProperties[i].sprite;
                m_thrownProperties[i].sprite = 0;
            }
        }
    }
}

void CBulletFactory::LoadBulletGUISprites() {
    int i;
    int j;
    for (i = 0; i < 31; i++) {
        m_projectileProperties[i].sprite = 0;
        char* name = m_projectileProperties[i].spriteName;
        if (name[0]) {
            for (j = 0; j < i; j++) {
                if (strcmp(name, m_projectileProperties[j].spriteName) == 0) {
                    m_projectileProperties[i].sprite = m_projectileProperties[j].sprite;
                    break;
                }
            }
            if (!m_projectileProperties[i].sprite) {
                m_projectileProperties[i].sprite = new CSprite(name, (CSprite::ESpriteLoadType)1, (ERenderPriority)111);
                if (!m_projectileProperties[i].sprite->m_texture)
                    m_projectileProperties[i].sprite = 0;
            }
        }
    }
    for (i = 0; i < 11; i++) {
        m_thrownProperties[i].sprite = 0;
        char* name = m_thrownProperties[i].spriteName;
        if (name[0]) {
            for (j = 0; j < i; j++) {
                if (strcmp(name, m_thrownProperties[j].spriteName) == 0) {
                    m_thrownProperties[i].sprite = m_thrownProperties[j].sprite;
                    break;
                }
            }
            if (!m_thrownProperties[i].sprite)
                m_thrownProperties[i].sprite = new CSprite(name, (CSprite::ESpriteLoadType)1, (ERenderPriority)111);
        }
    }
}

void CBulletFactory::LoadBulletParameters() {
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
    int numProjectiles;
    int numThrown;
    memcpy(&numProjectiles, file + 4, 4);
    memcpy(&numThrown, file + 8, 4);
    offset = 12;
    ChangeEndian(numProjectiles);
    ChangeEndian(numThrown);
    m_projectileProperties = (ProjectilePropertiesView*)new char[numProjectiles * 64];
    m_thrownProperties = (ThrownPropertiesView*)new char[numThrown * 76];
    for (int read = 0; read < numThrown + numProjectiles;) {
        short type;
        memcpy(&type, file + offset, 2);
        offset += 2;
        ChangeEndian(type);
        switch (type) {
        case 1:
            offset += 82;
            break;
        case 2: {
            short index;
            memcpy(&index, file + offset, 2);
            ChangeEndian(index);
            memcpy(&m_projectileProperties[index], file + (short)(offset + 2), 64);
            offset += 66;
            read++;
            ChangeEndian(*(unsigned int*)&m_projectileProperties[index].sprite);
            ChangeEndian(*(short*)((char*)&m_projectileProperties[index] + 52));
            ChangeEndian(*(unsigned int*)&m_projectileProperties[index].weight);
            ChangeEndian(*(unsigned int*)((char*)&m_projectileProperties[index] + 48));
            ChangeEndian(*(unsigned int*)((char*)&m_projectileProperties[index] + 56));
            break;
        }
        case 3: {
            short index;
            memcpy(&index, file + offset, 2);
            ChangeEndian(index);
            memcpy(&m_thrownProperties[index], file + (short)(offset + 2), 76);
            ChangeEndian(*(unsigned int*)&m_thrownProperties[index].sprite);
            ChangeEndian(*(short*)((char*)&m_thrownProperties[index] + 72));
            ChangeEndian(*(unsigned int*)&m_thrownProperties[index].weight);
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 48));
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 52));
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 56));
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 60));
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 64));
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 68));
            offset += 78;
            read++;
            break;
        }
        }
    }
    TLT_CloseFile(file);
}

bool CBulletFactory::CanBulletTypeCook(bool projectile, int type) {
    if (projectile)
        return false;
    return m_thrownProperties[type].canCook != 0;
}

void* CBulletFactory::GetBulletSprite(bool projectile, int type) {
    if (projectile)
        return m_projectileProperties[type].sprite;
    return m_thrownProperties[type].sprite;
}

void CBulletFactory::DestroyBullet(CBullet* bullet) {
    if (((BulletKindView*)bullet)->kind == 1)
        m_projectiles.DestroyBullet((CProjectileBullet*)bullet);
    else
        m_thrown.DestroyBullet((CThrownBullet*)bullet);
}

CBullet* CBulletFactory::CreateBullet(int type, bool projectile, CVector3 position, CVector3 velocity, float speed,
                                      EClsnId id, float damage, ISceneNode* owner) {
    if (projectile)
        return (CBullet*)m_projectiles.CreateBullet((EProjectileBulletTypes)type, position, velocity, speed, id, damage,
                                                    owner);
    return (CBullet*)m_thrown.CreateBullet((EThrownBulletTypes)type, position, velocity, speed, id, damage, owner);
}

void CBulletFactory::Shutdown() {
    DeleteBulletGUISprites();
    m_projectiles.Shutdown();
    m_thrown.Shutdown();
    delete[] m_thrownProperties;
    delete[] m_projectileProperties;
    m_thrownProperties = 0;
    m_projectileProperties = 0;
    m_initialised = false;
}

void CBulletFactory::Reset() {
    m_projectiles.Reset();
    m_thrown.Reset();
}

void CBulletFactory::Init() {
    LoadBulletParameters();
    LoadBulletGUISprites();
    m_projectiles.Init(m_projectileProperties);
    m_thrown.Init(m_thrownProperties);
    m_initialised = true;
}

CBulletFactory::~CBulletFactory() {
}

CBulletFactory::CBulletFactory() {
    m_initialised = false;
    m_projectileProperties = 0;
    m_thrownProperties = 0;
}

template <class Bullet, class Properties, class Types>
CBulletFactoryHelper<Bullet, Properties, Types>::CBulletFactoryHelper() {
    m_properties = 0;
    m_bullets = 0;
    m_free = 0;
    m_used = 0;
    m_initialised = false;
}

template <class Bullet, class Properties, class Types>
CBulletFactoryHelper<Bullet, Properties, Types>::~CBulletFactoryHelper() {
}

template <class Bullet, class Properties, class Types>
void CBulletFactoryHelper<Bullet, Properties, Types>::Init(Properties* properties) {
    int i;

    m_properties = properties;
    m_bullets = new Bullet[64];
    m_free = m_bullets;
    m_used = 0;
    for (i = 0; i < 63; i++)
        m_bullets[i].m_next = &m_bullets[i + 1];
    m_bullets[63].m_next = 0;
    m_initialised = true;
}

template <class Bullet, class Properties, class Types>
void CBulletFactoryHelper<Bullet, Properties, Types>::Reset() {
    int i;

    delete[] m_bullets;
    m_bullets = new Bullet[64];
    m_free = m_bullets;
    m_used = 0;
    for (i = 0; i < 63; i++)
        m_bullets[i].m_next = &m_bullets[i + 1];
    m_bullets[63].m_next = 0;
}

template <class Bullet, class Properties, class Types>
void CBulletFactoryHelper<Bullet, Properties, Types>::Shutdown() {
    delete[] m_bullets;
    m_bullets = 0;
    m_initialised = false;
}

template <class Bullet, class Properties, class Types>
Bullet* CBulletFactoryHelper<Bullet, Properties, Types>::CreateBullet(Types type, CVector3 position,
                                                                                         CVector3 velocity, float speed,
                                                                                         EClsnId id, float damage,
                                                                                         ISceneNode* owner) {
    Bullet* bullet = 0;

    if (m_free) {
        bullet = m_free;
        m_free = bullet->m_next;
        bullet->m_next = m_used;
        m_used = bullet;
        bullet->Init(type, &m_properties[type], position, velocity, speed, id, damage, owner);
    }
    return bullet;
}

template <class Bullet, class Properties, class Types>
void CBulletFactoryHelper<Bullet, Properties, Types>::DestroyBullet(Bullet* bullet) {
    Bullet** link;
    Bullet* current;

    bullet->unknown128();
    link = &m_used;
    current = m_used;
    while (current != bullet) {
        link = &current->m_next;
        current = current->m_next;
    }
    *link = bullet->m_next;
    bullet->m_next = m_free;
    m_free = bullet;
}

