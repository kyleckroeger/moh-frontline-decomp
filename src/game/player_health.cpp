// A fragment of player.cpp (0x800a1580): CPlayerObject::AddHealth adds a
// health pickup to the player's script health, clamped between 0 and the
// maximum (the pickup properties' byte at +112, or the multiplayer
// shell's per-controller maximum). When the health was not already at the
// maximum it plays the pickup's sound (event 7, kind 3, sound 6, 7 or 8 by
// the pickup's CRC, a random 3800..4400 pitch) and reports the pickup as
// taken; reaching the maximum turns off dump collisions of type 0. A taken
// pickup's dump object (and any in multiplayer) is marked for destruction.
// The class and functions are named by the mangled symbols; the members,
// the script object and data views, the event layout and the parameter
// meanings are inferred. The rest of the file is in other units.
long long MathFunRandomI64(long long, long long);
int AEMS_SendEvent(void*);
enum EDumpType {};
void EnableDumpCollisionsByType(EDumpType, int, bool);
void MarkDumpObjectForDestruction(void*);

extern bool g_bInMultiplayerMode;

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

    bool AddHealth(int, int, void*);

    unsigned char unknown004[1888];
    int m_controller;
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
