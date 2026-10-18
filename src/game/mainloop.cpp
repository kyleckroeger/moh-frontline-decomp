// The end of mainloop.cpp (0x8007b350): LoadLevelSettings applies the level
// resource's settings - the two player view values (from +124 and +132) to
// every player, the landmark and time stamp strings of the current mission
// and stage (user messages 452 and 476 onwards, four per mission), the
// shell's byte at +16, the fog colour and range, the screen's clear colour,
// the ambient light and three directional lights (normalised directions,
// colours and intensities). The level settings view (8-byte property values),
// the message table and the player and shell members are inferred; the
// constants are .sdata2 pool entries. Then the static initialisation: the global
// animated-object, bullet and weapon factories and the bullet-decal manager
// are built through their constructors, the last three with their
// destructors registered (the animated-object factory has none). The player
// table between them needs no construction. The game-loop functions before
// them are not reconstructed. The classes and globals are named by the symbols;
// the object sizes come from the symbols and the members are opaque here.
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
};

void SetFog(CColor&, float, float);
void SetLandmarkStampString(char*);
void SetTimeStampString(char*);

class CScreen {
public:
    void SetClearColor(const CColor&);

    unsigned char unknown000[268];
};

extern CScreen g_screen;

class CShellMenu {
public:
    int Get_currentMission();
    int Get_currentStage();

    unsigned char unknown0000[16];
    unsigned char m_value10;
    unsigned char unknown0011[5131];
    int m_mission;
    int m_stage;
    unsigned char unknown1424[1460];
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

/* inferred: the player's two view values, each clearing two update flags */
class CPlayerObject {
public:
    void SetViewValueA(float value) {
        m_viewValueA = value;
        m_viewFlagA = 0;
        m_viewFlagB = 0;
    }
    void SetViewValueB(float value) {
        m_viewValueB = value;
        m_viewFlagA = 0;
        m_viewFlagB = 0;
    }

    unsigned char unknown000[2036];
    float m_viewValueA;
    float m_viewValueB;
    unsigned char unknown7f8[278];
    unsigned char m_viewFlagA;
    unsigned char m_viewFlagB;
};

class CAnimObjectFactory {
public:
    CAnimObjectFactory();

    unsigned char unknown00[28];
};

class CBulletFactory {
public:
    CBulletFactory();
    ~CBulletFactory();

    unsigned char unknown00[52];
};

class CWeaponFactory {
public:
    CWeaponFactory();
    ~CWeaponFactory();

    unsigned char unknown00[28];
};

class CBulletDecalManager {
public:
    CBulletDecalManager();
    ~CBulletDecalManager();

    unsigned char unknown000[1484];
};


CAnimObjectFactory g_AnimObjFactory;
CPlayerObject* g_pPlayers[4];
CBulletFactory g_BulletFactory;
CWeaponFactory g_WeaponFactory;
CBulletDecalManager g_BulletDecalManager;

void LoadLevelSettings() {
    int i;
    LevelFileContentsStruct_* level = TLT_FindNextResourceByType((TLTResourceID)13, 0);
    float valueA = level->m_data->fov.f;
    float valueB = level->m_data->fovMultiplayer.f;
    int mission;
    int stage;
    if (g_bInMultiplayerMode) {
        mission = g_Shell.m_mission - 1;
        stage = g_Shell.m_stage - 1;
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
        player->SetViewValueA(valueA);
        player->SetViewValueB(valueB);
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
