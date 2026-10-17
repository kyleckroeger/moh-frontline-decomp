// A fragment of ShellMenu.cpp (0x800e7dd8): the CShellMenu constructor. After
// the studio and user-message-table members are constructed it sets the
// shell's defaults: the audio settings (7, 7 and the sound mode from the
// console's setting), the controller options, single-player game type, the
// default controller configuration and custom buttons, the cheat, movie and
// level fields, the four multiplayer player setups ("PLAYER n") and match
// settings, and clears the multiplayer records. Much of it repeats
// ResetShell, whose inline helpers it shares. The strings and the 0.0f are
// entries of the file's .sdata2 and .rodata pools.
// CShellMenu, IStudio, CUserMessageTable, CDualConfig and the functions are
// named by the mangled symbols and the globals by their symbols; the members,
// the records and the inline helpers (game type, player, button and audio
// setters) are inferred (members at their offsets, names not original). The
// rest of the file is in other units.
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

class CMemoryCard;

class CSoundStream;
extern CSoundStream* pSoundStream;

extern "C" unsigned long OSGetSoundMode(void);

class CDualConfig {
public:
    void SetUpDefaultConfig(int);
    void GetDefaultCustomControllerValues(char*);

    unsigned char data[72];
};

extern CDualConfig g_ControllerConfig;

struct AudioOptionsView {
    unsigned char m_settings[3];

    void Set(int index, unsigned char value) {
        if (this)
            m_settings[index] = value;
    }
};

class IStudio {
public:
    IStudio();
    ~IStudio();

    unsigned char data[24];
};

class CUserMessageTable {
public:
    CUserMessageTable();
    ~CUserMessageTable();

    unsigned char data[16];
};

class CShellMenu {
public:
    CShellMenu();

    inline void InitButtons() {
        for (int i = 0; i < 4; i++)
            g_ControllerConfig.GetDefaultCustomControllerValues(m_buttons[i]);
    }
    inline void ResetPlayers() {
        for (int i = 0; i < 4; i++)
            ResetPlayer(i);
    }
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

    unsigned char m_value000;
    unsigned char m_value001;
    unsigned char m_value002;
    unsigned char m_value003;
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
    AudioOptionsView m_audio;
    unsigned char unknown002b;
    unsigned char m_block02c[244];
    unsigned char m_block120[244];
    unsigned char m_options[4];
    unsigned char m_flag218_80 : 1;
    unsigned char m_flag218_40 : 1;
    unsigned char m_flag218_20 : 1;
    unsigned char m_flag218_10 : 1;
    unsigned char m_flag218_08 : 1;
    unsigned char m_flag218_04 : 1;
    unsigned char unknown0218_2 : 2;
    unsigned char unknown0219[3];
    int m_gameType;
    char m_buttons[4][13];
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
    unsigned char m_value1908;
    unsigned char unknown1909[3];
    CMemoryCard* m_card;
    unsigned char unknown1910[132];
    float m_value1994;
    unsigned char m_value1998;
    unsigned char m_value1999;
    unsigned char m_value199a;
    unsigned char m_value199b;
    unsigned char unknown199c[6];
    signed char m_value19a2;
    signed char m_value19a3;
    unsigned char m_value19a4;
    unsigned char m_value19a5;
    unsigned char m_value19a6;
    unsigned char unknown19a7;
    unsigned char m_value19a8;
    unsigned char m_value19a9;
    unsigned char m_value19aa;
    unsigned char m_value19ab;
    unsigned char m_table19ac[24];
    CUserMessageTable m_messages;
};

CShellMenu::CShellMenu() {
    m_value1908 = 0;
    m_value254 = 0;
    m_value000 = 1;
    m_value001 = 1;
    m_value002 = 1;
    m_value003 = 1;
    m_value00e = 0;
    memset(&m_block014, 0, sizeof(m_block014));
    memset(&m_block018, 0, sizeof(m_block018));
    memset(&m_block01c, 0, sizeof(m_block01c));
    memset(&m_block024, 0, sizeof(m_block024));
    m_audio.Set(0, 7);
    m_audio.Set(1, 7);
    if (OSGetSoundMode() == 0)
        m_audio.Set(2, 2);
    else
        m_audio.Set(2, 0);
    memset(m_block120, 0, sizeof(m_block120));
    memset(m_block02c, 0, sizeof(m_block02c));
    m_options[0] = 0;
    m_options[1] = 0;
    m_options[2] = 1;
    m_options[3] = 0;
    m_flag218_80 = 0;
    m_flag218_40 = 0;
    m_flag218_20 = 0;
    m_flag218_10 = 0;
    SetGameType(1);
    m_gameTypeSuffix = "";
    g_ControllerConfig.SetUpDefaultConfig(1);
    m_value18ec = 0;
    pSoundStream = 0;
    m_value1994 = 0.0f;
    m_value1998 = 0;
    m_value1999 = 0;
    m_value199a = 0;
    m_value199b = 0;
    m_value19a2 = -1;
    m_value19a3 = -1;
    m_value19a4 = 0;
    m_value19a5 = 0;
    m_value19a6 = 1;
    m_value00f = 0;
    m_value010 = 0;
    m_table19ac[0] = 3;
    m_table19ac[1] = 4;
    m_table19ac[2] = 3;
    m_table19ac[3] = 2;
    m_table19ac[4] = 4;
    m_table19ac[5] = 4;
    m_table19ac[6] = 3;
    m_table19ac[7] = 0;
    m_table19ac[8] = 3;
    m_table19ac[9] = 3;
    m_table19ac[10] = 3;
    m_table19ac[11] = 0;
    m_table19ac[12] = 3;
    m_table19ac[13] = 3;
    m_table19ac[14] = 3;
    m_table19ac[15] = 0;
    m_table19ac[16] = 3;
    m_table19ac[17] = 3;
    m_table19ac[18] = 3;
    m_table19ac[19] = 3;
    m_table19ac[20] = 0;
    m_table19ac[21] = 0;
    m_table19ac[22] = 0;
    m_table19ac[23] = 0;
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
    m_value19a8 = 0;
    m_value19a9 = 0;
    m_flag218_08 = 0;
    m_flag218_04 = 0;
    memset(m_block13fc, 0, sizeof(m_block13fc));
    memset(m_block1408, 0, sizeof(m_block1408));
    InitButtons();
    m_records[0].id = -1;
    m_records[1].id = -1;
    m_records[2].id = -1;
    m_records[3].id = -1;
    ResetPlayers();
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
