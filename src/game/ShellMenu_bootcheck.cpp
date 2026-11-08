#include "ShellMenu_block.h"

void* TLT_LoadFileNormal(const char*, int*, int);
enum ERenderPriority {};
struct CColor {
    unsigned char r, g, b, a;
    CColor(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_) {
        r = r_; g = g_; b = b_; a = a_;
    }
};
class CFont {
public:
    CFont(const char*, ERenderPriority);
    ~CFont();
    void DrawTextCentered(float, const char*);
    void SetColor(CColor);
    unsigned char unknown00[36];
};
class CScreen {
public:
    unsigned char unknown00[40];
    float m_scaleX;
    float m_scaleY;
};
class CInputManager {
public:
    void Update(float);
    bool GetValueAnyPlayer(unsigned long);

    unsigned char unknown[0x2500];
};
struct UserMessageView {
    const char* text;
    unsigned char unknown04[4];
};
class CUserMessageTable {
public:
    const char* GetText(int id) const {
        if (id < m_count)
            return m_entries[id].text;
        return 0;
    }
    UserMessageView* m_entries;
    unsigned char unknown04[8];
    int m_count;
};
void WaitRenderList();
void WaitScreen();
void SendRenderList();
void MohDVDErrorTask(int, int);
extern CScreen g_screen;
extern CInputManager g_inputMgr;
class CMemoryCard {
public:
    bool QuerySlot(int);
};
class CShellMenu {
public:
    void MemoryCardBootUpCheck();
    bool MemoryCardBootUpPortCheck(int);
    unsigned char unknown0000[6412];
    CMemoryCard* m_card;
    unsigned char unknown1910[180];
    CUserMessageTable m_messages;
};

void CShellMenu::MemoryCardBootUpCheck() {
    bool done0;
    bool done1;
    bool ok0;
    bool ok1;
    int i;
    const char* data = (const char*)TLT_LoadFileNormal("data\\objFont.gfn", 0, 0);
    CFont font(data, (ERenderPriority)111);
    font.SetColor(CColor(95, 95, 95, 128));
    float scale = g_screen.m_scaleY;
    g_inputMgr.Update(1.0f);
    const char* text = m_messages.GetText(769);
    float y = 50.0f * scale;
    for (i = 0; i < 60; i++) {
        MohDVDErrorTask(0, 0);
        font.DrawTextCentered(y, text);
        WaitRenderList();
        WaitScreen();
        SendRenderList();
    }
    done0 = false;
    done1 = false;
    do {
        ok0 = true;
        if (!done0 && !m_card->QuerySlot(0))
            ok0 = false;
        ok1 = true;
        if (!done1 && !m_card->QuerySlot(1))
            ok1 = false;
        if (!ok0 && !ok1) {
            float line = y;
            font.DrawTextCentered(line, "No Memory Card detected.");
            line += 22.0f;
            font.DrawTextCentered(line, "Please insert a Memory Card or press the");
            line += 22.0f;
            font.DrawTextCentered(line, "A Button to continue without saving.");
            g_inputMgr.Update(1.0f);
            if (g_inputMgr.GetValueAnyPlayer(128)) {
                done0 = true;
                done1 = true;
            }
        } else if (ok0 && !done0) {
            done0 = MemoryCardBootUpPortCheck(0);
        } else if (ok1 && !done1) {
            done1 = MemoryCardBootUpPortCheck(1);
        } else {
            done0 = true;
            done1 = true;
        }
        MohDVDErrorTask(0, 0);
        WaitRenderList();
        WaitScreen();
        SendRenderList();
    } while (!done0 || !done1);
}
