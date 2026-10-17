// A fragment of ShellMenu.cpp (0x800dffd8): CShellMenu::CheckIfValidPassword
// and SetUnlockedLevelStatus. CheckIfValidPassword hashes the entered
// password with a CRC-32 (polynomial 0x04c11db7, reflected, initial value and
// final xor all ones) and compares it with the known cheat codes: one
// activates every cheat, nine set individual cheat fields (and keep checking),
// five unlock missions 1 to 5 from their first level, and one jumps to the
// next level. The CRC object is allocated and never freed, as the code does.
// SetUnlockedLevelStatus sets a level's unlocked bit (levels 11-64 by mission
// and level number, eight to a byte, the first in the top bit), reporting an
// unknown level id. For levels 24, 34, 44, 63 and 64 the code clears the bit
// whatever the status is, and that is kept. The functions, CShellMenu and
// UnlockedLevelsInBits are named by the symbols; the CRC class (a
// Williams-style CRC model with inline members), the CShellMenu members, the
// cheat field names and the level bit layout's field names are inferred
// (members at their offsets, placeholders by offset or level id). The rest of
// the file is in other units.
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

void SetUnlockedLevelStatus(UnlockedLevelsInBits*, unsigned int, unsigned int);

void* operator new(unsigned long);

class CCrcModel {
public:
    CCrcModel(int width, unsigned long poly, unsigned long init, bool reflect, unsigned long xorOut) {
        m_width = width;
        m_poly = poly;
        m_init = init;
        m_xorOut = xorOut;
        m_reflect = reflect;
        m_mask = ((1UL << (width - 1)) - 1) << 1 | 1;
        Reset();
    }
    void Reset() { m_register = m_init; }
    static unsigned long Reflect(unsigned int value, unsigned int bits) {
        unsigned long t = value;
        for (unsigned int i = 0; i < bits; i++) {
            if (t & 1)
                value |= 1UL << ((bits - 1) - i);
            else
                value &= ~(1UL << ((bits - 1) - i));
            t >>= 1;
        }
        return value;
    }
    void Next(unsigned char ch) {
        unsigned long uch = ch;
        unsigned long topBit = 1UL << (m_width - 1);
        if (m_reflect)
            uch = Reflect(uch, 8);
        m_register ^= uch << (m_width - 8);
        for (int i = 0; i < 8; i++) {
            if (m_register & topBit)
                m_register = (m_register << 1) ^ m_poly;
            else
                m_register <<= 1;
            m_register &= m_mask;
        }
    }
    unsigned long Value() {
        if (m_reflect)
            return m_xorOut ^ Reflect(m_register, m_width);
        return m_xorOut ^ m_register;
    }

    int m_width;
    unsigned long m_mask;
    unsigned long m_poly;
    unsigned long m_init;
    unsigned long m_reflect;
    unsigned long m_xorOut;
    unsigned long m_register;
};

class CShellMenu {
public:
    bool CheckIfValidPassword(char*);
    void ActivateCheatNamed_All();
    void GetNextLevelCheat();

    unsigned char unknown0000[4];
    int m_mission;
    int m_level;
    unsigned char m_nextMission;
    unsigned char m_nextLevel;
    unsigned char unknown000e[6];
    unsigned char m_cheat20_0 : 2;
    unsigned char m_cheat20_1 : 2;
    unsigned char m_cheat20_2 : 2;
    unsigned char m_cheat20_3 : 2;
    unsigned char m_cheat21_0 : 2;
    unsigned char m_cheat21_1 : 2;
    unsigned char m_cheat21_2 : 2;
    unsigned char m_cheat21_3 : 2;
    unsigned char m_cheat22_0 : 2;
    unsigned char m_cheat22_1 : 2;
    unsigned char m_cheat22_2 : 2;
    unsigned char m_cheat22_3 : 2;
    unsigned char unknown0017[13];
    UnlockedLevelsInBits* unlocked() { return (UnlockedLevelsInBits*)m_unlockedLevels; }
    unsigned char m_unlockedLevels[8];
};

bool CShellMenu::CheckIfValidPassword(char* password) {
    bool found = false;
    CCrcModel* crc = new CCrcModel(32, 0x04c11db7, 0xffffffff, true, 0xffffffff);
    for (char* p = password; *p; p++)
        crc->Next(*p);
    unsigned long value = crc->Value();
    if (value == 0x2f29ccb7) {
        ActivateCheatNamed_All();
        return true;
    }
    if (value == 0x0412fe17) {
        m_cheat20_2 = 1;
        found = true;
    }
    if (value == 0x72c433df) {
        m_cheat21_2 = 1;
        found = true;
    }
    if (value == 0x3d8e0f1e) {
        m_cheat21_3 = 1;
        found = true;
    }
    if (value == 0x8a7e1481) {
        m_cheat20_0 = 1;
        found = true;
    }
    if (value == 0x5c3f5629) {
        m_cheat20_1 = 1;
        found = true;
    }
    if (value == 0x832b676b) {
        m_cheat20_3 = 1;
        found = true;
    }
    if (value == 0x06a5a920) {
        m_cheat21_1 = 1;
        found = true;
    }
    if (value == 0x7e6a8cd5) {
        m_cheat21_0 = 1;
        found = true;
    }
    if (value == 0x3255c06f) {
        m_cheat22_1 = 1;
        found = true;
    }
    if (value == 0x5a36312f) {
        m_mission = 1;
        m_level = 1;
        m_nextMission = 1;
        m_nextLevel = 1;
        SetUnlockedLevelStatus(unlocked(), m_mission * 10 + m_level, 1);
        return true;
    }
    if (value == 0x1a481bb2) {
        m_mission = 2;
        m_level = 1;
        m_nextMission = 2;
        m_nextLevel = 1;
        SetUnlockedLevelStatus(unlocked(), m_mission * 10 + m_level, 1);
        return true;
    }
    if (value == 0xcd04237d) {
        m_mission = 3;
        m_level = 1;
        m_nextMission = 3;
        m_nextLevel = 1;
        SetUnlockedLevelStatus(unlocked(), m_mission * 10 + m_level, 1);
        return true;
    }
    if (value == 0xa4a39546) {
        m_mission = 4;
        m_level = 1;
        m_nextMission = 4;
        m_nextLevel = 1;
        SetUnlockedLevelStatus(unlocked(), m_mission * 10 + m_level, 1);
        return true;
    }
    if (value == 0x0a77fa00) {
        m_mission = 5;
        m_level = 1;
        m_nextMission = 5;
        m_nextLevel = 1;
        SetUnlockedLevelStatus(unlocked(), m_mission * 10 + m_level, 1);
        return true;
    }
    if (value == 0xfa45471f) {
        GetNextLevelCheat();
        return true;
    }
    return found;
}

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
