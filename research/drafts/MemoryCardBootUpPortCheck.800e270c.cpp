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
class CScreenScale {
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
extern CScreenScale g_screen;
extern CInputManager g_inputMgr;
extern "C" {
int sprintf(char*, const char*, ...);
void GXDrawDone();
void VISetBlack(int);
void VIFlush();
void VIWaitForRetrace();
void OSResetSystem(int, unsigned long, int);
}
void TLT_CloseFile(void*);
class CMemoryCard {
public:
    int GetPort();
    bool OpenSlot(int);
    bool Format();
    int GetSaveGameOverheadSize(bool);
};
bool MemoryCardSlotOkForStartup(CMemoryCard*, int, int, int, int*, bool*, int*, bool*, bool*, bool*, bool*, int*, bool*, bool*);
class CShellMenu {
public:
    bool MemoryCardBootUpPortCheck(int);
    inline void ShowFormatting(CFont&, int);
    bool IsSlotOpen(int slot) {
        if (slot != m_card->GetPort() && !m_card->OpenSlot(slot))
            return false;
        return true;
    }
    bool FormatCard(int slot) {
        if (IsSlotOpen(slot))
            return m_card->Format();
        return false;
    }
    unsigned char unknown0000[6412];
    CMemoryCard* m_card;
    unsigned char unknown1910[180];
    CUserMessageTable m_messages;
};
class CScreen {
public:
    static CScreen* GetCurrent();
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual int GetHeight();
};

inline void CShellMenu::ShowFormatting(CFont& font, int slot) {
    float line = 50.0f * g_screen.m_scaleY;
    char working[128];
    int i = 0;
    do {
        i++;
        // g2 (11-13): the caller's letter expression; the original computes it once for both (1183 -> 1023).
        char letter = 'A' + (slot != 0);
        sprintf(working, m_messages.GetText(842), (int)letter);
        font.DrawTextCentered(line, working);
        font.DrawTextCentered(line + 22.0f, m_messages.GetText(843));
        font.DrawTextCentered(line + 22.0f + 22.0f, m_messages.GetText(844));
        MohDVDErrorTask(0, 0);
        WaitRenderList();
        WaitScreen();
        SendRenderList();
        g_inputMgr.Update(1.0f);
    } while (i != 60);
}

bool CShellMenu::MemoryCardBootUpPortCheck(int slot) {
    int step;
    bool done;
    const char* data;
    int mode;
    int line;
    const char* erase;
    bool result;
    int a0;
    int blocks;
    int status;
    bool f1, f2, f3, f4;
    bool formatted;
    bool f5, f6;
    char letter = 'A' + (slot != 0);
    formatted = false;
    float scale = g_screen.m_scaleY;
    result = true;
    data = (const char*)TLT_LoadFileNormal("data\\objFont.gfn", 0, 0);
    CFont font(data, (ERenderPriority)111);
    font.SetColor(CColor(95, 95, 95, 128));
    g_inputMgr.Update(1.0f);
    float spacing = 22.0f;
    step = (int)spacing;
    float top = 50.0f * scale;
    do {
        char text[260];
        char blockText[256];
        char fileText[256];
        char format1[80];
        char format2[80];
        char format3[80];
        int overhead = m_card->GetSaveGameOverheadSize(false);
        int overhead2 = m_card->GetSaveGameOverheadSize(false);
        bool ok = MemoryCardSlotOkForStartup(m_card, slot, overhead2, overhead, &a0, &f1, &blocks, &f2, &f3, &f4, &formatted, &status, &f5, &f6);
        if (formatted)
            break;
        if (!ok || !f4) {
            float y = top;
            done = false;
            CScreen::GetCurrent()->GetHeight();
            MohDVDErrorTask(0, 0);
            mode = 0;
            if (!f5) {
                sprintf(text, m_messages.GetText(798), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(799));
                y += 22.0f;
                sprintf(text, m_messages.GetText(800), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(801));
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(802));
            } else if (status != 0) {
                if (status == 3) {
                    sprintf(text, m_messages.GetText(845), (int)letter);
                    font.DrawTextCentered(y, text);
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(846));
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(847));
                } else {
                    sprintf(text, m_messages.GetText(803), (int)letter);
                    font.DrawTextCentered(y, text);
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(804));
                    y += 44.0f;
                    font.DrawTextCentered(y, m_messages.GetText(805));
                    y += 22.0f;
                    sprintf(text, m_messages.GetText(806), (int)letter);
                    font.DrawTextCentered(y, text);
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(807));
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(808));
                    y += 22.0f;
                    sprintf(text, m_messages.GetText(809), (int)letter);
                    font.DrawTextCentered(y, text);
                    y += 22.0f;
                    erase = "will erase all previously saved data.";
                    y += 22.0f;
                    sprintf(format3, "Formatting the Memory Card in Slot %c", (int)letter);
                    font.DrawTextCentered(y, format3);
                    y += 22.0f;
                    font.DrawTextCentered(y, erase);
                    mode = 2;
                }
            } else if (!f3) {
                sprintf(text, m_messages.GetText(812), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(813));
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(814));
                y += 22.0f;
                sprintf(text, m_messages.GetText(815), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(816));
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(817));
            } else if (!f2) {
                sprintf(text, m_messages.GetText(820), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(821));
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(822));
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(823));
                y += 22.0f;
                sprintf(text, m_messages.GetText(824), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                sprintf(text, m_messages.GetText(825), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                sprintf(text, m_messages.GetText(826), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                erase = "will erase all previously saved data.";
                y += 22.0f;
                sprintf(format1, "Formatting the Memory Card in Slot %c", (int)letter);
                font.DrawTextCentered(y, format1);
                y += 22.0f;
                font.DrawTextCentered(y, erase);
                mode = 2;
            } else if (f1) {
                sprintf(text, m_messages.GetText(791), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(792));
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(793));
                y += 22.0f;
                sprintf(text, m_messages.GetText(794), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                font.DrawTextCentered(y, m_messages.GetText(795));
                y += 22.0f;
                sprintf(text, m_messages.GetText(796), (int)letter);
                font.DrawTextCentered(y, text);
                y += 22.0f;
                erase = "will erase all previously saved data.";
                y += 22.0f;
                sprintf(format2, "Formatting the Memory Card in Slot %c", (int)letter);
                font.DrawTextCentered(y, format2);
                y += 22.0f;
                font.DrawTextCentered(y, erase);
                mode = 2;
            } else {
                if (!f4) {
                    result = false;
                    break;
                }
                bool generic = false;
                if (blocks > 0) {
                    if (f6) {
                        line = (int)top;
                        font.DrawTextCentered((float)line, "Insufficient free files to save a new file on the");
                        sprintf(fileText, "Memory Card in Slot %c. ", (int)letter);
                        line += step;
                        font.DrawTextCentered((float)line, fileText);
                        line += step;
                        font.DrawTextCentered((float)line, "This game requires at least 2 files and 4 blocks to");
                        line += step;
                        font.DrawTextCentered((float)line, "save, 1 file and 2 blocks are needed for the Preference");
                        line += step;
                        font.DrawTextCentered((float)line, "file and 1 file and 2 blocks for each Save Game file.");
                        line += step;
                        font.DrawTextCentered((float)line, "Existing files may be loaded and overwritten.");
                        line += step;
                        font.DrawTextCentered((float)line, "Press the B Button to manage the Memory Card");
                        line += step;
                        font.DrawTextCentered((float)line, "or press the A Button to continue.");
                        line += step;
                        y = (float)line;
                    } else {
                        sprintf(text, m_messages.GetText(788), (int)letter);
                        font.DrawTextCentered(y, text);
                        generic = true;
                    }
                } else if (f6) {
                    line = (int)top;
                    font.DrawTextCentered((float)line, "Insufficient free blocks to save a new file on the");
                    sprintf(blockText, "Memory Card in Slot %c. ", (int)letter);
                    line += step;
                    font.DrawTextCentered((float)line, blockText);
                    line += step;
                    font.DrawTextCentered((float)line, "This game requires at least 2 files and 4 blocks to");
                    line += step;
                    font.DrawTextCentered((float)line, "save, 1 file and 2 blocks are needed for the Preference");
                    line += step;
                    font.DrawTextCentered((float)line, "file and 1 file and 2 blocks for each Save Game file.");
                    line += step;
                    font.DrawTextCentered((float)line, "Existing files may be loaded and overwritten.");
                    line += step;
                    font.DrawTextCentered((float)line, "Press the B Button to manage the Memory Card");
                    line += step;
                    font.DrawTextCentered((float)line, "or press the A Button to continue.");
                    line += step;
                    y = (float)line;
                } else {
                    sprintf(text, m_messages.GetText(789), (int)letter);
                    font.DrawTextCentered(y, text);
                    generic = true;
                }
                if (generic) {
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(778));
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(779));
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(780));
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(781));
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(782));
                    y += 44.0f;
                    font.DrawTextCentered(y, m_messages.GetText(784));
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(785));
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(786));
                    y += 22.0f;
                    font.DrawTextCentered(y, m_messages.GetText(787));
                }
                mode = 1;
            }
            WaitRenderList();
            WaitScreen();
            SendRenderList();
            g_inputMgr.Update(1.0f);
            if (g_inputMgr.GetValueAnyPlayer(128))
                done = true;
            if (mode == 1) {
                if (g_inputMgr.GetValueAnyPlayer(127)) {
                    GXDrawDone();
                    VISetBlack(1);
                    VIFlush();
                    VIWaitForRetrace();
                    OSResetSystem(1, 1, 1);
                }
            } else if (mode == 2 && g_inputMgr.GetValueAnyPlayer(127)) {
                ShowFormatting(font, slot);
                if (!FormatCard(slot)) {
                    char failed[80];
                    sprintf(text, m_messages.GetText(836), (int)letter);
                    float failY = 50.0f * g_screen.m_scaleY;
                    do {
                        font.DrawTextCentered(failY, m_messages.GetText(835));
                        float next = failY + 22.0f;
                        sprintf(failed, m_messages.GetText(836), (int)letter);
                        font.DrawTextCentered(next, failed);
                        next += 44.0f;
                        font.DrawTextCentered(next, m_messages.GetText(832));
                        font.DrawTextCentered(next + 22.0f, m_messages.GetText(833));
                        WaitRenderList();
                        WaitScreen();
                        SendRenderList();
                        g_inputMgr.Update(1.0f);
                    } while (!g_inputMgr.GetValueAnyPlayer(128));
                    done = true;
                }
            }
        } else {
            done = true;
        }
    } while (!done);
    WaitRenderList();
    WaitScreen();
    SendRenderList();
    TLT_CloseFile((void*)data);
    return result;
}
