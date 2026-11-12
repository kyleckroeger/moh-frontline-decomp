// A fragment of bsbifunc.cpp (0x8002ab34): DisplayHintText, which (outside
// multiplayer only) splits a hint into lines of the hint buffer's eight
// 80-character rows, word-wrapping at spaces within the prompt line width (a
// last word that does not fit ends the line when the rest fits the maximum
// size), then shows the new lines newest first as pop-up messages (placement 4,
// centred 12 scaled lines down, five seconds, text size 22, colour
// 60/60/40/128) on the first player's handler. The file name is this project's;
// the original record is bsbifunc.cpp. The hint buffer, the handlers and the
// pop-up message belong to the file's .bss block, reached from its base
// register, so the block is defined here in original order with size views and
// pooled; the bsbifunc.cpp unit places it. The pop-up message's setters, the
// handler and the screen are named by the symbols; the screen view (vertical
// scale at +44) is inferred.
extern "C" {
unsigned long strlen(const char*);
char* strchr(const char*, int);
char* strncpy(char*, const char*, unsigned long);
char* strcpy(char*, const char*);
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
    unsigned char unknown[0x4c];
};
class PopUpMessageHandler;
void PromptUserMessage(char*, PopUpMessageHandler*, PopUpMessage*);

/* inferred: the screen's vertical scale */
struct ScreenView {
    unsigned char unknown00[44];
    float m_scaleY;
    unsigned char unknown30[0x100];
};
extern ScreenView g_screen;
extern bool g_bInMultiplayerMode;
extern int g_promptMaxSize;
extern int g_promptLineWidth;
extern int g_iHintBufferIndex;

/* inferred: the file's .bss block (0x80197470, 2,936 bytes), in original order
   with size views; each 12-byte entry is the destructor-chain record the
   compiler keeps for the static object after it. No constructors, so no
   initialiser is compiled. */
struct DtorChainView {
    unsigned char data[12];
};
struct BSScheduleView {
    unsigned char data[28];
};
struct ScreenFlashView {
    unsigned char data[0x14];
};
struct HintTextView {
    unsigned char data[12];
};
static DtorChainView s_chain1000;
BSScheduleView g_DistanceToTargetSchedule;
static DtorChainView s_chain1001;
BSScheduleView g_TargetSearchSchedule;
static DtorChainView s_chain1002;
BSScheduleView g_TargetCheckSchedule;
static DtorChainView s_chain1003;
BSScheduleView g_CorpseSearchSchedule;
static DtorChainView s_chain1004;
BSScheduleView g_BuddyPlayerCheckSchedule;
static DtorChainView s_chain1005;
BSScheduleView g_MMGPointCheckSchedule;
static DtorChainView s_chain1006;
PopUpMessageHandler* g_pPopUpMessageHandler[4];
static char g_szHintBuffer[8][80];
static char g_promptBuffer[20][80];
static DtorChainView s_chain1007;
static PopUpMessage g_popUpMessage;
unsigned long g_pGenericSoundMemory[20];
ScreenFlashView g_screenflash;
HintTextView g_HintTextArray[20];

static inline void SetUpMessage(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    g_popUpMessage.Set_placement(4);
    g_popUpMessage.Set_screenLocs(320, (unsigned short)(12.0f * g_screen.m_scaleY));
    g_popUpMessage.Set_property(1);
    g_popUpMessage.Set_onScreenTime(5.0f);
    g_popUpMessage.Set_textSize(22);
    g_popUpMessage.Set_rgba(r, g, b, a);
}

static inline void NextHintLine() {
    g_iHintBufferIndex = g_iHintBufferIndex == 7 ? 0 : g_iHintBufferIndex + 1;
}

void DisplayHintText(const char* text) {
    int length = strlen(text);
    int lines = 0;
    if (g_bInMultiplayerMode)
        return;
    while (length > g_promptLineWidth) {
        const char* end = text;
        while (end - text <= g_promptLineWidth) {
            const char* space = strchr(end, ' ');
            if (space) {
                if (space + 1 - text > g_promptMaxSize)
                    break;
                end = space + 1;
            } else {
                if (length <= g_promptMaxSize)
                    end = text + length;
                break;
            }
        }
        int n = end - text;
        strncpy(g_szHintBuffer[g_iHintBufferIndex], text, n);
        lines++;
        text = end;
        g_szHintBuffer[g_iHintBufferIndex][n] = 0;
        NextHintLine();
        length = strlen(text);
    }
    if (length > 0) {
        strcpy(g_szHintBuffer[g_iHintBufferIndex], text);
        g_szHintBuffer[g_iHintBufferIndex][length] = 0;
        NextHintLine();
        lines++;
    }
    int line = g_iHintBufferIndex != 0 ? g_iHintBufferIndex - 1 : 7;
    for (int i = lines - 1; i >= 0; i--) {
        SetUpMessage(60, 60, 40, 128);
        g_popUpMessage.SetString(g_szHintBuffer[line]);
        PromptUserMessage(g_szHintBuffer[line], g_pPopUpMessageHandler[0], &g_popUpMessage);
        line = line != 0 ? line - 1 : 7;
    }
}
