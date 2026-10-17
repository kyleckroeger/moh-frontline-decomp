// A fragment of ShellMenu.cpp (0x800e3d1c): CShellMenu::CorruptFileDialog
// shows a corrupt-file message with the shell's object font (loaded from the
// level files) until the player answers: the message and up to two extra
// lines, then the two answer prompts from the message table (838/839), with
// a frame drawn and the input updated each pass. Cancel (input 128) leaves
// the file alone; confirm (127) deletes the save game (opening its card
// slot first when needed) or the preferences file, and if that fails shows
// the failure messages (840/841) until cancel is pressed. The classes,
// functions and globals are named by the mangled symbols; the members, the
// message table and screen views, the inline helpers and the unused
// parameters are inferred.
void* TLT_LoadFileNormal(const char*, int*, int);

enum ERenderPriority {};

class CFont {
public:
    CFont(const char*, ERenderPriority);
    ~CFont();
    void DrawTextCentered(float, const char*);

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

    unsigned char unknown00[64];
};

class CMemoryCard {
public:
    int GetPort();
    bool OpenSlot(int);
    bool DeleteSaveGame(unsigned int);
    bool DeletePrefFile(int);
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

extern CScreen g_screen;
extern CInputManager g_inputMgr;

class CShellMenu {
public:
    void CorruptFileDialog(char*, char, char*, int, bool, int, const char*, const char*);
    bool DeleteGame(int slot, int file) {
        if (IsSlotOpen(slot))
            return m_card->DeleteSaveGame(file);
        return false;
    }
    bool IsSlotOpen(int slot) {
        if (m_card->GetPort() != slot && !m_card->OpenSlot(slot))
            return false;
        return true;
    }

    unsigned char unknown0000[6412];
    CMemoryCard* m_card;
    unsigned char unknown1910[180];
    CUserMessageTable m_messages;
};

void CShellMenu::CorruptFileDialog(char* message, char, char*, int slot, bool game, int file, const char* line1,
                                   const char* line2) {
    float scale = g_screen.m_scaleY;
    const char* data = (const char*)TLT_LoadFileNormal("data\\objFont.gfn", 0, 0);
    CFont font(data, (ERenderPriority)111);
    float top = 50.0f * scale;
    float next;
    while (true) {
        float y = top;
        font.DrawTextCentered(y, message);
        y += 22.0f;
        if (line1) {
            font.DrawTextCentered(y, line1);
            y += 22.0f;
        }
        if (line2) {
            font.DrawTextCentered(y, line2);
            y += 22.0f;
        }
        font.DrawTextCentered(y, m_messages.GetText(838));
        y += 22.0f;
        font.DrawTextCentered(y, m_messages.GetText(839));
        WaitRenderList();
        WaitScreen();
        SendRenderList();
        g_inputMgr.Update(1.0f);
        if (g_inputMgr.GetValueAnyPlayer(128))
            return;
        if (g_inputMgr.GetValueAnyPlayer(127))
            break;
    }
    int deleted;
    if (game)
        deleted = DeleteGame(slot, file);
    else
        deleted = m_card->DeletePrefFile(slot);
    if (!deleted) {
        next = top;
        next += 22.0f;
        do {
            font.DrawTextCentered(top, m_messages.GetText(840));
            font.DrawTextCentered(next, m_messages.GetText(841));
            WaitRenderList();
            WaitScreen();
            SendRenderList();
            g_inputMgr.Update(1.0f);
        } while (!g_inputMgr.GetValueAnyPlayer(128));
        return;
    }
}
