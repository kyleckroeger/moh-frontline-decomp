// A fragment of Pause.cpp (0x800dd98c): Pause::PauseUpdate runs the pause
// screen each frame. In states 4-5 it fades the master volume up towards 127
// (clamped, at the fade speed per 1/59.94 s). It pauses when a controller is
// not ready (in single player only while the player's flag bit at +918 is
// set; states 0 and 6, not 7, recording the controller that is missing) or when a player presses
// the pause button (input 121) in states 0 and 6. Pausing activates the
// studio screen, pauses the music and sound events and sends the pause sound
// event (volume scaled from the music volume); otherwise the studio updates
// while paused or in states 1-5. While paused in single player it records an
// eight-button cheat code (one byte per frame of pressed buttons) and toggles
// the bullet shield or immortality cheat when the code matches, then shows
// message 2. The classes, functions and globals are named by the mangled
// symbols; the members, the player, cheats and input views and the event
// layout are inferred. The rest of the file is in other units.
extern "C" int strncmp(const char*, const char*, unsigned long);

struct UISParam_t;

template <class T>
T MathFunClamp(T, T, T);

void SoundSetMasterVolume(int);
void SoundGetVolumes(float&, float&);
void MUSIC_Pause(bool);
void AEMS_SendEvent(void*);
void AEMS_Pause(bool);
void GameMessage(unsigned long, unsigned short, unsigned short, unsigned long, UISParam_t*, UISParam_t*);

class IStudio {
public:
    void iSirenUpdate(unsigned long);
    void iSirenActivateScreen(unsigned short, unsigned short);

    unsigned char unknown00[24];
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

extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;
extern CScene g_scene;
extern CInputManager g_inputMgr;
extern int g_PauseScreenLockedController;
extern CheatsView g_cheats;
extern char g_cheatSequence[9];
extern unsigned int g_cheatSequenceIndex;

class Pause {
public:
    void PauseUpdate(unsigned long);

    unsigned char unknown00[108];
    bool m_paused;
    unsigned char unknown6d[3];
    IStudio m_studio;
    int m_state;
    float m_volume;
    float m_fadeSpeed;
    unsigned char unknown94[2];
    unsigned char m_controller;
};

void Pause::PauseUpdate(unsigned long time) {
    static const char BLTSHIELD_CHEAT[6] = {8, 8, 4, 1, 32, 16};
    static const char IMMORTAILITY_CHEAT[8] = {8, 1, 8, 4, 32, 1, 16, (char)128};
    if (m_state >= 4 && m_state < 6) {
        float step = (1.0f / 59.94f) * (float)time;
        if (m_volume < 127.0f)
            m_volume = MathFunClamp<float>(m_volume + m_fadeSpeed * step, 0.0f, 127.0f);
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
    } else if (m_paused || (m_state >= 1 && m_state < 6)) {
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
