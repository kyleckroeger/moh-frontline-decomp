// A fragment of anim_obj.cpp (0x80094c98): CAnimObject::HandleBulletCollision.
// A projectile fills a 24-byte script event block with its type, damage,
// shooter's script object, the side hit (as in collisionvolume_rows.cpp) and
// the body part from the first contact, scales the damage by part (3 for
// parts 0 and 1, 1 for 2 and 9 to 12, else 0.5) and fires script event 43;
// a player shooter's hit and shot location (by part, through a jump table)
// are counted in g_Shell, and the impact sound plays at the contact. A
// thrown bullet explodes instead. The classes, functions and g_Shell (its
// 0x19dc-byte size) are named by the symbols; CBullet, ISceneNode and
// CAnimObject declare the used virtual functions at their table slots
// (placeholders named by offset), the event block, contact and collision
// views, the part and location values' meaning and the members are
// inferred, and the CVector3 view copies through a double pair as in
// player_weapon.cpp. The rest of the file is not part of this unit.
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

struct BSObject;
class CPlayerObject;

void* BSEventGetEventMemoryBlock(unsigned int);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void PlayImpactSound(unsigned short, unsigned short, CVector3, bool);

enum EShotLocations {};

class CShellMenu {
public:
    void Set_hits(unsigned int, bool);
    void Set_shotLocation(unsigned int, EShotLocations, bool);

    unsigned char unknown0000[6620];
};

extern CShellMenu g_Shell;

class ISceneNode {
public:
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
    virtual BSObject* GetScriptObject() const;
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
    virtual CPlayerObject* AsPlayerObject();
};

class CThrownBullet {
public:
    void Explode();
};

class CProjectileBullet;

class CBullet {
public:
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
    virtual void GetForward(CVector3&) const;
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
    virtual void vf124();
    virtual void vf128();
    virtual void vf12c();
    virtual float GetDamage() const;
    virtual int GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
};

/* inferred: a contact of a collision: its position at +16 and body part at +36 */
struct ContactView {
    unsigned char unknown00[16];
    CVector3 m_position;
    unsigned char unknown20[4];
    int m_part;
};

/* inferred: the collision's contact list at +24 */
class CCollision {
public:
    unsigned char unknown00[24];
    ContactView* m_contacts;
};

/* inferred: the bullet event block (24 bytes) passed with script event 43 */
struct BulletHitEvent {
    BSObject* m_shooter;
    int m_damage;
    int m_side;
    int m_type;
    int m_part;
};

class CAnimObject {
public:
    virtual void MarkForDestruction(int);
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
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;

    void HandleBulletCollision(CBullet*, const CCollision&);

    unsigned char unknown0004[9100];
    BSObject* m_script;
};

void CAnimObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    ContactView* contact;
    CProjectileBullet* projectile = bullet->AsProjectile();
    CThrownBullet* thrown = bullet->AsThrown();
    if (projectile) {
        BulletHitEvent* event = (BulletHitEvent*)BSEventGetEventMemoryBlock(24);
        event->m_type = bullet->GetScriptBulletType();
        event->m_damage = (int)bullet->GetDamage();
        ISceneNode* shooter = bullet->GetFiredBy();
        if (shooter)
            event->m_shooter = shooter->GetScriptObject();
        else
            event->m_shooter = 0;
        CVector3 direction;
        bullet->GetForward(direction);
        CVector3 forward;
        GetForward(forward);
        CVector3 right;
        GetRightward(right);
        float front = forward.x * direction.x + forward.y * direction.y + forward.z * direction.z;
        float side = right.x * direction.x + right.y * direction.y + right.z * direction.z;
        if (side < -0.7071f)
            event->m_side = 3;
        else if (side > 0.7071f)
            event->m_side = 2;
        else if (front < 0.0f)
            event->m_side = 0;
        else
            event->m_side = 1;
        contact = collision.m_contacts;
        event->m_part = contact->m_part;
        switch (event->m_part) {
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            event->m_damage = (int)(0.5f * event->m_damage);
            break;
        case 0:
        case 1:
            event->m_damage = (int)(3.0f * event->m_damage);
            break;
        case 2:
        case 9:
        case 10:
        case 11:
        case 12:
            event->m_damage = (int)(1.0f * event->m_damage);
            break;
        }
        BSObjectTriggerEvent(m_script, 43, event, 0, false);
        if (shooter->AsPlayerObject()) {
            g_Shell.Set_hits(1, false);
            switch (event->m_part) {
            case 3:
            case 4:
            case 5:
                g_Shell.Set_shotLocation(1, (EShotLocations)1, false);
                break;
            case 6:
            case 7:
            case 8:
                g_Shell.Set_shotLocation(1, (EShotLocations)2, false);
                break;
            case 13:
            case 14:
            case 15:
                g_Shell.Set_shotLocation(1, (EShotLocations)4, false);
                break;
            case 16:
            case 17:
            case 18:
                g_Shell.Set_shotLocation(1, (EShotLocations)5, false);
                break;
            case 19:
            case 20:
                g_Shell.Set_shotLocation(1, (EShotLocations)6, false);
                break;
            case 0:
            case 1:
                g_Shell.Set_shotLocation(1, (EShotLocations)0, false);
                break;
            case 2:
            case 9:
            case 10:
            case 11:
            case 12:
                g_Shell.Set_shotLocation(1, (EShotLocations)3, false);
                break;
            }
        }
        PlayImpactSound(14, 1, contact->m_position, false);
    } else if (thrown) {
        thrown->Explode();
    }
}
