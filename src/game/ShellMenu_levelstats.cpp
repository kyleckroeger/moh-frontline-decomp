// A fragment of ShellMenu.cpp (0x800e5a14): CShellMenu::GetCumulativeStats
// and CShellMenu::GetLevelStats.
// GetCumulativeStats finishes the running totals for the studio: the total
// accuracy, the favourite weapon (most kills over the 45 weapons), a new
// evaluation (the most-hit body location times five plus a random 0-4) when
// shots were fired since the last call, then fills 16 values (shots, hits,
// accuracy, favourite weapon, hits taken, kills, hits per body location,
// evaluation) clamped to 9999999 into the same table GetLevelStats fills.
// GetLevelStats finishes a level's statistics for the results screen. It walks the
// objective list (counting finished objectives into a value it never uses),
// rates the level outside mission 7 (three stars for 95% of the enemies
// killed within the level's record time of at least 75, two for 95%, else
// one) and records the rating, then fills the 21 values the studio shows
// (level number, rating, kills, shots, hits, accuracy, favourite weapon, hits
// per body location, evaluation, minutes rounded to the nearest, whether a
// medal or movie was unlocked) clamped to 9999999, and adds the level's
// counts into the running totals. The classes, functions and globals are
// named by the mangled symbols; the members, the objective list and level
// record views, the unused objective count and the most-hit-location helper
// (MostHit) are inferred.
extern "C" int rand();

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
    unsigned int* GetCumulativeStats();
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
    unsigned char unknown005c[8];
    float m_totalAccuracy;
    unsigned int m_totalWeaponKills[45];
    unsigned char m_totalWeapon;
    unsigned char m_totalEvaluation;
    unsigned char m_total11e;
    unsigned char unknown011f;
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
    unsigned int m_lastShotsFired;
    unsigned char unknown13fc[1304];
    unsigned int m_stats[21];
};

static inline int MostHit(const unsigned int* values) {
    unsigned int hits = 0;
    int location = 0;
    for (int j = 0; j < 7; j++) {
        unsigned int v = values[j];
        if (v > hits) {
            hits = v;
            location = j;
        }
    }
    return location;
}

unsigned int* CShellMenu::GetCumulativeStats() {
    if (m_totalShotsFired != 0)
        m_totalAccuracy = 100.0f * ((float)m_totalHits / (float)m_totalShotsFired);
    else
        m_totalAccuracy = 0.0f;
    m_total11e = m_total11e / 17.0f;
    unsigned int i = 0;
    unsigned int most = 0;
    int weapon = 0;
    for (; i < 45; i++) {
        if (m_totalWeaponKills[i] > most) {
            most = m_totalWeaponKills[i];
            weapon = i;
        }
    }
    m_totalWeapon = weapon;
    if (m_lastShotsFired < m_totalShotsFired) {
        int location = MostHit(m_totalShotLocations);
        m_totalEvaluation = location * 5 + rand() % 5;
    }
    m_lastShotsFired = m_totalShotsFired;
    m_stats[0] = m_totalShotsFired;
    m_stats[1] = m_totalHits;
    m_stats[2] = m_totalAccuracy;
    m_stats[3] = m_totalWeapon;
    m_stats[4] = m_totalHitsTaken;
    m_stats[5] = m_total38;
    m_stats[6] = m_totalEnemiesKilled;
    m_stats[8] = m_totalShotLocations[0];
    m_stats[9] = m_totalShotLocations[3];
    m_stats[10] = m_totalShotLocations[6];
    m_stats[11] = m_totalShotLocations[4];
    m_stats[12] = m_totalShotLocations[1];
    m_stats[13] = m_totalShotLocations[5];
    m_stats[14] = m_totalShotLocations[2];
    m_stats[15] = m_totalEvaluation;
    for (int k = 0; k < 16; k++) {
        if (m_stats[k] > 9999999)
            m_stats[k] = 9999999;
    }
    return m_stats;
}

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
    m_stats[0] = m_stage + m_mission * 10;
    m_stats[1] = m_rating;
    m_stats[2] = m_value00e;
    m_stats[3] = m_shotsFired;
    m_stats[4] = m_hits;
    m_stats[5] = m_accuracy;
    m_stats[6] = m_weapon;
    m_stats[7] = m_hitsTaken;
    m_stats[8] = m_stat300;
    m_stats[9] = m_enemiesKilled;
    m_stats[10] = 0;
    m_stats[11] = m_shotLocations[0];
    m_stats[12] = m_shotLocations[3];
    m_stats[13] = m_shotLocations[6];
    m_stats[14] = m_shotLocations[4];
    m_stats[15] = m_shotLocations[1];
    m_stats[16] = m_shotLocations[5];
    m_stats[17] = m_shotLocations[2];
    m_stats[18] = m_evaluation;
    unsigned int halfMinutes = m_levelTimer / 30;
    m_stats[19] = halfMinutes / 2 + halfMinutes % 2;
    bool unlocked = false;
    bool movie = CheckIfMovieAndMedalNeedsToBeUnlocked();
    bool special = CheckIfSpecialMedalsNeedToBeUnlocked();
    if (movie || special)
        unlocked = true;
    m_stats[20] = unlocked;
    for (unsigned int i = 0; i < 21; i++) {
        if (i != 2 && i != 10 && i != 20 && m_stats[i] > 9999999)
            m_stats[i] = 9999999;
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
    return m_stats;
}
