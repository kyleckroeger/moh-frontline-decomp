// A fragment of player.cpp (0x800a68cc): CPlayerObject::Respawn and Init.
// Respawn: unless the multiplayer level is ending, the player's script data
// takes its handicap (multiplayer: from the shell's table; else the script
// core's flags, or 255), and the player's state is reset: the height (1.1),
// the local capsule's line from the origin to that height, the bounding box
// (unit basis, 0.75 x 1.5 x 0.6, centred at (0.25, 0.7, -0.1)), the fire
// delay (RESPAWN_FIRE_DELAY), the state flags of +916..+920 and a number of
// timers, counters and vectors; the controller's rumble byte is cleared, the
// multiplayer data is reinitialised from the shell's player entry and the
// player restarts at its restart point.
// Init: initialises the interface, the same height, capsule line and box
// (after a Reset), and outside multiplayer creates a player weapon object
// for each weapon resource of the level (type from GetWeaponInfoByCRC,
// hidden, sharing the player's local and world boxes), topping up their
// ammunition from the previous level's when the shell says so; it creates
// three infinite-ammunition weapons, reads three values and the script
// explosion scales from the level resource, initialises the hearing volume
// (PlayBulletWhizBys), caches the screen size, resets the state as Respawn
// does (with a few differences), starts crouched when the player resource
// says so, creates the shared mounted-gun crosshair sprite in its static
// memory and the muzzle-flash sprite, and clears the controller and restart
// point. The class, functions and globals are named by the mangled symbols;
// the members, flags, resource layouts and views are inferred (Float3::Set
// is an inferred inline: its arguments are evaluated before the stores).
// CVector3 has a user copy constructor copying the two doublewords one by
// one: it builds by-value temporaries in place yet leaves the dead
// parameter copies of the inlined CLine3 constructor (an implicit copy
// gives the latter but not the former). The constants are entries of the
// file's .sdata2 pool and the sprite name a .rodata pool entry. The rest of
// the file is in other units.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& o) { d.pair[0] = o.d.pair[0]; d.pair[1] = o.d.pair[1]; }
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3 operator=(const CVector3& other) {
        d = other.d;
        return *this;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    CLine3& operator=(const CLine3& other) {
        if (&other != this) {
            m_start = other.m_start;
            m_end = other.m_end;
            m_dir = other.m_dir;
            m_value30 = other.m_value30;
            m_value34 = other.m_value34;
            m_flag38 = other.m_flag38;
            m_flag39 = other.m_flag39;
        }
        return *this;
    }
    void SetSE(CVector3 start, CVector3 end) {
        m_start.d = start.d;
        m_end.d = end.d;
        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

class IVolume {};

class CVolBox : public IVolume {
public:
    void SetBasis(CVector3, CVector3, CVector3);
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetCenter(CVector3);

    unsigned char unknown00[80];
};

struct MOH_core_Struct {
    unsigned char unknown00[112];
    unsigned char flags;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    MOH_core_Struct* core;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual unsigned int* GetScriptData();
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

struct MPPlayerData;

enum EWeaponTypes {};

class CShellMenu {
public:
    int Get_currentMission();
    int Get_currentStage();
    bool ShouldIUsePlayersFinishedLevelData(int, int);
    int GetPlayersFinishedLevelAmmo(int, int, EWeaponTypes);

    unsigned char unknown0000[5148];
    int m_mission;
    int m_stage;
    unsigned char unknown1424[24];
    unsigned int m_handicaps[4];
    unsigned char m_players[4][28];
};

struct InputManagerView {
    unsigned char unknown0000[9286];
    unsigned char m_rumble[4];
};

extern int g_bMPEndLevel;
extern bool g_bInMultiplayerMode;
extern CShellMenu g_Shell;
extern InputManagerView g_inputMgr;
extern int RESPAWN_FIRE_DELAY;
void RestartPlayer(int);

class ISceneNode {
public:
    enum EVolumeType {};
};

/* inferred: three floats set together */
struct Float3 {
    void Set(float ax, float ay, float az) {
        x = ax;
        y = ay;
        z = az;
    }

    float x;
    float y;
    float z;
};

class IMovingSceneNode;
class CBullet;
enum EClsnId {};
enum EObjectClass {};
enum TLTResourceID {};
enum ERenderPriority {};

void* DWI_alloc(const char*, int, int);

inline void* operator new(unsigned long, void* p) {
    return p;
}

class CSprite {
public:
    enum ESpriteLoadType {};

    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }
    static void* operator new(unsigned long, void* p) { return p; }
    CSprite(const char*, ESpriteLoadType, ERenderPriority);

    unsigned char unknown00[100];
};

class CScreen {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual int GetHeight();
    virtual int GetWidth();
};

class CAnimObject {
public:
    void SetLocalBoundingVolume(ISceneNode::EVolumeType, IVolume*);
    void SetWorldBoundingVolume(ISceneNode::EVolumeType, IVolume*);
};

class CPlayerObject;

class CPlayerWeaponObject : public CAnimObject {
public:
    void Init(EWeaponTypes, int, CPlayerObject*);
    void AddAmo(int);

    unsigned char unknown0000[12754];
    unsigned char m_flag31d2a : 2;
    unsigned char m_visible : 1;
    unsigned char m_flag31d2b : 5;
};

class CAnimObjectFactory {
public:
    CAnimObject* AllocateObject(unsigned long, EObjectClass);

    unsigned char unknown00[16];
};

extern CAnimObjectFactory g_AnimObjFactory;

class CWeapon {
public:
    void SetInfiniteAmo(bool);

    unsigned char unknown000[720];
    float m_value2d0;
};

class CWeaponFactory {
public:
    CWeapon* CreateWeapon(EWeaponTypes, ISceneNode*, int);

    unsigned char unknown00[16];
};

extern CWeaponFactory g_WeaponFactory;

class CHearingVolume {
public:
    void Init(IMovingSceneNode*, EClsnId, float, float, void (*)(IMovingSceneNode*, CBullet*));

    unsigned char unknown00[4];
};

class UserInterface {
public:
    void Init();

    unsigned char unknown00[1];
};

/* inferred: the level resource data read here */
struct LevelResourceData {
    unsigned char unknown000[60];
    unsigned long m_crc;
    unsigned char unknown040[172];
    const char* m_crossHairName;
    unsigned char unknown0f0[220];
    int m_startCrouched;
    unsigned char unknown1d0[12];
    float m_value1dc;
    unsigned char unknown1e0[4];
    float m_value1e4;
    unsigned char unknown1e8[4];
    float m_value1ec;
    unsigned char unknown1f0[12];
    float m_explosionVelocityScale;
    unsigned char unknown200[4];
    float m_explosionHeightScale;
};

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    LevelResourceData* m_data;
};

LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void PlayBulletWhizBys(IMovingSceneNode*, CBullet*);

extern CScreen* g_pScreen;
extern int g_cScreenWidth;
extern int g_cScreenHeight;
extern CSprite* g_pMGCrossHair;
extern unsigned char g_pMGCrossHairMem[100];
extern float g_fScriptExplosionVelocityScaleOnPlayer;
extern float g_fScriptExplosionHeightScaleOnPlayer;

class CPlayerObject {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void vf48();
    virtual void vf4c();
    virtual void vf50();
    virtual void vf54();
    virtual void vf58();
    virtual void vf5c();
    virtual BSObjectView* GetScriptObject() const;
    virtual void vf64();
    virtual void vf68();
    virtual void vf6c();
    virtual void vf70();
    virtual void vf74();
    virtual void vf78();
    virtual void vf7c();
    virtual void vf80();
    virtual void vf84();
    virtual void vf88();
    virtual void vf8c();
    virtual void vf90();
    virtual void vf94();
    virtual void vf98();
    virtual void vf9c();
    virtual void vfa0();
    virtual void vfa4();
    virtual void vfa8();
    virtual void vfac();
    virtual void vfb0();
    virtual void vfb4();
    virtual void vfb8();
    virtual void vfbc();
    virtual void vfc0();
    virtual void vfc4();
    virtual void vfc8();
    virtual void vfcc();
    virtual void vfd0();
    virtual void vfd4();
    virtual void vfd8();
    virtual void Reset();

    void Respawn();
    void Init();
    void InitMultiplayer(MPPlayerData*, bool);
    void StartPlayerCrouched();
    static EWeaponTypes GetWeaponInfoByCRC(int, int*, int*);

    unsigned char unknown0004[116];
    float m_value078;
    float m_value07c;
    unsigned char unknown0080[128];
    int m_value100;
    int m_value104;
    unsigned char unknown0108[8];
    CVolBox m_box;
    CVolBox m_worldBox;
    unsigned char unknown01b0[12];
    float m_value1bc;
    CLine3 m_line;
    unsigned char unknown0200[80];
    int m_value250;
    unsigned char unknown0254[68];
    float m_height;
    unsigned char unknown029c[4];
    float m_value2a0;
    unsigned char unknown02a4[28];
    CHearingVolume m_hearing;
    unsigned char unknown02c4[208];
    unsigned char m_flag394_80 : 1;
    unsigned char m_flag394_40 : 1;
    unsigned char m_flag394_20 : 1;
    unsigned char m_flag394_10 : 1;
    unsigned char m_flag394_08 : 1;
    unsigned char m_flag394_04 : 1;
    unsigned char m_flag394_02 : 1;
    unsigned char m_flag394_01 : 1;
    unsigned char m_flag395_80 : 1;
    unsigned char m_flag395_40 : 1;
    unsigned char m_flag395_20 : 1;
    unsigned char m_flag395_10 : 1;
    unsigned char m_flag395_08 : 1;
    unsigned char m_flag395_04 : 1;
    unsigned char m_flag395_02 : 1;
    unsigned char m_flag395_01 : 1;
    unsigned char m_flag396_80 : 1;
    unsigned char m_flag396_40 : 1;
    unsigned char m_flag396_20 : 1;
    unsigned char m_flag396_10 : 1;
    unsigned char m_flag396_08 : 1;
    unsigned char m_flag396_04 : 1;
    unsigned char m_flag396_02 : 1;
    unsigned char m_flag396_01 : 1;
    unsigned char m_flag397_80 : 1;
    unsigned char m_flag397_40 : 1;
    unsigned char m_flag397_20 : 1;
    unsigned char m_flag397_10 : 1;
    unsigned char m_flag397_08 : 1;
    unsigned char m_flag397_04 : 1;
    unsigned char m_flag397_02 : 1;
    unsigned char m_flag397_01 : 1;
    unsigned char m_flag398_80 : 1;
    unsigned char m_flag398_40 : 1;
    unsigned char m_flag398_20 : 1;
    unsigned char m_flag398_10 : 1;
    unsigned char m_flag398_08 : 1;
    unsigned char m_flag398_04 : 1;
    unsigned char m_flag398_02 : 1;
    unsigned char m_flag398_01 : 1;
    unsigned char unknown0399[3];
    float m_value39c;
    unsigned char unknown03a0[64];
    CVector3 m_vector3e0;
    unsigned char unknown03f0[32];
    float m_value410;
    unsigned char unknown0414[24];
    float m_value42c;
    float m_value430;
    unsigned char unknown0434[4];
    float m_value438;
    CSprite* m_muzzleFlash;
    float m_value440;
    float m_value444;
    unsigned char unknown0448[4];
    float m_value44c;
    float m_value450;
    float m_value454;
    float m_value458;
    int m_value45c;
    CPlayerWeaponObject* m_weaponObjects[45];
    unsigned char unknown0514[188];
    int m_value5d0;
    CWeapon* m_weapon5d4;
    CWeapon* m_weapon5d8;
    CWeapon* m_weapon5dc;
    int m_value5e0;
    unsigned char unknown05e4[232];
    int m_value6cc;
    Float3 m_value6d0;
    unsigned char unknown06dc[20];
    int m_value6f0;
    CSprite* m_crossHair;
    unsigned char unknown06f8[88];
    short m_value750;
    short m_value752;
    float m_value754;
    float m_value758;
    unsigned char unknown075c[4];
    int m_value760;
    int m_shellIndex;
    int m_controller;
    int m_restartIndex;
    unsigned char unknown0770[64];
    int m_value7b0;
    float m_value7b4;
    float m_value7b8;
    float m_value7bc;
    float m_value7c0;
    unsigned char m_value7c4;
    unsigned char unknown07c5[3];
    IMovingSceneNode* m_meshNode;
    unsigned char unknown07cc[676];
    UserInterface m_interface;
    unsigned char unknown0a71[2087];
    short m_value1298;
    unsigned char unknown129a[26];
    unsigned char m_value12b4;
    unsigned char unknown12b5[3];
    int m_fireDelay;
};

void CPlayerObject::Respawn() {
    if (!g_bMPEndLevel) {
        BSObjectView* script = GetScriptObject();
        if (script) {
            MOH_core_Struct* core = script->trigger->core;
            unsigned int* data = script->user->GetScriptData();
            if (g_bInMultiplayerMode)
                *data = g_Shell.m_handicaps[m_shellIndex];
            else if (core)
                *data = core->flags;
            else
                *data = 255;
        }
        m_value6f0 = -1;
        m_value6cc = 1;
        m_value100 = 0;
        m_value104 = 0;
        m_value1bc = 0.4f;
        m_height = 1.1f;
        m_line = CLine3(CVector3(0.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, m_height));
        m_box.SetBasis(CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
        m_box.SetWidth(0.75f);
        m_box.SetHeight(1.5f);
        m_box.SetDepth(0.6f);
        m_box.SetCenter(CVector3(0.25f, 0.7f, -0.1f));
        m_value5d0 = 0;
        m_value45c = -1;
        m_value454 = -1.0f;
        m_value458 = -1.0f;
        m_fireDelay = RESPAWN_FIRE_DELAY;
        m_flag394_04 = 0;
        m_flag397_02 = 0;
        m_flag395_80 = 0;
        m_flag395_40 = 0;
        m_flag397_08 = 1;
        m_flag397_04 = 0;
        m_flag395_20 = 0;
        m_flag395_08 = 0;
        m_flag394_01 = 0;
        m_flag395_01 = 0;
        m_value410 = 0.0f;
        m_flag396_80 = 0;
        m_flag396_08 = 0;
        m_flag394_10 = 1;
        m_flag396_01 = 1;
        m_flag396_02 = 0;
        m_flag397_80 = 0;
        m_flag397_01 = 0;
        m_flag397_20 = 1;
        m_flag397_10 = 1;
        m_value758 = 1.0f;
        m_value39c = 0.0f;
        m_value5e0 = 0;
        m_flag398_80 = 1;
        m_flag398_40 = 0;
        m_value7b0 = 0;
        m_value7b4 = 0.0f;
        m_value7b8 = 0.0f;
        m_value7bc = 0.0f;
        m_value7c0 = 0.0f;
        m_value7c4 = 0;
        m_value42c = 35.0f;
        m_value430 = 35.0f;
        m_flag396_40 = 0;
        m_value438 = 0.0f;
        m_vector3e0 = CVector3(0.0f, 0.0f, 0.0f);
        m_value44c = 0.0f;
        m_value450 = 0.0f;
        m_value078 = 0.0f;
        m_value07c = 0.0f;
        m_value440 = 0.0f;
        m_value444 = 0.0f;
        m_value750 = 1;
        m_value752 = 0;
        m_flag395_10 = 0;
        m_flag395_04 = 0;
        m_value754 = 0.0f;
        m_flag397_40 = 0;
        m_value760 = -1;
        m_value2a0 = 0.0f;
        g_inputMgr.m_rumble[m_controller] = 0;
        InitMultiplayer((MPPlayerData*)g_Shell.m_players[m_shellIndex], true);
        RestartPlayer(m_restartIndex);
    }
}

void CPlayerObject::Init() {
    m_interface.Init();
    m_value6f0 = -1;
    m_value6cc = 1;
    m_value100 = 0;
    m_value104 = 0;
    m_meshNode = 0;
    m_value1bc = 0.4f;
    m_height = 1.1f;
    m_line = CLine3(CVector3(0.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, m_height));
    Reset();
    m_box.SetBasis(CVector3(1.0f, 0.0f, 0.0f), CVector3(0.0f, 1.0f, 0.0f), CVector3(0.0f, 0.0f, 1.0f));
    m_box.SetWidth(0.75f);
    m_box.SetHeight(1.5f);
    m_box.SetDepth(0.6f);
    m_box.SetCenter(CVector3(0.25f, 0.7f, -0.1f));
    if (!g_bInMultiplayerMode) {
        LevelFileContentsStruct_* resource;
        for (resource = TLT_FindNextResourceByType((TLTResourceID)19, 0); resource;
             resource = TLT_FindNextResourceByType((TLTResourceID)19, resource)) {
            unsigned long crc = resource->m_data->m_crc;
            EWeaponTypes type = GetWeaponInfoByCRC(crc, 0, 0);
            if (!m_weaponObjects[type]) {
                m_weaponObjects[type] = (CPlayerWeaponObject*)g_AnimObjFactory.AllocateObject(crc, (EObjectClass)3);
                m_weaponObjects[type]->Init(type, 0, this);
                m_weaponObjects[type]->m_visible = 0;
                m_weaponObjects[type]->SetLocalBoundingVolume((ISceneNode::EVolumeType)3, &m_box);
                m_weaponObjects[type]->SetWorldBoundingVolume((ISceneNode::EVolumeType)3, &m_worldBox);
            }
        }
        int mission;
        int stage;
        if (g_bInMultiplayerMode) {
            mission = g_Shell.m_mission;
            stage = g_Shell.m_stage;
        } else {
            mission = g_Shell.Get_currentMission();
            stage = g_Shell.Get_currentStage();
        }
        if (g_Shell.ShouldIUsePlayersFinishedLevelData(mission, stage)) {
            for (int i = 0; i < 45; i++) {
                if (m_weaponObjects[i]) {
                    int ammo = g_Shell.GetPlayersFinishedLevelAmmo(mission, stage - 1, (EWeaponTypes)i);
                    m_weaponObjects[i]->AddAmo(ammo);
                }
            }
        }
    }
    m_weapon5d4 = g_WeaponFactory.CreateWeapon((EWeaponTypes)27, (ISceneNode*)this, 999);
    m_weapon5d4->SetInfiniteAmo(true);
    m_weapon5d8 = g_WeaponFactory.CreateWeapon((EWeaponTypes)43, (ISceneNode*)this, 999);
    m_weapon5d8->SetInfiniteAmo(true);
    m_weapon5dc = g_WeaponFactory.CreateWeapon((EWeaponTypes)39, (ISceneNode*)this, 999);
    m_weapon5dc->SetInfiniteAmo(true);
    LevelFileContentsStruct_* level = TLT_FindNextResourceByType((TLTResourceID)11, 0);
    m_value6d0.Set(level->m_data->m_value1dc, level->m_data->m_value1e4, level->m_data->m_value1ec);
    g_fScriptExplosionVelocityScaleOnPlayer = level->m_data->m_explosionVelocityScale;
    g_fScriptExplosionHeightScaleOnPlayer = level->m_data->m_explosionHeightScale;
    m_value5d0 = 0;
    m_value45c = -1;
    m_value454 = -1.0f;
    m_value458 = -1.0f;
    m_flag394_04 = 0;
    m_flag397_02 = 0;
    m_hearing.Init((IMovingSceneNode*)this, (EClsnId)8, 3.0f, 0.5f, PlayBulletWhizBys);
    g_cScreenWidth = g_pScreen->GetWidth();
    g_cScreenHeight = g_pScreen->GetHeight();
    m_value250 = 0;
    m_flag395_80 = 0;
    m_value12b4 = 0;
    m_flag397_08 = 1;
    m_flag397_04 = 0;
    m_flag395_20 = 0;
    m_flag395_08 = 0;
    m_flag394_01 = 0;
    m_flag395_01 = 0;
    m_value410 = 0.0f;
    m_flag396_80 = 0;
    m_flag396_08 = 0;
    m_flag394_10 = 1;
    m_flag396_01 = 1;
    m_flag396_02 = 0;
    m_flag397_80 = 0;
    m_flag397_01 = 0;
    m_flag397_20 = 1;
    m_flag397_10 = 1;
    m_value758 = 1.0f;
    m_value39c = 0.0f;
    m_value5e0 = 0;
    m_flag398_80 = 1;
    m_flag398_40 = 0;
    m_fireDelay = 0;
    m_value7b0 = 0;
    m_value7b4 = 0.0f;
    m_value7b8 = 0.0f;
    m_value7bc = 0.0f;
    m_value7c0 = 0.0f;
    m_value7c4 = 0;
    m_value42c = 35.0f;
    m_value430 = 35.0f;
    m_flag396_40 = 0;
    m_value438 = 0.0f;
    m_vector3e0 = CVector3(0.0f, 0.0f, 0.0f);
    m_value44c = 0.0f;
    m_value450 = 0.0f;
    m_value078 = 0.0f;
    m_value07c = 0.0f;
    m_value440 = 0.0f;
    m_value444 = 0.0f;
    m_value750 = 1;
    m_value752 = 0;
    m_flag395_10 = 0;
    LevelFileContentsStruct_* player = TLT_FindNextResourceByType((TLTResourceID)13, 0);
    if (player->m_data->m_startCrouched)
        StartPlayerCrouched();
    if (player->m_data->m_value1dc > 0.0f && m_weapon5dc)
        m_weapon5dc->m_value2d0 = player->m_data->m_value1dc;
    m_flag395_04 = 0;
    m_value754 = 0.0f;
    m_flag397_40 = 0;
    if (!g_pMGCrossHair) {
        LevelFileContentsStruct_* hud = TLT_FindNextResourceByType((TLTResourceID)11, 0);
        g_pMGCrossHair = new (g_pMGCrossHairMem) CSprite(hud->m_data->m_crossHairName, (CSprite::ESpriteLoadType)1, (ERenderPriority)10);
    }
    m_crossHair = g_pMGCrossHair;
    if (!m_muzzleFlash)
        m_muzzleFlash = new CSprite("data\\bitmaps\\MuzzleFlash.gsh", (CSprite::ESpriteLoadType)1, (ERenderPriority)7);
    m_value760 = -1;
    m_restartIndex = 0;
    m_controller = 0;
    m_value1298 = 0;
}
