// A fragment of ShellMenu.cpp (0x800dfca8): CShellMenu::ActivateCheatNamed_All
// unlocks everything: every level of the six missions gets three stars and
// is unlocked, the ten medals and ten secrets are awarded (each through a
// switch over its number), all seventeen movies are unlocked, the unused
// level entries are cleared again and the all-unlocked flag is set. The
// class and functions are named by the mangled symbols; the members (bit
// fields at their offsets) and the level bit-set views are inferred.
struct StarLevelsInBits {
    unsigned char level0;
    unsigned char level1 : 6;
    unsigned char level1Low : 2;
    unsigned char level2 : 6;
    unsigned char level2Low : 2;
    unsigned char level3 : 6;
    unsigned char level3Low : 2;
    unsigned char level4;
    unsigned char level5 : 4;
    unsigned char level5Mid : 2;
    unsigned char level5Low : 2;
    unsigned char level6;
    unsigned char level7;
};

struct UnlockedLevelsInBits {
    unsigned char level0 : 7;
    unsigned char level0Low : 1;
    unsigned char level1 : 3;
    unsigned char level1Bit4 : 1;
    unsigned char level1Mid : 3;
    unsigned char level1Low : 1;
    unsigned char level2 : 6;
    unsigned char level2Bit1 : 1;
    unsigned char level2Low : 1;
    unsigned char level3;
};

void SetStarRatingStatus(StarLevelsInBits*, unsigned int, unsigned int);
void SetUnlockedLevelStatus(UnlockedLevelsInBits*, unsigned int, unsigned int);

class CShellMenu {
public:
    void ActivateCheatNamed_All();

    unsigned char unknown0000[20];
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
    unsigned char unknown0016 : 4;
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
    unsigned char unknown001a[2];
    StarLevelsInBits m_stars;
    UnlockedLevelsInBits m_unlocked;
    unsigned char unknown0028[5076];
    bool m_movies[17];
    unsigned char unknown140d[1432];
    unsigned char m_value19a5;
};

void CShellMenu::ActivateCheatNamed_All() {
    for (int mission = 0; mission < 6; mission++) {
        for (int level = 0; level < 4; level++) {
            SetStarRatingStatus(&m_stars, (mission + 1) * 10 + (level + 1), 3);
            SetUnlockedLevelStatus(&m_unlocked, (mission + 1) * 10 + (level + 1), 1);
        }
    }
    for (int i = 0; i < 10; i++) {
        switch (i + 1) {
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
    }
    for (int i = 0; i < 10; i++) {
        switch (i + 1) {
        case 1:
            m_secret1 = 1;
            break;
        case 2:
            m_secret2 = 1;
            break;
        case 3:
            m_secret3 = 1;
            break;
        case 4:
            m_secret4 = 1;
            break;
        case 5:
            m_secret5 = 1;
            break;
        case 6:
            m_secret6 = 1;
            break;
        case 7:
            m_secret7 = 1;
            break;
        case 8:
            m_secret8 = 1;
            break;
        case 9:
            m_secret9 = 1;
            break;
        case 10:
            m_secret10 = 1;
            break;
        }
    }
    m_movies[0] = true;
    m_movies[1] = true;
    m_movies[2] = true;
    m_movies[3] = true;
    m_movies[4] = true;
    m_movies[5] = true;
    m_movies[6] = true;
    m_movies[7] = true;
    m_movies[8] = true;
    m_movies[9] = true;
    m_movies[10] = true;
    m_movies[11] = true;
    m_movies[12] = true;
    m_movies[13] = true;
    m_movies[14] = true;
    m_movies[15] = true;
    m_movies[16] = true;
    m_stars.level1Low = 0;
    m_stars.level2Low = 0;
    m_stars.level3Low = 0;
    m_stars.level5Mid = 0;
    m_stars.level5Low = 0;
    m_unlocked.level0Low = 0;
    m_unlocked.level1Bit4 = 0;
    m_unlocked.level1Low = 0;
    m_unlocked.level2Bit1 = 0;
    m_unlocked.level2Low = 0;
    m_value19a5 = 1;
}
