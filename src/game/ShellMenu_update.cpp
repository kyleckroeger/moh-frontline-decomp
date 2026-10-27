// A fragment of ShellMenu.cpp (0x800e7274): CShellMenu::ShellUpdate. It
// clears the no-controller flag (+6567), updates the interface studio (+6384)
// while it is active (+6408) and then the controller configuration screen
// when that is active, polls the sound stream request under a sound-system
// lock and queues g_soundStreamFilename again when the request has finished
// (state 3), and, when a controller is missing and the shell is not paused
// (+6555), prints "No Controller Inserted" centred on the screen and draws the
// shell sprite (+6612) centred, from a function-static quad whose corners
// are set on the first call. CShellMenu, IStudio, CDualConfig, CSoundStream,
// CSoundSysLock, CScreen, CSprite, CFont and the functions are named by the
// mangled symbols; the members, the status records, the CDualConfig flag and
// the sprite texture size bit-fields (as in sprite_draw.cpp) are inferred.
// The first GetWidth result is unused in the original too. The rest of the
// file is not part of this unit.
class CSoundSysLock {
public:
    CSoundSysLock();
    ~CSoundSysLock();
};

/* inferred: the stream and request status records */
struct SSndStreamStatus {
    unsigned char unknown00[12];
};

struct SSndStrmRequestStatus {
    int m_state;
    unsigned char unknown04[12];
};

class CSoundStream {
public:
    int QueueFile(const char*, int, int);
    void GetStatus(SSndStreamStatus&) const;
    void GetRequestStatus(int, SSndStrmRequestStatus&) const;

    unsigned char unknown00[32];
};

extern CSoundStream* pSoundStream;
extern int g_soundStreamRequest;
extern char g_soundStreamFilename[];

class IStudio {
public:
    void iSirenUpdate(unsigned long);
};

class CDualConfig {
public:
    void RunNewConfigRendererer();

    unsigned char unknown00[13];
    bool m_active;
    unsigned char unknown0e[54];
};

extern CDualConfig g_ControllerConfig;

class CScreen {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual int GetHeight();
    virtual int GetWidth();
    static CScreen* GetCurrent();
};

struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};

class CTexture {
public:
    unsigned char unknown00[60];
    int m_width : 10;
    int m_height : 10;
    int m_flags : 12;
};

class CSprite {
public:
    enum EPolyPrimType {};
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);

    unsigned char unknown00[36];
    CTexture m_texture;
};

class CFont {
public:
    void DrawTextCentered(float, const char*);
};

class CShellMenu {
public:
    void ShellUpdate(unsigned long);

    unsigned char unknown0000[6384];
    IStudio m_studio;
    unsigned char unknown18f1[23];
    bool m_studioActive;
    unsigned char unknown1909[146];
    bool m_paused;
    unsigned char unknown199c[11];
    bool m_noController;
    unsigned char unknown19a8[44];
    CSprite* m_sprite;
    CFont* m_font;
};

void CShellMenu::ShellUpdate(unsigned long time) {
    m_noController = false;
    if (m_studioActive) {
        m_studio.iSirenUpdate(time);
        if (g_ControllerConfig.m_active)
            g_ControllerConfig.RunNewConfigRendererer();
    }
    if (g_soundStreamRequest != -1) {
        SSndStreamStatus status;
        SSndStrmRequestStatus request;
        {
            CSoundSysLock lock;
            pSoundStream->GetStatus(status);
            pSoundStream->GetRequestStatus(g_soundStreamRequest, request);
        }
        if (request.m_state == 3)
            g_soundStreamRequest = pSoundStream->QueueFile(g_soundStreamFilename, 50, 0);
    }
    if (m_noController && !m_paused) {
        CScreen* screen = CScreen::GetCurrent();
        screen->GetWidth(); /* the original makes this call and drops the result */
        m_font->DrawTextCentered((float)screen->GetHeight() / 2.0f, "No Controller Inserted\n");
        float bottom;
        float right;
        float halfHeight;
        float halfWidth;
        float top;
        float left;
        float x = screen->GetWidth() / 2;
        float y = screen->GetHeight() / 2;
        halfHeight = 0.5f * m_sprite->m_texture.m_height;
        halfWidth = 0.5f * m_sprite->m_texture.m_width;
        left = x - halfWidth;
        top = y - halfHeight;
        right = x + halfWidth;
        bottom = y + halfHeight;
        static _spritepolyvert verts[4] = {
            { 0.0f, 0.0f, left, top, 0.0f, 0x80808080 },
            { 1.0f, 0.0f, right, top, 0.0f, 0x80808080 },
            { 1.0f, 1.0f, right, bottom, 0.0f, 0x80808080 },
            { 0.0f, 1.0f, left, bottom, 0.0f, 0x80808080 },
        };
        m_sprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
    }
}
