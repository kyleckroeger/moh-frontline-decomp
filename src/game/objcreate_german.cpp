// A fragment of objcreate.cpp (0x8003f1ec): CreateGerman. At the soldier limit
// the trigger is queued (when asked) in the 15-entry German creation queue,
// dropping the newest entry and counting lost guys when the queue wraps onto
// its head, and nothing is created. Otherwise a soldier script object from the
// object allocator is linked at the head of the German list, a soldier of the
// core's model is allocated, given the collision id of the core's team (1 -> 5,
// 2 -> 2 and counted as spawned, 0 -> 7, else -1 with a warning) and the core's
// three weapons (a default for an empty first slot), reset, placed at the
// core's position and yawed to its rotation, and added to the scene. The core's
// health is copied; with a script name the script object is created, attached
// and sent event 44 with the parameter, else the "no child script" warning is
// printed ten times. The soldier's AI is initialised, its first weapon selected
// (the inline CSoldierObject::SelectFirstWeapon) and its aim set forward with
// pitch and yaw limits; the proximity trigger status is updated. The queue is
// part of the file's .bss block (size views, pooled). The .rodata block starts
// with the header primes, the RTTI names and the strings of
// NonVisDestructFarthestSoldiers, CreatePlayer and CreateCollisionVolume, so
// those functions and BSGO_Player::Destroy are compiled here again as file
// duplicates (deferred inlining: reverse image order). The classes, functions
// and globals are named by the symbols; the soldier and aim views and the
// CSoldierObject table slots are inferred from their callers; other views as in
// objcreate_collvol.cpp and objcreate_nonvis.cpp.
void DebugMsg(const char*, ...);
#include "game/CVector3.h"

// Header-defined hash-table primes; every unit that includes the header carries a copy.
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

class BSObject;
class CDrawContext;
class CCollision;
enum EClsnId {};

#include "game/CMatrix.h"


class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

class IDestructible {
public:
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
    virtual void Destroy();
};
class ISubject : public IDestructible {
public:
    virtual ~ISubject();
};
class IObserver : public ISubject {
public:
    virtual ~IObserver();
};
/* ISceneNode's table in slot order up to the script object (+0x60), as AIFilter_visit.cpp. */
class ISceneNode : public IObserver {
public:
    enum EVolumeType {};
    virtual ~ISceneNode();
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
    virtual int GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObject* GetScriptObject() const;
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
};

class BSGO_Basic {
public:
    BSGO_Basic() : m_object(0), m_next(0), m_prev(0) {}

    BSObject* m_object;
    BSGO_Basic* m_next;
    BSGO_Basic* m_prev;

    virtual void Destroy() {}
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData(); /* result type not known */
    virtual void* GetEmbeddedHitReactionInfo(); /* result type not known */
};

class CPlayerObject;
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
    void CleanUp();

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

void DestroyBSObject(BSObject*);

/* inferred: the player script object (see player_ctor.cpp), its player at +44 */
class BSGO_Player : public BSGO_Basic {
public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData();

    unsigned char unknown10[28];
    CPlayerObject* m_player;
};

class BSUtilObjectInstanceMemoryAllocator {
public:
    void* GetFreeElement(bool);

    unsigned char unknown00[28];
};
struct TriggerObject_struct;
struct DtorChainView {
    unsigned char data[12];
};
struct DumpObjectContainerView {
    unsigned char data[0x10];
};
struct GermanQueueEntry {
    TriggerObject_struct* trigger;
    int param;
    int unknown08;
};
struct DelayDestroyListView {
    unsigned char data[0x50];
};
static DtorChainView s_chain861;
static BSUtilObjectInstanceMemoryAllocator g_pObjectAllocators[2];
static DtorChainView s_chain862;
static BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;
static DtorChainView s_chain863;
static BSUtilObjectInstanceMemoryAllocator g_pAnimatedVolumeAllocator;
static DtorChainView s_chain864;
static BSUtilObjectInstanceMemoryAllocator g_pEnvModMechAllocator;
static DtorChainView s_chain865;
static DumpObjectContainerView g_DumpObjectContainer;
static GermanQueueEntry g_pGermanCreationQueue[15];
static DelayDestroyListView g_pDelayDestroyList;

inline void* operator new(unsigned long, void* p) { return p; }

class CSoldierObject;
class BSGO_Soldier : public BSGO_Basic {
public:
    static void* operator new(unsigned long) { return g_pObjectAllocators[1].GetFreeElement(true); }
    BSGO_Soldier() {}
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData();
    virtual void* GetEmbeddedHitReactionInfo();

    int m_health;
    void* m_scriptData;
    unsigned char unknown18[204];
    CSoldierObject* m_node;
    int m_stateE8;
    int m_stateEC;
    int m_stateF0;
    int m_stateF4;
};

struct TriggerCoreView {
    unsigned char unknown00[16];
    float position[3];
    CQuaternion rotation;
    short shape;
    short size[3];
    unsigned char unknown34;
    union {
        bool created;       /* player cores */
        unsigned char team; /* soldier cores */
    };
    unsigned char unknown36[18];
    unsigned long model;
    char* scriptName;
    unsigned char unknown50[32];
    unsigned char health;
    unsigned char unknown71[39];
    int weapons[3];
};

struct TriggerObject_struct {
    unsigned int flags;
    TriggerCoreView* core;
    TriggerCoreView* GetCore() { return core; }
};

class BSGO_Destructable : public BSGO_Basic {
public:
    static void* operator new(unsigned long) { return g_pObjectAllocators[0].GetFreeElement(true); }
    BSGO_Destructable() {}
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData();

    int m_health;
    unsigned char unknown14[4];
    void* m_scriptData;
    class CCollisionVolume* m_node;
};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
};
class CVolSphere : public IVolume {
public:
    CVolSphere() {}
    virtual ~CVolSphere();
    void SetRadius(float);
    void SetCenter(CVector3);
    unsigned char unknown04[20];
};
class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    void SetBasis(CVector3, CVector3, CVector3);
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetCenter(CVector3);
    unsigned char unknown04[76];
};

class CCollisionVolume : public ISceneNode {
public:
    CCollisionVolume();
    void Init(IVolume*, CMatrix*, EClsnId);
    virtual void SetScript(BSObject*);
};
extern BSGO_Destructable* g_pBoxList;

int CreateBSObject(const char*, BSObject**, BSGO_Basic*, TriggerObject_struct*, void**);
bool BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);

class CScene {
public:
    void Add(ISceneNode&);
    ISceneNode* GetPlayer(int) const;
    unsigned char unknown[0x4850];
};
extern CScene g_scene;

struct NonVisDestructView {
    unsigned char unknown0000[12640];
    bool nonVisDestruct;
};

extern bool g_bInMultiplayerMode;
class CStaticObject;
class CWeapon {
public:
    void Update(float, bool);
};
struct AnimDataView {
    unsigned char unknown00[0x4a];
    short m_flags;
};
struct WeaponView {
    unsigned char unknown00[0x280];
    AnimDataView* m_anim;
};
class CAIFilterSoldierObject {
public:
    void UpdateManualUpdateVars();
};
struct SoldierAimView {
    void SetDirection(CVector3 d) { m_direction = d; }
    void SetPitchLimits(float lo, float hi) { m_limits[0] = lo; m_limits[1] = hi; }
    void SetYawLimits(float lo, float hi) { m_limits[2] = lo; m_limits[3] = hi; }

    unsigned char unknown00[0x3fa0];
    CVector3 m_direction;
    float m_limits[4];
};
class CSoldierObject : public ISceneNode {
public:
    virtual void Reset();
    virtual void f0e0();
    virtual void f0e4();
    virtual void f0e8();
    virtual void f0ec();
    virtual void f0f0();
    virtual void f0f4();
    virtual void f0f8();
    virtual void f0fc();
    virtual void f100();
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void f10c();
    virtual void f110();
    virtual void f114();
    virtual void f118();
    virtual void f11c();
    virtual void f120();
    virtual void f124();
    virtual void f128();
    virtual void SetScript(BSObject*);
    virtual void f130();
    virtual void AttachObject(CStaticObject*, char*, int);
    virtual void DetachObject(char*);
    virtual void f13c();
    virtual void f140();
    virtual void f144();
    virtual void InitAI();

    void AddWeapon(int, int);
    void SelectFirstWeapon();
    void SetBulletEmitter(CStaticObject*, char*);
    unsigned char unknown04[0x30e0];
    CAIFilterSoldierObject* m_filter;
    unsigned char unknown30e8[0x48];
    SoldierAimView* m_aim;
    unsigned char unknown3134[0xedc];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
};
enum EObjectClass {};
class CAnimObjectFactory {
public:
    CSoldierObject* AllocateObject(unsigned long, EObjectClass);
    unsigned char unknown[0x1c];
};
extern CAnimObjectFactory g_AnimObjFactory;

extern BSGO_Basic* g_pGermanList;
extern int g_maxActiveSoldiers;
extern int g_numActiveSoldiers;
extern int g_iNumberOfEnemiesSpawned;

extern int g_iNumGermansQueuedThisFrame;
extern int g_iGermanCreationQueueHeadIndex;
extern int g_iGermanCreationQueueTailIndex;

inline void CSoldierObject::SelectFirstWeapon() {
    CWeapon* weapon = m_weapons[m_currentWeapon];
    if (weapon) {
        weapon->Update(1.0f, false);
    } else {
        if (weapon) {
            if (!(((WeaponView*)weapon)->m_anim->m_flags & 1))
                DetachObject("lt_hand");
            else
                DetachObject("rt_hand");
        }
        m_currentWeapon = 0;
        return;
    }
    m_currentWeapon = 0;
    char* bone;
    int boneHash;
    if (!(((WeaponView*)m_weapons[m_currentWeapon])->m_anim->m_flags & 1)) {
        bone = "lt_hand";
        boneHash = 0x4c83c769;
    } else {
        bone = "rt_hand";
        boneHash = 0xd58a96d3;
    }
    if (g_bInMultiplayerMode) {
        DetachObject("lt_hand");
        DetachObject("rt_hand");
    } else {
        DetachObject(bone);
    }
    AttachObject((CStaticObject*)m_weapons[m_currentWeapon], bone, boneHash);
    if (!g_bInMultiplayerMode)
        m_filter->UpdateManualUpdateVars();
    SetBulletEmitter((CStaticObject*)m_weapons[m_currentWeapon], bone);
}

BSGO_Soldier* CreateGerman(TriggerObject_struct* trigger, int param, bool queueIfFull) {
    if (g_numActiveSoldiers >= g_maxActiveSoldiers) {
        if (queueIfFull) {
            int tail = g_iGermanCreationQueueTailIndex;
            g_iNumGermansQueuedThisFrame++;
            g_pGermanCreationQueue[tail].trigger = trigger;
            g_pGermanCreationQueue[tail].param = param;
            g_iGermanCreationQueueTailIndex = tail + 1;
            g_pGermanCreationQueue[tail].unknown08 = 0;
            g_iGermanCreationQueueTailIndex = g_iGermanCreationQueueTailIndex >= 15 ? 0 : g_iGermanCreationQueueTailIndex;
            static int guysLostCount = 0;
            if (g_iGermanCreationQueueTailIndex == g_iGermanCreationQueueHeadIndex) {
                guysLostCount++;
                g_iNumberOfEnemiesSpawned--;
                g_iGermanCreationQueueTailIndex--;
                g_iGermanCreationQueueTailIndex = g_iGermanCreationQueueTailIndex < 0 ? 14 : g_iGermanCreationQueueTailIndex;
                g_pGermanCreationQueue[g_iGermanCreationQueueTailIndex].trigger = 0;
                g_pGermanCreationQueue[g_iGermanCreationQueueTailIndex].param = -1;
                g_pGermanCreationQueue[g_iGermanCreationQueueTailIndex].unknown08 = 0;
            } else {
                guysLostCount = 0;
            }
        }
        return 0;
    }

    int z, y, x;
    g_numActiveSoldiers++;
    TriggerCoreView* core = trigger->core;
    BSGO_Soldier* object = new BSGO_Soldier;
    if (g_pGermanList)
        g_pGermanList->m_prev = object;
    object->m_next = g_pGermanList;
    object->m_prev = 0;
    g_pGermanList = object;

    CSoldierObject* soldier = g_AnimObjFactory.AllocateObject(trigger->core->model, (EObjectClass)1);
    int team;
    switch (core->team) {
    case 1:
        team = 5;
        break;
    case 2:
        team = 2;
        g_iNumberOfEnemiesSpawned++;
        break;
    case 0:
        team = 7;
        break;
    default:
        team = -1;
        DebugMsg("Unknown team affiliation\n");
        break;
    }
    soldier->SetCollisionId((EClsnId)team);

    if (core->weapons[0] == 0)
        soldier->AddWeapon(0, 0x5760e46d);
    else
        soldier->AddWeapon(0, core->weapons[0]);
    soldier->AddWeapon(1, core->weapons[1]);
    soldier->AddWeapon(2, core->weapons[2]);

    CVector3 position;
    position.x = core->position[0];
    position.y = core->position[1];
    position.z = core->position[2];
    soldier->Reset();
    soldier->SetPosition(position);

    {
        CMatrix matrix;
        core->rotation.GetMatrix(matrix);
        float rx, ry, rz;
        matrix.ToEulerXYZ(rx, ry, rz);
        soldier->Yaw(rz);
    }

    g_scene.Add(*soldier);
    object->m_node = soldier;
    object->m_stateEC = 0;
    object->m_stateE8 = 0;
    object->m_stateF4 = 0;
    object->m_stateF0 = 0;
    object->m_health = core->health;

    if (core->scriptName) {
        CreateBSObject(core->scriptName, &object->m_object, object, trigger, &object->m_scriptData);
        soldier->SetScript(object->m_object);
        BSObjectTriggerEvent(object->m_object, 44, (void*)param, 0, false);
    } else {
        x = (int)position.x;
        y = (int)position.y;
        z = (int)position.z;
        DebugMsg("SOON TO BE ERROR! Soldier at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Soldier at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Soldier at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Soldier at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Soldier at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Soldier at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Soldier at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Soldier at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Soldier at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
        DebugMsg("(repeated for importance)! Soldier at x: %d  y: %d  z: %d  has no child script.  Thats bad\n", x, y, z);
    }
    soldier->InitAI();

    soldier->SelectFirstWeapon();

    position.Set(0.0f, 0.0f, 1.0f);
    soldier->m_aim->SetDirection(position);
    soldier->m_aim->SetPitchLimits(-1.2217305f, 1.5707964f);
    soldier->m_aim->SetYawLimits(-0.34906584f, 0.34906584f);
    ForceUpdateProximityTriggerStatus(object, false);
    return object;
}

BSGO_Destructable* CreateCollisionVolume(TriggerObject_struct* trigger, int param) {
    TriggerCoreView* core;
    int const c_param = param;
    TriggerObject_struct* const c_trigger = trigger;
    int z;
    int y;
    int x;
    core = c_trigger->core;
    CCollisionVolume* node;
    IVolume* volume;
    BSGO_Destructable* object = new BSGO_Destructable;
    if (g_pBoxList)
        g_pBoxList->m_prev = object;
    object->m_next = g_pBoxList;
    object->m_prev = 0;
    g_pBoxList = object;

    node = (CCollisionVolume*)g_pEnvModMechAllocator.GetFreeElement(true);
    node = new (node) CCollisionVolume;

    CMatrix matrix;
    CVector3 position;
    position.x = core->position[0];
    position.y = core->position[1];
    position.z = core->position[2];
    core->rotation.GetMatrix(matrix);
    matrix.SetPos(position);

    volume = 0;
    if (core->shape == 0) {
        CVolSphere* sphere = new (g_pVolumeAllocator.GetFreeElement(true)) CVolSphere;
        sphere->SetRadius((float)core->size[0] / 16.0f);
        sphere->SetCenter(CVector3(0.0f, 0.0f, 0.0f));
        sphere->TransformedCopy(*sphere, matrix);
        volume = sphere;
    } else if (core->shape == 2) {
        CVolBox* box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;
        box->SetWidth((float)core->size[0] / 16.0f);
        box->SetDepth(-((float)core->size[2] / 16.0f));
        box->SetHeight((float)core->size[1] / 16.0f);
        box->SetCenter(CVector3(0.0f, 0.0f, 0.0f));
        box->SetBasis(CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
        box->TransformedCopy(*box, matrix);
        volume = box;
    }
    node->Init(volume, &matrix, (EClsnId)12);
    g_scene.Add(*node);
    object->m_node = node;
    object->m_health = core->health;

    if (core->scriptName) {
        CreateBSObject(core->scriptName, &object->m_object, object, c_trigger, &object->m_scriptData);
        node->SetScript(object->m_object);
        BSObjectTriggerEvent(object->m_object, 44, (void*)c_param, 0, false);
    } else {
        x = (int)position.x;
        y = (int)position.y;
        z = (int)position.z;
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
    return object;
}

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
        matrix.SetPos(CVector3(core->position[0], core->position[1], core->position[2]));
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

void BSGO_Player::Destroy() {
    m_player->CleanUp();
    DestroyBSObject(m_object);
    m_object = 0;
    m_player = 0;
}

void NonVisDestructFarthestSoldiers(int count) {
    count -= g_maxActiveSoldiers - g_numActiveSoldiers;
    if (count <= 0)
        return;
    DebugMsg("-----The maximum number of soldiers in existance has been reached.\n");
    DebugMsg("-----This number is currently %d.  It is controled by commondata.crp\n", g_maxActiveSoldiers);
    DebugMsg("-----Attempting to remove %d soldiers\n", count);

    CVector3 playerPos;
    g_scene.GetPlayer(0)->GetPosition(playerPos);

    float distance[128];
    ISceneNode* nodes[128];
    int found = 0;
    for (BSGO_Basic* g = g_pGermanList; g; g = g->m_next, found++) {
        ISceneNode* node = g->GetSceneNode();
        if (((NonVisDestructView*)node)->nonVisDestruct) {
            found--;
            continue;
        }
        CVector3 pos;
        node->GetPosition(pos);
        pos -= playerPos;
        nodes[found] = node;
        distance[found] = pos.LengthSq();
    }

    float limit = 1.0e35f;
    int removed = 0;
    while (removed < count) {
        float best = 0.0f;
        ISceneNode* farthest = 0;
        for (int i = 0; i < found; i++) {
            if (distance[i] > best && distance[i] < limit) {
                farthest = nodes[i];
                best = distance[i];
            }
        }
        limit = best;
        if (farthest) {
            if (BSObjectTriggerEvent(farthest->GetScriptObject(), 45, 0, 0, true)) {
                removed++;
                DebugMsg("-----I found a suitable candidate.\n");
            }
        } else {
            DebugMsg("WARNING:WARNING: I tried to NonVisDestruct %d soldiers because there\n", count);
            DebugMsg("WARNING:WARNING: are too many in existance, but I was only able to\n");
            DebugMsg("WARNING:WARNING: NonVisDestruct %d soldiers.  I'll try again later\n", removed);
            return;
        }
    }
}
