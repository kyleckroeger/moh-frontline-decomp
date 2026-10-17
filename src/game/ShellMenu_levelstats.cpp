// A fragment of ShellMenu.cpp (0x800e5dc4): CShellMenu::GetLevelStats
// finishes a level's statistics for the results screen. It walks the
// objective list (counting finished objectives into a value it never uses),
// rates the level outside mission 7 (three stars for 95% of the enemies
// killed within the level's record time of at least 75, two for 95%, else
// one) and records the rating, then fills the 21 values the studio shows
// (level number, rating, kills, shots, hits, accuracy, favourite weapon, hits
// per body location, evaluation, minutes rounded to the nearest, whether a
// medal or movie was unlocked) clamped to 9999999, and adds the level's
// counts into the running totals. The classes, functions and globals are
// named by the mangled symbols; the members, the objective list and level
// record views and the unused objective count are inferred.
struct StarLevelsInBits;

void SetStarRatingStatus(StarLevelsInBits*, unsigned int, unsigned int);

struct ObjectiveView {
    int state;
    unsigned char unknown04[12];
};

struct ObjectiveListView {
    unsigned int m_count;
    ObjectiveView m_objectives[16];
};

extern ObjectiveListView theObjectiveList;

struct LevelRecordView {
    float time;
    unsigned char unknown04[184];
};

class CShellMenu {
public:
    unsigned int* GetLevelStats();
    LevelRecordView& Record(int mission, int stage) { return m_records[mission][stage]; }
    bool CheckIfMovieAndMedalNeedsToBeUnlocked();
    bool CheckIfSpecialMedalsNeedToBeUnlocked();

    unsigned char unknown0000[4];
    unsigned int m_mission;
    unsigned int m_stage;
    unsigned char unknown000c[2];
    signed char m_value00e;
    unsigned char unknown000f;
    unsigned char m_enemyCount;
    unsigned char unknown0011[11];
    unsigned char unknown001c[16];
    unsigned int m_totalShotsFired;
    unsigned int m_totalHits;
    unsigned int m_totalHitsTaken;
    unsigned int m_total38;
    unsigned int m_totalEnemiesKilled;
    unsigned int m_totalShotLocations[7];
    unsigned char unknown005c[12];
    unsigned int m_totalWeaponKills[45];
    unsigned char unknown011c[4];
    unsigned int m_shotsFired;
    unsigned int m_hits;
    unsigned int m_hitsTaken;
    unsigned int m_stat300;
    unsigned int m_enemiesKilled;
    unsigned int m_shotLocations[7];
    unsigned int m_levelTimer;
    unsigned int m_stat340;
    float m_accuracy;
    unsigned int m_weaponKills[45];
    unsigned char m_weapon;
    unsigned char m_evaluation;
    unsigned char m_rating;
    unsigned char unknown0213[69];
    LevelRecordView m_records[6][4];
    unsigned char unknown13f8[1308];
    unsigned int m_levelStats[21];
};

unsigned int* CShellMenu::GetLevelStats() {
    int completed = 0;
    for (unsigned int objective = 0; objective < theObjectiveList.m_count; objective++) {
        if (theObjectiveList.m_objectives[objective].state == 1)
            completed++;
    }
    if (m_mission != 7) {
        int enemies = m_enemyCount ? m_enemyCount : 1;
        float percent = (float)m_enemiesKilled / enemies;
        percent *= 100.0f;
        if (percent < 0.0f)
            percent = 0.0f;
        if (100.0f < percent)
            percent = 100.0f;
        int mission = m_mission - 1;
        int stage = m_stage - 1;
        float time = m_records[mission][stage].time;
        if (percent >= 95.0f && time >= 75.0f)
            m_rating = 3;
        else if (percent >= 95.0f)
            m_rating = 2;
        else
            m_rating = 1;
        SetStarRatingStatus((StarLevelsInBits*)((char*)this + 28), m_stage + m_mission * 10, m_rating);
    }
    m_levelStats[0] = m_stage + m_mission * 10;
    m_levelStats[1] = m_rating;
    m_levelStats[2] = m_value00e;
    m_levelStats[3] = m_shotsFired;
    m_levelStats[4] = m_hits;
    m_levelStats[5] = m_accuracy;
    m_levelStats[6] = m_weapon;
    m_levelStats[7] = m_hitsTaken;
    m_levelStats[8] = m_stat300;
    m_levelStats[9] = m_enemiesKilled;
    m_levelStats[10] = 0;
    m_levelStats[11] = m_shotLocations[0];
    m_levelStats[12] = m_shotLocations[3];
    m_levelStats[13] = m_shotLocations[6];
    m_levelStats[14] = m_shotLocations[4];
    m_levelStats[15] = m_shotLocations[1];
    m_levelStats[16] = m_shotLocations[5];
    m_levelStats[17] = m_shotLocations[2];
    m_levelStats[18] = m_evaluation;
    unsigned int halfMinutes = m_levelTimer / 30;
    m_levelStats[19] = halfMinutes / 2 + halfMinutes % 2;
    bool unlocked = false;
    bool movie = CheckIfMovieAndMedalNeedsToBeUnlocked();
    bool special = CheckIfSpecialMedalsNeedToBeUnlocked();
    if (movie || special)
        unlocked = true;
    m_levelStats[20] = unlocked;
    for (unsigned int i = 0; i < 21; i++) {
        if (i != 2 && i != 10 && i != 20 && m_levelStats[i] > 9999999)
            m_levelStats[i] = 9999999;
    }
    if (m_totalShotsFired < 9999999)
        m_totalShotsFired += m_shotsFired;
    if (m_totalHits < 9999999)
        m_totalHits += m_hits;
    if (m_totalHitsTaken < 9999999)
        m_totalHitsTaken += m_hitsTaken;
    if (m_total38 < 9999999)
        m_total38 += m_stat300;
    if (m_totalEnemiesKilled < 9999999)
        m_totalEnemiesKilled += m_enemiesKilled;
    if (m_totalShotLocations[0] < 9999999)
        m_totalShotLocations[0] += m_shotLocations[0];
    if (m_totalShotLocations[3] < 9999999)
        m_totalShotLocations[3] += m_shotLocations[3];
    if (m_totalShotLocations[6] < 9999999)
        m_totalShotLocations[6] += m_shotLocations[6];
    if (m_totalShotLocations[4] < 9999999)
        m_totalShotLocations[4] += m_shotLocations[4];
    if (m_totalShotLocations[1] < 9999999)
        m_totalShotLocations[1] += m_shotLocations[1];
    if (m_totalShotLocations[5] < 9999999)
        m_totalShotLocations[5] += m_shotLocations[5];
    if (m_totalShotLocations[2] < 9999999)
        m_totalShotLocations[2] += m_shotLocations[2];
    for (unsigned int i = 0; i < 45; i++)
        m_totalWeaponKills[i] += m_weaponKills[i];
    return m_levelStats;
}
