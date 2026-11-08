extern "C" {
int sprintf(char*, const char*, ...);
char* strcpy(char*, const char*);
void* memset(void*, int, unsigned long);
}
void* TLT_LoadFileNormal(const char*, int*, int);
void* DWI_alloc(const char*, int, int);
void UpdateREAL();
void WaitRenderList();
void WaitScreen();
void SendRenderList();
void LoadShellBigFile();
void LoadShellSoundModules();
enum ERenderPriority {};
struct CColor {
    unsigned char r, g, b, a;
    CColor(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_) {
        r = r_; g = g_; b = b_; a = a_;
    }
};
class LoadElf {
public:
    static LoadElf* Get();
    unsigned char unknown0000[12288];
    int m_state;
    int m_mission;
    int m_stage;
};
struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};
struct CRenderBinData {
    unsigned char unknown00[28];
};
class CSprite : public CRenderBinData {
public:
    enum ESpriteLoadType {};
    enum EPolyPrimType {};
    CSprite(const char*, ESpriteLoadType, ERenderPriority);
    virtual ~CSprite();
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }
    static void operator delete(void*);
    unsigned char unknown20[68];
};
class CFont {
public:
    CFont(const char*, ERenderPriority);
    ~CFont();
    void SetColor(CColor);
    void SetScale(float, float);
    void SetDepth(unsigned long);
    unsigned char unknown00[40];
};
class CScreen {
public:
    unsigned long GetDepthBufferMax();
    unsigned char unknown00[0x100];
};
class CInputManager {
public:
    void Update(float);
    unsigned char unknown[0x2500];
};
class CMemoryCard {
public:
    CMemoryCard();
    void Initialize();
    unsigned char unknown[0x14160];
};
class CConfigFile {
public:
    CConfigFile(const char*);
    ~CConfigFile();
    bool SetSection(const char*);
    bool GetBool(const char*, bool);
    bool GetString(const char*, char*, int);
    unsigned char unknown[788];
};
class CUserMessageTable {
public:
    void Load(char*, char*);
    unsigned char unknown[16];
};
class IStudio {
public:
    void ISirenStartup(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
    void iSirenLoadScreen(unsigned short, unsigned short);
    void iSirenUpdate(unsigned long);
    void iSirenActivateScreen(unsigned short, unsigned short);
};
struct StarLevelsInBits;
void SetStarRatingStatus(StarLevelsInBits*, unsigned int, unsigned int);
class CDualConfig {
public:
    void Init();
    unsigned char unknown[60];
    CUserMessageTable* m_messages;
};
extern CInputManager g_inputMgr;
extern CScreen g_screen;
extern char g_soundStreamFilename[];
extern const char* g_ConfigFilename;
extern CDualConfig g_ControllerConfig;

class CShellMenu {
public:
    void ShellInit(unsigned long);
    void MoveStatsIntoProperAreaForIstudio();
    void MemoryCardBootUpCheck();
    bool MemoryCardAutoLoad(int);
    void GetLevelUnlockedStatus(int*);
    void GetNextLevel();

    unsigned char unknown0000[4];
    unsigned int m_mission;
    unsigned int m_stage;
    unsigned char unknown000c[3];
    unsigned char m_complete;
    unsigned char unknown0010[12];
    StarLevelsInBits* starsDummy;
    unsigned char unknown0020[0x1410 - 0x20];
    int m_statsReady;
    unsigned char unknown1414[0x143c - 0x1414];
    int m_volume[4];
    unsigned char unknown144c[0x14cc - 0x144c];
    unsigned char m_mpStats[0x400];
    unsigned char unknown18cc[0x18ec - 0x18cc];
    bool m_returning;
    unsigned char unknown18ed[3];
    IStudio m_studio;
    unsigned char unknown18f1[0x190c - 0x18f1];
    CMemoryCard* m_card;
    unsigned char unknown1910[0x19a2 - 0x1910];
    signed char m_a2;
    signed char m_a3;
    unsigned char m_a4;
    unsigned char unknown19a5;
    bool m_promptsOn;
    unsigned char unknown19a7[0x19c4 - 0x19a7];
    CUserMessageTable m_messages;
    CSprite* m_sprite;
    CFont* m_font;
};

static inline const char* StartScreen(char* name) {
    switch (LoadElf::Get()->m_state) {
    case 2:
        return "data/victory.gsh";
    case 1:
    case 3:
        return "data/return.gsh";
    case 0:
    default:
        return "data/start.gsh";
    case 4: {
        int stage = LoadElf::Get()->m_stage;
        sprintf(name, "data/vict%d_%d.gsh", LoadElf::Get()->m_mission, stage);
        return name;
    }
    }
}

static inline bool IsZero(int d) { return d == 0; }

void CShellMenu::ShellInit(unsigned long time) {
    char name[28];
    const char* file;
    int i;
    CSprite* sprite;
    file = StartScreen(name);
    sprite = new CSprite(file, (CSprite::ESpriteLoadType)1, (ERenderPriority)52);
    _spritepolyvert verts[4] = {
        {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0xffffffff},
        {1.0f, 0.0f, 640.0f, 0.0f, 0.0f, 0xffffffff},
        {1.0f, 1.0f, 640.0f, 448.0f, 0.0f, 0xffffffff},
        {0.0f, 1.0f, 0.0f, 448.0f, 0.0f, 0xffffffff},
    };
    for (i = 0; i < 2; i++) {
        sprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
        UpdateREAL();
        WaitRenderList();
        WaitScreen();
        SendRenderList();
        g_inputMgr.Update(1.0f);
    }
    delete sprite;
    m_a2 = -1;
    m_a3 = -1;
    m_a4 = 0;
    m_card = new CMemoryCard;
    g_soundStreamFilename[0] = 0;
    CConfigFile config(g_ConfigFilename);
    if (config.SetSection("shell menu")) {
        config.GetString("filename", g_soundStreamFilename, 256);
        m_promptsOn = config.GetBool("bPromptsOn", true);
    } else {
        strcpy(g_soundStreamFilename, "data\\shell\\Shell1.asf");
    }
    LoadShellBigFile();
    m_studio.ISirenStartup(50, 8, 30, 200, 16);
    unsigned long screen = 0;
    if (m_returning) {
        m_messages.Load("data\\chars.bum", "data\\binum.bum");
        if (!m_statsReady) {
            screen = m_complete ? 14 : 15;
            if (m_complete) {
                unsigned char last[7];
                last[0] = 0;
                unsigned long value = 14;
                last[1] = 4;
                last[2] = 3;
                last[3] = 3;
                last[4] = 3;
                last[5] = 4;
                last[6] = 2;
                bool isLast = IsZero(last[m_mission] - m_stage);
                if (isLast)
                    value = 35;
                screen = value;
                if (m_mission == 6 && m_stage == 1) {
                    screen = 15;
                    SetStarRatingStatus((StarLevelsInBits*)((char*)this + 28), m_mission * 10 + m_stage, 3);
                }
            }
        } else {
            screen = 42;
            MoveStatsIntoProperAreaForIstudio();
        }
    }
    LoadShellSoundModules();
    m_card->Initialize();
    if (!m_returning) {
        m_messages.Load("data\\chars.bum", "data\\binum.bum");
        MemoryCardBootUpCheck();
        if (!MemoryCardAutoLoad(0))
            MemoryCardAutoLoad(1);
    }
    m_studio.iSirenLoadScreen(0, screen);
    m_studio.iSirenUpdate(time);
    m_studio.iSirenActivateScreen(0, screen);
    sprite = new CSprite("unplugged.gsh", (CSprite::ESpriteLoadType)2, (ERenderPriority)102);
    m_sprite = sprite;
    const char* fontData = (const char*)TLT_LoadFileNormal("data/objFont.gfn", 0, 0);
    m_font = new CFont(fontData, (ERenderPriority)111);
    m_font->SetColor(CColor(63, 63, 63, 128));
    m_font->SetScale(1.0f, 1.0f);
    m_font->SetDepth(g_screen.GetDepthBufferMax());
    int unlocked[24];
    memset(unlocked, 0, sizeof(unlocked));
    GetLevelUnlockedStatus(unlocked);
    if (m_returning && m_complete)
        GetNextLevel();
    g_ControllerConfig.m_messages = &m_messages;
    g_ControllerConfig.Init();
    m_statsReady = 0;
    if (!m_returning) {
        m_volume[0] = 100;
        m_volume[1] = 100;
        m_volume[2] = 100;
        m_volume[3] = 100;
    }
    memset(m_mpStats, 0, 0x400);
    *(int*)(m_mpStats + 0xf8) = -1;
    *(int*)(m_mpStats + 0x1f8) = -1;
    *(int*)(m_mpStats + 0x2f8) = -1;
    *(int*)(m_mpStats + 0x3f8) = -1;
}
