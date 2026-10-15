// A fragment of ShellMenu.cpp (0x800e82a8): CShellMenu::GetLevelUnlockedStatus
// fills an array of 24 flags (6 missions of 4 levels) from the unlocked
// level bits at +36; for levels 24, 34, 44, 63 and 64 the code reports 0, and
// an unknown id keeps the previous flag, as the code does. The class, the
// function and UnlockedLevelsInBits are named by the symbols; the bits
// follow ShellMenu_unlock.cpp and the menu view is inferred. The rest of the
// file is in other units.
struct UnlockedLevelsInBits {
    unsigned int level11 : 1;
    unsigned int level12 : 1;
    unsigned int level13 : 1;
    unsigned int level14 : 1;
    unsigned int level21 : 1;
    unsigned int level22 : 1;
    unsigned int level23 : 1;
    unsigned int level24 : 1;
    unsigned int level31 : 1;
    unsigned int level32 : 1;
    unsigned int level33 : 1;
    unsigned int level34 : 1;
    unsigned int level41 : 1;
    unsigned int level42 : 1;
    unsigned int level43 : 1;
    unsigned int level44 : 1;
    unsigned int level51 : 1;
    unsigned int level52 : 1;
    unsigned int level53 : 1;
    unsigned int level54 : 1;
    unsigned int level61 : 1;
    unsigned int level62 : 1;
    unsigned int level63 : 1;
    unsigned int level64 : 1;
};

class CShellMenu {
public:
    void GetLevelUnlockedStatus(int*);

    unsigned char unknown00[36];
    UnlockedLevelsInBits m_unlocked;
};

void CShellMenu::GetLevelUnlockedStatus(int* status) {
    int unlocked = 0;
    for (int mission = 0; mission < 6; mission++) {
        for (int level = 0; level < 4; level++) {
            switch ((mission + 1) * 10 + (level + 1)) {
            case 11:
                unlocked = m_unlocked.level11;
                break;
            case 12:
                unlocked = m_unlocked.level12;
                break;
            case 13:
                unlocked = m_unlocked.level13;
                break;
            case 14:
                unlocked = m_unlocked.level14;
                break;
            case 21:
                unlocked = m_unlocked.level21;
                break;
            case 22:
                unlocked = m_unlocked.level22;
                break;
            case 23:
                unlocked = m_unlocked.level23;
                break;
            case 24:
                unlocked = 0;
                break;
            case 31:
                unlocked = m_unlocked.level31;
                break;
            case 32:
                unlocked = m_unlocked.level32;
                break;
            case 33:
                unlocked = m_unlocked.level33;
                break;
            case 34:
                unlocked = 0;
                break;
            case 41:
                unlocked = m_unlocked.level41;
                break;
            case 42:
                unlocked = m_unlocked.level42;
                break;
            case 43:
                unlocked = m_unlocked.level43;
                break;
            case 44:
                unlocked = 0;
                break;
            case 51:
                unlocked = m_unlocked.level51;
                break;
            case 52:
                unlocked = m_unlocked.level52;
                break;
            case 53:
                unlocked = m_unlocked.level53;
                break;
            case 54:
                unlocked = m_unlocked.level54;
                break;
            case 61:
                unlocked = m_unlocked.level61;
                break;
            case 62:
                unlocked = m_unlocked.level62;
                break;
            case 63:
                unlocked = 0;
                break;
            case 64:
                unlocked = 0;
                break;
            }
            status[mission * 4 + level] = unlocked;
        }
    }
}
