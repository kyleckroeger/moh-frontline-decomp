// A fragment of bsbifunc.cpp (0x800274a0): script built-ins that return a
// thrown grenade's timer as an integer (written below the popped arguments, the
// value kept in memory), tell a thrown grenade it was caught, store the first
// of the script object's player's first nine weapon slots holding a grenade
// type (33, 34, 38 or 40; otherwise -1) in the calling node's script data, and
// return weapon information for the script object's player (for kind 2: the
// current weapon slot, or the current weapon's first or second information
// value; otherwise -1). Each reads its arguments below the script stack top and
// pops the built-in's arguments. The file name is this project's; the original
// record is bsbifunc.cpp and the built-ins around these are not reconstructed.
// GetWeaponType returns the type of the script object's player's current
// weapon (kind 0, negative slot) or of a slot's weapon (kind 0) or of a weapon
// slot by number (kind 2), -1 when the slot is empty, and 2 for other kinds.
// GetSoundInfo takes a sound source (the script object's player, a soldier, the
// script object's node, or the trigger's position) and returns information
// about it: the azimuth from the first player, the distance from the first
// player times 256, the current weapon's sound type, or the speed of the
// script object's mechanism scaled to 220 (with a 0.16 offset in mission 4,
// stage 2 of single player).
// The functions, classes and globals are named by the mangled symbols;
// ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (GetScriptObject at +96) and BSGO_Basic in the order of
// __vt__10BSGO_Basic; the player (weapon slots at +16400, current slot at
// +16500), weapon, thrown-grenade, script-data, script-object and built-in
// record views, the slot and information helpers are inferred (members at their
// offsets, names not original).
/* The MSL inline sqrtf (three Newton steps from frsqrte). */
extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y ;
    }
    return x;
}

enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3 {
public:
    float LengthSq() const { return x * x + y * y + z * z; }
    CVector3& operator-=(const CVector3& o) {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
class CStaticObject;
struct BSObjectView;

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
    virtual BSObjectView* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
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


struct WeaponPropertiesView {
    unsigned char unknown00[18];
    short info1;
};

class CWeapon {
public:
    short GetWeaponSoundType() const;

    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    unsigned char unknown284[2];
    short m_info0;
    unsigned char unknown288[16];
    int m_type;
};

struct PlayerWeaponView {
    unsigned char unknown0000[12516];
    CWeapon* weapon;
};

class CPlayerObject {
public:
    CWeapon* GetCurrentWeapon() const;
    PlayerWeaponView* GetWeapon(int) const;

    CWeapon* GetSlotWeapon(int index) {
        if (index < 0)
            return m_weapons[m_currentWeapon];
        return m_weapons[index];
    }

    unsigned char unknown0000[16400];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
};

/* inferred: the moving state, speed values and velocity of a static object */
class CStaticObject {
public:
    unsigned char unknown000[128];
    float m_speedScale;
    unsigned char unknown084[84];
    float m_pathSpeed;
    float m_pathSpeedMax;
    unsigned char unknown0e0[36];
    int m_moveState;
    unsigned char unknown108[88];
    CVector3 m_velocity;
};

class CScene {
public:
    ISceneNode* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;

class CShellMenu {
public:
    unsigned int Get_currentMission();
    unsigned int Get_currentStage();

    unsigned char unknown0000[6624];
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;

unsigned short GetAzimuth(CVector3, const CPlayerObject*);

class CThrownBullet {
public:
    void SomebodyCaughtYou();

    unsigned char unknown00[144];
    float m_timer;
};

struct ScriptDataView {
    unsigned char unknown00[128];
    int grenadeSlot;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual ScriptDataView* GetScriptData();
    virtual CPlayerObject* GetSceneNode();
};

struct TriggerCoreView {
    unsigned char unknown00[16];
    float position[3];
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

extern BSObjectView* g_pBSObject;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_GetGrenadeTimer(int** stack, void*) {
    int value = 0;
    value = (*(CThrownBullet**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)))->m_timer;
    **stack = *(int*)&value;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_CatchGrenade(int** stack, void*) {
    (*(CThrownBullet**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)))->SomebodyCaughtYou();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetGrenadeSlot(int** stack, void* object) {
    int slot = -1;
    for (int i = 0; i < 9; i++) {
        CWeapon* weapon = g_pBSObject->user->GetSceneNode()->GetSlotWeapon(i);
        if (weapon) {
            int type = weapon->m_type;
            if (type == 33 || type == 34 || type == 38 || type == 40) {
                slot = i;
                break;
            }
        }
    }
    ((ISceneNode*)object)->GetScriptObject()->user->GetScriptData()->grenadeSlot = slot;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

inline int GetWeaponInfo(CWeapon* weapon, int info) {
    switch (info) {
    case 0:
        return weapon->m_info0;
    case 1:
        return weapon->m_properties->info1;
    }
    return -1;
}

void BIFunc_GetWeaponInfo(int** stack, void*) {
    int which = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int value = -1;
    int info = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 2));
    switch (which) {
    case 2: {
        CPlayerObject* player = g_pBSObject->user->GetSceneNode();
        if (info == 2)
            value = player->m_currentWeapon;
        else
            value = GetWeaponInfo(player->GetSlotWeapon(-1), info);
        break;
    }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&value;
}

void BIFunc_GetWeaponType(int** stack, void*) {
    int value;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int kind = *(*stack - (count - 1));
    int index = *(*stack - (count - 2));
    /* The kind is switched on through a copy (the original's register choice). */
    int selector;
    selector = kind;
    switch (selector) {
    case 0: {
        CPlayerObject* player = g_pBSObject->user->GetSceneNode();
        if (index < 0) {
            CWeapon* weapon = player->GetCurrentWeapon();
            if (weapon)
                value = weapon->m_type;
            else
                value = -1;
        } else {
            CWeapon* weapon = player->GetWeapon(index)->weapon;
            if (weapon)
                value = weapon->m_type;
            else
                value = -1;
        }
        break;
    }
    case 2: {
        CWeapon* weapon = g_pBSObject->user->GetSceneNode()->GetSlotWeapon(index);
        if (weapon)
            value = weapon->m_type;
        else
            value = -1;
        break;
    }
    case 1:
    default:
        value = 2;
        break;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&value;
}

void BIFunc_GetSoundInfo(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int info = *(*stack - (count - 1));
    int source = *(*stack - (count - 2));
    int result = 0;
    CWeapon* weapon = 0;
    CVector3 position;

    switch (source) {
    case 0: {
        CPlayerObject* player = g_pBSObject->user->GetSceneNode();
        ((ISceneNode*)player)->GetPosition(position);
        weapon = player->GetCurrentWeapon();
        break;
    }
    case 2: {
        CPlayerObject* player = g_pBSObject->user->GetSceneNode();
        ((ISceneNode*)player)->GetPosition(position);
        weapon = player->m_weapons[player->m_currentWeapon];
        break;
    }
    case 5:
        ((ISceneNode*)g_pBSObject->user->GetSceneNode())->GetPosition(position);
        break;
    case 1: {
        TriggerCoreView* core = g_pBSObject->trigger->core;
        float x;
        float y;
        float z;
        z = core->position[2];
        y = core->position[1];
        x = core->position[0];

        position.x = x;
        position.y = y;
        position.z = z;
        break;
    }
    }
    switch (info) {
    case 0: {
        CPlayerObject* player = (CPlayerObject*)g_scene.GetPlayer(0);

        result = GetAzimuth(position, player);
        break;
    }
    case 1: {
        ISceneNode* player = g_scene.GetPlayer(0);
        CVector3 listener;

        if (player) {
            player->GetPosition(listener);
        } else {
            listener.x = listener.y = listener.z = 0.0f;
        }
        position -= listener;
        result = 256.0f * sqrtf(position.LengthSq());
        break;
    }
    case 2:
        result = weapon->GetWeaponSoundType();
        break;
    case 3: {
        CStaticObject* object = ((ISceneNode*)g_pBSObject->user->GetSceneNode())->AsStaticObject();

        if (g_Shell.Get_currentMission() == 4 && g_Shell.Get_currentStage() == 2 && !g_bInMultiplayerMode) {
            bool moving = false;

            if (object->m_moveState != 0 && object->m_moveState != 1)
                moving = true;
            if (moving)
                result = (0.16f + object->m_pathSpeed) * (220.0f / (0.16f + object->m_pathSpeedMax));
            else {
                float length = sqrtf(object->m_velocity.LengthSq());

                result = length * (220.0f * object->m_speedScale);
            }
        } else {
            bool moving = false;

            if (object->m_moveState != 0 && object->m_moveState != 1)
                moving = true;
            if (moving)
                result = object->m_pathSpeed * (220.0f / object->m_pathSpeedMax);
            else {
                float length = sqrtf(object->m_velocity.LengthSq());

                result = length * (220.0f * object->m_speedScale);
            }
        }
        break;
    }
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&result;
}
