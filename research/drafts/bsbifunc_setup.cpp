// bsbifunc.cpp: per-level setup, update and shutdown of the script built-ins, as
// partial source. Types are views inferred from these functions.
class BSObject;
typedef bool (*ScheduleFn)(BSObject*, int, int, int, int*, int*);
class BSSchedule {
public:
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
    ~PopUpMessageHandler();
};
extern BSSchedule g_DistanceToTargetSchedule;
extern BSSchedule g_TargetSearchSchedule;
extern BSSchedule g_TargetCheckSchedule;
extern BSSchedule g_CorpseSearchSchedule;
extern BSSchedule g_BuddyPlayerCheckSchedule;
extern BSSchedule g_MMGPointCheckSchedule;
extern SoundSchedule g_SoundSchedule;
extern PopUpMessageHandler* g_pPopUpMessageHandler[4];

bool DoDistanceToTarget(BSObject*, int, int, int, int*, int*);
bool DoTargetSearch(BSObject*, int, int, int, int*, int*);
bool DoTargetCheck(BSObject*, int, int, int, int*, int*);
bool DoCorpseSearch(BSObject*, int, int, int, int*, int*);
bool DoBuddyPlayerCheck(BSObject*, int, int, int, int*, int*);
bool DoMMGPointCheck(BSObject*, int, int, int, int*, int*);

enum TLTResourceID {};
struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    char** m_data;
};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

struct HintText {
    int a;
    int b;
    int c;
};
extern int g_iHighestObjectiveNumber;
extern unsigned long g_pGenericSoundMemory[20];
extern HintText g_HintTextArray[20];
extern int g_iHintBufferIndex;

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

static inline unsigned long Swap32(unsigned long x) {
    return (x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | (x << 24);
}

int BSResetBIFuncs() {
    TLT_FindNextResourceByType((TLTResourceID)11, 0);
    LevelFileContentsStruct_* resource = TLT_FindNextResourceByType((TLTResourceID)1, 0);
    unsigned long* file = (unsigned long*)TLT_LoadFileFromLevelBigFile(resource->m_data[1], 0);
    file[1] = Swap32(file[1]);
    file[2] = Swap32(file[2]);
    file[3] = Swap32(file[3]);
    file[4] = Swap32(file[4]);
    g_SoundSchedule.Init();
    g_DistanceToTargetSchedule.Reset(DoDistanceToTarget, 5, 1);
    g_TargetSearchSchedule.Reset(DoTargetSearch, 5, 1);
    g_TargetCheckSchedule.Reset(DoTargetCheck, 5, 7);
    g_CorpseSearchSchedule.Reset(DoCorpseSearch, 5, 29);
    g_BuddyPlayerCheckSchedule.Reset(DoBuddyPlayerCheck, 1, 120);
    g_MMGPointCheckSchedule.Reset(DoMMGPointCheck, 5, 7);
    g_iHighestObjectiveNumber = 0;
    g_pGenericSoundMemory[0] = 0xaeaeaeae;
    for (int i = 0; i < 20; i++) {
        g_HintTextArray[i].c = 0;
        g_HintTextArray[i].a = 0;
        g_HintTextArray[i].b = 0;
    }
    g_iHintBufferIndex = 0;
    return 0;
}
