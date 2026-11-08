// A fragment of ShellMenu.cpp (0x800e6950): CShellMenu's statistics counters,
// each capped at 9999999. The multiplayer records (from +5324, 256 bytes per
// player) count hits per shot location and shots fired (not once the level
// has ended) and remember who last shot a player; resetting clears four
// records and sets each last shooter to -1. The current level's counters count
// shots fired, hits, hits taken, enemies killed (and kills per weapon type of
// the first player's current weapon) and hits per shot location; resetting
// the level clears them with the level timer and flags, the secrets' status is
// read and set through ten 2-bit fields and the medals' status read from ten
// bit-fields (the seventh two bits wide). The classes,
// enums and functions are named by the mangled symbols; CShellMenu, the
// records, the weapon view and the counter names are inferred (the scene view
// only needs a size outside small data), and the unused
// bool parameters are left unnamed. The rest of the file is not part of this
// unit.
enum EShotLocations {};

extern "C" void* memset(void*, int, unsigned long);

class CWeapon {
public:
    unsigned char unknown000[664];
    int m_type;
};

class CPlayerObject {
public:
    CWeapon* GetCurrentWeapon() const;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char data[16];
};

extern CScene g_scene;
extern int g_bMPEndLevel;

struct MPStatsView {
    int shotsFired;
    int locations[61];
    int lastShooter;
    unsigned char unknown0fc[4];
};

class CShellMenu {
public:
    void Add_MPShotLocation(int, int, int, EShotLocations);
    void Add_MPShotFired(int, int);
    void ResetMPPlayersStats();
    void Set_shotLocation(unsigned int, EShotLocations, bool);
    void Set_enemiesKilled(unsigned int, bool);
    void Set_hitsTaken(unsigned int, bool);
    void Set_hits(unsigned int, bool);
    void Set_shotsFired(unsigned int, bool);
    void ResetCurrentLevelStats();
    void GetSecretsStatus(int*);
    void SetIndividualSecretsStatus(int, int);
    void GetMedalStatus(int*);

    unsigned char unknown0000[14];
    unsigned char m_flag14;
    unsigned char unknown000f[5];
    unsigned char m_secret1 : 2;
    unsigned char m_secret2 : 2;
    unsigned char m_secret3 : 2;
    unsigned char m_secret4 : 2;
    unsigned char m_secret5 : 2;
    unsigned char m_secret6 : 2;
    unsigned char m_secret7 : 2;
    unsigned char m_secret8 : 2;
    unsigned char m_secret9 : 2;
    unsigned char m_secret10 : 2;
    unsigned char unknown0017;
    unsigned char m_medal1 : 1;
    unsigned char m_medal2 : 1;
    unsigned char m_medal3 : 1;
    unsigned char m_medal4 : 1;
    unsigned char m_medal5 : 1;
    unsigned char m_medal6 : 1;
    unsigned char m_medal7 : 2;
    unsigned char m_medal8 : 1;
    unsigned char m_medal9 : 1;
    unsigned char m_medal10 : 1;
    unsigned char unknown0019 : 5;
    unsigned char unknown001a[262];
    unsigned int m_shotsFired;
    unsigned int m_hits;
    unsigned int m_hitsTaken;
    unsigned int m_stat300;
    unsigned int m_enemiesKilled;
    unsigned int m_shotLocations[7];
    unsigned int m_levelTimer;
    unsigned int m_stat340;
    float m_stat344;
    unsigned int m_weaponKills[45];
    unsigned char m_flag528;
    unsigned char m_flag529;
    unsigned char m_flag530;
    unsigned char unknown0213[4793];
    MPStatsView m_mpStats[4];
};

void CShellMenu::Add_MPShotLocation(int shooter, int target, int count, EShotLocations location) {
    if (g_bMPEndLevel)
        return;
    m_mpStats[shooter].locations[location] += count;
    if (m_mpStats[shooter].locations[location] >= 9999999)
        m_mpStats[shooter].locations[location] = 9999999;
    m_mpStats[target].lastShooter = shooter;
}

void CShellMenu::Add_MPShotFired(int player, int count) {
    if (g_bMPEndLevel)
        return;
    m_mpStats[player].shotsFired += count;
    if (m_mpStats[player].shotsFired >= 9999999)
        m_mpStats[player].shotsFired = 9999999;
}

void CShellMenu::ResetMPPlayersStats() {
    memset(m_mpStats, 0, sizeof(m_mpStats));
    m_mpStats[0].lastShooter = -1;
    m_mpStats[1].lastShooter = -1;
    m_mpStats[2].lastShooter = -1;
    m_mpStats[3].lastShooter = -1;
}

void CShellMenu::Set_shotLocation(unsigned int count, EShotLocations location, bool) {
    if (m_shotLocations[location] < 9999999)
        m_shotLocations[location] += count;
}

void CShellMenu::Set_enemiesKilled(unsigned int count, bool) {
    CPlayerObject* player;

    if (m_enemiesKilled < 9999999)
        m_enemiesKilled += count;
    player = g_scene.GetPlayer(0);
    if (player->GetCurrentWeapon()) {
        int type = player->GetCurrentWeapon()->m_type;
        if (m_weaponKills[type] < 9999999)
            m_weaponKills[type] += count;
    }
}

void CShellMenu::Set_hitsTaken(unsigned int count, bool) {
    if (m_hitsTaken < 9999999)
        m_hitsTaken += count;
}

void CShellMenu::Set_hits(unsigned int count, bool) {
    if (m_hits < 9999999)
        m_hits += count;
}

void CShellMenu::Set_shotsFired(unsigned int count, bool) {
    if (m_shotsFired < 9999999)
        m_shotsFired += count;
}

void CShellMenu::ResetCurrentLevelStats() {
    unsigned int i;

    m_stat344 = 0.0f;
    m_enemiesKilled = 0;
    m_flag529 = 0;
    m_hits = 0;
    m_hitsTaken = 0;
    m_stat300 = 0;
    m_flag530 = 0;
    m_flag528 = 0;
    m_shotLocations[0] = 0;
    m_shotLocations[3] = 0;
    m_shotLocations[6] = 0;
    m_shotLocations[4] = 0;
    m_shotLocations[1] = 0;
    m_shotLocations[5] = 0;
    m_shotLocations[2] = 0;
    for (i = 0; i < 45; i++)
        m_weaponKills[i] = 0;
    m_shotsFired = 0;
    m_levelTimer = 0;
    m_stat340 = 0;
    m_flag14 = 0;
}

void CShellMenu::GetSecretsStatus(int* status) {
    int i;
    int value = 0;

    for (i = 0; i < 10; i++) {
        switch (i + 1) {
        case 1:
            value = m_secret1;
            break;
        case 2:
            value = m_secret2;
            break;
        case 3:
            value = m_secret3;
            break;
        case 4:
            value = m_secret4;
            break;
        case 5:
            value = m_secret5;
            break;
        case 6:
            value = m_secret6;
            break;
        case 7:
            value = m_secret7;
            break;
        case 8:
            value = m_secret8;
            break;
        case 9:
            value = m_secret9;
            break;
        case 10:
            value = m_secret10;
            break;
        }
        status[i] = value;
    }
}

void CShellMenu::SetIndividualSecretsStatus(int secret, int status) {
    switch (secret) {
        case 1:
            m_secret1 = status;
            break;
        case 2:
            m_secret2 = status;
            break;
        case 3:
            m_secret3 = status;
            break;
        case 4:
            m_secret4 = status;
            break;
        case 5:
            m_secret5 = status;
            break;
        case 6:
            m_secret6 = status;
            break;
        case 7:
            m_secret7 = status;
            break;
        case 8:
            m_secret8 = status;
            break;
        case 9:
            m_secret9 = status;
            break;
        case 10:
            m_secret10 = status;
            break;
    }
}

void CShellMenu::GetMedalStatus(int* status) {
    int value = 0;
    int i;

    for (i = 0; i < 9; i++) {
        switch (i + 1) {
        case 1:
            value = m_medal1;
            break;
        case 2:
            value = m_medal2;
            break;
        case 3:
            value = m_medal3;
            break;
        case 4:
            value = m_medal4;
            break;
        case 5:
            value = m_medal5;
            break;
        case 6:
            value = m_medal6;
            break;
        case 7:
            value = m_medal7;
            break;
        case 8:
            value = m_medal8;
            break;
        case 9:
            value = m_medal9;
            break;
        case 10:
            value = m_medal10;
            break;
        }
        status[i] = value;
    }
}
