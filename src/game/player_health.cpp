// A fragment of player.cpp (0x800a1278): CPlayerObject::OnCollision handles a
// collision of the player with another scene node. A pickup (collision id 14)
// is identified through its dump object: the health pickups by CRC (10, 25 or
// 50 health), anything else as the weapon slot that GetWeaponInfoByCRC names
// (an unknown pickup is reported and stops colliding); the weapon or ammunition
// and the health are then added. For anything but a bullet, every contact is
// recorded (other object, depth and the normal, negated when the player is the
// collision's first object), a normal steeper than 0.642 up sets the on-floor
// flag, and on world objects the highest contact also sets the surface sound
// (1 without a surface). The pickup's lookup and its adding are inferred
// static inlines (the switch value is passed by value to its own inline).
// CPlayerObject::AddHealth adds a
// health pickup to the player's script health, clamped between 0 and the
// maximum (the pickup properties' byte at +112, or the multiplayer
// shell's per-controller maximum). When the health was not already at the
// maximum it plays the pickup's sound (event 7, kind 3, sound 6, 7 or 8 by
// the pickup's CRC, a random 3800..4400 pitch) and reports the pickup as
// taken; reaching the maximum turns off dump collisions of type 0. A taken
// pickup's dump object (and any in multiplayer) is marked for destruction.
// AddAmmoOrWeapon does the same for a weapon or ammunition pickup: a weapon
// not yet selectable is picked up (unless its CRC is one of sixteen ammo-only
// pickups) with a sound by the weapon's sound type, its pickup message from
// the user message table (an inferred inline lookup) and a switch to it when
// the player has no weapon; ammunition up to the maximum is added with a
// "%d" count message, finishing a fresh pickup's reload, and script event
// 162 is triggered with the sound. The class and functions are named by the mangled symbols; the members,
// the script object and data views, the event layout and the parameter
// meanings are inferred. The rest of the file is in other units.
long long MathFunRandomI64(long long, long long);
int AEMS_SendEvent(void*);
enum EDumpType {};
void EnableDumpCollisionsByType(EDumpType, int, bool);
void MarkDumpObjectForDestruction(void*);

extern "C" int sprintf(char*, const char*, ...);
void appendMessageToPrompt(const char*, int);
void displayPromptBuffer(int);

extern bool g_bInMultiplayerMode;

/* inferred: the user message table (entries of text and type, count +12) */
struct MessageView {
    const char* text;
    unsigned char type;
    unsigned char unknown05[3];
};

struct MessageTableView {
    MessageView* entries;
    unsigned char unknown04[8];
    int count;

    const char* GetText(int i) const { return i < count ? entries[i].text : 0; }
};

extern MessageTableView umTable;

/* inferred: the weapon's properties (maximum ammo +18) */
struct WeaponPropertiesView {
    unsigned char unknown00[18];
    short m_maxAmmo;
};

class CWeapon {
public:
    short GetWeaponSoundType() const;

    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    short m_ammo;
};

class CPlayerWeaponObject {
public:
    bool IsSelectable() const;
    void AddAmo(int);
    void DoneReloading();

    unsigned char unknown0000[12516];
    CWeapon* m_weapon;
    unsigned char unknown30e8[234];
    unsigned char m_flags31d2a : 2;
    bool m_picked : 1;
    unsigned char m_flags31d2b : 5;
};

class CShellMenu {
public:
    unsigned char unknown0000[5180];
    int m_maxHealth[4];
};

extern CShellMenu g_Shell;

/* inferred: the script data's health (first word) */
struct ScriptDataView {
    int m_health;
};

class BSGO_Basic {
    unsigned char unknown00[12];
public:
    virtual void Destroy();
    virtual ScriptDataView* GetScriptData();
};

/* inferred: the pickup's properties (maximum health at +112) */
struct PickupPropertiesView {
    unsigned char unknown00[112];
    unsigned char m_maxHealth;
};

struct ScriptOwnerView {
    unsigned char unknown00[4];
    PickupPropertiesView* m_properties;
};

struct BSObject {
    unsigned char unknown00[8];
    ScriptOwnerView* m_owner;
    BSGO_Basic* m_user;
};

class CCollision;
class CStaticObject;
class ISceneNode;
void* HitDumpObject(CStaticObject*, int*, int*, EDumpType*, int*);
void DebugMsg(const char*, ...);

class CSurfaceType {
public:
    static short GetSoundIndex(int);
};

struct SurfaceView {
    unsigned char unknown00[12];
    short m_type : 11;
};

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) { d.v[0] = ax; d.v[1] = ay; d.v[2] = az; }
    CVector3(const CVector3& other) { d.pair[0] = other.d.pair[0]; d.pair[1] = other.d.pair[1]; }
    void SetNegative(const CVector3& o) {
        float x = -o.d.v[0];
        float y = -o.d.v[1];
        float z = -o.d.v[2];
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3 operator-() const { return CVector3(-d.v[0], -d.v[1], -d.v[2]); }

    CVector3Pair d;
} __attribute__((aligned(8)));

struct ContactView {
    CVector3 m_normal;
    unsigned char unknown10[16];
    float m_depth;
    SurfaceView* m_surface;
    unsigned char unknown28[4];
    ContactView* m_next;
};

class CCollision {
public:
    ISceneNode* m_first;
    ISceneNode* m_second;
    unsigned char unknown08[16];
    ContactView* m_contacts;
};

class ISceneNode {
public:
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
    virtual void Draw(void*);
    virtual int GetLocalBoundingVolume(int) const;
    virtual int GetWorldBoundingVolume(int) const;
    virtual void GetTMLocalToWorld(void*) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(void*) const;
    virtual int IsDrawEnabled() const;
    virtual int GetCollisionId() const;
    virtual void SetCollisionId(int);
    virtual void* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(void*, const CCollision&);
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
    virtual void* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual void* AsLight();
    virtual const void* AsLight() const;
    virtual void* AsBullet();
};

struct SContact {

    CVector3 m_normal;
    float m_depth;
    void* m_object;
};

namespace dwi {
template <class T>
class fast_vec {
public:
    void push_back(T x) { m_data[m_size++] = x; }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

class CPlayerObject {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void OnCollision(const CCollision&);
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
    virtual void TriggerScriptEvent(int, int, bool);

    bool AddHealth(int, int, void*);
    bool AddAmmoOrWeapon(int, int, void*);
    static int GetWeaponInfoByCRC(int, int*, int*);
    void CycleWeapon(int, int);

    unsigned char unknown004[676];
    dwi::fast_vec<SContact> m_contacts;
    unsigned char unknown2c0[208];
    float m_highestContact;
    unsigned char m_landed : 1;
    unsigned char m_onFloor : 1;
    unsigned char m_flags394 : 6;
    unsigned char unknown395[199];
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[45];
    unsigned char unknown514[572];
    short m_surfaceSound;
    unsigned char unknown752[18];
    int m_controller;
    int m_promptPlayer;
};

static inline void Give(CPlayerObject* self, const int& crc, const int& amount, CPlayerWeaponObject* weapon, int health,
                        void* dump) {
    if (weapon)
        self->AddAmmoOrWeapon(crc, amount, dump);
    if (health > 0)
        self->AddHealth(crc, health, dump);
}

static inline void Classify(CPlayerObject* self, ISceneNode* other, const int& crc, int value, int& health,
                            CPlayerWeaponObject*& weapon) {
    switch (value) {
    case 0x598e4105:
    case 0x72441aa1:
        health = 10;
        break;
    case 0x10efeba2:
        health = 25;
        break;
    case (int)0xf7baddb9:
        health = 50;
        break;
    default:
        weapon = self->m_weapons[CPlayerObject::GetWeaponInfoByCRC(value, 0, 0)];
        if (!weapon) {
            DebugMsg("What the heck are we trying to pick up?  crc = 0x%x\n", crc);
            other->SetCollisionId(-1);
        }
        break;
    }
}

static inline void PickUp(CPlayerObject* self, ISceneNode* other) {
    CPlayerWeaponObject* weapon;
    int health;
    void* dump;
    health = 0;
    weapon = 0;
    int amount;
    int crc;
    int extra;
    EDumpType kind;
    dump = HitDumpObject((CStaticObject*)other, &amount, &crc, &kind, &extra);
    Classify(self, other, crc, crc, health, weapon);
    Give(self, crc, amount, weapon, health, dump);
}

void CPlayerObject::OnCollision(const CCollision& collision) {
    bool first;
    ISceneNode* other;
    ContactView* contact;
    first = collision.m_first == (ISceneNode*)this;
    other = collision.m_first == (ISceneNode*)this ? collision.m_second : collision.m_first;
    if (other->GetCollisionId() == 14)
        PickUp(this, other);
    if (!other->AsBullet()) {
        for (contact = collision.m_contacts; contact; contact = contact->m_next) {
            SContact c;
            c.m_object = other->AsAnimObject();
            c.m_depth = contact->m_depth;
            if (first)
                c.m_normal.SetNegative(contact->m_normal);
            else
                c.m_normal = contact->m_normal;
            m_contacts.push_back(c);
            if (contact->m_normal.d.v[2] > 0.642f)
                m_onFloor = 1;
            if (other->AsWorldObject()) {
                if (contact->m_normal.d.v[2] > m_highestContact) {
                    m_highestContact = contact->m_normal.d.v[2];
                    SurfaceView* surface = contact->m_surface;
                    if (surface && surface->m_type >= 0)
                        m_surfaceSound = CSurfaceType::GetSoundIndex(surface->m_type);
                    else
                        m_surfaceSound = 1;
                }
            }
        }
        m_landed = 1;
    }
}

bool CPlayerObject::AddHealth(int crc, int amount, void* dump) {
    bool added = false;
    int sound;
    switch (crc) {
    case 0x598e4105:
    case 0x72441aa1:
        sound = 6;
        break;
    case 0x10efeba2:
        sound = 7;
        break;
    case (int)0xf7baddb9:
        sound = 8;
        break;
    default:
        sound = -1;
        break;
    }
    BSObject* script = GetScriptObject();
    if (script) {
        PickupPropertiesView* properties = script->m_owner->m_properties;
        ScriptDataView* data = script->m_user->GetScriptData();
        if (properties && data) {
            int maximum;
            if (g_bInMultiplayerMode)
                maximum = g_Shell.m_maxHealth[m_controller];
            else
                maximum = properties->m_maxHealth;
            int health = data->m_health + amount;
            if (maximum != data->m_health) {
                if (sound != -1) {
                    int event[10];
                    event[0] = 7;
                    event[1] = 3;
                    event[2] = sound;
                    event[3] = MathFunRandomI64(3800, 4400);
                    event[4] = 0;
                    event[5] = 0;
                    event[6] = 0;
                    event[7] = 24999;
                    event[8] = 0;
                    event[9] = 32000;
                    AEMS_SendEvent(event);
                }
                added = true;
            }
            if (health > maximum)
                health = maximum;
            if (health < 0)
                health = 0;
            data->m_health = health;
            if (health == maximum)
                EnableDumpCollisionsByType((EDumpType)0, -1, false);
        }
    }
    if ((added || g_bInMultiplayerMode) && dump) {
        MarkDumpObjectForDestruction(dump);
        return true;
    }
    return added;
}

bool CPlayerObject::AddAmmoOrWeapon(int crc, int amount, void* dump) {
    CPlayerWeaponObject* weapon = m_weapons[GetWeaponInfoByCRC(crc, 0, 0)];
    CWeapon* info = weapon->m_weapon;
    int pickupMessage = -1;
    int ammoMessage = -1;
    int sound = 9;
    GetWeaponInfoByCRC(crc, &pickupMessage, &ammoMessage);
    bool noPickup;
    switch (crc) {
    case 0xa8b9679d:
    case 0x985b620e:
    case 0x900b140b:
    case 0x87a43cca:
    case 0x9884da57:
    case 0x0c839118:
    case 0xcc7202da:
    case 0xaf3a77e7:
    case 0x112a938a:
    case 0x4405e37b:
    case 0x2fa05716:
    case 0x2446755f:
    case 0x36ead81a:
    case 0x5c95b07a:
    case 0x47285958:
    case 0x72787373:
        noPickup = true;
        break;
    case 0x1a496fab:
    case 0x1e042f0c:
        sound = 29;
        noPickup = false;
        break;
    default:
        noPickup = false;
        break;
    }
    bool taken = false;
    bool picked = false;
    if (!weapon->IsSelectable() && !noPickup) {
        short type = info->GetWeaponSoundType();
        if (type < 7)
            sound = 30;
        else if (type < 14)
            sound = 31;
        else if (type < 20)
            sound = 32;
        else if (type < 22)
            sound = 29;
        else if (type < 24)
            sound = 33;
        weapon->m_picked = true;
        picked = true;
        if (pickupMessage >= 0) {
            appendMessageToPrompt(umTable.GetText(pickupMessage), m_promptPlayer);
            displayPromptBuffer(m_promptPlayer);
        }
        if (m_currentWeapon < 0) {
            TriggerScriptEvent(162, sound, false);
            CycleWeapon(1, -1);
        }
        taken = true;
    }
    short maxAmmo = info->m_properties->m_maxAmmo;
    short ammo = info->m_ammo;
    if (amount > 0 && ammo < maxAmmo) {
        if (ammoMessage >= 0) {
            char text[32];
            int room = maxAmmo - ammo;
            if (room < amount)
                sprintf(text, "%d", room);
            else
                sprintf(text, "%d", amount);
            appendMessageToPrompt(text, m_promptPlayer);
            appendMessageToPrompt(umTable.GetText(ammoMessage), m_promptPlayer);
            displayPromptBuffer(m_promptPlayer);
        }
        taken = true;
        weapon->AddAmo(amount);
        if (picked)
            weapon->DoneReloading();
        TriggerScriptEvent(162, sound, false);
    }
    if ((taken || g_bInMultiplayerMode) && dump) {
        MarkDumpObjectForDestruction(dump);
        return true;
    }
    return taken;
}
