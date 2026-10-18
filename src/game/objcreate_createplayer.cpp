// A fragment of objcreate.cpp (0x8003c688): CreatePlayer. The trigger's core
// is marked created and the indexed player is placed at the core's rotation
// and position; the player's script object (embedded at +0x770) is bound to
// the player and its counters cleared. The starting health comes from the
// previous level's finished data when the shell says so (outside
// multiplayer), adjusted by difficulty: 1 takes the core's health, 2 at
// least half of it, 3 keeps it and any other reports an unknown level and
// takes the core's health; in multiplayer it is the shell's per-player
// health, else the core's. The script object is then created from the core's
// script name, attached to the player, sent event 44 (with the multiplayer
// flag) and the player's AI is initialised; the script object is returned.
// The functions, classes and globals are named by the symbols; the trigger
// core, script-object, player and shell views are inferred, and the copy of
// the position goes through an inferred inline Set (its arguments are
// evaluated before the stores) and the half through a local. The constants
// are entries of the file's .sdata2 pool and the message a .rodata one. The
// rest of the file is not part of this unit.
class CVector3 {
public:
    CVector3(const CVector3& o) { Set(o.x, o.y, o.z); }
    void Set(float ax, float ay, float az) {
        x = ax;
        y = ay;
        z = az;
    }

    float x;
    float y;
    float z;
};

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetPos(CVector3);

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

struct TriggerCoreView {
    unsigned char unknown00[16];
    CVector3 position;
    CQuaternion rotation;
    unsigned char unknown2c[9];
    bool created;
    unsigned char unknown36[22];
    char* scriptName;
    unsigned char unknown50[32];
    unsigned char health;
};

struct TriggerObject_struct {
    unsigned int flags;
    TriggerCoreView* core;
};

class BSObject;
class BSGO_Basic;
class CPlayerObject;

int CreateBSObject(const char*, BSObject**, BSGO_Basic*, TriggerObject_struct*, void**);
int BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void DebugMsg(const char*, ...);

/* inferred: the player's script object, embedded in the player at +0x770 */
struct BSGO_PlayerView {
    BSObject* object;
    unsigned char unknown04[12];
    int health;
    unsigned char unknown14[8];
    void* data;
    unsigned char unknown20[12];
    CPlayerObject* owner;
    int field30;
    int field34;
    int field38;
    int field3c;
};

class CPlayerObject {
public:
    virtual void Function08();
    virtual void Function0c();
    virtual void Function10();
    virtual void Function14();
    virtual void Function18();
    virtual void Function1c();
    virtual void Function20();
    virtual void Function24();
    virtual void Function28();
    virtual void Function2c();
    virtual void Function30();
    virtual void Function34();
    virtual void Function38();
    virtual void Function3c();
    virtual void Function40();
    virtual void Function44();
    virtual void Function48();
    virtual void Function4c();
    virtual void Function50();
    virtual void Function54();
    virtual void Function58();
    virtual void Function5c();
    virtual void Function60();
    virtual void Function64();
    virtual void Function68();
    virtual void Function6c();
    virtual void Function70();
    virtual void Function74();
    virtual void Function78();
    virtual void Function7c();
    virtual void Function80();
    virtual void Function84();
    virtual void Function88();
    virtual void Function8c();
    virtual void Function90();
    virtual void Function94();
    virtual void Function98();
    virtual void Function9c();
    virtual void Functiona0();
    virtual void Functiona4();
    virtual void Functiona8();
    virtual void Functionac();
    virtual void Functionb0();
    virtual void Functionb4();
    virtual void Functionb8();
    virtual void Functionbc();
    virtual void Functionc0();
    virtual void Functionc4();
    virtual void Functionc8();
    virtual void Functioncc();
    virtual void Functiond0();
    virtual void Functiond4();
    virtual void Functiond8();
    virtual void Functiondc();
    virtual void Functione0();
    virtual void Functione4();
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void Functionec();
    virtual void Functionf0();
    virtual void Functionf4();
    virtual void Functionf8();
    virtual void Functionfc();
    virtual void Function100();
    virtual void Function104();
    virtual void Function108();
    virtual void Function10c();
    virtual void Function110();
    virtual void Function114();
    virtual void Function118();
    virtual void Function11c();
    virtual void Function120();
    virtual void SetScriptObject(BSObject*);
    void InitAI();

    unsigned char unknown004[1888];
    int playerIndex;
    unsigned char unknown768[8];
    BSGO_PlayerView script;
};

class CShellMenu {
public:
    int Get_currentMission();
    int Get_currentStage();
    unsigned char Get_currentDifficulty();
    bool ShouldIUsePlayersFinishedLevelData(int, int);
    float GetPlayersFinishedLevelHealth(int, int);

    unsigned char unknown0000[5148];
    int mpMission;
    int mpStage;
    unsigned char unknown1424[24];
    int mpHealth[4];
    unsigned char unknown144c[400];
};

extern CShellMenu g_Shell;
extern CPlayerObject* g_pPlayers[4];
extern bool g_bInMultiplayerMode;

BSGO_PlayerView* CreatePlayer(TriggerObject_struct* trigger, int index) {
    int health;
    int mission;
    BSGO_PlayerView* script;
    CPlayerObject* player;
    TriggerCoreView* core;
    int stage;

    core = trigger->core;
    player = g_pPlayers[index];

    core->created = true;
    script = &player->script;
    {
        CMatrix matrix;

        core->rotation.GetMatrix(matrix);
        matrix.SetPos(core->position);
        player->SetTMLocalToWorld(matrix);
    }
    script->owner = player;
    script->field34 = 0;
    script->field30 = 0;
    script->field3c = 0;
    script->field38 = 0;
    if (g_bInMultiplayerMode) {
        mission = g_Shell.mpMission;
        stage = g_Shell.mpStage;
    } else {
        mission = g_Shell.Get_currentMission();
        stage = g_Shell.Get_currentStage();
    }
    if (!g_bInMultiplayerMode && g_Shell.ShouldIUsePlayersFinishedLevelData(mission, stage)) {
        health = (int)g_Shell.GetPlayersFinishedLevelHealth(mission, stage - 1);
        switch (g_Shell.Get_currentDifficulty()) {
        case 1:
            health = core->health;
            break;
        case 2: {
            float scale = 0.5f;
            health = (float)health < core->health * scale ? (int)(core->health * scale) : health;
            break;
        }
        case 3:
            break;
        default:
            DebugMsg("Unknown difficulty level.  Assuming Level TE\n");
            health = core->health;
            break;
        }
    } else if (g_bInMultiplayerMode) {
        health = g_Shell.mpHealth[player->playerIndex];
    } else {
        health = core->health;
    }
    script->health = health;
    CreateBSObject(core->scriptName, &script->object, (BSGO_Basic*)script, trigger, &script->data);
    player->SetScriptObject(script->object);
    if (g_bInMultiplayerMode)
        BSObjectTriggerEvent(script->object, 44, 0, 0, true);
    else
        BSObjectTriggerEvent(script->object, 44, 0, 0, false);
    player->InitAI();
    return script;
}
