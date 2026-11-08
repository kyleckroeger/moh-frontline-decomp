// A fragment of ShellMenu.cpp (0x800e6524): the weak, empty iDebugMsg emitted
// in this file, then CShellMenu's level timer (accumulated unless the player's
// flag at +919 is set) and the finished-level records kept per mission and
// level (1-based, decremented in place; health, per-weapon ammo and a success
// flag; whether to use the previous level's record), and the multiplayer stats
// records and score. Add_MPPlayerKilled credits a kill: it counts the killer's
// weapon and victim, the victim's death and a kill or team kill (each capped
// at 9999999), recomputes the score and the team's total, ends the level at
// the score limit and flags the leaders as winners. The file name is this
// project's; the original record is ShellMenu.cpp. The classes and functions
// are named by the mangled symbols; CShellMenu, the records, the player and
// weapon views and the scene view are inferred (members at their offsets,
// names not original; 6 missions of 4 levels, 45 ammo slots and 256-byte
// multiplayer records are sizes implied by the strides), as are the result
// types and the GetTeam accessor.
enum EWeaponTypes {};

class CWeapon {
public:
    unsigned char unknown000[664];
    int m_type;
};

class CPlayerObject {
public:
    CWeapon* GetCurrentWeapon() const;
    int GetTeam() const { return m_team; }

    unsigned char unknown000[919];
    unsigned char m_flag919 : 1;
    unsigned char unknown397 : 7;
    unsigned char unknown398[972];
    int m_controller;
    unsigned char unknown768[2888];
    int m_team;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char data[16];
};

extern CScene g_scene;
extern int g_bMPEndLevel;

struct FinishedLevelView {
    float health;
    unsigned int ammo[45];
    bool successful;
    unsigned char unknownb9[3];
};

struct MPPlayerStatsView {
    unsigned char unknown000[32];
    int weaponKills[45];
    int victims[4];
    int kills;
    int teamKills;
    int score;
    int teamScore;
    int deaths;
    unsigned char unknown0f8[4];
    bool winner;
    unsigned char unknown0fd[3];
};

static inline void AddStat(int& value) {
    value++;
    if (value >= 9999999)
        value = 9999999;
}

void iDebugMsg(const char*, ...);

class CShellMenu {
public:
    void UpdateLevelTimer(CPlayerObject*, unsigned int);
    void ResetLevelTimer();
    bool ShouldIUsePlayersFinishedLevelData(int, int);
    unsigned int GetPlayersFinishedLevelAmmo(int, int, EWeaponTypes);
    float GetPlayersFinishedLevelHealth(int, int);
    void SetPlayersFinishedLevelAmmo(int, int, EWeaponTypes, unsigned int);
    void SetPlayersFinishedLevelHealth(int, int, float);
    void SetPlayersFinishedLevelSuccessfully(int, int);
    MPPlayerStatsView* GetMPPlayerStats(int);
    int Get_MPPlayerScore(int);
    void Add_MPPlayerKilled(int, int);

    unsigned char unknown0000[336];
    unsigned int m_levelTimer;
    unsigned char unknown0154[260];
    FinishedLevelView m_finished[6][4];
    unsigned char unknown13f8[28];
    int m_activeWindowCount;
    unsigned char unknown1418[16];
    int m_scoreLimit;
    unsigned char unknown142c[4];
    int m_scoreLimitOn;
    unsigned char unknown1434[152];
    MPPlayerStatsView m_mpStats[8];
};

__declspec(weak) void iDebugMsg(const char*, ...) {
}

void CShellMenu::UpdateLevelTimer(CPlayerObject* player, unsigned int elapsed) {
    if (!player->m_flag919)
        m_levelTimer += elapsed;
}

void CShellMenu::ResetLevelTimer() {
    m_levelTimer = 0;
}

bool CShellMenu::ShouldIUsePlayersFinishedLevelData(int mission, int level) {
    if (level > 1 && mission < 7) {
        mission--;
        level -= 2;
        return m_finished[mission][level].successful;
    }
    return false;
}

unsigned int CShellMenu::GetPlayersFinishedLevelAmmo(int mission, int level, EWeaponTypes weapon) {
    mission--;
    level--;
    return m_finished[mission][level].ammo[weapon];
}

float CShellMenu::GetPlayersFinishedLevelHealth(int mission, int level) {
    mission--;
    level--;
    return m_finished[mission][level].health;
}

void CShellMenu::SetPlayersFinishedLevelAmmo(int mission, int level, EWeaponTypes weapon, unsigned int ammo) {
    mission--;
    level--;
    m_finished[mission][level].ammo[weapon] = ammo;
}

void CShellMenu::SetPlayersFinishedLevelHealth(int mission, int level, float health) {
    mission--;
    level--;
    m_finished[mission][level].health = health;
}

void CShellMenu::SetPlayersFinishedLevelSuccessfully(int mission, int level) {
    mission--;
    level--;
    m_finished[mission][level].successful = true;
}

MPPlayerStatsView* CShellMenu::GetMPPlayerStats(int player) {
    return &m_mpStats[player];
}

int CShellMenu::Get_MPPlayerScore(int player) {
    return m_mpStats[player].score;
}

void CShellMenu::Add_MPPlayerKilled(int killer, int victim) {
    int total;
    int team;
    int controller;
    int type;
    if (g_bMPEndLevel)
        return;
    type = g_scene.GetPlayer(killer)->GetCurrentWeapon()->m_type;
    if (m_mpStats[killer].weaponKills[type] < 9999999)
        m_mpStats[killer].weaponKills[type]++;
    controller = g_scene.GetPlayer(victim)->m_controller;
    m_mpStats[killer].victims[controller]++;
    if (m_mpStats[killer].victims[controller] >= 9999999)
        m_mpStats[killer].victims[controller] = 9999999;
    AddStat(m_mpStats[victim].deaths);
    team = g_scene.GetPlayer(killer)->GetTeam();
    if (team == g_scene.GetPlayer(victim)->GetTeam()) {
        AddStat(m_mpStats[killer].teamKills);
    } else {
        AddStat(m_mpStats[killer].kills);
    }
    m_mpStats[killer].score = m_mpStats[killer].kills - m_mpStats[killer].teamKills;
    total = 0;
    for (int i = 0; i < m_activeWindowCount; i++) {
        if (team == g_scene.GetPlayer(i)->GetTeam())
            total += m_mpStats[i].score;
    }
    for (int i = 0; i < m_activeWindowCount; i++) {
        if (team == g_scene.GetPlayer(i)->GetTeam())
            m_mpStats[i].teamScore = total;
    }
    if (m_scoreLimitOn && m_scoreLimit <= total)
        g_bMPEndLevel = 1;
    int best = -32767;
    for (int i = 0; i < m_activeWindowCount; i++) {
        if (m_mpStats[i].teamScore > best)
            best = m_mpStats[i].teamScore;
    }
    for (int i = 0; i < m_activeWindowCount; i++) {
        if (m_mpStats[i].teamScore == best)
            m_mpStats[i].winner = true;
        else
            m_mpStats[i].winner = false;
    }
}
