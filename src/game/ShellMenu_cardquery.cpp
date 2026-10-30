// A fragment of ShellMenu.cpp (0x800e4738): CShellMenu::MemoryCardLoadGame
// loads a saved game: with the slot open and the file opened it reads the
// shell's save block (from the start of the object to +6368, the CRC at +6364)
// into a new buffer, closing the card (and failing) when the read or close
// fails; it then resets the game type to single player, checks the block's CRC
// and on a match copies the block over the shell - keeping the current sound
// mode byte (+42) when it or the loaded one is 2 - and resets the audio
// (returns 1); a CRC failure is reported (-1). The game-type switch, the audio reset and the slot
// check are inferred inlines shared with the shell's other units; the 'self'
// local is inferred. Then the memory card queries: the corrupt file's name
// (copied from the card at +82240) and the files-free, any-device, maximum
// blocks, encoding and 8K-sector checks of a port, passed to the card at
// +6412. CShellMenu and CMemoryCard are named by the mangled symbols; the
// members are inferred from offsets. The rest of the file is in other units.
extern "C" char* strcpy(char*, const char*);
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

class CSoundSysLock {
public:
    CSoundSysLock();
    ~CSoundSysLock();
};

class CSoundStream {
public:
    void SetVolume(int);
};

enum ESoundMode {};

void SoundSetVolumes(float, float);
void SoundSetMode(ESoundMode);
extern "C" void OSSetSoundMode(unsigned long);
extern "C" void* memcpy(void*, const void*, unsigned long);
void DebugMsg(const char*, ...);

class CMemoryCard {
public:
    int GetPort();
    bool OpenSlot(int);
    bool Open(unsigned int, bool);
    bool Read(unsigned char*, unsigned int);
    bool Close();
    int GetFilesFree(int);
    int IsAnyDeviceInPort(int);
    int GetMaxBlocksOnCard(int);
    int IsEncodingCorrect(int);
    int Is8KCard(int);

    unsigned char unknown00000[82240];
    char m_corruptFileName[32];
};

struct AudioOptionsView {
    signed char m_settings[3];

    signed char Get(int index) { return this == 0 ? -1 : m_settings[index]; }
    void Store(int index, const unsigned char& value) {
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
    int MemoryCardLoadGame(int, unsigned int);
    void MemoryCardGetCorruptFileName(char*, int);
    int MemoryCardNumFilesFree(int);
    int MemoryCardIsAnyObjectInSlot(int);
    int MemoryCardGetMaxBlocks(int);
    int MemoryCardIsEncodingCorrect(int);
    int MemoryCardIs8KSectorSize(int);
    bool CheckDataCRC(unsigned char*, int, unsigned long) const;
    bool IsSlotOpen(int slot) {
        if (m_card->GetPort() != slot && !m_card->OpenSlot(slot))
            return false;
        return true;
    }
    void ResetAudio() {
        int current[3];
        int settings[3];

        current[0] = m_audio.Get(0);
        current[1] = m_audio.Get(1);
        current[2] = m_audio.Get(2);
        memcpy(settings, current, sizeof(current));
        m_audio.Store(0, settings[0]);
        m_audio.Store(1, settings[1]);
        m_audio.Store(2, settings[2]);
        float effects = (m_audio.Get(0) - 1) / 6.0f;
        float music = (m_audio.Get(1) - 1) / 6.0f;
        SoundSetVolumes(effects, music);
        {
            CSoundSysLock lock;
            if (pSoundStream)
                pSoundStream->SetVolume(127.0f * (m_audio.Get(1) - 1) / 6.0f);
        }
        switch (m_audio.Get(2)) {
        case 0:
            SoundSetMode((ESoundMode)1);
            OSSetSoundMode(1);
            break;
        case 1:
            SoundSetMode((ESoundMode)2);
            OSSetSoundMode(1);
            break;
        case 2:
            SoundSetMode((ESoundMode)0);
            OSSetSoundMode(0);
            break;
        }
    }

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
    unsigned char unknown18cc[16];
    unsigned long m_crc;
    unsigned char m_saveEnd[4];
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


int CShellMenu::MemoryCardLoadGame(int slot, unsigned int file) {
    CShellMenu* const self = this;
    if (!IsSlotOpen(slot))
        return 0;
    if (!m_card->Open(file, false))
        return 0;
    int size = (char*)m_saveEnd - (char*)self;
    int crcOffset = (char*)&m_crc - (char*)self;
    unsigned char* data = new unsigned char[size];
    if (!m_card->Read(data, size)) {
        m_card->Close();
        delete[] data;
        return 0;
    }
    if (!m_card->Close()) {
        delete[] data;
        return 0;
    }
    SetGameType(1);
    m_gameTypeSuffix = "";
    unsigned long crc = 0;
    memcpy(&crc, data + crcOffset, sizeof(crc));
    if (CheckDataCRC(data, size - 4, crc)) {
        unsigned char soundMode = ((unsigned char*)m_audio.m_settings)[2];
        memcpy(self, data, size);
        if (((unsigned char*)m_audio.m_settings)[2] == 2 || soundMode == 2)
            ((unsigned char*)m_audio.m_settings)[2] = soundMode;
        delete[] data;
        ResetAudio();
        return 1;
    }
    DebugMsg("CRC check on load game failed: %x\n", crc);
    delete[] data;
    return -1;
}

void CShellMenu::MemoryCardGetCorruptFileName(char* name, int) {
    strcpy(name, m_card->m_corruptFileName);
}

int CShellMenu::MemoryCardNumFilesFree(int port) {
    return m_card->GetFilesFree(port);
}

int CShellMenu::MemoryCardIsAnyObjectInSlot(int port) {
    return m_card->IsAnyDeviceInPort(port);
}

int CShellMenu::MemoryCardGetMaxBlocks(int port) {
    return m_card->GetMaxBlocksOnCard(port);
}

int CShellMenu::MemoryCardIsEncodingCorrect(int port) {
    return m_card->IsEncodingCorrect(port);
}

int CShellMenu::MemoryCardIs8KSectorSize(int port) {
    return m_card->Is8KCard(port);
}
