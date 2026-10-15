// A fragment of ShellMenu.cpp (0x800e058c): SetUnlockedLevelStatus sets a
// level's unlocked bit (levels 11-64 by mission and level number, eight to
// a byte, the first in the top bit), reporting an unknown level id. For
// levels 24, 34, 44, 63 and 64 the code clears the bit whatever the status
// is, and that is kept. The function and UnlockedLevelsInBits are named by
// the symbols; the bit layout follows the code and the field names are
// placeholders by level id. The rest of the file is in other units.
void DebugMsg(const char*, ...);

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

void SetUnlockedLevelStatus(UnlockedLevelsInBits* bits, unsigned int level, unsigned int status) {
    switch (level) {
    case 11:
        bits->level11 = status;
        break;
    case 12:
        bits->level12 = status;
        break;
    case 13:
        bits->level13 = status;
        break;
    case 14:
        bits->level14 = status;
        break;
    case 21:
        bits->level21 = status;
        break;
    case 22:
        bits->level22 = status;
        break;
    case 23:
        bits->level23 = status;
        break;
    case 24:
        bits->level24 = 0;
        break;
    case 31:
        bits->level31 = status;
        break;
    case 32:
        bits->level32 = status;
        break;
    case 33:
        bits->level33 = status;
        break;
    case 34:
        bits->level34 = 0;
        break;
    case 41:
        bits->level41 = status;
        break;
    case 42:
        bits->level42 = status;
        break;
    case 43:
        bits->level43 = status;
        break;
    case 44:
        bits->level44 = 0;
        break;
    case 51:
        bits->level51 = status;
        break;
    case 52:
        bits->level52 = status;
        break;
    case 53:
        bits->level53 = status;
        break;
    case 54:
        bits->level54 = status;
        break;
    case 61:
        bits->level61 = status;
        break;
    case 62:
        bits->level62 = status;
        break;
    case 63:
        bits->level63 = 0;
        break;
    case 64:
        bits->level64 = 0;
        break;
    default:
        DebugMsg("Level ID: %d\n", level);
        break;
    }
}
