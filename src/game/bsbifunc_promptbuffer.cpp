// A fragment of bsbifunc.cpp (0x8002aecc): displayPromptBuffer, which shows
// the prompt lines written since the last call, newest first: each line is set
// up as a grey pop-up message (placement 4, centred 12 scaled lines down, five
// seconds, text size 22) and sent to every player's pop-up handler for port -1,
// otherwise to that port's handler (always port 0 outside multiplayer); then
// the shown position catches up and the prompt moves to its next line. The file
// name is this project's; the original record is bsbifunc.cpp. The prompt
// buffer, the handlers and the pop-up message belong to the file's .bss block,
// reached from its base register, so the block is defined here in original
// order with size views and pooled; the bsbifunc.cpp unit places it. The
// pop-up message's setters, the handler and the screen are named by the
// symbols; the screen view (vertical scale at +44) is inferred.
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
extern char* g_promptCursor;
extern int g_promptstart;
extern int g_prompt;
extern int g_promptColumn;

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

static inline void NextPromptLine() {
    g_prompt = (g_prompt + 1) % 20;
    g_promptCursor = g_promptBuffer[g_prompt];
    g_promptColumn = 0;
}

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
