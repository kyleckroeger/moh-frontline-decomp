// bsbifunc.cpp: per-level setup, update and shutdown of the script built-ins, as
// partial source. Types are views inferred from these functions.
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

int BSBifuncInvalid(int**, void*);
LevelFileContentsStruct_* TLT_FindNextResourceByTypeAndVal(TLTResourceID, int, int, LevelFileContentsStruct_*);

struct HintText {
    int a;
    int b;
    int c;
};
extern int g_iHighestObjectiveNumber;
extern unsigned long g_pGenericSoundMemory[20];
extern HintText g_HintTextArray[20];
extern int g_iHintBufferIndex;

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
static inline void ChangeEndian(short& value) {
    value = (short)((((unsigned short)value) >> 8) | (value << 8));
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
    g_iHighestObjectiveNumber = 0;
    g_pGenericSoundMemory[19] = 0xaeaeaeae;
    for (int i = 0; i < 20; i++) {
        g_HintTextArray[i].c = 0;
        g_HintTextArray[i].a = 0;
        g_HintTextArray[i].b = 0;
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
    int m_entries; // an offset in the file, then a pointer
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
    file->m_entries = (int)file + file->m_entries;
    g_pBuiltInFunctions = (BIFEntry*)file->m_entries;
    g_pNumBuiltInFunctions = file->m_count;

    int i;
    for (i = 0; i < file->m_count; i++) {
        BIFEntry* entry = &g_pBuiltInFunctions[i];
        ChangeEndian(entry->m_crc);
        ChangeEndian(entry->m_field08);
        ChangeEndian(entry->m_field0a);
        ChangeEndian(entry->m_field0c);
        g_pBuiltInFunctions[i].m_function = BSBifuncInvalid;
    }
    for (i = 0; i < file->m_count; i++) {
        for (int j = 0; j < 288; j++) {
            if (g_BIFCRCArray[j].m_crc == g_pBuiltInFunctions[i].m_crc) {
                g_pBuiltInFunctions[i].m_function = g_BIFCRCArray[j].m_function;
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
    g_iHighestObjectiveNumber = 0;
    g_pGenericSoundMemory[19] = 0xaeaeaeae;
    for (int h = 0; h < 20; h++) {
        g_HintTextArray[h].c = 0;
        g_HintTextArray[h].a = 0;
        g_HintTextArray[h].b = 0;
    }
    g_iHintBufferIndex = 0;
    if (g_bInMultiplayerMode) {
        g_pMPWeaponSet = TLT_FindNextResourceByTypeAndVal((TLTResourceID)22, 0, g_levelWeaponSetID, 0);
        if (g_NumPlayers < 3) {
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
