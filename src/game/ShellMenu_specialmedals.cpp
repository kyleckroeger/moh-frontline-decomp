// A fragment of ShellMenu.cpp (0x800e1914): CShellMenu::
// CheckIfMovieAndMedalNeedsToBeUnlocked and
// CheckIfSpecialMedalsNeedToBeUnlocked. The first takes the star ratings of
// all levels, copies the current mission's four, marks each of the mission's
// stages (the stage count of each mission is a local table) rated three
// stars, and when all of them are, awards the mission's medal unless it is
// already held (status 1) and sets the mission's two unlock flags; it returns
// whether the mission was completed with a new medal. The second, once the
// last stage of mission 5 is reached, remembers that, then counts the six
// mission medals held (status 1, starting from the sixth and wrapping round)
// and awards the special medals the count earns: with all six the eighth and
// ninth medals and the seventh at its second grade, with five the ninth and
// the second grade, with four the second grade, with one to three the first
// grade. It returns whether a medal was newly awarded. The class and
// functions are named by the mangled symbols; the members (bit-fields at
// their offsets, the unlock flag pairs), the result types and the row
// pointer of the star copy are inferred.
extern "C" void* memset(void*, int, unsigned long);

/* inferred: the per-mission unlock pair (movie, medal) */
struct SUnlockFlags {
    bool m_movie;
    bool m_medal;
};

class CShellMenu {
public:
    bool CheckIfMovieAndMedalNeedsToBeUnlocked();
    bool CheckIfSpecialMedalsNeedToBeUnlocked();
    void GetMedalStatus(int*);
    void GetAllLevelsStarStatus(int*);

    unsigned char unknown0000[4];
    unsigned int m_mission;
    unsigned int m_stage;
    unsigned char unknown000c[12];
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
    unsigned char unknown001a[510];
    unsigned char m_flags218_7 : 5;
    unsigned char m_finalMission : 1;
    unsigned char m_flags218_1 : 2;
    unsigned char unknown0219[4579];
    SUnlockFlags m_unlocked[11];
};

bool CShellMenu::CheckIfMovieAndMedalNeedsToBeUnlocked() {
    int* row;
    int stars[24];
    memset(stars, 0, sizeof(stars));
    GetAllLevelsStarStatus(stars);
    unsigned char stages[7];
    stages[0] = 0;
    stages[1] = 4;
    stages[2] = 3;
    stages[3] = 3;
    stages[4] = 3;
    stages[5] = 4;
    stages[6] = 2;
    int count = stages[(unsigned char)m_mission];
    int missionStars[4];
    row = missionStars;
    for (int k = 0; k < 4; k++)
        row[k] = stars[(m_mission - 1) * 4 + k];
    bool complete[4];
    for (int j = 0; j < 4; j++)
        complete[3 - j] = false;
    int* p = row;
    for (int i = 0; i < count; i++) {
        if (*p == 3)
            complete[i] = true;
        p++;
    }
    bool unlock = false;
    switch (count) {
    case 2:
        if (complete[0] && complete[1])
            unlock = true;
        break;
    case 3:
        if (complete[0] && complete[1] && complete[2])
            unlock = true;
        break;
    case 4:
        if (complete[0] && complete[1] && complete[2] && complete[3])
            unlock = true;
        break;
    default:
        unlock = false;
        break;
    }
    int medals[9];
    GetMedalStatus(medals);
    if (unlock) {
        if (medals[m_mission - 1] != 1) {
            switch (m_mission) {
            case 1:
                m_medal1 = 1;
                break;
            case 2:
                m_medal2 = 1;
                break;
            case 3:
                m_medal3 = 1;
                break;
            case 4:
                m_medal4 = 1;
                break;
            case 5:
                m_medal5 = 1;
                break;
            case 6:
                m_medal6 = 1;
                break;
            case 7:
                m_medal7 = 1;
                break;
            case 8:
                m_medal8 = 1;
                break;
            case 9:
                m_medal9 = 1;
                break;
            case 10:
                m_medal10 = 1;
                break;
            }
        } else {
            unlock = false;
        }
        int index = m_mission - 1;
        m_unlocked[index].m_movie = true;
        m_unlocked[index].m_medal = true;
    }
    return unlock;
}

bool CShellMenu::CheckIfSpecialMedalsNeedToBeUnlocked() {
    unsigned char stages[7];
    stages[0] = 0;
    stages[1] = 4;
    stages[2] = 3;
    stages[3] = 3;
    stages[4] = 3;
    stages[5] = 4;
    stages[6] = 2;
    bool lastStage = m_stage == stages[m_mission];
    if (!m_finalMission) {
        if (!lastStage)
            return false;
        if (m_mission != 5)
            return false;
        m_finalMission = 1;
    }
    int medals[9];
    GetMedalStatus(medals);
    int count = 0;
    int i = 5;
    for (int n = 0; n < 6; n++) {
        if (medals[i] == 1)
            count++;
        if (i == 5)
            i = -1;
        i++;
    }
    bool unlocked = false;
    switch (count) {
    case 6:
        if (medals[7] != 1) {
            m_medal8 = 1;
            unlocked = true;
        }
    case 5:
        if (medals[8] != 1) {
            m_medal9 = 1;
            unlocked = true;
        }
    case 4:
        if (medals[6] != 2) {
            m_medal7 = 2;
            unlocked = true;
        }
        break;
    case 1:
    case 2:
    case 3:
        if (medals[6] != 1) {
            m_medal7 = 1;
            unlocked = true;
        }
        break;
    }
    return unlocked;
}
