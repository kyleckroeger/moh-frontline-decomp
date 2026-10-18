// A fragment of player.cpp (0x800a0cec): CPlayerObject::HandleBulletCollision.
// Unless bullet hits are ignored (bit 0x40 of +919), a projectile or a thrown
// bullet of type 5 or 9 sends the player's script event 43 with a 24-byte
// event: the shooter's script object, the bullet's damage (zeroed by one
// cheat, 1000 with another), the hit direction from the bullet's forward
// against the player's right and forward (left, right, front, behind; also
// shown on the interface) and the bullet's damage type. In multiplayer,
// team damage is dropped unless friendly fire is on, the damage is scaled by
// the hit location of the first contact (head 0-1 three times, torso 2 and
// 9-12 unchanged, limbs half) and, while the player is alive, the shot is
// added to the shooter's statistics by body area (a jump table). The pad
// rumbles by the damage over 20, an impact sound plays at the bullet, and a
// thrown bullet of type 5 is destroyed; in multiplayer a thrown bullet then
// explodes. The classes and functions are named by the mangled symbols; the
// members, the event layout, the script-object chain and the constants'
// meanings and the damage-scaling inline are inferred. The rest of the file
// is in other units.
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
struct BSObject;
enum EClsnId {};

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class ISceneNode {
public:
    enum EVolumeType {};
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};
void* BSEventGetEventMemoryBlock(unsigned int);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void PlayImpactSound(unsigned short, unsigned short, CVector3, bool);
template <class T> T MathFunClamp(T, T, T);

extern bool g_bInMultiplayerMode;
/* inferred: a damage multiplier applied through an inline (a plain 1.0f
   factor would be folded away) */
inline float ScaleDamage(float damage, float scale) { return scale * damage; }

struct CheatsView {
    bool m_noDamage;
    unsigned char unknown01[13];
    bool m_bigDamage;
    unsigned char unknown0f[1];
};

extern CheatsView g_cheats;

enum EShotLocations {};

class CShellMenu {
public:
    void Add_MPShotLocation(int, int, int, EShotLocations);

    unsigned char unknown0000[5172];
    int m_friendlyFire;
};

extern CShellMenu g_Shell;

class CInputManager {
public:
    void SetFeedback(unsigned long, long, long, float, float);

    unsigned char unknown000[24];
};

extern CInputManager g_inputMgr;

class UserInterface {
public:
    void AddHitDirection(unsigned int);
};

/* inferred: the script object's user (vtable after 12 bytes) */
class BSGO_Basic {
    unsigned char unknown00[12];
public:
    virtual void Destroy();
    virtual int* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObject {
    unsigned char unknown00[12];
    BSGO_Basic* m_user;
};

class CThrownBullet : public ISceneNode {
public:
    virtual ~CThrownBullet();
    void Explode();

    unsigned char unknown004[160];
    int m_type;
};

class CBullet : public ISceneNode {
public:
    virtual ~CBullet();
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
    virtual void vf130();
    virtual float GetDamage();
    virtual int GetDamageType();
    virtual ISceneNode* GetFiredBy();
    virtual CThrownBullet* AsThrown();
    virtual void* AsProjectile();
};

/* inferred: the bullet-hit script event (24 bytes) */
struct BulletHitEvent {
    BSObject* m_shooter;
    int m_damage;
    int m_direction;
    int m_damageType;
    int m_location;
};

/* inferred: a collision contact's hit location (+36) */
struct ContactView {
    unsigned char unknown00[36];
    int m_location;
};

class CCollision {
public:
    unsigned char unknown00[24];
    ContactView* m_contacts;
};

class CPlayerObject : public ISceneNode {
public:
    virtual ~CPlayerObject();
    void HandleBulletCollision(CBullet*, const CCollision&);

    unsigned char unknown004[588];
    BSObject* m_script;
    unsigned char unknown254[323];
    unsigned char m_flags919a : 1;
    bool m_ignoreBullets : 1;
    unsigned char m_flags919b : 6;
    unsigned char unknown398[971];
    int m_controller;
    unsigned char unknown768[4];
    int m_playerIndex;
    unsigned char unknown770[768];
    UserInterface m_interface;
    unsigned char unknownA71[2111];
    int m_team;
};

void CPlayerObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    if (m_ignoreBullets)
        return;
    void* projectile = bullet->AsProjectile();
    bool grenade = false;
    CThrownBullet* thrown = bullet->AsThrown();
    if (thrown)
        grenade = thrown->m_type == 5 || thrown->m_type == 9;
    if (projectile || grenade) {
        ISceneNode* firer = bullet->GetFiredBy();
        BulletHitEvent* event = (BulletHitEvent*)BSEventGetEventMemoryBlock(24);
        event->m_damageType = bullet->GetDamageType();
        event->m_damage = bullet->GetDamage();
        if (firer)
            event->m_shooter = firer->GetScriptObject();
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
        if (side < -0.7071f) {
            m_interface.AddHitDirection(2);
            event->m_direction = 3;
        } else if (side > 0.7071f) {
            m_interface.AddHitDirection(4);
            event->m_direction = 2;
        } else if (front < 0.0f) {
            m_interface.AddHitDirection(1);
            event->m_direction = 0;
        } else {
            m_interface.AddHitDirection(3);
            event->m_direction = 1;
        }
        if (g_cheats.m_noDamage)
            event->m_damage = 0;
        else if (g_cheats.m_bigDamage)
            event->m_damage = 1000;
        if (g_bInMultiplayerMode) {
            CPlayerObject* shooter = firer->AsPlayerObject();
            if (g_Shell.m_friendlyFire == 0 && shooter->m_team == m_team)
                event->m_damage = 0;
            event->m_location = collision.m_contacts->m_location;
            switch (event->m_location) {
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
                event->m_damage = 0.5f * event->m_damage;
                break;
            case 0:
            case 1:
                event->m_damage = ScaleDamage(3.0f * event->m_damage, 1.0f);
                break;
            case 2:
            case 9:
            case 10:
            case 11:
            case 12:
                event->m_damage = (float)event->m_damage;
                break;
            }
            CPlayerObject* attacker = event->m_shooter->m_user->GetSceneNode()->AsPlayerObject();
            if (event->m_damage != 0 && *m_script->m_user->GetScriptData() > 0) {
                int location;
                switch (event->m_location) {
                case 3:
                case 4:
                case 5:
                    location = 1;
                    break;
                case 6:
                case 7:
                case 8:
                    location = 2;
                    break;
                case 13:
                case 14:
                case 15:
                    location = 4;
                    break;
                case 16:
                case 17:
                case 18:
                    location = 5;
                    break;
                case 19:
                case 20:
                    location = 6;
                    break;
                case 0:
                case 1:
                    location = 0;
                    break;
                case 2:
                case 9:
                case 10:
                case 11:
                case 12:
                    location = 3;
                    break;
                default:
                    location = 3;
                    break;
                }
                g_Shell.Add_MPShotLocation(attacker->m_playerIndex, m_playerIndex, 1, (EShotLocations)location);
            }
        }
        BSObjectTriggerEvent(m_script, 43, event, 0, false);
        g_inputMgr.SetFeedback(m_controller, 12, 0, MathFunClamp<float>(event->m_damage / 20.0f, 0.0f, 1.0f), 0.1f);
        CVector3 position;
        bullet->GetPosition(position);
        unsigned short kind = 1;
        if (thrown) {
            if (thrown->m_type == 5)
                kind = 5;
            else
                kind = 2;
        }
        PlayImpactSound(14, kind, position, true);
        if (thrown && thrown->m_type == 5)
            thrown->MarkForDestruction(0);
    }
    if (g_bInMultiplayerMode && thrown)
        thrown->Explode();
}
