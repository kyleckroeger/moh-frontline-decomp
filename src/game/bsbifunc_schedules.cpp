// A fragment of bsbifunc.cpp (0x800343b4): the per-frame update of the script
// built-ins' schedules (the distance-to-target, target search and target check
// schedules, the sound schedule with one step, then the corpse search, buddy
// player check and machine-gun point check schedules) and their shutdown at the
// end of a level (the same schedules shut down, then the four pop-up message
// handlers deleted), and their reset for a new level (the level's built-in
// table file header byte-swapped, the sound schedule initialised, the six
// schedules reset with their functions, periods and offsets, the highest
// objective number cleared, the generic sound memory's last word marked
// 0xaeaeaeae and the 20 hint texts and the hint buffer index cleared), then
// BSInitBIFuncs (0x80034718), which loads the level's built-in table, checks its
// version and checksum, binds each entry to the engine's built-in by CRC, and
// initialises the schedules, the four pop-up message handlers, the hint texts
// and the multiplayer prompt sizes. Each returns 0. The file name is this project's; the original
// record is bsbifunc.cpp, whose other built-ins are reconstructed in the other
// bsbifunc_*.cpp fragments. The schedules sit in the file's .bss block and are
// reached from its base register, so the block is defined here with size views and
// pooled; the bsbifunc.cpp unit places it. The schedule, sound schedule and pop-up
// handler classes are named by the symbols; their sizes come from the original and
// their members are inferred.
void DebugMsg(const char*, ...);
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

bool DoDistanceToTarget(BSObject*, int, int, int, int*, int*);
bool DoTargetSearch(BSObject*, int, int, int, int*, int*);
bool DoTargetCheck(BSObject*, int, int, int, int*, int*);
bool DoCorpseSearch(BSObject*, int, int, int, int*, int*);
bool DoBuddyPlayerCheck(BSObject*, int, int, int, int*, int*);
bool DoMMGPointCheck(BSObject*, int, int, int, int*, int*);

enum TLTResourceID {};
/* inferred: the level resource's file names (the built-in table file second) */
struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    char** m_data;
};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);
int BSBifuncInvalid(int**, void*);
LevelFileContentsStruct_* TLT_FindNextResourceByTypeAndVal(TLTResourceID, int, int, LevelFileContentsStruct_*);

struct HintText {
    int a;
    int b;
    int c;
};

extern int g_iHighestObjectiveNumber;
extern int g_iHintBufferIndex;

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

static inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}
static inline void ChangeEndian(unsigned int& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}
template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<unsigned int*>(&value));
}
static inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

int BSResetBIFuncs() {
    TLT_FindNextResourceByType((TLTResourceID)11, 0);
    LevelFileContentsStruct_* resource = TLT_FindNextResourceByType((TLTResourceID)1, 0);
    int* file = (int*)TLT_LoadFileFromLevelBigFile(resource->m_data[1], 0);
    ChangeEndian(file[1]);
    ChangeEndian(file[2]);
    ChangeEndian(file[3]);
    ChangeEndian(file[4]);
    g_SoundSchedule.Init();
    g_DistanceToTargetSchedule.Reset(DoDistanceToTarget, 5, 1);
    g_TargetSearchSchedule.Reset(DoTargetSearch, 5, 1);
    g_TargetCheckSchedule.Reset(DoTargetCheck, 5, 7);
    g_CorpseSearchSchedule.Reset(DoCorpseSearch, 5, 29);
    g_BuddyPlayerCheckSchedule.Reset(DoBuddyPlayerCheck, 1, 120);
    g_MMGPointCheckSchedule.Reset(DoMMGPointCheck, 5, 7);
    g_pGenericSoundMemory[19] = 0xaeaeaeae;
    g_iHighestObjectiveNumber = 0;
    HintText* h = g_HintTextArray;
    for (int i = 0; i < 20; i++, h++) {
        h->c = 0;
        h->a = 0;
        h->b = 0;
    }
    g_iHintBufferIndex = 0;
    return 0;
}

// The built-in function table is loaded from the level's "bif" file, whose fields
// are little endian. Each entry's CRC names one of the engine's built-ins.
struct BIFEntry {
    int (*m_function)(int**, void*);
    int m_crc;
    short m_field08;
    short m_field0a;
    short m_field0c;
    short m_field0e;
};
struct BIFFile {
    unsigned char unknown00[4];
    int m_version;
    int m_checksum;
    int m_count;
    BIFEntry* m_entries; // an offset in the file, then a pointer
};
struct BIFCRCEntry {
    int m_crc;
    int (*m_function)(int**, void*);
};
extern BIFCRCEntry g_BIFCRCArray[288];
extern BIFEntry* g_pBuiltInFunctions;
extern int g_pNumBuiltInFunctions;
extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;
extern int g_levelWeaponSetID;
extern int g_promptLineWidth;
extern int g_promptMaxSize;
extern LevelFileContentsStruct_* g_pMPWeaponSet;

static inline void SwapEntry(BIFEntry& entry) {
    ChangeEndian(entry.m_crc);
    ChangeEndian(entry.m_field08);
    ChangeEndian(entry.m_field0a);
    ChangeEndian(entry.m_field0c);
}

// bsbifunc_block.h recreates the head of the file's constant block, which
// BSInitBIFuncs reaches through a base register. It is included just before
// BSInitBIFuncs: any earlier position swaps the .rodata and .bss base registers.
#include "bsbifunc_block.h"

int BSInitBIFuncs() {
    TLT_FindNextResourceByType((TLTResourceID)11, 0);
    LevelFileContentsStruct_* resource = TLT_FindNextResourceByType((TLTResourceID)1, 0);
    BIFFile* file = (BIFFile*)TLT_LoadFileFromLevelBigFile(resource->m_data[1], 0);
    ChangeEndian(file->m_version);
    ChangeEndian(file->m_checksum);
    ChangeEndian(file->m_count);
    ChangeEndian(file->m_entries);
    if (file->m_version != 29) {
        DebugMsg("Version error on bif data file\n");
        DebugMsg("  engine version = %d.  file version = %d\n", 29, file->m_version);
        DebugMsg("  The script compiler and/or the engine needs to be updated\n");
    }
    if (file->m_checksum != 0x3352b) {
        DebugMsg("Checksum error on bif data file\n");
        DebugMsg("  engine checksum = %d.  file checksum = %d\n", 0x3352b, file->m_checksum);
        DebugMsg("  Scripts need to be rebuilt or the engine needs to be built with a new bsstruct.h\n");
    }
    file->m_entries = (BIFEntry*)((int)file + (int)file->m_entries);
    g_pBuiltInFunctions = file->m_entries;
    g_pNumBuiltInFunctions = file->m_count;

    for (int i = 0; i < file->m_count; i++) {
        SwapEntry(g_pBuiltInFunctions[i]);
        g_pBuiltInFunctions[i].m_function = BSBifuncInvalid;
    }
    for (int i2 = 0; i2 < file->m_count; i2++) {
        for (int j = 0; j < 288; j++) {
            if (g_BIFCRCArray[j].m_crc == g_pBuiltInFunctions[i2].m_crc) {
                g_pBuiltInFunctions[i2].m_function = g_BIFCRCArray[j].m_function;
                break;
            }
        }
    }

    g_SoundSchedule.Init();
    g_DistanceToTargetSchedule.Init(DoDistanceToTarget, 5, 1);
    g_TargetSearchSchedule.Init(DoTargetSearch, 5, 1);
    g_TargetCheckSchedule.Init(DoTargetCheck, 5, 7);
    g_CorpseSearchSchedule.Init(DoCorpseSearch, 5, 29);
    g_BuddyPlayerCheckSchedule.Init(DoBuddyPlayerCheck, 1, 120);
    g_MMGPointCheckSchedule.Init(DoMMGPointCheck, 5, 7);
    for (int k = 0; k < 4; k++)
        g_pPopUpMessageHandler[k] = new PopUpMessageHandler("data/chars.bum", "data/binum.bum");
    g_pGenericSoundMemory[19] = 0xaeaeaeae;
    g_iHighestObjectiveNumber = 0;
    HintText* h = g_HintTextArray;
    for (int n = 0; n < 20; n++, h++) {
        h->c = 0;
        h->a = 0;
        h->b = 0;
    }
    g_iHintBufferIndex = 0;
    if (g_bInMultiplayerMode) {
        g_pMPWeaponSet = TLT_FindNextResourceByTypeAndVal((TLTResourceID)22, 0, g_levelWeaponSetID, 0);
        if (g_NumPlayers <= 2) {
            g_promptLineWidth = 36;
            g_promptMaxSize = 48;
        } else {
            g_promptLineWidth = 24;
            g_promptMaxSize = 24;
        }
    } else {
        g_pMPWeaponSet = 0;
    }
    return 0;
}
