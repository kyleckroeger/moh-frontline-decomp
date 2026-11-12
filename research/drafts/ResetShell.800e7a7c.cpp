extern "C" {
void* memset(void*, int, unsigned long);
int sprintf(char*, const char*, ...);
}

// Inferred: the cheat settings (bytes and a float) reset with the shell.
struct CheatsView {
    unsigned char flag00;
    unsigned char flag01;
    unsigned char flag02;
    unsigned char flag03;
    float value04;
    unsigned char flag08;
    unsigned char flag09;
    unsigned char flag0a;
    unsigned char flag0b;
    unsigned char flag0c;
    unsigned char flag0d;
    unsigned char flag0e;
    unsigned char flag0f;
    unsigned char flag10;

    void Reset() {
        flag00 = 0;
        flag01 = 0;
        flag02 = 0;
        flag03 = 0;
        value04 = 0.0f;
        flag08 = 0;
        flag09 = 0;
        flag0a = 0;
        flag0b = 0;
        flag0d = 0;
        flag0e = 0;
        flag0c = 0;
        flag0f = 0;
        flag10 = 0;
    }
};

extern CheatsView g_cheats;

// Inferred: a multiplayer player's setup record (28 bytes).
struct MPPlayerView {
    int id;
    int character;
    char name[11];
    unsigned char flag13;
    unsigned char flag14;
    unsigned char flag15;
    unsigned char flag16;
    unsigned char flag17;
    unsigned char flag18;
};

// Inferred: a 256-byte multiplayer record ending in an id.
struct MPRecordView {
    unsigned char data[248];
    int id;
    unsigned char unknownfc[4];
};

class CMemoryCard {
public:
    ~CMemoryCard();
};

class IStudio {
public:
    ~IStudio();

    unsigned char data[28];
};

class CUserMessageTable {
public:
    ~CUserMessageTable();

    unsigned char data[16];
};

class CShellMenu {
public:
    ~CShellMenu();
    void ResetShell();

    inline void SetGameType(int type) {
        switch (m_gameType = type) {
        case 1:
            m_gameTypeSuffix = "";
            m_gameTypeIndex = 0;
            break;
        case 2:
            m_gameTypeSuffix = "s";
            m_gameTypeIndex = 1;
            break;
        case 3:
            m_gameTypeSuffix = "i";
            m_gameTypeIndex = 2;
            break;
        case 4:
            m_gameTypeSuffix = "";
            m_gameTypeIndex = 0;
            break;
        case 5:
            m_gameTypeSuffix = "";
            m_gameTypeIndex = 3;
            break;
        default:
            m_gameTypeSuffix = "";
            m_gameTypeIndex = 0;
            break;
        }
    }

    inline void ResetPlayer(int player) {
        unsigned short index = player;
        m_players[index].flag13 = 0;
        m_players[index].flag14 = 1;
        m_players[index].flag15 = 0;
        m_players[index].flag16 = 1;
        m_players[index].flag17 = 1;
        m_players[index].id = index;
        m_players[index].character = index;
        m_players[index].flag18 = 1;
        m_handicaps[index] = 100;
        m_teams[0] = -1;
        m_teams[1] = -1;
        m_teams[2] = -1;
        m_teams[3] = -1;
        sprintf(m_players[index].name, "PLAYER %d", index + 1);
    }

    unsigned char unknown0000[4];
    int m_value004;
    int m_value008;
    unsigned char m_value00c;
    unsigned char m_value00d;
    unsigned char m_value00e;
    unsigned char m_value00f;
    unsigned char m_value010;
    unsigned char unknown0011[3];
    unsigned char m_block014[4];
    struct {
        unsigned char unknown0 : 5;
        unsigned char flag04 : 1;
        unsigned char unknown1 : 2;
        unsigned char unknown019[3];
    } m_block018;
    struct {
        unsigned char unknown01c[5];
        unsigned char bits : 2;
        unsigned char unknown021 : 6;
        unsigned char unknown022[2];
    } m_block01c;
    struct {
        unsigned char unknown024[2];
        unsigned char unknown0 : 4;
        unsigned char flag08 : 1;
        unsigned char unknown1 : 3;
        unsigned char unknown027;
    } m_block024;
    unsigned char unknown0028[4];
    unsigned char m_block02c[244];
    unsigned char m_block120[244];
    unsigned char unknown0214[4];
    unsigned char unknown0218_0 : 1;
    unsigned char m_flag218_40 : 1;
    unsigned char m_flag218_20 : 1;
    unsigned char unknown0218_1 : 1;
    unsigned char m_flag218_08 : 1;
    unsigned char m_flag218_04 : 1;
    unsigned char unknown0218_2 : 2;
    unsigned char unknown0219[3];
    int m_gameType;
    unsigned char unknown0220[52];
    unsigned char m_value254;
    unsigned char unknown0255[4515];
    int m_value13f8;
    unsigned char m_block13fc[12];
    unsigned char m_block1408[5];
    unsigned char unknown140d[3];
    int m_value1410;
    int m_value1414;
    int m_value1418;
    int m_value141c;
    int m_value1420;
    int m_value1424;
    int m_value1428;
    int m_value142c;
    int m_value1430;
    int m_value1434;
    int m_value1438;
    int m_handicaps[4];
    MPPlayerView m_players[4];
    int m_teams[4];
    MPRecordView m_records[4];
    unsigned char unknown18cc[24];
    const char* m_gameTypeSuffix;
    int m_gameTypeIndex;
    unsigned char m_value18ec;
    unsigned char unknown18ed[3];
    IStudio m_studio;
    CMemoryCard* m_card;
    unsigned char unknown1910[149];
    unsigned char m_value19a5;
    unsigned char m_value19a6;
    unsigned char unknown19a7[2];
    unsigned char m_value19a9;
    unsigned char m_value19aa;
    unsigned char m_value19ab;
    unsigned char unknown19ac[24];
    CUserMessageTable m_messages;
};

CShellMenu::~CShellMenu() {
    if (m_card) {
        delete m_card;
        m_card = 0;
    }
}

void CShellMenu::ResetShell() {
    m_value254 = 0;
    m_value00e = 0;
    memset(&m_block014, 0, sizeof(m_block014));
    memset(&m_block018, 0, sizeof(m_block018));
    memset(&m_block01c, 0, sizeof(m_block01c));
    memset(&m_block024, 0, sizeof(m_block024));
    g_cheats.Reset();
    memset(m_block120, 0, sizeof(m_block120));
    memset(m_block02c, 0, sizeof(m_block02c));
    m_flag218_40 = 0;
    m_flag218_20 = 0;
    SetGameType(1);
    m_gameTypeSuffix = "";
    m_value18ec = 0;
    m_value19a5 = 0;
    m_value19a6 = 1;
    m_value00f = 0;
    m_value010 = 0;
    m_value19aa = 1;
    m_value19ab = 1;
    m_value004 = 6;
    m_value008 = 1;
    m_value00c = 6;
    m_value00d = 1;
    m_block01c.bits = 0;
    m_block024.flag08 = 1;
    m_block018.flag04 = 0;
    m_value13f8 = 0;
    m_flag218_08 = 0;
    m_flag218_04 = 0;
    m_value19a9 = 0;
    memset(m_block13fc, 0, sizeof(m_block13fc));
    memset(m_block1408, 0, sizeof(m_block1408));
    for (int i = 0; i < 4; i++)
        ResetPlayer(i);
    m_value1418 = 1;
    m_value1420 = 1;
    m_value1424 = 0;
    m_value1428 = 10;
    m_value1434 = 1;
    m_value1438 = 1;
    m_value1410 = 0;
    m_value1414 = 0;
    m_value141c = 7;
    if (m_value1424 > 0)
        m_value142c = 1;
    else
        m_value142c = 0;
    if (m_value1428 > 0)
        m_value1430 = 1;
    else
        m_value1430 = 0;
    memset(m_records, 0, sizeof(m_records));
    m_records[0].id = -1;
    m_records[1].id = -1;
    m_records[2].id = -1;
    m_records[3].id = -1;
}
