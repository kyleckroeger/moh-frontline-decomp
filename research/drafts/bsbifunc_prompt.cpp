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

static inline void NextHintLine() {
    g_iHintBufferIndex = g_iHintBufferIndex == 7 ? 0 : g_iHintBufferIndex + 1;
}

// Landed in src/game/bsbifunc_promptbuffer.cpp; kept here because the prompt
// built-ins below inline it.
void displayPromptBuffer(int port) {
    char* text;
    int line = g_prompt;
    while (line != g_promptstart) {
        SetUpMessage(63, 63, 63, 63);
        text = g_promptBuffer[line];
        g_popUpMessage.SetString(text);
        if (!g_bInMultiplayerMode)
            port = 0;
        if (port == -1) {
            for (int i = 0; i < 4; i++)
                PromptUserMessage(g_promptBuffer[line], g_pPopUpMessageHandler[i], &g_popUpMessage);
        } else {
            PromptUserMessage(g_promptBuffer[line], g_pPopUpMessageHandler[port], &g_popUpMessage);
        }
        line = line != 0 ? line - 1 : 19;
    }
    g_promptstart = g_prompt;
    NextPromptLine();
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
