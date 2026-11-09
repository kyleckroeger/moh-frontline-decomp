// bsbifunc.cpp: the script prompt and hint text built-ins, as partial source.
// The prompt helpers are inlined into each built-in in the original. Types are
// views inferred from these functions; names come from the symbols.
extern "C" {
unsigned long strlen(const char*);
char* strchr(const char*, int);
char* strncpy(char*, const char*, unsigned long);
char* strcpy(char*, const char*);
int sprintf(char*, const char*, ...);
}

class PopUpMessage {
public:
    void Set_placement(unsigned char);
    void Set_screenLocs(unsigned short, unsigned short);
    void Set_property(unsigned char);
    void Set_onScreenTime(float);
    void Set_textSize(unsigned char);
    void Set_rgba(unsigned char, unsigned char, unsigned char, unsigned char);
    void SetString(const char*);
    unsigned char unknown[0x98];
};
class PopUpMessageHandler;
void PromptUserMessage(char*, PopUpMessageHandler*, PopUpMessage*);

struct BuiltInFunction {
    unsigned char unknown00[10];
    short m_pop;
    short m_numArgs;
    unsigned char unknown0e[2];
};
extern BuiltInFunction* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

struct ScreenView {
    unsigned char unknown00[44];
    float m_scaleY;
    unsigned char unknown30[0x100];
};
extern ScreenView g_screen;
extern char g_bInMultiplayerMode;
extern PopUpMessageHandler* g_pPopUpMessageHandler[4];
extern PopUpMessage g_popUpMessage;
extern char g_promptBuffer[20][80];
extern char* g_promptCursor;
extern int g_promptstart;
extern int g_prompt;
extern int g_promptColumn;
extern int g_promptMaxSize;
extern int g_promptLineWidth;
extern char g_szHintBuffer[8][80];
extern int g_iHintBufferIndex;

static inline int Arg(int** sp, int index) {
    return (*sp)[-(g_pBuiltInFunctions[g_iCurrentBIFIndex].m_numArgs - 1 - index)];
}

static inline void PopArgs(int** sp) {
    *sp -= g_pBuiltInFunctions[g_iCurrentBIFIndex].m_pop;
}

static inline void NextPromptLine() {
    g_promptColumn = 0;
    g_prompt = (g_prompt + 1) % 20;
    g_promptCursor = g_promptBuffer[g_prompt];
}

// Appends text to the prompt, wrapping at spaces within the line width.
static inline void AppendToPrompt(const char* text) {
    int length = strlen(text);
    while (length > g_promptMaxSize - g_promptColumn) {
        const char* end = text;
        while (end - text <= g_promptLineWidth - g_promptColumn) {
            const char* space = strchr(end, ' ');
            if (!space || space + 1 - text > g_promptMaxSize - g_promptColumn)
                break;
            end = space + 1;
        }
        strncpy(g_promptCursor, text, end - text);
        g_promptCursor[end - text] = 0;
        text = end;
        NextPromptLine();
        length = strlen(end);
    }
    strcpy(g_promptCursor, text);
    g_promptCursor += length;
    g_promptColumn += length;
}

static inline void SetUpMessage(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    g_popUpMessage.Set_placement(4);
    g_popUpMessage.Set_screenLocs(320, (int)(12.0f * g_screen.m_scaleY));
    g_popUpMessage.Set_property(1);
    g_popUpMessage.Set_onScreenTime(5.0f);
    g_popUpMessage.Set_textSize(22);
    g_popUpMessage.Set_rgba(r, g, b, a);
}

// Shows the prompt lines written since the last call, newest first. A port of -1
// sends them to every player's handler; outside multiplayer it is always port 0.
void displayPromptBuffer(int port) {
    int line = g_prompt;
    while (line != g_promptstart) {
        SetUpMessage(63, 63, 63, 63);
        char* text = g_promptBuffer[line];
        g_popUpMessage.SetString(text);
        if (!g_bInMultiplayerMode)
            port = 0;
        if (port == -1) {
            for (int i = 0; i < 4; i++)
                PromptUserMessage(text, g_pPopUpMessageHandler[i], &g_popUpMessage);
        } else {
            PromptUserMessage(text, g_pPopUpMessageHandler[port], &g_popUpMessage);
        }
        line = line != 0 ? line - 1 : 19;
    }
    g_promptstart = g_prompt;
    NextPromptLine();
}

static inline void NextHintLine() {
    g_iHintBufferIndex = g_iHintBufferIndex == 7 ? 0 : g_iHintBufferIndex + 1;
}

// Single player only: splits the hint into lines and shows them, newest first.
void DisplayHintText(const char* text) {
    int length = strlen(text);
    int lines = 0;
    if (g_bInMultiplayerMode)
        return;
    while (length > g_promptLineWidth) {
        const char* end = text;
        while (end - text <= g_promptLineWidth) {
            const char* space = strchr(end, ' ');
            if (!space) {
                if (length <= g_promptMaxSize)
                    end = text + length;
                break;
            }
            if (space + 1 - text > g_promptMaxSize)
                break;
            end = space + 1;
        }
        strncpy(g_szHintBuffer[g_iHintBufferIndex], text, end - text);
        int index = g_iHintBufferIndex;
        lines++;
        NextHintLine();
        g_szHintBuffer[index][end - text] = 0;
        length = strlen(end);
        text = end;
    }
    if (length > 0) {
        strcpy(g_szHintBuffer[g_iHintBufferIndex], text);
        lines++;
        int index = g_iHintBufferIndex;
        NextHintLine();
        g_szHintBuffer[index][length] = 0;
    }
    int line = g_iHintBufferIndex != 0 ? g_iHintBufferIndex - 1 : 7;
    while (--lines >= 0) {
        SetUpMessage(60, 60, 40, 128);
        g_popUpMessage.SetString(g_szHintBuffer[line]);
        PromptUserMessage(g_szHintBuffer[line], g_pPopUpMessageHandler[0], &g_popUpMessage);
        line = line != 0 ? line - 1 : 7;
    }
}

void BIFunc_DisplayPromptInteger(int** sp, void*) {
    char text[32];
    int show = Arg(sp, 1);
    sprintf(text, "%d", Arg(sp, 0));
    AppendToPrompt(text);
    if (show != 0 && g_promptCursor != g_promptBuffer[g_prompt])
        displayPromptBuffer(-1);
    PopArgs(sp);
}

class CUserMessageTable {
public:
    char* GetText(int id) const { return id < m_count ? m_entries[id].text : 0; }
    struct Entry {
        char* text;
        int id;
    };
    Entry* m_entries;
    unsigned char unknown04[8];
    int m_count;
};
extern CUserMessageTable umTable;

void BIFunc_DisplayPromptText(int** sp, void*) {
    int show = Arg(sp, 1);
    const char* text = umTable.GetText(Arg(sp, 0));
    AppendToPrompt(text);
    if (show != 0 && g_promptCursor != g_promptBuffer[g_prompt])
        displayPromptBuffer(-1);
    PopArgs(sp);
}

class CWeapon {
public:
    void AddAmo(short);
};
class CPlayerWeaponObject {
public:
    bool IsSelectable() const;
    void DoneReloading();
    unsigned char unknown0000[0x30e4];
    CWeapon* m_weapon;
    unsigned char unknown30e8[0x31b0 - 0x30e8];
    int m_state;
    unsigned char unknown31b4[0x31d2 - 0x31b4];
    unsigned char m_flags : 2;
    unsigned char m_reloaded : 1;
    unsigned char m_flags2 : 5;
};
class CPlayerObject {
public:
    CPlayerWeaponObject* GetWeapon(int) const;
    CPlayerWeaponObject* GetCurrentWeapon() const;
    void CycleWeapon(int, int);
};
class CScene {
public:
    CPlayerObject* GetPlayer(int) const;
    unsigned char unknown[0x1000];
};
extern CScene g_scene;

// Adds ammunition for a weapon and, for one not yet selectable when asked to
// announce it, unlocks it and shows a message. Empty-handed, the player switches.
void BIFunc_AddPlayerAmmo(int** sp, void*) {
    int notify = Arg(sp, 2);
    int message = Arg(sp, 3);
    int weapon = Arg(sp, 0);
    int amount = Arg(sp, 1);
    CPlayerObject* player = g_scene.GetPlayer(0);
    CPlayerWeaponObject* object = player->GetWeapon(weapon);
    object->m_weapon->AddAmo(amount);
    if (!object->IsSelectable() && notify != 0) {
        object->m_reloaded = 1;
        object->DoneReloading();
        if (message >= 0) {
            AppendToPrompt(umTable.GetText(message));
            displayPromptBuffer(-1);
        }
    }
    if (!player->GetCurrentWeapon()) {
        object->m_state = 3;
        // A virtual function at +100 of the player's table; not identified.
        typedef void (*Handler)(CPlayerObject*, int, int, int);
        (*(Handler*)(*(char**)player + 100))(player, 162, 0, 0);
        player->CycleWeapon(1, -1);
    }
    PopArgs(sp);
}
