// A fragment of ShellMenu.cpp (0x800e14f8): CShellMenu's language subtitle
// index and suffix getters, and copying a player's 13-byte controller button
// map (at +544, one per player) out of or into the shell. GetNextLevelCheat
// advances the current mission and level to the next level (missions 1 and 5
// have four levels, 2 to 4 three, mission 6 two and then wraps to mission 1),
// marks the old level's star rating, runs the movie/medal and special medal
// unlock checks, then makes the new level current and unlocked. CShellMenu
// and the functions are named by the mangled symbols; the members (current
// mission and level at +4/+8, next at +12/+13, the star and unlock bit sets
// at +28/+36) and the result types are inferred. The rest of the file is not part of this unit.
struct StarLevelsInBits;
struct UnlockedLevelsInBits;

void SetStarRatingStatus(StarLevelsInBits*, unsigned int, unsigned int);
void SetUnlockedLevelStatus(UnlockedLevelsInBits*, unsigned int, unsigned int);

class CShellMenu {
public:
    void GetNextLevelCheat();
    void CheckIfMovieAndMedalNeedsToBeUnlocked();
    void CheckIfSpecialMedalsNeedToBeUnlocked();
    int Get_LanguageSubtitleIndex();
    int Get_LanguageSuffix();
    void RetrieveShellControllerButtons(char*, int);
    void CopyControllerButtons(char*, int);

    unsigned char unknown0000[4];
    int m_mission;
    int m_level;
    unsigned char m_nextMission;
    unsigned char m_nextLevel;
    unsigned char unknown000e[14];
    unsigned char m_starLevels[8];
    unsigned char m_unlockedLevels[8];
    unsigned char unknown002c[500];
    char m_buttons[4][13];
    unsigned char unknown0254[5776];
    int m_languageSuffix;
    int m_languageSubtitleIndex;
};

int CShellMenu::Get_LanguageSubtitleIndex() {
    return m_languageSubtitleIndex;
}

int CShellMenu::Get_LanguageSuffix() {
    return m_languageSuffix;
}

void CShellMenu::RetrieveShellControllerButtons(char* buttons, int player) {
    int i;

    for (i = 0; i < 13; i++)
        buttons[i] = m_buttons[player][i];
}

void CShellMenu::CopyControllerButtons(char* buttons, int player) {
    int i;

    for (i = 0; i < 13; i++)
        m_buttons[player][i] = buttons[i];
}

void CShellMenu::GetNextLevelCheat() {
    m_nextMission = m_mission;
    m_nextLevel = m_level;
    m_nextLevel++;
    switch (m_nextMission) {
    case 1:
        if (m_nextLevel > 4) {
            m_nextMission++;
            m_nextLevel = 1;
        }
        break;
    case 2:
        if (m_nextLevel > 3) {
            m_nextMission++;
            m_nextLevel = 1;
        }
        break;
    case 3:
        if (m_nextLevel > 3) {
            m_nextMission++;
            m_nextLevel = 1;
        }
        break;
    case 4:
        if (m_nextLevel > 3) {
            m_nextMission++;
            m_nextLevel = 1;
        }
        break;
    case 5:
        if (m_nextLevel > 4) {
            m_nextMission++;
            m_nextLevel = 1;
        }
        break;
    case 6:
        if (m_nextLevel > 2) {
            m_nextMission = 1;
            m_nextLevel = 1;
        }
        break;
    }
    SetStarRatingStatus((StarLevelsInBits*)m_starLevels, m_level + m_mission * 10, 3);
    CheckIfMovieAndMedalNeedsToBeUnlocked();
    CheckIfSpecialMedalsNeedToBeUnlocked();
    m_mission = m_nextMission;
    m_level = m_nextLevel;
    SetUnlockedLevelStatus((UnlockedLevelsInBits*)m_unlockedLevels, m_nextLevel + m_nextMission * 10, 1);
}
