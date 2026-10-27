// Pause.cpp (0x800dd5a8), the whole file: the pause screen. SetState
// (pausing and resuming music and ambient sound, the master volume), the
// objective list (an objective's completion status read and set, an
// objective added from the objective-text table), PauseShutdown (the
// controller lock released, the studio's siren shut down), UnpauseGame,
// bIsGamePaused, PauseDraw (while paused: the studio screen and, unless hidden,
// up to 14 objectives with a completed/pending icon and, for text types below
// 50, their text in white, 24 units apart from y 100; in states 3-5 only the
// studio screen), PauseUpdate (each frame: the master volume faded up in
// states 4-5; pausing when a controller is not ready or a player presses the
// pause button; the studio activated or updated; while paused in single player
// an eight-button cheat code recorded and the bullet shield or immortality
// cheat toggled), Reset (the paused flag cleared, the objective list emptied),
// PauseInit (the studio started and its screen loaded, the objective icon
// quad copied from a template, the level's pause font created in grey at the
// screen's far depth, the objective list emptied, the volumes reset and the
// two objective icons loaded) and the destructor and constructor. Pause,
// PauseState, IStudio and the globals are named by the mangled symbols; the
// members, the objective list and the objective-text table views are
// inferred. The sprites and the font are deleted through the virtual
// destructor at the slot CRenderBin's sprites and fonts use (after 28 bytes
// of members).
// The file is compiled with deferred inlining and its functions are written
// here in reverse image order (the compiler emits them reversed), which gives
// the original .rodata order PauseInit addresses from the section base.
extern "C" int strncmp(const char*, const char*, unsigned long);
extern "C" void* memcpy(void*, const void*, unsigned long);

// The header's prime table (unused here; this file's .rodata starts with it).
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

enum PauseState {};
enum ERenderPriority {};
enum TLTResourceID {};
struct UISParam_t;
struct LevelFileContentsStruct_;

template <class T>
T MathFunClamp(T, T, T);

void SoundSetMasterVolume(int);
void SoundGetVolumes(float&, float&);
void MUSIC_Pause(bool);
void AEMS_SendEvent(void*);
void AEMS_Pause(bool);
void GameMessage(unsigned long, unsigned short, unsigned short, unsigned long, UISParam_t*, UISParam_t*);
void* DWI_alloc(const char*, int, int);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

/* inferred: the level's pause-font resource (the file name at +356) */
struct PauseResourceData {
    unsigned char unknown000[356];
    const char* m_font;
};

struct PauseResourceView {
    unsigned char unknown00[8];
    PauseResourceData* data;
};

PauseResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);

class CColor {
public:
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/* inferred view: 28 bytes of members, then the table with the virtual
   destructor in its first slot */
class CRenderBin {
public:
    unsigned char unknown00[28];

    virtual ~CRenderBin();
};

class CFont : public CRenderBin {
public:
    CFont(const char*, ERenderPriority);
    virtual ~CFont();
    void SetColor(CColor);
    void SetScale(float, float);
    void SetDepth(unsigned long);
    void DrawText(float, float, const char*);

    unsigned char unknown20[8];
};

struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned int color;
};

class CSprite : public CRenderBin {
public:
    enum ESpriteLoadType {};
    enum EPolyPrimType {};
    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }
    CSprite(const char*, ESpriteLoadType, ERenderPriority);
    virtual ~CSprite();
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);

    unsigned char unknown20[68];
};

class CScreen {
public:
    static CScreen* GetCurrent();
    unsigned long GetDepthBufferMax();
};

class IStudio {
public:
    IStudio();
    ~IStudio();
    void ISirenStartup(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
    void iSirenLoadScreen(unsigned short, unsigned short);
    void iSirenUpdate(unsigned long);
    void iSirenDrawProcessFnc(unsigned long);
    void iSirenActivateScreen(unsigned short, unsigned short);
    void iSirenShutDown();

    unsigned char unknown00[24];
};

class CShellMenu {
public:
    void CompletedAnotherObjective();
    unsigned int Get_currentMission();
    unsigned int Get_currentStage();

    unsigned char data[6620];
};

class CDeviceManager {
public:
    bool IsReady(int);
};

class CInputManager : public CDeviceManager {
public:
    int GetValue(unsigned long, unsigned long);

    unsigned char unknown00[64];
};

class CPlayerObject {
public:
    unsigned char unknown000[918];
    unsigned char m_flags396;
    unsigned char unknown397[973];
    int m_controller;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

struct CheatsView {
    bool immortality;
    unsigned char unknown01[7];
    bool bulletShield;
    unsigned char unknown09[11];
};

struct ObjectiveView {
    unsigned int text;
    unsigned char type;
    bool completed;
    unsigned char unknown06[2];
};

struct ObjectiveListView {
    /* inferred inline helper: clearing the list in one needs it for the register use */
    void Clear() {
        count = 0;
        for (int i = 0; i < 100; i++) {
            objectives[i].completed = false;
            objectives[i].text = 0;
            objectives[i].type = 0;
        }
    }

    int count;
    ObjectiveView objectives[100];
};

struct ObjectiveTextView {
    unsigned int text;
    unsigned char type;
    unsigned char unknown05[3];
};

struct ObjectiveTextTableView {
    ObjectiveTextView* entries;
    unsigned char unknown04[8];
    int count;
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;
extern CScene g_scene;
extern CInputManager g_inputMgr;
extern int g_PauseScreenLockedController;
extern CheatsView g_cheats;
extern ObjectiveTextTableView umTable;

extern ObjectiveListView theObjectiveList;
extern char g_cheatSequence[9]; // file-local in the original
extern unsigned int g_cheatSequenceIndex; // file-local in the original

class Pause {
public:
    Pause();
    ~Pause();
    void SetState(PauseState);
    void PauseShutdown();
    void UnpauseGame();
    bool bIsGamePaused();
    void PauseDraw(unsigned long);
    void PauseUpdate(unsigned long);
    void Reset();
    void PauseInit();
    void DrawObjectives();

    CFont* m_font;
    CSprite* m_completedSprite;
    CSprite* m_pendingSprite;
    _spritepolyvert m_vertices[4];
    bool m_paused;
    unsigned char unknown6d[3];
    IStudio m_studio;
    PauseState m_state;
    float m_volume;
    float m_volumeFade;
    bool m_hideObjectives;
    bool m_flag95;
    char m_controller;
};

Pause::Pause() : m_paused(false) {
    m_font = 0;
    m_hideObjectives = true;
    m_flag95 = false;
}

Pause::~Pause() {
    delete m_pendingSprite;
    delete m_completedSprite;
    delete m_font;
}

void Pause::PauseInit() {
    m_controller = -1;
    m_paused = false;
    m_flag95 = false;
    m_studio.ISirenStartup(2, 8, 30, 200, 16);
    m_studio.iSirenLoadScreen(0, 1);
    if (!g_bInMultiplayerMode && g_Shell.Get_currentMission() == 6 && g_Shell.Get_currentStage() == 1)
        m_studio.iSirenLoadScreen(0, 0);
    _spritepolyvert vertices[4] = {
        {0.0f, 0.0f, 28.0f, 96.0f, 0.0f, 0x80808080},
        {1.0f, 0.0f, 52.0f, 96.0f, 0.0f, 0x80808080},
        {1.0f, 1.0f, 52.0f, 120.0f, 0.0f, 0x80808080},
        {0.0f, 1.0f, 28.0f, 120.0f, 0.0f, 0x80808080},
    };
    memcpy(m_vertices, vertices, sizeof(vertices));
    CScreen* screen = CScreen::GetCurrent();
    if (screen) {
        PauseResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)11, 0);
        const char* file = (const char*)TLT_LoadFileFromLevelBigFile(resource->data->m_font, 0);
        m_font = new CFont(file, (ERenderPriority)111);
        m_font->SetColor(CColor(63, 63, 63, 128));
        m_font->SetScale(1.0f, 1.0f);
        m_font->SetDepth(screen->GetDepthBufferMax());
    }
    theObjectiveList.Clear();
    m_volume = 0.0f;
    m_volumeFade = 0.0f;
    m_state = (PauseState)0;
    m_completedSprite = new CSprite("data\\bitmaps\\checkedbox.gsh", (CSprite::ESpriteLoadType)1, (ERenderPriority)110);
    m_pendingSprite = new CSprite("data\\bitmaps\\clearcheckbox.gsh", (CSprite::ESpriteLoadType)1, (ERenderPriority)110);
    g_PauseScreenLockedController = -1;
}

void Pause::Reset() {
    m_paused = false;
    theObjectiveList.Clear();
}

void Pause::PauseUpdate(unsigned long time) {
    static const char BLTSHIELD_CHEAT[6] = {8, 8, 4, 1, 32, 16};
    static const char IMMORTAILITY_CHEAT[8] = {8, 1, 8, 4, 32, 1, 16, (char)128};
    if ((int)m_state >= 4 && (int)m_state < 6) {
        float step = (1.0f / 59.94f) * (float)time;
        if (m_volume < 127.0f)
            m_volume = MathFunClamp<float>(m_volume + m_volumeFade * step, 0.0f, 127.0f);
        SoundSetMasterVolume((int)m_volume);
    }
    bool paused = m_paused;
    bool ready;
    if (!g_bInMultiplayerMode) {
        if (!(g_scene.GetPlayer(0)->m_flags396 & 1))
            ready = true;
        else
            ready = g_inputMgr.IsReady(g_scene.GetPlayer(0)->m_controller);
    } else {
        ready = true;
        for (int i = 0; i < g_NumPlayers; i++)
            ready &= g_inputMgr.IsReady(g_scene.GetPlayer(i)->m_controller);
    }
    if (ready) {
        if (m_state == 0 || m_state == 6) {
            for (int i = 0; i < g_NumPlayers; i++) {
                if (g_inputMgr.GetValue(g_scene.GetPlayer(i)->m_controller, 121) && m_paused != 1) {
                    m_controller = i;
                    paused = true;
                    g_PauseScreenLockedController = g_scene.GetPlayer(i)->m_controller;
                    break;
                }
            }
        }
    } else if (m_state != 7 && (m_state == 0 || m_state == 6)) {
        paused = true;
        for (int i = 0; i < g_NumPlayers; i++) {
            if (!g_inputMgr.IsReady(g_scene.GetPlayer(i)->m_controller)) {
                m_controller = i;
                g_PauseScreenLockedController = g_scene.GetPlayer(i)->m_controller;
            }
        }
    }
    if (paused && !m_paused) {
        m_studio.iSirenActivateScreen(0, 1);
        MUSIC_Pause(true);
        float music;
        float effects;
        SoundGetVolumes(music, effects);
        int event[10] = {7, 3, 0, 4096, 0, 0, 2500, 0, 0, 0};
        event[9] = 32767.0f * music;
        AEMS_SendEvent(event);
        AEMS_Pause(true);
        m_paused = paused;
        g_cheatSequenceIndex = 0;
    } else if (m_paused || (m_state >= 1 && (int)m_state < 6)) {
        m_studio.iSirenUpdate(time);
    }
    if (m_paused && !g_bInMultiplayerMode) {
        char code = g_inputMgr.GetValue(0, 133) & 1 | (g_inputMgr.GetValue(0, 132) & 1) << 1 |
                    (g_inputMgr.GetValue(0, 134) & 1) << 2 | (g_inputMgr.GetValue(0, 127) & 1) << 3 |
                    (g_inputMgr.GetValue(0, 130) & 1) << 4 | (g_inputMgr.GetValue(0, 129) & 1) << 5 |
                    (g_inputMgr.GetValue(0, 123) & 1) << 6 | (g_inputMgr.GetValue(0, 124) & 1) << 7;
        if (code && g_cheatSequenceIndex < 8) {
            g_cheatSequence[g_cheatSequenceIndex++] = code;
            g_cheatSequence[g_cheatSequenceIndex] = 0;
        }
        bool entered = false;
        if (!strncmp(g_cheatSequence, BLTSHIELD_CHEAT, 6)) {
            entered = true;
            g_cheats.bulletShield = !g_cheats.bulletShield;
        }
        if (!strncmp(g_cheatSequence, IMMORTAILITY_CHEAT, 8)) {
            entered = true;
            g_cheats.immortality = !g_cheats.immortality;
        }
        if (entered) {
            g_cheatSequenceIndex = 0;
            g_cheatSequence[0] = 0;
            UISParam_t* none = 0;
            char param[4];
            GameMessage(2, 0, 0, 0, none, (UISParam_t*)param);
        }
    }
}

void Pause::PauseDraw(unsigned long time) {
    if (m_paused) {
        m_studio.iSirenDrawProcessFnc(time);
        if (!m_hideObjectives) {
            DrawObjectives();
        }
    } else if (m_state >= 3 && m_state < 6) {
        m_studio.iSirenDrawProcessFnc(time);
    }
}

inline void Pause::DrawObjectives() {
    unsigned short y = 100;
    for (unsigned int i = 0; i < 100; i++) {
        if (y < 424) {
            if (m_font && theObjectiveList.objectives[i].text) {
                m_vertices[0].y = y + 1;
                m_vertices[1].y = y + 1;
                m_vertices[2].y = y + 24;
                m_vertices[3].y = y + 24;
                if (theObjectiveList.objectives[i].completed)
                    m_completedSprite->DrawPoly(m_vertices, 4, (CSprite::EPolyPrimType)5);
                else
                    m_pendingSprite->DrawPoly(m_vertices, 4, (CSprite::EPolyPrimType)5);
                if (theObjectiveList.objectives[i].type < 50) {
                    m_font->SetColor(CColor(255, 255, 255, 255));
                    m_font->DrawText(56.0f, y + 6, (const char*)theObjectiveList.objectives[i].text);
                }
            }
            y += 24;
        }
    }
}

bool Pause::bIsGamePaused() {
    return m_paused;
}

void Pause::UnpauseGame() {
    g_PauseScreenLockedController = -1;
    MUSIC_Pause(false);
    AEMS_Pause(false);
    m_paused = false;
    g_cheatSequenceIndex = 0;
}

void Pause::PauseShutdown() {
    g_PauseScreenLockedController = -1;
    m_studio.iSirenShutDown();
}

void AddObjective(unsigned int index, unsigned int id) {
    int type = (int)id < umTable.count ? umTable.entries[id].type : 0;
    unsigned int text = (int)id < umTable.count ? umTable.entries[id].text : 0;
    int i = index - 1;

    theObjectiveList.objectives[i].text = text;
    theObjectiveList.objectives[i].completed = false;
    theObjectiveList.objectives[i].type = type;
    theObjectiveList.count++;
}

void SetObjectiveStatus(unsigned int index, bool completed) {
    int i = index - 1;
    theObjectiveList.objectives[i].completed = completed;
    if (completed)
        g_Shell.CompletedAnotherObjective();
}

bool GetObjectiveStatus(unsigned int index) {
    return theObjectiveList.objectives[index - 1].completed;
}

void Pause::SetState(PauseState state) {
    if (state != m_state) {
        switch (state) {
        case 3:
            m_studio.iSirenActivateScreen(0, 0);
            MUSIC_Pause(true);
            AEMS_Pause(true);
            break;
        case 4:
            SoundSetMasterVolume(0);
            m_volumeFade = 25.4f;
            MUSIC_Pause(false);
            AEMS_Pause(false);
            break;
        case 6:
            if (m_volume < 127.0f)
                SoundSetMasterVolume(127);
            break;
        }
    }
    m_state = state;
}
