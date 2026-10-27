// mainloop.cpp (0x8007a5f8), the whole file. Gameloop_Restart restarts the
// level in place: it silences and stops the sound and music, resets the
// publisher, destructor queue, lights and scene, clears the scene-node links
// of the global camera, the player's camera, the sky box and the world (+4/+8),
// resets the compartments, re-acquires the world's four id sets, resets the
// factories, particle systems and decals, re-initialises and re-adds the
// player, reloads the scripts, the player start and the level settings, waits
// for the world to load and restarts music, sound schedule and pause state.
// Gameloop_Loop runs one game-loop step: it resumes and pauses debug timer 4,
// updates the script system (BSUtilUpdate, timed into profile bin 11 with
// OSGetTick and counted), then for each player places the player's camera:
// the camera's world matrix is the player's view matrix (+192) times the
// player's local-to-world matrix, the field of view comes from the player's
// angle in degrees (halved aspect for two players), and the camera is made
// current, updated and made current again. Gameloop_Shutdown dumps the profiler
// timers, resets the time stamps, shuts the scene and sky box down, shuts down
// and deletes every player, then shuts down the scripting system, the
// factories, the sound schedule and level sound modules, the pause screen and
// the level contents. Gameloop_Init loads a level - it
// takes the mission and stage (from the shell's multiplayer setup when that is
// active), restarts the sound system, clears the cheats in multiplayer, loads
// and registers the level's animation file, the level contents and sound
// modules, creates the world, the factories and the players (one, or one per
// connected controller with its viewports, key map and multiplayer data), sets
// each player's camera clip range and perspective, adds the sky box, loads the
// level settings and the world (reporting progress through the callback twice
// per step), sets the multiplayer time limit and registers the profiler
// timers. Then LoadLevelSettings applies the level
// resource's settings - the two player camera values (from +124 and +132) to
// every player, the landmark and time stamp strings of the current mission
// and stage (user messages 452 and 476 onwards, four per mission), the
// shell's byte at +16, the fog colour and range, the screen's clear colour,
// the ambient light and three directional lights (normalised directions,
// colours and intensities). The level settings view (8-byte property values),
// the message table, the shell's multiplayer view, the camera members (the
// clip range at +36/+40 and two validity flags at +322) and the player and
// world members are inferred; the constants are .sdata2 pool entries. Then the
// static initialisation: the global
// animated-object, bullet and weapon factories and the bullet-decal manager
// are built through their constructors, the last three with their
// destructors registered (the animated-object factory has none). The player
// table between them needs no construction. The classes and globals are named by the
// symbols; the object sizes come from the symbols and the other members are
// opaque here.
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
        return y;
    }
    return x;
}

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            d.v[0] *= scale;
            d.v[1] *= scale;
            d.v[2] *= scale;
        }
    }

    CVector3Data d;
} __attribute__((aligned(8)));

struct CColor {
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CLight {
public:
    enum EDefaultLights {};
    static void SetDefaultLight(EDefaultLights, CVector3, CColor, float);
    static void ResetClass();
};

void SetFog(CColor&, float, float);
void SetLandmarkStampString(char*);
void SetTimeStampString(char*);

class CScreen {
public:
    void SetClearColor(const CColor&);
    int GetVSyncs();

    unsigned char unknown000[268];
};

extern CScreen g_screen;

enum ESoundMode {};

/* inferred: the shell's multiplayer setup */
struct MPPlayerData {
    unsigned char unknown00[28];
};

struct MPShellView {
    int active;
    int numPlayers;
    int weaponSetID;
    int mission;
    int stage;
    int timeLimit;
    unsigned char unknown18[4];
    int timeLimitOn;
    unsigned char unknown20[28];
    MPPlayerData players[4];
    int controllers[4];
};

class CShellMenu {
public:
    int Get_currentMission();
    int Get_currentStage();
    ESoundMode GetSoundModeValue();
    void SetGlobalCheatStructure();
    void SetupAllPlayersButtonsForMultiplayerGame();

    unsigned char unknown0000[16];
    unsigned char m_value10;
    unsigned char unknown0011[5119];
    MPShellView m_mp;
    unsigned char unknown14cc[1296];
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;

/* inferred: the user message table (entries of text and type, count +12) */
struct MessageView {
    char* text;
    unsigned char type;
    unsigned char unknown05[3];
};

struct MessageTableView {
    MessageView* entries;
    unsigned char unknown04[8];
    int count;

    char* GetText(int i) const { return i < count ? entries[i].text : 0; }
};

extern MessageTableView umTable;

/* inferred: a level property (a value and its type word) */
struct LevelValue {
    int i;
    unsigned char unknown04[4];
};

struct LevelFloat {
    float f;
    unsigned char unknown04[4];
};

/* inferred: a light of the level settings (direction, colour, intensity) */
struct LevelLightView {
    LevelFloat direction[3];
    LevelValue color[3];
    LevelFloat intensity;
};

/* inferred: the level resource's settings */
struct LevelSettingsView {
    unsigned char unknown000[124];
    LevelFloat fov;
    LevelFloat fovMultiplayer;
    unsigned char unknown08c[8];
    LevelValue fogColor[3];
    LevelFloat fogNear;
    LevelFloat fogFar;
    LevelValue clearColor[3];
    unsigned char unknown0d4[24];
    LevelValue ambientColor[3];
    LevelFloat ambientIntensity;
    LevelLightView lights[3];
    unsigned char unknown1b4[168];
    LevelValue value25c;
};

struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    LevelSettingsView* m_data;
};

enum TLTResourceID {};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);

class ISceneNode;

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void Multiply(const CMatrix&, const CMatrix&);

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CCamera {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void AttemptUpdate(float);
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
    virtual void vf60();
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
    virtual void vfdc();
    virtual void vfe0();
    virtual void vfe4();
    virtual void vfe8(const CMatrix&);
    virtual void vfec();
    virtual void vff0();
    virtual void vff4();
    virtual void vff8();
    virtual void vffc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    static const float ASPECT_4x3;
    void SetPerspective(float, float);
    void SetNear(float value) {
        m_near = value;
        m_projectionValid = false;
        m_flag143 = false;
    }
    void ClearNodes() {
        m_node08 = 0;
        m_node04 = 0;
    }
    void SetFar(float value) {
        m_far = value;
        m_projectionValid = false;
        m_flag143 = false;
    }

    ISceneNode* m_node04;
    ISceneNode* m_node08;
    unsigned char unknown00c[24];
    float m_near;
    float m_far;
    unsigned char unknown02c[278];
    bool m_projectionValid;
    bool m_flag143;
    unsigned char unknown144[172];
};

extern const float DEFAULT_FOV;

struct MPPlayerData;

/* inferred: the player's two view values, each clearing two update flags */
/* inferred: the player's member object at +1904 (its virtual table at +12) */
class PlayerMemberView {
    unsigned char unknown00[12];

public:
    virtual void Shutdown();

    unsigned char unknown10[28];
    int m_value2c;
    unsigned char unknown30[48];
};

class CPlayerObject {
public:
    CPlayerObject();
    virtual void vf08();
    virtual ~CPlayerObject();
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
    virtual void GetTMLocalToWorld(CMatrix&) const;
    void Init();
    void ReInit();
    void InitMultiplayer(MPPlayerData*, bool);
    void SetViewports(int a, int b) {
        m_viewport = a;
        m_viewportA64 = a;
        m_viewport1288 = b;
    }

    ISceneNode* m_node04;
    ISceneNode* m_node08;
    unsigned char unknown00c[180];
    CMatrix m_matrix;
    unsigned char unknown100[812];
    float m_fov;
    unsigned char unknown430[820];
    int m_controller;
    int m_viewport;
    int m_player;
    PlayerMemberView m_member;
    CCamera m_camera;
    unsigned char unknown9c0[164];
    int m_viewportA64;
    unsigned char unknowna68[2080];
    int m_viewport1288;
    unsigned char unknown128c[68];
};

class CAnimObjectFactory {
public:
    CAnimObjectFactory();
    void SetLargestObjectSize(int);
    void Init(unsigned long, char*);
    void Shutdown();
    void Reset();

    unsigned char unknown00[28];
};

class CBulletFactory {
public:
    CBulletFactory();
    ~CBulletFactory();
    void Init();
    void Shutdown();
    void Reset();

    unsigned char unknown00[52];
};

class CWeaponFactory {
public:
    CWeaponFactory();
    ~CWeaponFactory();
    void Init();
    void Shutdown();
    void Reset();

    unsigned char unknown00[28];
};

class CBulletDecalManager {
public:
    CBulletDecalManager();
    ~CBulletDecalManager();
    void Init__NotTheVirtualOne();
    void Reset();

    unsigned char unknown000[1484];
};


CAnimObjectFactory g_AnimObjFactory;
CPlayerObject* g_pPlayers[4];
CBulletFactory g_BulletFactory;
CWeaponFactory g_WeaponFactory;
CBulletDecalManager g_BulletDecalManager;

void LoadLevelSettings();

class CSprite;
struct AnimFileFormat_t;
class CAnimDebugFile;
struct BSObject;
struct TriggerObject_struct;

extern "C" int sprintf(char*, const char*, ...);
extern "C" void AnimFileRelocate(AnimFileFormat_t*, int);
void* TLT_LoadFileNormal(const char*, int*, int);
void TLT_CloseFile(void*);
void TLT_Init(int, int);
void LoadLevelSoundModules(int, int);
void SoundClose();
void SoundInit(ESoundMode);
void NullifyScreenAndLibrary();
void PauseIsScreen();
void PauseIsLibrary();
int BSUtilInitScriptingSystem(bool);
void FindPlayerStartTriggerAndCreatePlayer(bool, int);
int BSSendMessage(unsigned short, BSObject*, TriggerObject_struct*, BSObject*, void*);
void InitTimeStampStuff();
void SetXFilesTextOnscreenTime(float);
void UpdateREAL();
void DebugProfTimerInitAll();
void DebugProfTimerRegister(int, char*, int);

class CAnimDatabase {
public:
    void AddFile(AnimFileFormat_t*, CAnimDebugFile*);

    unsigned char unknown00[176];
};
extern CAnimDatabase g_AnimDB;

/* IDestructible's and ISceneNode's first virtuals (table order) */
class CCompartment {
public:
    static void GlobalReset();
    void Reset();
};

class CWorldObject {
public:
    struct SAcquireSet {
        int ids[4];
    };
    CWorldObject();
    virtual void MarkForDestruction(int);
    virtual ~CWorldObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    void Acquire(const SAcquireSet&, bool);
    CCompartment* GetCompartment(int);

    ISceneNode* m_node04;
    ISceneNode* m_node08;
    unsigned char unknown00c[188];
    int m_pending;
    unsigned char unknown0cc[156];
    float m_loaded;
    int m_total;
};
extern CWorldObject::SAcquireSet g_idSet;

class ISceneNode {};
class CSkyBox : public ISceneNode {
public:
    static CSkyBox* CreateInstance();
    static CSkyBox* GetInstance();
    static void DestroyInstance();
    void SetTextures();
    void ClearNodes() {
        m_node08 = 0;
        m_node04 = 0;
    }

    unsigned char unknown00[4];
    ISceneNode* m_node04;
    ISceneNode* m_node08;
};

class CScene {
public:
    void AddPlayer(CPlayerObject*);
    void SetCamera(CCamera*);
    void Add(ISceneNode&);
    void SetWorld(CWorldObject*);
    void Shutdown();
    void Reset();
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};
extern CScene g_scene;

struct KEYMAP {
    unsigned char unknown000[260];
};
extern KEYMAP g_config_keymap[4];

class CInputManager {
public:
    void SetKeymap(unsigned long, KEYMAP*);

    unsigned char unknown0000[9296];
};
extern CInputManager g_inputMgr;

class CDualConfig {
public:
    void SetupButtonsForGame();

    unsigned char unknown00[56];
    int m_player;
    unsigned char unknown3c[8];
};
extern CDualConfig g_ControllerConfig;

enum PauseState {};

class Pause {
public:
    void PauseInit();
    void PauseShutdown();
    void SetState(PauseState);

    unsigned char unknown00[152];
};
extern Pause g_Pause;

class CAIFilterGlobal {
public:
    unsigned char unknown00[20];
    int m_largestObjectSize;
};
extern CAIFilterGlobal g_aigAIFilterGlobalObject;

class CStaticObjectFactory {
public:
    static void Init();
    static void Reset();
};

class CTankObject {
public:
    static void VerifyAIDataSize();
};

/* inferred: the cheat flags (bytes and one float) */
struct CheatsView {
    unsigned char flag00;
    unsigned char flag01;
    unsigned char flag02;
    unsigned char flag03;
    float value04;
    unsigned char flag08;
    unsigned char flag09;
    unsigned char flag0a;
    unsigned char flag0b;
    unsigned char flag0c;
    unsigned char flag0d;
    unsigned char flag0e;
    unsigned char flag0f;
    unsigned char flag10;
};
extern CheatsView g_cheats;

extern int g_levelWeaponSetID;
extern int g_FinalSinglePlayerController;
extern AnimFileFormat_t* g_pLevelEMT; // file-local in the original
extern CWorldObject* g_pWorld;
extern CBulletFactory* g_pBulletFactory;
extern CWeaponFactory* g_pWeaponFactory;
extern int g_mpTimeLimit;
extern int g_bMPEndLevel;

class SoundSchedule {
public:
    void Init();
    void Shutdown();
};
extern SoundSchedule g_SoundSchedule;

void DebugProfTimerDumpAll();
void ResetTimeStampStuff();
void BSUtilShutdownScriptingSystem();
void UnloadLevelSoundModules();
void TLT_Shutdown();

class CPublisher {
public:
    static void Reset();
};

class CDestructorQueue {
public:
    static void Reset();
};

class CParticleSystem {
public:
    static void Reset();
};

/* inferred: one bin of the debug profiler */
struct SProfileBin {
    int m_id;
    int m_values[5];
};

extern SProfileBin g_ProfileBins[31];
extern CCamera g_camera;

extern "C" unsigned long OSGetTick();
void DebugProfTimerResume(int);
void DebugProfTimerPause(int);
void BSUtilUpdate(float);
void SoundSetMasterVolume(int);
void SoundSync();
void MUSIC_Stop();
void MUSIC_Start(int, int, int);
void AmbientTrack_Select(int);
void ResetOneShotBank();

void Gameloop_Restart() {
    SoundSetMasterVolume(0);
    g_SoundSchedule.Shutdown();
    MUSIC_Stop();
    AmbientTrack_Select(0);
    ResetOneShotBank();
    CPublisher::Reset();
    CDestructorQueue::Reset();
    CLight::ResetClass();
    g_scene.Reset();
    g_camera.ClearNodes();
    g_pPlayers[0]->m_camera.ClearNodes();
    g_scene.SetCamera(&g_camera);
    CSkyBox* skybox = CSkyBox::GetInstance();
    skybox->ClearNodes();
    g_scene.Add(*skybox);
    CCompartment::GlobalReset();
    for (int i = 0; i < 4; i++) {
        CCompartment* compartment = g_pWorld->GetCompartment(i);
        if (compartment)
            compartment->Reset();
    }
    CWorldObject::SAcquireSet set = {0, 1, 2, 3};
    g_pWorld->Acquire(set, true);
    g_AnimObjFactory.Reset();
    CStaticObjectFactory::Reset();
    CParticleSystem::Reset();
    g_BulletFactory.Reset();
    g_WeaponFactory.Reset();
    g_BulletDecalManager.Reset();
    g_pPlayers[0]->ReInit();
    g_pPlayers[0]->m_node04 = 0;
    g_pPlayers[0]->m_node08 = 0;
    g_scene.AddPlayer(g_pPlayers[0]);
    g_pPlayers[0]->m_member.m_value2c = 0;
    BSUtilInitScriptingSystem(true);
    FindPlayerStartTriggerAndCreatePlayer(true, 0);
    LoadLevelSettings();
    while (g_pWorld->m_pending > 0) {
        UpdateREAL();
        SoundSync();
        g_pWorld->BeginUpdate(1.0f);
    }
    g_pWorld->m_node08 = 0;
    g_pWorld->m_node04 = 0;
    g_scene.SetWorld(g_pWorld);
    MUSIC_Start(0, 500, 0);
    g_SoundSchedule.Init();
    SoundSetMasterVolume(127);
    g_Pause.SetState((PauseState)0);
}

void Gameloop_Loop(float delta) {
    DebugProfTimerResume(4);
    DebugProfTimerPause(4);
    g_ProfileBins[11].m_values[1] -= OSGetTick();
    BSUtilUpdate(delta);
    g_ProfileBins[11].m_values[1] += OSGetTick();
    g_ProfileBins[11].m_values[4]++;
    CCamera* camera;
    const CMatrix* matrix;
    int i;
    CPlayerObject* player;
    for (i = 0; i < g_NumPlayers; i++) {
        player = g_scene.GetPlayer(i);
        camera = &player->m_camera;
        matrix = &player->m_matrix;
        CMatrix local;
        player->GetTMLocalToWorld(local);
        CMatrix world;
        world.Multiply(*matrix, local);
        camera->vfe8(world);
        camera->vf110();
        float fov = 0.017453292f * player->m_fov;
        if (g_NumPlayers == 2)
            camera->SetPerspective(fov, 0.5f * fov * CCamera::ASPECT_4x3);
        else
            camera->SetPerspective(fov, fov * CCamera::ASPECT_4x3);
        g_scene.SetCamera(camera);
        camera->AttemptUpdate(delta);
        g_scene.SetCamera(camera);
    }
}

void Gameloop_Shutdown() {
    DebugProfTimerDumpAll();
    ResetTimeStampStuff();
    g_scene.Shutdown();
    CSkyBox::DestroyInstance();
    for (int i = 0; i < g_NumPlayers; i++) {
        if (g_pPlayers[i]) {
            g_pPlayers[i]->m_member.Shutdown();
            delete g_pPlayers[i];
            g_pPlayers[i] = 0;
        }
    }
    BSUtilShutdownScriptingSystem();
    g_AnimObjFactory.Shutdown();
    g_BulletFactory.Shutdown();
    g_WeaponFactory.Shutdown();
    g_pWeaponFactory = 0;
    g_pBulletFactory = 0;
    g_SoundSchedule.Shutdown();
    UnloadLevelSoundModules();
    g_Pause.PauseShutdown();
    TLT_Shutdown();
}

static inline void SetupCamera(CCamera* camera, float fov) {
    camera->SetNear(0.4f);
    camera->SetFar(200.0f);
    camera->SetPerspective(fov, fov * CCamera::ASPECT_4x3);
    g_scene.SetCamera(camera);
}

static inline float LoadFraction(CWorldObject* world) {
    if (world->m_total != 0)
        return world->m_loaded / (float)(world->m_total * 2);
    return 0.0f;
}

void Gameloop_Init(void (*progress)(CSprite*, CSprite*, float), CSprite* background, CSprite* bar) {
    char name[128];
    int mission;
    int stage;
    if (g_bInMultiplayerMode) {
        mission = g_Shell.m_mp.mission;
        stage = g_Shell.m_mp.stage;
    } else {
        mission = g_Shell.Get_currentMission();
        stage = g_Shell.Get_currentStage();
    }
    MPShellView* mp;
    MPShellView* info;
    info = mp = &g_Shell.m_mp;
    g_bInMultiplayerMode = mp->active != 0;
    if (g_bInMultiplayerMode) {
        mission = g_Shell.m_mp.mission = mp->mission;
        stage = g_Shell.m_mp.stage = mp->stage;
        g_NumPlayers = mp->numPlayers;
        g_levelWeaponSetID = mp->weaponSetID;
    } else {
        g_NumPlayers = 1;
    }
    SoundClose();
    SoundInit(g_Shell.GetSoundModeValue());
    progress(0, bar, 0.1f);
    progress(0, bar, 0.1f);
    g_Shell.SetGlobalCheatStructure();
    if (g_bInMultiplayerMode) {
        g_cheats.flag00 = 0;
        g_cheats.flag01 = 0;
        g_cheats.flag02 = 0;
        g_cheats.flag03 = 0;
        g_cheats.value04 = 0.0f;
        g_cheats.flag08 = 0;
        g_cheats.flag09 = 0;
        g_cheats.flag0a = 0;
        g_cheats.flag0b = 0;
        g_cheats.flag0d = 0;
        g_cheats.flag0e = 0;
        g_cheats.flag0c = 0;
        g_cheats.flag0f = 0;
        g_cheats.flag10 = 0;
    }
    sprintf(name, "data/emt_%d_%d.emt", mission, stage);
    g_pLevelEMT = (AnimFileFormat_t*)TLT_LoadFileNormal(name, 0, 256);
    AnimFileRelocate(g_pLevelEMT, 0);
    g_AnimDB.AddFile(g_pLevelEMT, 0);
    TLT_CloseFile(g_pLevelEMT);
    progress(0, bar, 0.2f);
    progress(0, bar, 0.2f);
    TLT_Init(mission, stage);
    progress(0, bar, 0.3f);
    progress(0, bar, 0.3f);
    LoadLevelSoundModules(mission, stage);
    progress(0, bar, 0.35f);
    progress(0, bar, 0.35f);
    g_pWorld = new CWorldObject;
    progress(0, bar, 0.4f);
    progress(0, bar, 0.4f);
    g_inputMgr.SetKeymap(0, g_config_keymap);
    if (mp->active == 0) {
        g_ControllerConfig.m_player = 0;
        g_ControllerConfig.SetupButtonsForGame();
    } else {
        g_Shell.SetupAllPlayersButtonsForMultiplayerGame();
    }
    NullifyScreenAndLibrary();
    PauseIsScreen();
    PauseIsLibrary();
    g_Pause.PauseInit();
    g_AnimObjFactory.SetLargestObjectSize(g_aigAIFilterGlobalObject.m_largestObjectSize);
    if (g_bInMultiplayerMode)
        g_AnimObjFactory.Init(52, 0);
    else
        g_AnimObjFactory.Init(26, 0);
    CStaticObjectFactory::Init();
    g_BulletFactory.Init();
    g_WeaponFactory.Init();
    g_pWeaponFactory = &g_WeaponFactory;
    g_pBulletFactory = &g_BulletFactory;
    g_BulletDecalManager.Init__NotTheVirtualOne();
    if (!g_bInMultiplayerMode) {
        g_pPlayers[0] = new CPlayerObject;
        g_pPlayers[0]->Init();
        g_scene.AddPlayer(g_pPlayers[0]);
        g_pPlayers[0]->SetViewports(0, 0);
        g_pPlayers[0]->m_controller = g_FinalSinglePlayerController;
        BSUtilInitScriptingSystem(false);
        FindPlayerStartTriggerAndCreatePlayer(false, 0);
    } else {
        BSUtilInitScriptingSystem(false);
        int i;
        int count;
        for (i = 0, count = 0; i < 4; i++) {
            if (info->controllers[i] != -1) {
                g_pPlayers[count] = new CPlayerObject;
                g_pPlayers[count]->Init();
                g_pPlayers[count]->m_player = count;
                if (g_NumPlayers < 3)
                    g_pPlayers[count]->SetViewports(count, count);
                else
                    g_pPlayers[count]->SetViewports(i, count);
                g_pPlayers[count]->m_controller = info->controllers[i];
                g_inputMgr.SetKeymap(info->controllers[i], &g_config_keymap[info->controllers[i]]);
                FindPlayerStartTriggerAndCreatePlayer(false, count);
                g_pPlayers[count]->InitMultiplayer(&info->players[i], false);
                g_scene.AddPlayer(g_pPlayers[count]);
                count++;
            }
        }
        BSSendMessage(678, 0, 0, 0, 0);
    }
    progress(0, bar, 0.5f);
    progress(0, bar, 0.5f);
    float fov = 0.017453292f * DEFAULT_FOV;
    for (int j = 0; j < g_NumPlayers; j++)
        SetupCamera(&g_pPlayers[j]->m_camera, fov);
    CSkyBox* skybox = CSkyBox::CreateInstance();
    g_scene.Add(*skybox);
    skybox->SetTextures();
    InitTimeStampStuff();
    SetXFilesTextOnscreenTime(15.0f);
    LoadLevelSettings();
    progress(0, bar, 0.6f);
    progress(0, bar, 0.6f);
    g_pWorld->Acquire(g_idSet, true);
    progress(0, bar, 0.8f);
    progress(0, bar, 0.8f);
    g_scene.SetWorld(g_pWorld);
    while (g_pWorld->m_pending > 0) {
        UpdateREAL();
        g_pWorld->BeginUpdate(1.0f);
        progress(0, bar, 0.2f * LoadFraction(g_pWorld) + 0.8f);
    }
    progress(0, bar, 1.0f);
    progress(0, bar, 1.0f);
    if (g_bInMultiplayerMode) {
        if (mp->timeLimitOn)
            g_mpTimeLimit = (int)((float)g_screen.GetVSyncs() + 59.94f * (float)(int)(60.0f * (float)mp->timeLimit));
        else
            g_mpTimeLimit = -1;
        g_bMPEndLevel = 0;
    }
    CTankObject::VerifyAIDataSize();
    DebugProfTimerInitAll();
    DebugProfTimerRegister(0, "Total", -1);
    DebugProfTimerRegister(1, "Update", 0);
    DebugProfTimerRegister(2, "Draw", 0);
    DebugProfTimerRegister(3, "Sound", 0);
    DebugProfTimerRegister(4, "Debug", 0);
}

void LoadLevelSettings() {
    int i;
    LevelFileContentsStruct_* level = TLT_FindNextResourceByType((TLTResourceID)13, 0);
    float valueA = level->m_data->fov.f;
    float valueB = level->m_data->fovMultiplayer.f;
    int mission;
    int stage;
    if (g_bInMultiplayerMode) {
        mission = g_Shell.m_mp.mission - 1;
        stage = g_Shell.m_mp.stage - 1;
    } else {
        mission = g_Shell.Get_currentMission() - 1;
        stage = g_Shell.Get_currentStage() - 1;
    }
    int index = stage + mission * 4;
    char* landmark = umTable.GetText(index + 452);
    char* time = umTable.GetText(index + 476);
    SetLandmarkStampString(landmark);
    SetTimeStampString(time);
    g_Shell.m_value10 = level->m_data->value25c.i;
    for (i = 0; i < g_NumPlayers; i++) {
        CPlayerObject* player = g_pPlayers[i];
        player->m_camera.SetNear(valueA);
        player->m_camera.SetFar(valueB);
    }
    CColor color(0, 0, 0, 128);
    color.r = level->m_data->fogColor[0].i;
    color.g = level->m_data->fogColor[1].i;
    color.b = level->m_data->fogColor[2].i;
    SetFog(color, level->m_data->fogNear.f, level->m_data->fogFar.f);
    color.r = level->m_data->clearColor[0].i;
    color.g = level->m_data->clearColor[1].i;
    color.b = level->m_data->clearColor[2].i;
    g_screen.SetClearColor(color);
    CVector3 direction;
    color.r = level->m_data->ambientColor[0].i;
    color.g = level->m_data->ambientColor[1].i;
    color.b = level->m_data->ambientColor[2].i;
    float intensity = level->m_data->ambientIntensity.f;
    CLight::SetDefaultLight((CLight::EDefaultLights)0, direction, color, intensity);
    color.r = level->m_data->lights[0].color[0].i;
    color.g = level->m_data->lights[0].color[1].i;
    color.b = level->m_data->lights[0].color[2].i;
    direction.d.v[0] = level->m_data->lights[0].direction[0].f;
    direction.d.v[1] = level->m_data->lights[0].direction[1].f;
    direction.d.v[2] = level->m_data->lights[0].direction[2].f;
    direction.Normalize();
    intensity = level->m_data->lights[0].intensity.f;
    CLight::SetDefaultLight((CLight::EDefaultLights)1, direction, color, intensity);
    color.r = level->m_data->lights[1].color[0].i;
    color.g = level->m_data->lights[1].color[1].i;
    color.b = level->m_data->lights[1].color[2].i;
    direction.d.v[0] = level->m_data->lights[1].direction[0].f;
    direction.d.v[1] = level->m_data->lights[1].direction[1].f;
    direction.d.v[2] = level->m_data->lights[1].direction[2].f;
    direction.Normalize();
    intensity = level->m_data->lights[1].intensity.f;
    CLight::SetDefaultLight((CLight::EDefaultLights)2, direction, color, intensity);
    color.r = level->m_data->lights[2].color[0].i;
    color.g = level->m_data->lights[2].color[1].i;
    color.b = level->m_data->lights[2].color[2].i;
    direction.d.v[0] = level->m_data->lights[2].direction[0].f;
    direction.d.v[1] = level->m_data->lights[2].direction[1].f;
    direction.d.v[2] = level->m_data->lights[2].direction[2].f;
    direction.Normalize();
    intensity = level->m_data->lights[2].intensity.f;
    CLight::SetDefaultLight((CLight::EDefaultLights)3, direction, color, intensity);
}
