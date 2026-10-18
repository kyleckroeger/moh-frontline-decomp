// A fragment of player.cpp (0x800a1580): CPlayerObject::AddHealth adds a
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
    virtual void TriggerScriptEvent(int, int, bool);

    bool AddHealth(int, int, void*);
    bool AddAmmoOrWeapon(int, int, void*);
    static int GetWeaponInfoByCRC(int, int*, int*);
    void CycleWeapon(int, int);

    unsigned char unknown004[1112];
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[45];
    unsigned char unknown514[592];
    int m_controller;
    int m_promptPlayer;
};

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
