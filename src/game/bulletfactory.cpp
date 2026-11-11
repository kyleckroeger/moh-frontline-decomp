// CBulletFactory queries and bullet destruction: whether a thrown bullet type
// can be cooked, a bullet type's sprite (projectile or thrown property
// table), destruction through the projectile or thrown helper, and creation
// through the helper for the bullet's kind. The class,
// template and enum names come from the mangled symbols; the property-table
// views and the bullet's kind member are inferred from offsets, and the
// factory is a non-virtual view. The rest of the file is not part of this
// unit.
class CBullet;
class CProjectileBullet;
class CThrownBullet;
class ISceneNode;
enum EClsnId {};

#include "game/CVector3.h"

struct ProjectileBulletProperties_struct;
struct ThrownBulletProperties_struct;
enum EProjectileBulletTypes {};
enum EThrownBulletTypes {};

template <class T, class P, class E> class CBulletFactoryHelper {
public:
    void DestroyBullet(T*);
    T* CreateBullet(E, CVector3, CVector3, float, EClsnId, float, ISceneNode*);

    unsigned char unknown00[20];
};

struct ProjectilePropertiesView {
    unsigned char unknown00[44];
    void* sprite;
    unsigned char unknown30[16];
};

struct ThrownPropertiesView {
    unsigned char unknown00[44];
    void* sprite;
    unsigned char unknown30[27];
    signed char canCook;
};

struct BulletKindView {
    unsigned char unknown00[40];
    int kind;
};

class CBulletFactory {
public:
    bool CanBulletTypeCook(bool, int);
    void* GetBulletSprite(bool, int);
    void DestroyBullet(CBullet*);
    CBullet* CreateBullet(int, bool, CVector3, CVector3, float, EClsnId, float, ISceneNode*);

    ProjectilePropertiesView* m_projectileProperties;
    ThrownPropertiesView* m_thrownProperties;
    CBulletFactoryHelper<CProjectileBullet, ProjectileBulletProperties_struct, EProjectileBulletTypes> m_projectiles;
    CBulletFactoryHelper<CThrownBullet, ThrownBulletProperties_struct, EThrownBulletTypes> m_thrown;
};

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
