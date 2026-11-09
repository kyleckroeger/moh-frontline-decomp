// bsbifunc.cpp: the two multiplayer built-ins, as partial source. They were left to
// the very end, because the PC port has no multiplayer.
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
static inline void displayPromptBuffer(int port) {
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
extern "C" int sprintf(char*, const char*, ...);
enum EScriptBulletType {};
int GetMPWeaponPrompt(EScriptBulletType);
struct BSObject;
class CPlayerObject {
public:
    bool AddHealth(int, int, void*);
    const char* GetPlayerName();
    unsigned char unknown000[0x768];
    int m_port;         // +0x768
    int m_playerIndex;  // +0x76c
    unsigned char unknown770[0xb40];
    int m_team;         // +0x12b0
};
class CShellMenu {
public:
    void Add_MPPlayerKilled(int, int);
};
extern CShellMenu g_Shell;
struct ScriptClassView {
    unsigned char unknown00[0x48];
    int m_crc;
};
struct ScriptDataView {
    unsigned char unknown00[4];
    ScriptClassView* m_class;
};
struct ScriptOwnerView {
    unsigned char unknown00[0xc];
    void** m_table;
};
struct BSObject {
    unsigned char unknown00[8];
    ScriptDataView* m_data;
    ScriptOwnerView* m_owner;
};
extern BSObject* g_pBSObject;
typedef void* (*GetObjectFn)(ScriptOwnerView*);
typedef CPlayerObject* (*GetPlayerObjectFn)(void*);
static inline void* Slot(void* object, int offset) { return *(void**)(*(char**)object + offset); }

// A script object's scene node (owner table +0x10), then its player (node table +0x9c).
static inline CPlayerObject* PlayerOf(BSObject* object) {
    void* node = ((GetObjectFn)object->m_owner->m_table[0x10 / 4])(object->m_owner);
    return ((GetPlayerObjectFn)Slot(node, 0x9c))(node);
}

// Adds health from a pick-up, telling the player which kind they took. The pick-up's
// class CRC chooses the message: 266 (two kinds), 267 or 268.
void BIFunc_MPAddPlayerHealth(int** sp, void*) {
    int amount = Arg(sp, 1);
    CPlayerObject* player = PlayerOf((BSObject*)Arg(sp, 0));
    bool added = player->AddHealth(g_pBSObject->m_data->m_class->m_crc, amount, 0);
    if (added) {
        int message = -1;
        switch (g_pBSObject->m_data->m_class->m_crc) {
        case 0x598e4105:
        case 0x72441aa1:
            message = 266;
            break;
        case 0x10efeba2:
            message = 267;
            break;
        case -0x8452247:
            message = 268;
            break;
        }
        if (message >= 0) {
            AppendToPrompt(umTable.GetText(message));
            displayPromptBuffer(player->m_port);
        }
    }
    PopArgs(sp);
    **sp = added;
}

// Records a kill and tells both players. An attacker of 0 means the victim itself;
// a weapon message below 0 leaves the weapon out of the text.
void BIFunc_UpdateFragCount(int** sp, void*) {
    BSObject* attackerObject = (BSObject*)Arg(sp, 0);
    EScriptBulletType bullet = (EScriptBulletType)Arg(sp, 1);
    if (g_bInMultiplayerMode) {
        if (!attackerObject)
            attackerObject = g_pBSObject;
        CPlayerObject* victim = PlayerOf(g_pBSObject);
        CPlayerObject* attacker = PlayerOf(attackerObject);
        g_Shell.Add_MPPlayerKilled(attacker->m_playerIndex, victim->m_playerIndex);
        int weapon = GetMPWeaponPrompt(bullet);
        if (attacker->m_team == victim->m_team) {
            if (attacker == victim) {
                AppendToPrompt(umTable.GetText(768)); // Suicide.
                displayPromptBuffer(attacker->m_port);
            } else {
                // A team kill.
                char text[128];
                sprintf(text, "%s%s", umTable.GetText(764), victim->GetPlayerName());
                AppendToPrompt(text);
                displayPromptBuffer(attacker->m_port);
                if (weapon < 0)
                    sprintf(text, "%s%s", umTable.GetText(766), attacker->GetPlayerName());
                else
                    sprintf(text, "%s%s's %s", umTable.GetText(766), attacker->GetPlayerName(), umTable.GetText(weapon));
                AppendToPrompt(text);
                displayPromptBuffer(victim->m_port);
            }
        } else {
            char text[128];
            sprintf(text, "%s%s", umTable.GetText(763), victim->GetPlayerName());
            AppendToPrompt(text);
            displayPromptBuffer(attacker->m_port);
            if (weapon < 0)
                sprintf(text, "%s%s", umTable.GetText(765), attacker->GetPlayerName());
            else
                sprintf(text, "%s%s's %s", umTable.GetText(765), attacker->GetPlayerName(), umTable.GetText(weapon));
            AppendToPrompt(text);
            displayPromptBuffer(victim->m_port);
        }
    }
    PopArgs(sp);
}
