// A fragment of bulletfactory.cpp (0x800ce318):
// CBulletFactoryHelper<CProjectileBullet,...>::Shutdown, CreateBullet and
// DestroyBullet (as for the thrown bullets).
// CBulletFactoryHelper, the bullet classes, their properties and type enums
// are named by the mangled symbols; the helper's members (bullet array, free
// and used lists, property table, flag), the bullets' list link at +160, the
// virtual called before a bullet is released (slot +296, name unknown) and
// the sizes are inferred. The members are weak template copies emitted in
// bulletfactory.cpp, defined __declspec(weak) out of the class and
// instantiated explicitly; only this unit's members are defined. The rest of
// the file is not part of this unit.
#include "game/CVector3.h"

class ISceneNode;
enum EClsnId {};
enum EThrownBulletTypes {};
enum EProjectileBulletTypes {};

struct ThrownBulletProperties_struct {
    unsigned char data[76];
};

struct ProjectileBulletProperties_struct {
    unsigned char data[64];
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

    unsigned char unknown004[156];
    CThrownBullet* m_next;
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

    unsigned char unknown004[156];
    CProjectileBullet* m_next;
};

template <class Bullet, class Properties, class Types>
class CBulletFactoryHelper {
public:
    CBulletFactoryHelper();
    ~CBulletFactoryHelper();
    void Shutdown();
    Bullet* CreateBullet(Types, CVector3, CVector3, float, EClsnId, float, ISceneNode*);
    void DestroyBullet(Bullet*);

    Bullet* m_bullets;
    Bullet* m_free;
    Bullet* m_used;
    Properties* m_properties;
    bool m_initialised;
};

template <class Bullet, class Properties, class Types>
__declspec(weak) void CBulletFactoryHelper<Bullet, Properties, Types>::Shutdown() {
    delete[] m_bullets;
    m_bullets = 0;
    m_initialised = false;
}

template <class Bullet, class Properties, class Types>
__declspec(weak) Bullet* CBulletFactoryHelper<Bullet, Properties, Types>::CreateBullet(Types type, CVector3 position,
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
__declspec(weak) void CBulletFactoryHelper<Bullet, Properties, Types>::DestroyBullet(Bullet* bullet) {
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

template class CBulletFactoryHelper<CProjectileBullet, ProjectileBulletProperties_struct, EProjectileBulletTypes>;
