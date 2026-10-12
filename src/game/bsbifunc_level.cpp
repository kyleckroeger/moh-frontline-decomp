// A fragment of bsbifunc.cpp (0x80021a74): built-ins that scale the calling
// node's static object relative to its size by script (a scale, a time and a
// completion event), end the level (recording whether it was won, destroying
// the buddy data and setting the game-state end bit), add a hint (a level of
// 0x7fffffff stands for 20; the text comes from the string table, or null past
// its end; levels and slots are 1-based), toggle the player's trigger checks
// and change the player's team (the AI object's team and its byte copy),
// followed by the weak CPlayerObject::GetAIDoodad (the doodad at +0x24) and
// ModifyTotalKillableEnemyCount (for an enemy trigger created as a duplicate,
// creation mode 2, it reports that and adds the count to the spawned total;
// its message is a .rodata pool entry). The
// file name is this project's; the original record is bsbifunc.cpp and the
// built-ins around these are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (AsStaticObject at
// +116), IMovingSceneNode's and CStaticObject's after them in the order of
// __vt__13CStaticObject (ScaleFromScript at +312; the unnamed entry at +300 as
// a placeholder), the argument helpers are inferred, and the shell, string
// table, hint table, player, doodad and AI object views are inferred (members
// at their offsets, names not original).
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
struct AIDoodadView;
struct AIDoodadView;

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
    virtual int GetScriptObject() const;
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

struct CAIObjectTeamView {
    unsigned char unknown000[20];
    signed char teamByte;
    unsigned char unknown015[435];
    int team;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObjectTeamView* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterView* filter;
};

class IMovingSceneNode : public ISceneNode {
public:
    virtual ~IMovingSceneNode();
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
};

class BSObject;
class BPDLightVolume;
enum EBSEventEnum {};

class CStaticObject : public IMovingSceneNode {
public:
    virtual ~CStaticObject();
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();
    virtual int GetWorldLinearVelocity() const;
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual int GetLightVolume();
    virtual void Init();
    virtual void SetScript(BSObject*);
    virtual void unknown12c();
    virtual void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void ScaleFromScript(float, float, EBSEventEnum);
    virtual void StartMotionPlayback(int, int, EBSEventEnum);
    virtual void* AsThrownObject();
    virtual void* AsWeaponObject();
    virtual void* AsHierTankObject();
};

class CPlayerObject {
public:
    AIDoodadView* GetAIDoodad();

    unsigned char unknown000[36];
    AIDoodadView m_doodad;
    unsigned char unknown02c[876];
    unsigned char m_triggerChecks : 1;
    unsigned char unknown398 : 7;
};

struct ShellEndView {
    unsigned char unknown00[15];
    bool levelWon;
};

struct StringTableEntryView {
    const char* text;
    int unknown04;
};

struct StringTableView {
    StringTableEntryView* entries;
    unsigned char unknown04[8];
    int count;
};

extern ShellEndView g_Shell;
extern unsigned int g_uiGameState;
extern StringTableView umTable;
extern const char* g_HintTextArray[][3];

void DestroyBuddyDataStructures();

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline int BSArgInt(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

inline float BSArgFloat(int** stack, int index) {
    return *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

void BIFunc_ScaleRel(int** stack, void* object) {
    float scale = BSArgFloat(stack, 1);
    float time = BSArgFloat(stack, 2);
    EBSEventEnum event = (EBSEventEnum)BSArgInt(stack, 3);
    CStaticObject* staticObject = ((ISceneNode*)object)->AsStaticObject();
    staticObject->ScaleFromScript(scale, time, event);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_EndLevel(int** stack, void*) {
    g_Shell.levelWon = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    DestroyBuddyDataStructures();
    g_uiGameState |= 1;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AddHint(int** stack, void*) {
    const char* text;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int level = *(*stack - (count - 1));
    int slot = *(*stack - (count - 2));
    int string = *(*stack - (count - 3));
    if (level == 0x7fffffff)
        level = 20;
    if (string < umTable.count)
        text = umTable.entries[string].text;
    else
        text = 0;
    g_HintTextArray[level - 1][slot - 1] = text;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_TogglePlayerVsTriggerChecks(int** stack, void* object) {
    int enable = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    player->m_triggerChecks = enable != 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_ChangePlayersTeam(int** stack, void* object) {
    int team = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CAIFilterView* filter = ((ISceneNode*)((ISceneNode*)object)->AsPlayerObject())->GetAIDoodad()->filter;
    filter->object->team = team;
    filter->object->teamByte = team;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) AIDoodadView* CPlayerObject::GetAIDoodad() {
    return &m_doodad;
}

// Inferred views for BIFunc_ModifyTotalKillableEnemyCount: the current script
// object's trigger and core record (type at +12, the creation mode at +53).
// g_pBSObject is declared through this view; the original symbol is the
// script machine's current BSObject.
struct LevelTriggerCoreView {
    unsigned char unknown00[12];
    int type;
    unsigned char unknown10[37];
    unsigned char creation;
};

struct LevelTriggerView {
    unsigned char unknown00[4];
    LevelTriggerCoreView* core;
};

struct LevelScriptObjectView {
    unsigned char unknown00[8];
    LevelTriggerView* trigger;
};

extern LevelScriptObjectView* g_pBSObject;
extern int g_iNumberOfEnemiesSpawned;
void DebugMsg(const char*, ...);

void BIFunc_ModifyTotalKillableEnemyCount(int** stack, void*) {
    int count = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    LevelTriggerCoreView* core = g_pBSObject->trigger->core;
    if (core->type == 3 && core->creation == 2) {
        DebugMsg("-------------- Duplicate Creation:  Ignoring for total enemy count\n");
        g_iNumberOfEnemiesSpawned += count;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
