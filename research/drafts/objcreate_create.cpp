// objcreate.cpp: creating collision volumes and soldiers from level triggers, as
// partial source. Views are inferred from these functions. Virtual functions not
// yet identified are called through their table offset, marked "vtable +N".
void DebugMsg(const char*, ...);

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { d = other.d; }
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3Data d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetPos(CVector3);
    void ToEulerXYZ(float&, float&, float&) const;
    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;
    float x, y, z, w;
};

// The level data behind a trigger.
struct TriggerCoreView {
    unsigned char unknown00[16];
    float position[3];
    CQuaternion rotation;
    short shape;        // +0x2c: 0 sphere, 2 box
    short size[3];      // +0x2e: sphere radius or box width, then height, then depth
    unsigned char unknown34;
    unsigned char team; // +0x35: soldiers only
    unsigned char unknown36[18];
    unsigned int model; // +0x48
    char* scriptName;   // +0x4c
    unsigned char unknown50[32];
    unsigned char health; // +0x70
    unsigned char unknown71[39];
    int weapons[3];     // +0x98
};

struct TriggerObject_struct {
    unsigned int flags;
    TriggerCoreView* core;
};

class BSObject;
class BSGO_Basic;
int CreateBSObject(const char*, BSObject**, BSGO_Basic*, TriggerObject_struct*, void**);
bool BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);

class BSUtilObjectInstanceMemoryAllocator {
public:
    void* GetFreeElement(bool);
    unsigned char unknown00[28];
};
extern BSUtilObjectInstanceMemoryAllocator g_pObjectAllocators[2];
extern BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;
extern BSUtilObjectInstanceMemoryAllocator g_pEnvModMechAllocator;

inline void* operator new(unsigned long, void* p) { return p; }

class BSGO_Basic {
public:
    BSGO_Basic() : m_script(0), m_next(0), m_prev(0) {}
    // The data comes first: CodeWarrior puts the virtual table pointer where the
    // first virtual function is declared, and the original has it at +0x0c
    // (the constructor clears +0, +4 and +8, then stores the table at +0x0c).
    BSObject* m_script;
    BSGO_Basic* m_next;
    BSGO_Basic* m_prev;
    virtual void Function08();
};
class BSGO_Destructable : public BSGO_Basic {
public:
    BSGO_Destructable() {}
    virtual void Function08();
    int m_health;           // +0x10
    unsigned char unknown14[4];
    void* m_scriptData;     // +0x18
    class CCollisionVolume* m_node; // +0x1c
};
class BSGO_Soldier : public BSGO_Basic {
public:
    BSGO_Soldier() {}
    virtual void Function08();
    int m_health;           // +0x10
    void* m_scriptData;     // +0x14
    unsigned char unknown18[204];
    class CSoldierObject* m_node; // +0xe4 (BSGO_Soldier::GetSceneNode reads it here)
    int m_stateE8;
    int m_stateEC;
    int m_stateF0;
    int m_stateF4;
};

static inline void* Slot(void* object, int offset) { return *(void**)(*(char**)object + offset); }

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
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

class ISceneNode;
class CScene {
public:
    void Add(ISceneNode&);
    unsigned char unknown[0x148]; // g_scene's size: addressed absolutely, not through r13
};
extern CScene g_scene;

enum EClsnId {};
class ISceneNode {};
class CCollisionVolume : public ISceneNode {
public:
    CCollisionVolume();
    void Init(IVolume*, CMatrix*, EClsnId);
};

typedef void (*TransformVolumeFn)(IVolume*, IVolume*, const CMatrix*);
typedef void (*SetScriptObjectFn)(void*, BSObject*);

extern BSGO_Destructable* g_pBoxList;

// Trigger sizes are fixed point, in sixteenths.
static const float kSizeScale = 0.0625f;

BSGO_Destructable* CreateCollisionVolume(TriggerObject_struct* trigger, int param) {
    TriggerCoreView* core = trigger->core;
    BSGO_Destructable* object = new (g_pObjectAllocators[0].GetFreeElement(true)) BSGO_Destructable;
    if (g_pBoxList)
        g_pBoxList->m_prev = object;
    object->m_next = g_pBoxList;
    object->m_prev = 0;
    g_pBoxList = object;

    CCollisionVolume* node = new (g_pEnvModMechAllocator.GetFreeElement(true)) CCollisionVolume;

    CMatrix matrix;
    CVector3 position(core->position[0], core->position[1], core->position[2]);
    core->rotation.GetMatrix(matrix);
    matrix.SetPos(position);

    IVolume* volume = 0;
    if (core->shape == 0) {
        CVolSphere* sphere = new (g_pVolumeAllocator.GetFreeElement(true)) CVolSphere;
        volume = sphere;
        sphere->SetRadius((float)core->size[0] * kSizeScale);
        sphere->SetCenter(CVector3(0.0f, 0.0f, 0.0f));
        ((TransformVolumeFn)Slot(volume, 0x10))(volume, volume, &matrix); // vtable +0x10
    } else if (core->shape == 2) {
        CVolBox* box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;
        volume = box;
        box->SetWidth((float)core->size[0] * kSizeScale);
        box->SetDepth(-((float)core->size[2] * kSizeScale));
        box->SetHeight((float)core->size[1] * kSizeScale);
        box->SetCenter(CVector3(0.0f, 0.0f, 0.0f));
        box->SetBasis(CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
        ((TransformVolumeFn)Slot(volume, 0x10))(volume, volume, &matrix); // vtable +0x10
    }
    node->Init(volume, &matrix, (EClsnId)12);
    g_scene.Add(*node);
    object->m_node = node;
    object->m_health = core->health;

    if (!core->scriptName) {
        int x = (int)position.d.v[0];
        int y = (int)position.d.v[1];
        int z = (int)position.d.v[2];
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
    } else {
        CreateBSObject(core->scriptName, &object->m_script, object, trigger, &object->m_scriptData);
        ((SetScriptObjectFn)Slot(node, 0xdc))(node, object->m_script); // vtable +0xdc
        BSObjectTriggerEvent(object->m_script, 44, (void*)param, 0, false);
    }
    return object;
}

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
    unsigned char unknown00[0x3fa0];
    CVector3 m_direction;
    float m_limits[4];
};
class CSoldierObject {
public:
    void AddWeapon(int, int);
    void SetBulletEmitter(CStaticObject*, char*);
    unsigned char unknown00[0x30e4];
    CAIFilterSoldierObject* m_filter;
    unsigned char unknown30e8[0x48];
    SoldierAimView* m_aim;
    unsigned char unknown3134[0xedc];
    CWeapon* m_weapons[25];
    int m_currentWeapon; // +0x4074
};
enum EObjectClass {};
class CAnimObjectFactory {
public:
    CSoldierObject* AllocateObject(unsigned long, EObjectClass);
    unsigned char unknown[0x1c]; // g_AnimObjFactory's size: addressed absolutely, not through r13
};
extern CAnimObjectFactory g_AnimObjFactory;

extern BSGO_Soldier* g_pGermanList;
extern int g_maxActiveSoldiers;
extern int g_numActiveSoldiers;
extern int g_iNumberOfEnemiesSpawned;
extern bool g_bInMultiplayerMode;

// Soldiers that can't be created yet wait in a ring of 15.
struct GermanQueueEntry {
    TriggerObject_struct* trigger;
    int param;
    int unknown08;
};
extern GermanQueueEntry g_pGermanCreationQueue[15];
extern int g_iNumGermansQueuedThisFrame;
extern int g_iGermanCreationQueueHeadIndex;
extern int g_iGermanCreationQueueTailIndex;

typedef void (*NoArgFn)(void*);
typedef void (*SetTeamFn)(void*, int);
typedef void (*SetPositionFn)(void*, CVector3);
typedef void (*SetHeadingFn)(void*, float);
typedef void (*HideWeaponFn)(void*, const char*);
typedef void (*AttachWeaponFn)(void*, CWeapon*, const char*, unsigned int);

// The original wraps this count in a guarded function-local static.
struct LostCounter {
    LostCounter() : count(0) {}
    int count;
};

BSGO_Soldier* CreateGerman(TriggerObject_struct* trigger, int param, bool queueIfFull) {
    if (g_numActiveSoldiers < g_maxActiveSoldiers) {
        g_numActiveSoldiers++;
        TriggerCoreView* core = trigger->core;
        BSGO_Soldier* object = new (g_pObjectAllocators[1].GetFreeElement(true)) BSGO_Soldier;
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
        case 0:
            team = 7;
            break;
        case 2:
            team = 2;
            g_iNumberOfEnemiesSpawned++;
            break;
        default:
            team = -1;
            DebugMsg("Unknown team affiliation\n");
            break;
        }
        ((SetTeamFn)Slot(soldier, 0x5c))(soldier, team); // vtable +0x5c

        if (core->weapons[0] == 0)
            soldier->AddWeapon(0, 0x5760e46d); // the default weapon's hash
        else
            soldier->AddWeapon(0, core->weapons[0]);
        soldier->AddWeapon(1, core->weapons[1]);
        soldier->AddWeapon(2, core->weapons[2]);

        CVector3 position(core->position[0], core->position[1], core->position[2]);
        ((NoArgFn)Slot(soldier, 0xdc))(soldier); // vtable +0xdc
        ((SetPositionFn)Slot(soldier, 0x108))(soldier, position); // vtable +0x108

        CMatrix matrix;
        core->rotation.GetMatrix(matrix);
        float rx, ry, rz;
        matrix.ToEulerXYZ(rx, ry, rz);
        ((SetHeadingFn)Slot(soldier, 0x104))(soldier, rz); // vtable +0x104

        g_scene.Add(*(ISceneNode*)soldier);
        object->m_node = soldier;
        object->m_stateEC = 0;
        object->m_stateE8 = 0;
        object->m_stateF4 = 0;
        object->m_stateF0 = 0;
        object->m_health = core->health;

        if (!core->scriptName) {
            int x = (int)position.d.v[0];
            int y = (int)position.d.v[1];
            int z = (int)position.d.v[2];
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
        } else {
            CreateBSObject(core->scriptName, &object->m_script, object, trigger, &object->m_scriptData);
            ((SetScriptObjectFn)Slot(soldier, 0x12c))(soldier, object->m_script); // vtable +0x12c
            BSObjectTriggerEvent(object->m_script, 44, (void*)param, 0, false);
        }
        ((NoArgFn)Slot(soldier, 0x148))(soldier); // vtable +0x148

        // Select weapon 0. The inner test in the else branch can never pass; the
        // original compiled it anyway, probably from an inlined helper that hides
        // the old weapon.
        CWeapon* weapon = soldier->m_weapons[soldier->m_currentWeapon];
        if (weapon) {
            weapon->Update(1.0f, false);
            soldier->m_currentWeapon = 0;
            const char* bone;
            unsigned int boneHash;
            if (!(((WeaponView*)soldier->m_weapons[soldier->m_currentWeapon])->m_anim->m_flags & 1)) {
                bone = "lt_hand";
                boneHash = 0x4c83c769;
            } else {
                bone = "rt_hand";
                boneHash = 0xd58a96d3;
            }
            if (g_bInMultiplayerMode) {
                ((HideWeaponFn)Slot(soldier, 0x138))(soldier, "lt_hand"); // vtable +0x138
                ((HideWeaponFn)Slot(soldier, 0x138))(soldier, "rt_hand");
            } else {
                ((HideWeaponFn)Slot(soldier, 0x138))(soldier, bone);
            }
            ((AttachWeaponFn)Slot(soldier, 0x134))(soldier, soldier->m_weapons[soldier->m_currentWeapon], bone, boneHash); // vtable +0x134
            if (!g_bInMultiplayerMode)
                soldier->m_filter->UpdateManualUpdateVars();
            soldier->SetBulletEmitter((CStaticObject*)soldier->m_weapons[soldier->m_currentWeapon], (char*)bone);
        } else {
            if (weapon) {
                if (!(((WeaponView*)weapon)->m_anim->m_flags & 1))
                    ((HideWeaponFn)Slot(soldier, 0x138))(soldier, "lt_hand");
                else
                    ((HideWeaponFn)Slot(soldier, 0x138))(soldier, "rt_hand");
            }
            soldier->m_currentWeapon = 0;
        }

        soldier->m_aim->m_direction = CVector3(0.0f, 0.0f, 1.0f);
        soldier->m_aim->m_limits[0] = -1.2217305f; // -70 degrees
        soldier->m_aim->m_limits[1] = 1.5707964f;  // 90 degrees
        soldier->m_aim->m_limits[2] = -0.34906584f; // -20 degrees
        soldier->m_aim->m_limits[3] = 0.34906584f;  // 20 degrees
        ForceUpdateProximityTriggerStatus(object, false);
        return object;
    }

    if (queueIfFull) {
        int tail = g_iGermanCreationQueueTailIndex;
        g_pGermanCreationQueue[tail].trigger = trigger;
        g_iNumGermansQueuedThisFrame++;
        g_pGermanCreationQueue[tail].param = param;
        int next = tail + 1;
        if (next >= 15)
            next = 0;
        g_pGermanCreationQueue[tail].unknown08 = 0;
        static LostCounter guysLostCount;
        if (next == g_iGermanCreationQueueHeadIndex) {
            // Full: drop this soldier again and step the tail back.
            guysLostCount.count++;
            g_iNumberOfEnemiesSpawned--;
            tail = next - 1;
            if (tail < 0)
                tail = 14;
            g_iGermanCreationQueueTailIndex = tail;
            g_pGermanCreationQueue[tail].trigger = 0;
            g_pGermanCreationQueue[tail].param = -1;
            g_pGermanCreationQueue[tail].unknown08 = 0;
        } else {
            guysLostCount.count = 0;
            g_iGermanCreationQueueTailIndex = next;
        }
    }
    return 0;
}
