// A fragment of bsbifunc.cpp (0x800300cc): built-ins that report whether the
// first player is disguised (the script data's two disguise flags set and no
// weapon other than type 37 drawn; the arguments are popped twice), whether the
// player is aiming (a flag bit of the player), add a hit direction to the
// player's interface (directions 0 to 3 as interface directions 1, 3, 4 and 2),
// return the player's weapon refire rate (0 without a weapon), start the
// player's death sequence (in multiplayer also dropping the multiplayer dump
// object; in single player for the first player) stop the first player's path
// and start it (the first player moving on two spline paths found by id in the
// AI filter's spline path list, none for id 0, at a speed). Each pops the
// built-in's arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode and
// BSGO_Basic in the order of __vt__10BSGO_Basic, CPlayerObject derives from
// ISceneNode, and the weapon, player, script-data, spline-path list, scene and
// built-in record views and the weapon-type and spline-path search helpers are
// inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
class CAISplinePath;
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

struct WeaponPropertiesView {
    unsigned char unknown0[6];
    short refireRate;
};

class CWeapon {
public:
    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    unsigned char unknown284[20];
    int m_type;
};

class UserInterface {
public:
    void AddHitDirection(unsigned int);
};

struct DisguiseView {
    unsigned char unknown00[16];
    int disguised;
    int active;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual DisguiseView* GetScriptData();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

class CPlayerObject : public ISceneNode {
public:
    CWeapon* GetCurrentWeapon() const;
    void StartDeathSequence();
    void DropMPDumpObject();
    void StopPath();
    void MoveOnPath(CAISplinePath*, CAISplinePath*, float);

    unsigned char unknown004[912];
    unsigned char unknown394 : 4;
    unsigned char m_aiming : 1;
    unsigned char unknown394b : 3;
    unsigned char unknown395[1755];
    UserInterface m_interface;
};

class CAISplinePath {
public:
    unsigned char unknown00[8];
    unsigned int m_id;
};

struct SplinePathListView {
    unsigned int count;
    CAISplinePath* paths;
};

class CAIFilterGlobal {
public:
    CAISplinePath* FindSplinePath(int id) {
        SplinePathListView* list = m_splinePaths;
        unsigned int i;
        for (i = 0; i < list->count; i++) {
            if (id == list->paths[i].m_id)
                break;
        }
        return &list->paths[i];
    }

    unsigned char unknown00[12];
    SplinePathListView* m_splinePaths;
    unsigned char unknown10[8];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

#include "game/CScene.h"


extern CScene g_scene;
extern bool g_bInMultiplayerMode;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline int GetWeaponType(CPlayerObject* player) {
    CWeapon* weapon = player->GetCurrentWeapon();
    int type = -1;
    if (weapon)
        type = weapon->m_type;
    return type;
}

inline bool HasArmedWeapon(CPlayerObject* player) {
    int type = GetWeaponType(player);
    if (type == 37 || type == -1)
        return false;
    return true;
}

void BIFunc_IsPlayerDisguised(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    DisguiseView* disguise = g_scene.GetPlayer(0)->GetScriptObject()->user->GetScriptData();
    bool disguised = false;
    if (disguise->disguised && disguise->active && !HasArmedWeapon(g_scene.GetPlayer(0)))
        disguised = true;
    **stack = disguised;
}

void BIFunc_IsPlayerAiming(int** stack, void* object) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = ((ISceneNode*)object)->AsPlayerObject()->m_aiming;
}

void BIFunc_PlayerAddHitDirection(int** stack, void* object) {
    int direction = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    if (direction == 0)
        player->m_interface.AddHitDirection(1);
    else if (direction == 1)
        player->m_interface.AddHitDirection(3);
    else if (direction == 2)
        player->m_interface.AddHitDirection(4);
    else if (direction == 3)
        player->m_interface.AddHitDirection(2);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerGetWeaponRefireRate(int** stack, void* object) {
    CWeapon* weapon = ((ISceneNode*)object)->AsPlayerObject()->GetCurrentWeapon();
    int rate = 0;
    if (weapon)
        rate = weapon->m_properties->refireRate;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = rate;
}

void BIFunc_PlayerDeath(int** stack, void* object) {
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    if (g_bInMultiplayerMode) {
        player->StartDeathSequence();
        player->DropMPDumpObject();
    } else {
        g_scene.GetPlayer(0)->StartDeathSequence();
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerStopPath(int** stack, void*) {
    g_scene.GetPlayer(0)->StopPath();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerStartPath(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int first = *(*stack - (count - 1));
    int second = *(*stack - (count - 2));
    float speed = *(float*)(*stack - (count - 3));
    CAISplinePath* firstPath;
    CAISplinePath* secondPath;
    if (first)
        firstPath = g_aigAIFilterGlobalObject.FindSplinePath(first);
    else
        firstPath = 0;
    if (second)
        secondPath = g_aigAIFilterGlobalObject.FindSplinePath(second);
    else
        secondPath = 0;
    g_scene.GetPlayer(0)->MoveOnPath(firstPath, secondPath, speed);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
