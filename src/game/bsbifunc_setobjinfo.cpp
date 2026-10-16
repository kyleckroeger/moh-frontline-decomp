// A fragment of bsbifunc.cpp (0x8002be34): the built-in script function
// SetObjInfo, which sets one kind of object information (the first argument)
// from the target and value arguments and returns whether it was set: 2 adds
// the value to the player's script health, clamped to the trigger's health
// and to 0; 4 switches the soldier's weapon to the value's slot through the
// inline CSoldierObject weapon switch (as in bsbifunc_givegrenade.cpp); 5
// enables or disables dump collisions; 6 sets a soldier flag; 7 points the
// soldier's AI at the target's scene node position, raised by 0.5. The file
// name is this project's; the original record is bsbifunc.cpp. The views,
// the inline and the virtual function names are inferred as in
// bsbifunc_givegrenade.cpp; the empty case 3 and the case order follow the
// dispatch, and the 'self' copy used only for the first call of each case
// and the coordinate locals reproduce the original's register use. The rest
// of the file is in other units.
extern bool g_bInMultiplayerMode;

class CAIFilterSoldierObject {
public:
    void UpdateManualUpdateVars();
};

struct CWeaponPropertiesView {
    unsigned char unknown00[74];
    short hand;
};

class CWeapon {
public:
    void Update(float, bool);

    unsigned char unknown000[640];
    CWeaponPropertiesView* m_properties;
};

class CStaticObject;

class CSoldierObject {
public:
    virtual void Function008();
    virtual void Function00c();
    virtual void Function010();
    virtual void Function014();
    virtual void Function018();
    virtual void Function01c();
    virtual void Function020();
    virtual void Function024();
    virtual void Function028();
    virtual void Function02c();
    virtual void Function030();
    virtual void Function034();
    virtual void Function038();
    virtual void Function03c();
    virtual void Function040();
    virtual void Function044();
    virtual void Function048();
    virtual void Function04c();
    virtual void Function050();
    virtual void Function054();
    virtual void Function058();
    virtual void Function05c();
    virtual void Function060();
    virtual void Function064();
    virtual void Function068();
    virtual void Function06c();
    virtual void Function070();
    virtual void Function074();
    virtual void Function078();
    virtual void Function07c();
    virtual void Function080();
    virtual void Function084();
    virtual void Function088();
    virtual void Function08c();
    virtual void Function090();
    virtual void Function094();
    virtual void Function098();
    virtual void Function09c();
    virtual void Function0a0();
    virtual void Function0a4();
    virtual void Function0a8();
    virtual void Function0ac();
    virtual void Function0b0();
    virtual void Function0b4();
    virtual void Function0b8();
    virtual void Function0bc();
    virtual void Function0c0();
    virtual void Function0c4();
    virtual void Function0c8();
    virtual void Function0cc();
    virtual void Function0d0();
    virtual void Function0d4();
    virtual void Function0d8();
    virtual void Function0dc();
    virtual void Function0e0();
    virtual void Function0e4();
    virtual void Function0e8();
    virtual void Function0ec();
    virtual void Function0f0();
    virtual void Function0f4();
    virtual void Function0f8();
    virtual void Function0fc();
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
    virtual void Function128();
    virtual void Function12c();
    virtual void Function130();
    virtual void AttachWeapon(CWeapon*, const char*, int);
    virtual void DetachWeapon(const char*);
    void AddWeapon(int, int);
    void SetBulletEmitter(CStaticObject*, char*);

    inline void SwitchWeapon(int slot) {
        CWeapon* current = m_weapons[m_currentWeapon];
        const char* bone;
        int crc;

        if (current) {
            current->Update(1.0f, false);
        } else {
            if (current) {
                if ((((slot / 128) + current->m_properties->hand) & 1) == 0)
                    DetachWeapon("lt_hand");
                else
                    DetachWeapon("rt_hand");
            }
            m_currentWeapon = slot & ~0x80;
            return;
        }
        m_currentWeapon = slot & ~0x80;
        if ((((slot / 128) + m_weapons[m_currentWeapon]->m_properties->hand) & 1) == 0) {
            bone = "lt_hand";
            crc = 0x4C83C769;
        } else {
            bone = "rt_hand";
            crc = 0xD58A96D3;
        }
        if (g_bInMultiplayerMode) {
            DetachWeapon("lt_hand");
            DetachWeapon("rt_hand");
        } else {
            DetachWeapon(bone);
        }
        AttachWeapon(m_weapons[m_currentWeapon], bone, crc);
        if (!g_bInMultiplayerMode)
            m_aiFilter->UpdateManualUpdateVars();
        SetBulletEmitter((CStaticObject*)m_weapons[m_currentWeapon], (char*)bone);
    }

    unsigned char unknown0004[12512];
    CAIFilterSoldierObject* m_aiFilter;
    unsigned char unknown30e8[121];
    bool m_flag3161;
    unsigned char unknown3162[3758];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
};

struct ScriptDataView {
    int value;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual ScriptDataView* GetScriptData();
    virtual class ISceneNode* GetSceneNode();
};

struct TriggerCoreView {
    unsigned char unknown00[112];
    unsigned char health;
};

struct TriggerObject_struct {
    unsigned int flags;
    TriggerCoreView* core;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

class CVector3 {
public:
    float x;
    float y;
    float z;
};

struct AIObjectView {
    unsigned char unknown000[336];
    CVector3 lookPoint;
    unsigned char unknown15c[1493];
    bool lookPointSet;
};

struct AIHolderView {
    unsigned char unknown00[8];
    AIObjectView* ai;
};

struct AIDoodadView {
    unsigned char unknown00[4];
    AIHolderView* holder;
};

class ISceneNode {
public:
    virtual void Function008();
    virtual void Function00c();
    virtual void Function010();
    virtual void Function014();
    virtual void Function018();
    virtual void Function01c();
    virtual void Function020();
    virtual void Function024();
    virtual void Function028();
    virtual void Function02c();
    virtual void Function030();
    virtual void Function034();
    virtual void Function038();
    virtual void Function03c();
    virtual void GetPosition(CVector3&) const;
    virtual void Function044();
    virtual void Function048();
    virtual void Function04c();
    virtual void Function050();
    virtual void Function054();
    virtual void Function058();
    virtual void Function05c();
    virtual BSObjectView* GetScriptObject() const;
    virtual void Function064();
    virtual void Function068();
    virtual void Function06c();
    virtual void Function070();
    virtual void Function074();
    virtual void Function078();
    virtual void Function07c();
    virtual void Function080();
    virtual void Function084();
    virtual void Function088();
    virtual void Function08c();
    virtual void Function090();
    virtual CSoldierObject* AsSoldierObject();
    virtual void Function098();
    virtual void Function09c();
    virtual void Function0a0();
    virtual void Function0a4();
    virtual void Function0a8();
    virtual void Function0ac();
    virtual void Function0b0();
    virtual void Function0b4();
    virtual void Function0b8();
    virtual void Function0bc();
    virtual void Function0c0();
    virtual void Function0c4();
    virtual void Function0c8();
    virtual AIDoodadView* GetAIDoodad();
};

class CPlayerObject;

class CScene {
public:
    ISceneNode* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;

enum EDumpType {};
void EnableDumpCollisionsByType(EDumpType, int, bool);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_SetObjInfo(int** stack, void* object) {
    ISceneNode* self = (ISceneNode*)object;
    bool result = false;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int which = *(*stack - (count - 1));
    BSObjectView* target = *(BSObjectView**)(*stack - (count - 2));
    int value = *(*stack - (count - 3));

    switch (which) {
    case 2: {
        BSObjectView* script = g_scene.GetPlayer(0)->GetScriptObject();

        if (script) {
            TriggerCoreView* core = script->trigger->core;
            ScriptDataView* data = script->user->GetScriptData();

            if (core && data) {
                int health = data->value + value;

                if (health > core->health)
                    health = core->health;
                if (health < 0)
                    health = 0;
                data->value = health;
                result = true;
            }
        }
        break;
    }
    case 3:
        break;
    case 4:
        self->AsSoldierObject()->SwitchWeapon(value);
        result = true;
        break;
    case 5:
        EnableDumpCollisionsByType((EDumpType)0, -1, value != 0);
        break;
    case 6: {
        CSoldierObject* soldier = self->AsSoldierObject();

        if (soldier)
            soldier->m_flag3161 = value != 0;
        break;
    }
    case 7:
        if (self && target && self->AsSoldierObject()) {
            AIHolderView* holder = 0;

            if (object)
                holder = ((ISceneNode*)object)->GetAIDoodad()->holder;
            if (holder) {
                ISceneNode* node = 0;
                BSGO_Basic* user = 0;

                if (target)
                    user = target->user;
                if (user)
                    node = user->GetSceneNode();
                if (node) {
                    CVector3 position;
                    AIObjectView* ai;

                    node->GetPosition(position);
                    float z = position.z += 0.5f;
                    float y = position.y;
                    float x = position.x;
                    ai = holder->ai;
                    ai->lookPoint.x = x;
                    ai->lookPoint.y = y;
                    ai->lookPoint.z = z;
                    ai->lookPointSet = true;
                }
            }
        }
        break;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}
