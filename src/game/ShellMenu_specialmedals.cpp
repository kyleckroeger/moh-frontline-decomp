// A fragment of ShellMenu.cpp (0x800e1c1c): CShellMenu::
// CheckIfSpecialMedalsNeedToBeUnlocked. Once the last stage of mission 5 is
// reached (the stage count of each mission is a local table) it remembers
// that, then counts the six mission medals held (status 1, starting from the
// sixth and wrapping round) and awards the special medals the count earns:
// with all six the eighth and ninth medals and the seventh at its second
// grade, with five the ninth and the second grade, with four the second
// grade, with one to three the first grade. It returns whether a medal was
// newly awarded. The class and functions are named by the mangled symbols;
// the members (bit-fields at their offsets) and the result type are inferred.
class CShellMenu {
public:
    bool CheckIfSpecialMedalsNeedToBeUnlocked();
    void GetMedalStatus(int*);

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
};

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
