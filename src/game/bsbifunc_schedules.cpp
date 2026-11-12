// A fragment of bsbifunc.cpp (0x800343b4): the per-frame update of the script
// built-ins' schedules (the distance-to-target, target search and target check
// schedules, the sound schedule with one step, then the corpse search, buddy
// player check and machine-gun point check schedules) and their shutdown at the
// end of a level (the same schedules shut down, then the four pop-up message
// handlers deleted). Both return 0. The file name is this project's; the original
// record is bsbifunc.cpp, whose other built-ins are reconstructed in the other
// bsbifunc_*.cpp fragments. The schedules sit in the file's .bss block and are
// reached from its base register, so the block is defined here with size views and
// pooled; the bsbifunc.cpp unit places it. The schedule, sound schedule and pop-up
// handler classes are named by the symbols; their sizes come from the original and
// their members are inferred.
class BSObject;
typedef bool (*ScheduleFn)(BSObject*, int, int, int, int*, int*);
class BSSchedule {
public:
    void Init(ScheduleFn, int, int);
    void Update();
    void Shutdown();
    void Reset(ScheduleFn, int, int);
    unsigned char unknown[28];
};
class SoundSchedule {
public:
    void Init();
    void Update(int);
    void Shutdown();
    unsigned char unknown[8];
};
class PopUpMessageHandler {
public:
    PopUpMessageHandler(char*, char*);
    ~PopUpMessageHandler();
    unsigned char unknown[0x60c];
};
extern SoundSchedule g_SoundSchedule;

struct HintText {
    int a;
    int b;
    int c;
};

/* inferred: the file's .bss block (0x80197470, 2,936 bytes) is reached from one
   base register, so it is defined here in original order with size views and
   pooled; each 12-byte entry is the destructor-chain record the compiler keeps
   for the static object after it. No constructors, so no initialiser is compiled. */
struct DtorChainView {
    unsigned char data[12];
};
/* view: sizes from the original */
struct PopUpMessageView {
    unsigned char data[0x4c];
};
struct ScreenFlashView {
    unsigned char data[0x14];
};
static DtorChainView s_chain1000;
BSSchedule g_DistanceToTargetSchedule;
static DtorChainView s_chain1001;
BSSchedule g_TargetSearchSchedule;
static DtorChainView s_chain1002;
BSSchedule g_TargetCheckSchedule;
static DtorChainView s_chain1003;
BSSchedule g_CorpseSearchSchedule;
static DtorChainView s_chain1004;
BSSchedule g_BuddyPlayerCheckSchedule;
static DtorChainView s_chain1005;
BSSchedule g_MMGPointCheckSchedule;
static DtorChainView s_chain1006;
PopUpMessageHandler* g_pPopUpMessageHandler[4];
static char g_szHintBuffer[0x280];
static char g_promptBuffer[0x640];
static DtorChainView s_chain1007;
static PopUpMessageView g_popUpMessage;
unsigned long g_pGenericSoundMemory[20];
ScreenFlashView g_screenflash;
HintText g_HintTextArray[20];

int BSUpdateBIFuncs() {
    g_DistanceToTargetSchedule.Update();
    g_TargetSearchSchedule.Update();
    g_TargetCheckSchedule.Update();
    g_SoundSchedule.Update(1);
    g_CorpseSearchSchedule.Update();
    g_BuddyPlayerCheckSchedule.Update();
    g_MMGPointCheckSchedule.Update();
    return 0;
}

int BSCleanUpBIFuncs() {
    g_DistanceToTargetSchedule.Shutdown();
    g_TargetSearchSchedule.Shutdown();
    g_TargetCheckSchedule.Shutdown();
    g_SoundSchedule.Shutdown();
    g_CorpseSearchSchedule.Shutdown();
    g_BuddyPlayerCheckSchedule.Shutdown();
    g_MMGPointCheckSchedule.Shutdown();
    for (int i = 0; i < 4; i++) {
        if (g_pPopUpMessageHandler[i])
            delete g_pPopUpMessageHandler[i];
    }
    return 0;
}

