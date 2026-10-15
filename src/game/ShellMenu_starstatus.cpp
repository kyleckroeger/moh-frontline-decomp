// A fragment of ShellMenu.cpp (0x800e8424): CShellMenu::GetAllLevelsStarStatus
// fills an array of 24 star ratings (6 missions of 4 levels) from the star
// bits at +28; for levels 24, 34, 44, 63 and 64 the code reports 0, and an
// unknown id keeps the previous rating, as the code does. The class, the
// function and StarLevelsInBits are named by the symbols; the bits follow
// ShellMenu_stars.cpp and the menu view is inferred. The rest of the file is
// in other units.
struct StarLevelsInBits {
    unsigned int level11 : 2;
    unsigned int level12 : 2;
    unsigned int level13 : 2;
    unsigned int level14 : 2;
    unsigned int level21 : 2;
    unsigned int level22 : 2;
    unsigned int level23 : 2;
    unsigned int level24 : 2;
    unsigned int level31 : 2;
    unsigned int level32 : 2;
    unsigned int level33 : 2;
    unsigned int level34 : 2;
    unsigned int level41 : 2;
    unsigned int level42 : 2;
    unsigned int level43 : 2;
    unsigned int level44 : 2;
    unsigned int level51 : 2;
    unsigned int level52 : 2;
    unsigned int level53 : 2;
    unsigned int level54 : 2;
    unsigned int level61 : 2;
    unsigned int level62 : 2;
    unsigned int level63 : 2;
    unsigned int level64 : 2;
};

class CShellMenu {
public:
    void GetAllLevelsStarStatus(int*);

    unsigned char unknown00[28];
    StarLevelsInBits m_stars;
};

void CShellMenu::GetAllLevelsStarStatus(int* status) {
    int rating = 0;
    for (int mission = 0; mission < 6; mission++) {
        for (int level = 0; level < 4; level++) {
            switch ((mission + 1) * 10 + (level + 1)) {
            case 11:
                rating = m_stars.level11;
                break;
            case 12:
                rating = m_stars.level12;
                break;
            case 13:
                rating = m_stars.level13;
                break;
            case 14:
                rating = m_stars.level14;
                break;
            case 21:
                rating = m_stars.level21;
                break;
            case 22:
                rating = m_stars.level22;
                break;
            case 23:
                rating = m_stars.level23;
                break;
            case 24:
                rating = 0;
                break;
            case 31:
                rating = m_stars.level31;
                break;
            case 32:
                rating = m_stars.level32;
                break;
            case 33:
                rating = m_stars.level33;
                break;
            case 34:
                rating = 0;
                break;
            case 41:
                rating = m_stars.level41;
                break;
            case 42:
                rating = m_stars.level42;
                break;
            case 43:
                rating = m_stars.level43;
                break;
            case 44:
                rating = 0;
                break;
            case 51:
                rating = m_stars.level51;
                break;
            case 52:
                rating = m_stars.level52;
                break;
            case 53:
                rating = m_stars.level53;
                break;
            case 54:
                rating = m_stars.level54;
                break;
            case 61:
                rating = m_stars.level61;
                break;
            case 62:
                rating = m_stars.level62;
                break;
            case 63:
                rating = 0;
                break;
            case 64:
                rating = 0;
                break;
            }
            status[mission * 4 + level] = rating;
        }
    }
}
