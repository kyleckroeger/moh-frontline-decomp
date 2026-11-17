// The end of bsbifunc.cpp (0x80034be4): the static initialisation of the
// script built-ins' data. Six target/corpse/buddy/multiplayer check schedules,
// the sound schedule and the pop-up message are built through their
// constructors with their destructors registered, and the screen flash (a
// fade control from (128, 128, 128, 255) to (128, 128, 128, 0); its duration
// is an entry of the file's .sdata2 pool) is built through CFaderControl's
// constructor and its own table. The file's .bss block and .sbss data are
// defined here; the initialised .sdata variables between them are not part of
// this unit. The globals, BSSchedule, SoundSchedule, PopUpMessage,
// CFaderControl and CScreenFlash are named by the symbols; object sizes come
// from the symbols, element types from the other units' views, and the
// opaque objects' contents and the colour constructor are inferred.
// CScreenFlash has no virtual function of its own, so its table is weak (a
// weak duplicate) and its type information is local to the file; the unit is
// built with RTTI on so those records are emitted here.
#include "game/CColor.h"


class CFaderControl {
public:
    CFaderControl(CColor, CColor, float);
    virtual void Update(float);

    CColor m_from;
    CColor m_to;
    float m_duration;
    float m_time;
};

class CScreenFlash : public CFaderControl {
public:
    CScreenFlash() : CFaderControl(CColor(128, 128, 128, 255), CColor(128, 128, 128, 0), 10.0f) {}
};

class BSSchedule {
public:
    BSSchedule();
    ~BSSchedule();

    unsigned char unknown00[28];
};

class SoundSchedule {
public:
    SoundSchedule();
    ~SoundSchedule();

    unsigned char unknown00[8];
};

class PopUpMessage {
public:
    PopUpMessage();
    ~PopUpMessage();

    unsigned char unknown00[76];
};

class PopUpMessageHandler;
struct BSBuiltinView;
struct MPWeaponSetView;

BSBuiltinView* g_pBuiltInFunctions;
int g_iCurrentBIFIndex;
int g_pNumBuiltInFunctions; /* type not known */
static BSSchedule g_DistanceToTargetSchedule;
static BSSchedule g_TargetSearchSchedule;
static BSSchedule g_TargetCheckSchedule;
static BSSchedule g_CorpseSearchSchedule;
static BSSchedule g_BuddyPlayerCheckSchedule;
static BSSchedule g_MMGPointCheckSchedule;
SoundSchedule g_SoundSchedule;
PopUpMessageHandler* g_pPopUpMessageHandler[4];
static char g_szHintBuffer[640];
static int g_iHintBufferIndex;
static char g_promptBuffer[20][80];
static int g_prompt;
static int g_promptColumn;
static PopUpMessage g_popUpMessage;
unsigned char g_pGenericSoundMemory[80];
CScreenFlash g_screenflash;
const char* g_HintTextArray[20][3];
int g_iHighestObjectiveNumber;
static MPWeaponSetView* g_pMPWeaponSet;
