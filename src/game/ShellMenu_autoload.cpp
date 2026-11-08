// ShellMenu.cpp: CShellMenu::MemoryCardAutoLoad. Class layouts are inferred views of what this function
// uses. ShellMenu_block.h recreates the constant block its offsets depend on.
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
    float GetHeight();
    void DrawTextCenteredF(float, const char*, ...);
    unsigned char unknown00[40];
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
extern "C" {
int sprintf(char*, const char*, ...);
int strncmp(const char*, const char*, unsigned long);
}
void DebugMsg(const char*, ...);
void TLT_CloseFile(void*);

class CMemoryCard {
public:
    int GetPort();
    bool OpenSlot(int);
    int IsFormated();
    bool ArePrefsPresent();
    bool LoadPrefs(unsigned char*, unsigned int);
    bool IsSaveGamePresent(int);

    unsigned char unknown00000[82240];
    char m_corruptFileName[32];
};

class CShellMenu {
public:
    int MemoryCardAutoLoad(int);
    int MemoryCardLoadGame(int, unsigned int);
    void CorruptFileDialog(char*, char, char*, int, bool, int, const char*, const char*);
    bool IsSlotOpen(int slot) {
        if (slot != m_card->GetPort() && !m_card->OpenSlot(slot))
            return false;
        return true;
    }
    int IsFormated(int slot) {
        if (IsSlotOpen(slot))
            return m_card->IsFormated();
        return 0;
    }
    bool ArePrefsPresent(int slot) {
        if (IsSlotOpen(slot))
            return m_card->ArePrefsPresent();
        return false;
    }
    bool LoadPrefsNow() {
        struct {
            unsigned char file;
            char tag[11];
        } prefs;
        if (!m_card->LoadPrefs((unsigned char*)&prefs, 12)) {
            DebugMsg("m_MemoryCard->LoadPrefs() return false\n");
            return false;
        }
        if (strncmp(prefs.tag, "Frontline", 9) != 0) {
            DebugMsg("Prefs file is corrupted since I can't find the frontline signature\n");
            return false;
        }
        m_prefsFile = prefs.file;
        return true;
    }
    bool LoadPrefs(int slot) {
        if (!IsSlotOpen(slot))
            return false;
        return LoadPrefsNow();
    }

    unsigned char unknown0000[6412];
    CMemoryCard* m_card;
    unsigned char m_prefsFile;
    unsigned char unknown1911[179];
    CUserMessageTable m_messages;
};

int CShellMenu::MemoryCardAutoLoad(int slot) {
    if (!IsSlotOpen(slot))
        return 0;
    if (!IsFormated(slot))
        return 0;
    if (ArePrefsPresent(slot)) {
        if (!LoadPrefs(slot)) {
            char message[128];
            char name[128];
            char letter = 'A';
            sprintf(name, "%sprefs", "MOHFL");
            if (slot == 1)
                letter = 'B';
            sprintf(message, m_messages.GetText(829), letter);
            CorruptFileDialog(message, letter, name, slot, false, 0, 0, 0);
            return 0;
        }
    } else {
        return 0;
    }
    DebugMsg("Memory Card Prefs point to slot %d for auto-load\n", m_prefsFile);
    if (!m_card->IsSaveGamePresent(m_prefsFile)) {
        DebugMsg("Memory Card Prefs auto-load game is not present!\n");
        return 0;
    }
    if (MemoryCardLoadGame(slot, m_prefsFile) < 1) {
        char text[128];
        char name[128];
        char letter = 'A';
        if (slot == 1)
            letter = 'B';
        sprintf(text, m_messages.GetText(831), (int)letter);
        sprintf(name, "%sx%02d", "MOHFL", m_prefsFile);
        const char* corrupt = m_card->m_corruptFileName;
        CorruptFileDialog((char*)m_messages.GetText(830), letter, name, slot, true, m_prefsFile, corrupt, text);
        return 0;
    }
    const char* data = (const char*)TLT_LoadFileNormal("data\\objFont.gfn", 0, 0);
    CFont font(data, (ERenderPriority)111);
    font.SetColor(CColor(63, 63, 63, 128));
    for (int i = 0; i < 60; i++) {
        font.GetHeight();
        MohDVDErrorTask(0, 0);
        const char* first = m_messages.GetText(770);
        const char* second = m_messages.GetText(771);
        font.DrawTextCentered(60.0f, first);
        font.DrawTextCenteredF(90.0f, "%s%s", second, slot == 0 ? "A" : "B");
        WaitRenderList();
        WaitScreen();
        SendRenderList();
    }
    TLT_CloseFile((void*)data);
    return 1;
}
