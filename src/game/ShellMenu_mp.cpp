// A fragment of ShellMenu.cpp (0x800de904): CShellMenu's multiplayer settings
// as exchanged with the interface studio. PassMP_StatsToIStudio copies out a
// player's five statistics and the hit accuracy in percent (the hit counts
// over the shot count, at most 100%). The player count is the number of
// open windows (also stored); the open windows are passed out (others as 0)
// and saved and counted; the active players are those whose controller is ready and whose
// window is open (else -1); the level settings, per-player controller value,
// model and team colour, name (at most ten characters) and advanced settings
// are copied out to (Pass...) or in from (Setup...) the studio's integer
// arrays, the Pass functions returning the count copied; GetSoundModeValue
// maps the sound option to the studio's value. The file name is this
// project's; the original record is ShellMenu.cpp and SetGlobalCheatStructure
// after these is not reconstructed. The classes, functions and globals are
// named by the mangled symbols; CShellMenu, the 28-byte player records and the
// sound options and the 256-byte statistics records are inferred views (members at their offsets, names not
// original), the options accessor is an inferred inline helper, and the result
// types are inferred.
extern "C" {
void* memcpy(void*, const void*, unsigned long);
void* memset(void*, int, unsigned long);
char* strcpy(char*, const char*);
}

class CDeviceManager {
public:
    bool IsReady(int);

    unsigned char unknown00[64];
};

extern CDeviceManager g_inputMgr;

struct MPPlayerSettingsView {
    int teamColor;
    int model;
    char name[11];
    unsigned char advanced[5];
    unsigned char controller;
    unsigned char unknown19[3];
};

struct MPStatsView {
    int shots;
    int hits[7];
    unsigned char unknown20[196];
    int stats[5];
    unsigned char unknownf8[8];
};

struct SoundOptionsView {
    unsigned char unknown0[2];
    signed char mode;
};

class CShellMenu {
public:
    void PassMP_StatsToIStudio(int, int*);
    int Get_NumPlayers();
    void Pass_ActiveWindowsToIStudio(int*);
    void Save_ActiveWindowsFromIStudio(int*);
    void SetActivePlayers(int*);
    void PassMultiPlayerLevelSettingsToIStudio(int*);
    void SetupMultiPlayerLevelSettings(int*);
    int PassPlayersMultiPlayerControllerValue(int);
    void SetupPlayersMultiPlayerControllerValue(int, int);
    int PassPlayersModelAndTeamColorSettingForMultiplayer(int, int*);
    void SetupPlayersModelAndTeamColorSettingForMultiplayer(int, int*);
    void PassPlayersMultiPlayerName(int, char*);
    void SetupPlayersMultiPlayerName(int, char*);
    int PassPlayersAdvancedMultiPlayerSettingToIStudio(int, int*);
    void SetupPlayersAdvancedMultiPlayerSettings(int, int*);
    int GetSoundModeValue();

    SoundOptionsView* GetSoundOptions() { return &m_soundOptions; }

    unsigned char unknown0000[40];
    SoundOptionsView m_soundOptions;
    unsigned char unknown002b[5097];
    int m_activeWindowCount;
    int m_levelSetting0;
    int unknown1418;
    int m_levelSetting1;
    int m_levelSetting2;
    int m_levelSetting3;
    int m_levelFlag2;
    int m_levelFlag3;
    int m_levelSetting4;
    int m_levelSetting5;
    int m_advanced[4];
    MPPlayerSettingsView m_players[4];
    int m_activePlayers[4];
    MPStatsView m_mpStats[4];
    int m_activeWindows[4];
};

void CShellMenu::PassMP_StatsToIStudio(int player, int* values) {
    int stats[6];
    stats[0] = m_mpStats[player].stats[0];
    stats[1] = m_mpStats[player].stats[1];
    stats[2] = m_mpStats[player].stats[2];
    stats[3] = m_mpStats[player].stats[3];
    stats[4] = m_mpStats[player].stats[4];
    float accuracy = 0.0f;
    if (m_mpStats[player].shots > 0) {
        float hits = 0.0f;
        for (int i = 0; i < 7; i++)
            hits += m_mpStats[player].hits[i];
        accuracy = hits / m_mpStats[player].shots;
        if (accuracy > 1.0f)
            accuracy = 1.0f;
    }
    stats[5] = 100.0f * accuracy;
    memcpy(values, stats, 24);
}

int CShellMenu::Get_NumPlayers() {
    int count = 0;
    if (m_activeWindows[0] != 0)
        count++;
    if (m_activeWindows[1] != 0)
        count++;
    if (m_activeWindows[2] != 0)
        count++;
    if (m_activeWindows[3] != 0)
        count++;
    m_activeWindowCount = count;
    return count;
}

void CShellMenu::Pass_ActiveWindowsToIStudio(int* out) {
    int windows[4];

    memset(windows, 0, sizeof(windows));
    if (m_activeWindows[0] > 0)
        windows[0] = m_activeWindows[0];
    if (m_activeWindows[1] > 0)
        windows[1] = m_activeWindows[1];
    if (m_activeWindows[2] > 0)
        windows[2] = m_activeWindows[2];
    if (m_activeWindows[3] > 0)
        windows[3] = m_activeWindows[3];
    memcpy(out, windows, sizeof(windows));
}

void CShellMenu::Save_ActiveWindowsFromIStudio(int* windows) {
    int count = 0;
    m_activeWindows[0] = windows[0];
    m_activeWindows[1] = windows[1];
    m_activeWindows[2] = windows[2];
    m_activeWindows[3] = windows[3];
    if (m_activeWindows[0] != 0)
        count++;
    if (m_activeWindows[1] != 0)
        count++;
    if (m_activeWindows[2] != 0)
        count++;
    if (m_activeWindows[3] != 0)
        count++;
    m_activeWindowCount = count;
}

void CShellMenu::SetActivePlayers(int* windows) {
    for (int i = 0; i < 4; i++) {
        if (g_inputMgr.IsReady(i) && windows[i] != 0)
            m_activePlayers[i] = i;
        else
            m_activePlayers[i] = -1;
    }
}

void CShellMenu::PassMultiPlayerLevelSettingsToIStudio(int* out) {
    int values[6];
    values[0] = m_levelSetting0;
    values[1] = m_levelSetting1;
    values[2] = m_levelSetting2;
    values[3] = m_levelSetting3;
    values[4] = m_levelSetting4;
    values[5] = m_levelSetting5;
    memcpy(out, values, sizeof(values));
}

void CShellMenu::SetupMultiPlayerLevelSettings(int* in) {
    m_levelSetting0 = in[0];
    m_levelSetting1 = in[1];
    m_levelSetting2 = in[2];
    m_levelSetting3 = in[3];
    m_levelSetting4 = in[4];
    m_levelSetting5 = in[5];
    if (m_levelSetting2 > 0)
        m_levelFlag2 = 1;
    else
        m_levelFlag2 = 0;
    if (m_levelSetting3 > 0)
        m_levelFlag3 = 1;
    else
        m_levelFlag3 = 0;
}

int CShellMenu::PassPlayersMultiPlayerControllerValue(int player) {
    return m_players[player].controller;
}

void CShellMenu::SetupPlayersMultiPlayerControllerValue(int player, int value) {
    m_players[player].controller = value;
}

int CShellMenu::PassPlayersModelAndTeamColorSettingForMultiplayer(int player, int* out) {
    int values[2];
    values[0] = m_players[player].model;
    values[1] = m_players[player].teamColor;
    memcpy(out, values, sizeof(values));
    return 2;
}

void CShellMenu::SetupPlayersModelAndTeamColorSettingForMultiplayer(int player, int* in) {
    m_players[player].model = in[0];
    m_players[player].teamColor = in[1];
}

void CShellMenu::PassPlayersMultiPlayerName(int player, char* out) {
    strcpy(out, m_players[player].name);
}

void CShellMenu::SetupPlayersMultiPlayerName(int player, char* name) {
    name[10] = 0;
    strcpy(m_players[player].name, name);
}

int CShellMenu::PassPlayersAdvancedMultiPlayerSettingToIStudio(int player, int* out) {
    int values[6];
    values[0] = m_players[player].advanced[0];
    values[1] = m_players[player].advanced[1];
    values[2] = m_players[player].advanced[2];
    values[3] = m_players[player].advanced[3];
    values[4] = m_players[player].advanced[4];
    values[5] = m_advanced[player];
    memcpy(out, values, sizeof(values));
    return 6;
}

void CShellMenu::SetupPlayersAdvancedMultiPlayerSettings(int player, int* in) {
    m_players[player].advanced[0] = in[0];
    m_players[player].advanced[1] = in[1];
    m_players[player].advanced[2] = in[2];
    m_players[player].advanced[3] = in[3];
    m_players[player].advanced[4] = in[4];
    m_advanced[player] = in[5];
}

int CShellMenu::GetSoundModeValue() {
    signed char mode = !GetSoundOptions() ? -1 : m_soundOptions.mode;
    switch (mode) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 0;
    default:
        return 0;
    }
}
