// UserInterface.cpp: loads the HUD's sprites and fonts, as partial source. The
// multiplayer branches stay in, since the same function serves both modes.
extern "C" int sprintf(char*, const char*, ...);
inline void* operator new(unsigned long, void* p) { return p; }
void* DWI_alloc(const char*, int, int);

enum ERenderPriority {};
class CSprite {
public:
    enum ESpriteLoadType {};
    CSprite(const char*, ESpriteLoadType, ERenderPriority);
};
class CColor {
public:
    CColor(unsigned int rgba) : m_rgba(rgba) {}
    CColor(const CColor& other) : m_rgba(other.m_rgba) {}
    unsigned int m_rgba;
};
class CFont {
public:
    CFont(const char*, ERenderPriority);
    void SetColor(CColor);
    void SetScale(float, float);
    void SetDepth(unsigned long);
    unsigned char unknown[0x28];
};
class CScreen {
public:
    static CScreen* GetCurrent();
    static unsigned long GetDepthBufferMax();
};
enum TLTResourceID {};
struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    char** m_data;
};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
const char* TLT_LoadFileFromLevelBigFile(const char*, int*);
extern bool g_bInMultiplayerMode;

class UserInterface {
public:
    void Init();
    static CSprite* circleSprite;
    static CSprite* compassSprite;
    static CSprite* compassEdgeSprite;
    static CSprite* healthSprite;
    static CSprite* hitMeterSprite;
    static CSprite* dotSprite;
    static CSprite* greySprite;
    static CSprite* starSprite;
    static CSprite* headshotImages[20];
    static CSprite* blackSprite;
    static CSprite* ammoSprite;
    static CFont* bulletText;
    static CFont* smallMPFont;
    unsigned char unknown000[0x7a8];
    float m_hitFade[4];       // +0x7a8
    unsigned char unknown7b8[0x1c];
    float m_field7d4;
    float m_field7d8;
    float m_field7dc;
    float m_field7e0;
    unsigned char unknown7e4[0x14];
    float m_field7f8;
    float m_field7fc;
    bool m_field800;
    unsigned char unknown801[0x13];
    bool m_field814;
    unsigned char unknown815[0xb];
    float m_field820;
};

static inline void LoadSprite(CSprite*& sprite, const char* name, int priority) {
    if (!sprite)
        sprite = new (DWI_alloc(0, 100, 0x400)) CSprite(name, (CSprite::ESpriteLoadType)1, (ERenderPriority)priority);
}

void UserInterface::Init() {
    if (g_bInMultiplayerMode)
        LoadSprite(circleSprite, "data\\blackCircle.gsh", 0x6f);
    LoadSprite(compassSprite, "data\\compassDial.gsh", 0x6f);
    LoadSprite(compassEdgeSprite, "data\\compassEdge.gsh", 0x6f);
    LoadSprite(healthSprite, "data\\healthMeter.gsh", 0x6f);
    LoadSprite(hitMeterSprite, "data\\hitMeter.gsh", 0x6f);
    if (g_bInMultiplayerMode) {
        LoadSprite(dotSprite, "data\\dot.gsh", 0x6f);
        LoadSprite(greySprite, "data\\grey.gsh", 0x6f);
        LoadSprite(starSprite, "data\\star.gsh", 0x6f);
        char path[20] = "";
        for (int i = 0; i < 20; i++) {
            if (!headshotImages[i]) {
                sprintf(path, "bitmaps\\Model%02d.gsh", i + 1);
                headshotImages[i] = new (DWI_alloc(0, 100, 0x400)) CSprite(path, (CSprite::ESpriteLoadType)1, (ERenderPriority)0x6f);
            }
        }
    }
    LoadSprite(blackSprite, "data\\blackSprite.gsh", 10);
    ammoSprite = 0;

    LevelFileContentsStruct_* fonts = TLT_FindNextResourceByType((TLTResourceID)11, 0);
    const char* fontData = TLT_LoadFileFromLevelBigFile(fonts->m_data[3], 0);
    if (!bulletText)
        bulletText = new CFont(fontData, (ERenderPriority)0x6f);
    CColor grey(0x40404040);
    bulletText->SetColor(grey);
    bulletText->SetScale(1.0f, 1.0f);
    if (CScreen::GetCurrent())
        bulletText->SetDepth(CScreen::GetDepthBufferMax());
    if (g_bInMultiplayerMode && !smallMPFont) {
        fontData = TLT_LoadFileFromLevelBigFile(fonts->m_data[7], 0);
        smallMPFont = new CFont(fontData, (ERenderPriority)0x6f);
        CColor white(0x80808080);
        smallMPFont->SetColor(white);
    }

    m_field7dc = 4.5f;
    m_field7d4 = 0.0f;
    m_field7e0 = 4.5f;
    m_field7d8 = 0.0f;
    m_hitFade[0] = 1.0f;
    m_hitFade[1] = 1.0f;
    m_hitFade[2] = 1.0f;
    m_hitFade[3] = 1.0f;
    m_field7fc = 1.0f;
    m_field7f8 = 1.0f;
    m_field800 = false;
    if (g_bInMultiplayerMode)
        m_field820 = 1.0f;
    LevelFileContentsStruct_* level = TLT_FindNextResourceByType((TLTResourceID)13, 0);
    if (level && *(int*)((char*)level->m_data + 0x1d4))
        m_field814 = true;
}
