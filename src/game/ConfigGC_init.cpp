// A fragment of ConfigGC.cpp (0x800fad00): CDualConfig::Init creates the two
// fonts (+4, +8; the font file loaded from the level for each, grey colour
// (64, 64, 64, 64), height 12, the first also scale 1, both at the current
// screen's maximum depth) and the hot-box sprite (+0), then copies the
// current configuration's 13 button bytes into both config texts as OnEnter
// does, terminates them, initialises the button movement array and sets the
// controller setting (+52) to 1. The classes, functions and texts are named
// by the symbols; this is a non-virtual view (the font and sprite classes are
// only constructed here; the font's 40-byte and the sprite's 100-byte sizes
// come from the allocations), and the members are inferred. CColor follows
// fader.cpp, CSprite's allocator bulletfactory_sprites.cpp. The rest of the
// file is in other units.
void* DWI_alloc(const char*, int, int);
char* TLT_LoadFileNormal(const char*, int*, int);

enum ERenderPriority {};

struct CColor {
    CColor() {}
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}
    CColor& operator=(const CColor& c) {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        return *this;
    }

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CFont {
public:
    CFont(const char*, ERenderPriority);
    void SetColor(CColor);
    void SetHeight(float);
    void SetScale(float, float);
    void SetDepth(unsigned long);

    unsigned char unknown00[40];
};

class CSprite {
public:
    enum ESpriteLoadType {};

    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }
    CSprite(const char*, ESpriteLoadType, ERenderPriority);

    unsigned char unknown00[100];
};

class CScreen {
public:
    static CScreen* GetCurrent();
    unsigned long GetDepthBufferMax();
};

class CShellMenu {
public:
    void RetrieveShellControllerButtons(char*, int);

    unsigned char unknown0000[0x19dc];
};

extern CShellMenu g_Shell;
extern char currConfigText[14];
extern char visualConfigText[14];

class CDualConfig {
public:
    void Init();
    void CopyDefaultConfig(int, char*);
    void InitializeButtonMovementArray();

    CSprite* m_sprite;
    CFont* m_font;
    CFont* m_font2;
    unsigned char unknown0c[40];
    int m_controllerSetting;
    unsigned char unknown38[8];
    int m_config;
};

void CDualConfig::Init() {
    char* file = TLT_LoadFileNormal("data/objFont.gfn", 0, 0);
    m_font = new CFont(file, (ERenderPriority)62);
    m_font->SetColor(CColor(64, 64, 64, 64));
    m_font->SetHeight(12.0f);
    m_font->SetScale(1.0f, 1.0f);
    CScreen* screen = CScreen::GetCurrent();
    if (screen)
        m_font->SetDepth(screen->GetDepthBufferMax());
    char* file2 = TLT_LoadFileNormal("data/objFont.gfn", 0, 0);
    m_font2 = new CFont(file2, (ERenderPriority)62);
    m_font2->SetColor(CColor(64, 64, 64, 64));
    m_font2->SetHeight(12.0f);
    if (screen)
        m_font2->SetDepth(screen->GetDepthBufferMax());
    m_sprite = new CSprite("data\\MOHFL_exports\\hotbox.gsh", (CSprite::ESpriteLoadType)2, (ERenderPriority)52);
    char buttons[13];
    if (m_config > 5)
        g_Shell.RetrieveShellControllerButtons(buttons, m_config - 6);
    else
        CopyDefaultConfig(m_config, buttons);
    for (int i = 0; i < 13; i++) {
        currConfigText[i] = buttons[i];
        visualConfigText[i] = buttons[i];
    }
    currConfigText[13] = 0;
    visualConfigText[13] = 0;
    InitializeButtonMovementArray();
    m_controllerSetting = 1;
}
