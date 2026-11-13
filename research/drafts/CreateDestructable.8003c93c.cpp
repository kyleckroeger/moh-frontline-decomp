// best WIP (harvest1, 2026-11-12): 11 real lines (was 33; index said 33), from
// scratch/loop3/scripting/cd/a1.cpp: x/y/z declared at the top. Left: the placement-new knot (State
// question 7: CW null-checks placement new; systems5 cd_i6/cd_o4 reach 3/1 only with cast copies or a
// double beq).
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




#include "game/CVector3.h"


/* inferred: three floats */
struct Float3 {
    float x;
    float y;
    float z;
};

#include "game/CMatrix.h"


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
    Float3 position;
    CQuaternion rotation;
    unsigned char unknown2c[9];
    bool created;
    unsigned char unknown36[18];
    int objectType;
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

#include "game/CPlayerObject.h"


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


class BSUtilObjectInstanceMemoryAllocator {
public:
    void* GetFreeElement(bool);

    unsigned char unknown00[40];
};

extern BSUtilObjectInstanceMemoryAllocator g_pObjectAllocators[2];
extern BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;

inline void* operator new(unsigned long, void* p) {
    return p;
}

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    CVolBox& operator=(const CVolBox&);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetCenter(CVector3);

    unsigned char unknown04[76];
};

#include "game/ISceneNode.h"


/* inferred: a mesh's bounding box: basis rows and centre */
struct MeshBoxView {
    unsigned char unknown00[16];
    CVector3 m_right;
    CVector3 m_front;
    CVector3 m_up;
    CVector3 m_center;
};

struct MeshDataView {
    unsigned char unknown00[32];
    MeshBoxView* m_box;
};

struct MeshView {
    unsigned char unknown00[4];
    MeshDataView* m_data;
};

class CStaticObject {
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
    virtual void Function124();
    virtual void SetScriptObject(BSObject*);
    void SetLocalBoundingVolume(ISceneNode::EVolumeType, IVolume*);
    void SetWorldBoundingVolume(ISceneNode::EVolumeType, IVolume*);

    unsigned char unknown004[44];
    MeshView* m_mesh;
};

class CStaticObjectFactory {
public:
    static CStaticObject* AllocateStaticObject(int, int);
};

#include "game/CScene.h"


extern CScene g_scene;

class BSGO_Basic {
public:
    BSGO_Basic() : m_object(0), m_next(0), m_prev(0) {}

    BSObject* m_object;
    BSGO_Basic* m_next;
    BSGO_Basic* m_prev;

    virtual void Destroy();
};

/* inferred: a destructable's script object (48 bytes) */
class BSGO_Destructable : public BSGO_Basic {
public:
    static void* operator new(unsigned long) { return g_pObjectAllocators[0].GetFreeElement(true); }
    BSGO_Destructable() {}
    virtual void Destroy();

    int m_health;
    unsigned char unknown14[4];
    void* m_data;
    CStaticObject* m_object28;
    int m_value2c;
    int m_value30;
    int m_value34;
    int m_value38;
};

extern BSGO_Basic* g_pBoxList;
void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);

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
    script = (BSGO_PlayerView*)player->m_scriptObject;
    {
        CMatrix matrix;

        core->rotation.GetMatrix(matrix);
        matrix.SetPos(CVector3(core->position.x, core->position.y, core->position.z));
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
        health = g_Shell.mpHealth[player->m_controller];
    } else {
        health = core->health;
    }
    script->health = health;
    CreateBSObject(core->scriptName, &script->object, (BSGO_Basic*)script, trigger, &script->data);
    player->SetScript(script->object);
    if (g_bInMultiplayerMode)
        BSObjectTriggerEvent(script->object, 44, 0, 0, true);
    else
        BSObjectTriggerEvent(script->object, 44, 0, 0, false);
    player->InitAI();
    return script;
}

BSGO_Destructable* CreateDestructable(TriggerObject_struct* trigger, int param) {
    TriggerObject_struct* const c_trigger = trigger;
    int const c_param = param;
    int z;
    int y;
    int x;

    TriggerCoreView* core = c_trigger->core;
    BSGO_Destructable* script = new BSGO_Destructable;
    if (g_pBoxList)
        g_pBoxList->m_prev = script;
    script->m_next = g_pBoxList;
    script->m_prev = 0;
    g_pBoxList = script;
    CStaticObject* object = CStaticObjectFactory::AllocateStaticObject(core->objectType, -1);
    CMatrix matrix;
    CVector3 position(core->position.x, core->position.y, core->position.z);
    core->rotation.GetMatrix(matrix);
    matrix.SetPos(position);
    object->SetTMLocalToWorld(matrix);
    CVolBox* box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;
    MeshView* mesh = object->m_mesh;
    CVector3 right = mesh->m_data->m_box->m_right;
    CVector3 front = mesh->m_data->m_box->m_front;
    CVector3 up = mesh->m_data->m_box->m_up;
    box->SetWidth(2.0f * right.Length());
    box->SetHeight(2.0f * up.Length());
    box->SetDepth(2.0f * front.Length());
    right.Normalize();
    front.Normalize();
    up.Normalize();
    box->SetBasis(right, front, up);
    box->SetCenter(mesh->m_data->m_box->m_center);
    object->SetLocalBoundingVolume((ISceneNode::EVolumeType)3, box);
    object->SetLocalBoundingVolume((ISceneNode::EVolumeType)0, box);
    CVolBox* worldBox = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;
    *worldBox = *box;
    object->SetWorldBoundingVolume((ISceneNode::EVolumeType)3, worldBox);
    object->SetWorldBoundingVolume((ISceneNode::EVolumeType)0, worldBox);
    object->Function124();
    g_scene.Add(*(ISceneNode*)object);
    script->m_object28 = object;
    script->m_value30 = 0;
    script->m_value2c = 0;
    script->m_value38 = 0;
    script->m_value34 = 0;
    script->m_health = core->health;
    if (core->scriptName) {
        CreateBSObject(core->scriptName, &script->m_object, script, c_trigger, &script->m_data);
        object->SetScriptObject(script->m_object);
        BSObjectTriggerEvent(script->m_object, 44, (void*)c_param, 0, false);
    } else {
        x = position.x;
        y = position.y;
        z = position.z;
        DebugMsg("SOON TO BE ERROR! Mechanic at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Mechanic at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Mechanic at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Mechanic at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Mechanic at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Mechanic at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Mechanic at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Mechanic at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Mechanic at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Mechanic at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
    }
    ForceUpdateProximityTriggerStatus(script, true);
    return script;
}
