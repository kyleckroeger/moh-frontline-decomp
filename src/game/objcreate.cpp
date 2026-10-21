// Script game-object creation: InitObjectCreators (carves the allocators'
// memory out of one block - its size from CreateObjectGetMemoryRequirements,
// inlined - and resets the creation lists, counters, the soldier creation
// queue, the delayed-destroy list, the dump radius and container),
// CreateObjectGetMemoryRequirements (the memory the file's allocators need),
// the dump container's destructor and the accessors of the script
// game-object classes that follow it. The file is built with deferred
// inlining, which emits the functions in reverse order, so they are defined
// here in reverse address order. The class names come from the mangled
// symbols; the members are inferred from offsets and the accessors' result
// types are left incomplete. The accessors are inline in the original (weak
// symbols), so they are defined __declspec(weak). The unit ends with the
// file's static initialisation: the two object allocators and the volume,
// animated-volume and environment-mechanism allocators (their destructors
// registered; the array through a generated array destructor) and the dump
// container (700 bytes allocated and cleared, the rest of its words zero).
// The allocator and container members, the container's constructor and the
// queue and delayed-destroy entry layouts are inferred; the file's .bss
// block is defined here, its .sbss variables are declared extern (the
// file-local ones marked). The creators before them are not part of this
// unit.
extern "C" void* memset(void*, int, unsigned long);
class ISceneNode;
struct ProximityDataView;
struct ScriptDataView;
struct HitReactionInfoView;

class CDumpObjectContainer {
public:
    CDumpObjectContainer() {
        m_objects = new char[700];
        memset(m_objects, 0, 700);
        data08 = 0;
        data04 = 0;
        data0c = 0;
    }
    ~CDumpObjectContainer();
    void Clear() {
        memset(m_objects, 0, 700);
        data08 = 0;
        data04 = 0;
        data0c = 0;
    }

    char* m_objects;
    int data04;
    int data08;
    int data0c;
};

class BSUtilObjectInstanceMemoryAllocator {
public:
    BSUtilObjectInstanceMemoryAllocator() {}
    ~BSUtilObjectInstanceMemoryAllocator();
    int ComputeNeededSize(int, int, int);
    void Init(void*, int, int, int);

    unsigned char unknown00[28];
};

static BSUtilObjectInstanceMemoryAllocator g_pObjectAllocators[2];
static BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;
static BSUtilObjectInstanceMemoryAllocator g_pAnimatedVolumeAllocator;
static BSUtilObjectInstanceMemoryAllocator g_pEnvModMechAllocator;
static CDumpObjectContainer g_DumpObjectContainer;

/* inferred: a queued soldier creation (the creator, its index, a word) */
struct GermanCreationEntry {
    void* data00;
    int index;
    int data08;
};

/* inferred: an object waiting to be destroyed (two words) */
struct DelayDestroyEntry {
    void* data00;
    void* data04;
};

static GermanCreationEntry g_pGermanCreationQueue[15];
static DelayDestroyEntry g_pDelayDestroyList[10];

struct LevelFileContentsStruct_;
enum TLTResourceID {};
struct TLTResourceView {
    unsigned char unknown00[8];
    char* data;
};
TLTResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* BSUtilGetMemory(int, int);

extern void* g_pBoxList;
extern void* g_pGermanList;
extern void* g_pTankList;
extern int g_frameCounter; // file-local in the original
extern void* g_delayDestroyHead; // file-local in the original
extern void* g_delayDestroyTail; // file-local in the original
extern int g_numActiveSoldiers;
extern int g_maxActiveSoldiers;
extern int g_iNumGermansQueuedThisFrame; // file-local in the original
extern int g_iGermanCreationQueueHeadIndex;
extern int g_iGermanCreationQueueTailIndex;
extern float g_fDumpObjectCollisionRadius; // file-local in the original
extern int g_iNumberOfEnemiesSpawned;

class BSGO_Dummy {
public:
    ISceneNode* GetSceneNode();
    ProximityDataView* GetProximityData();

    unsigned char unknown00[16];
    ISceneNode* m_sceneNode;
    unsigned char m_proximity[4];
};

class BSGO_Destructable {
public:
    ScriptDataView* GetScriptData();
    ISceneNode* GetSceneNode();
    ProximityDataView* GetProximityData();

    unsigned char unknown00[16];
    unsigned char m_script[12];
    ISceneNode* m_sceneNode;
    unsigned char m_proximity[4];
};

class BSGO_Tank {
public:
    ScriptDataView* GetScriptData();
    ISceneNode* GetSceneNode();
    ProximityDataView* GetProximityData();

    unsigned char unknown00[16];
    unsigned char m_script[212];
    ISceneNode* m_sceneNode;
    unsigned char m_proximity[4];
};

class BSGO_Soldier {
public:
    ScriptDataView* GetScriptData();
    ISceneNode* GetSceneNode();
    ProximityDataView* GetProximityData();
    HitReactionInfoView* GetEmbeddedHitReactionInfo();

    unsigned char unknown00[16];
    unsigned char m_script[136];
    unsigned char m_hitReaction[76];
    ISceneNode* m_sceneNode;
    unsigned char m_proximity[4];
};


__declspec(weak) HitReactionInfoView* BSGO_Soldier::GetEmbeddedHitReactionInfo() {
    return (HitReactionInfoView*)m_hitReaction;
}

__declspec(weak) ProximityDataView* BSGO_Soldier::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) ISceneNode* BSGO_Soldier::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ScriptDataView* BSGO_Soldier::GetScriptData() {
    return (ScriptDataView*)m_script;
}

__declspec(weak) ProximityDataView* BSGO_Tank::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) ISceneNode* BSGO_Tank::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ScriptDataView* BSGO_Tank::GetScriptData() {
    return (ScriptDataView*)m_script;
}

__declspec(weak) ProximityDataView* BSGO_Destructable::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) ISceneNode* BSGO_Destructable::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ScriptDataView* BSGO_Destructable::GetScriptData() {
    return (ScriptDataView*)m_script;
}

__declspec(weak) ProximityDataView* BSGO_Dummy::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) ISceneNode* BSGO_Dummy::GetSceneNode() {
    return m_sceneNode;
}

CDumpObjectContainer::~CDumpObjectContainer() {
    delete[] m_objects;
    m_objects = 0;
}

// The memory needed by the file's allocators, each block after the first
// two padded to 16 bytes.
int CreateObjectGetMemoryRequirements() {
    int size = 0;
    size += g_pObjectAllocators[0].ComputeNeededSize(48, 200, 0);
    size += g_pObjectAllocators[1].ComputeNeededSize(248, 25, 0);
    size += (16 - (size & 15)) & 15;
    size += g_pVolumeAllocator.ComputeNeededSize(80, 450, 16);
    size += (16 - (size & 15)) & 15;
    size += g_pEnvModMechAllocator.ComputeNeededSize(128, 50, 16);
    size += (16 - (size & 15)) & 15;
    size += g_pAnimatedVolumeAllocator.ComputeNeededSize(1984, 50, 16);
    return size;
}

void InitObjectCreators() {
    int size = CreateObjectGetMemoryRequirements();
    char* memory = (char*)BSUtilGetMemory(size, 16);
    g_pObjectAllocators[0].Init(memory, 48, 200, 0);
    memory += g_pObjectAllocators[0].ComputeNeededSize(48, 200, 0);
    g_pObjectAllocators[1].Init(memory, 248, 25, 0);
    memory += g_pObjectAllocators[1].ComputeNeededSize(248, 25, 0);
    memory += (16 - ((int)memory & 15)) & 15;
    g_pVolumeAllocator.Init(memory, 80, 450, 16);
    memory += g_pVolumeAllocator.ComputeNeededSize(80, 450, 16);
    memory += (16 - ((int)memory & 15)) & 15;
    g_pEnvModMechAllocator.Init(memory, 128, 50, 16);
    memory += g_pEnvModMechAllocator.ComputeNeededSize(128, 50, 16);
    memory += (16 - ((int)memory & 15)) & 15;
    g_pAnimatedVolumeAllocator.Init(memory, 1984, 50, 16);
    memory += g_pAnimatedVolumeAllocator.ComputeNeededSize(1984, 50, 16);
    for (int i = 0; i < 10; i++)
        g_pDelayDestroyList[i].data00 = g_pDelayDestroyList[i].data04 = 0;
    g_pBoxList = 0;
    g_pGermanList = 0;
    g_pTankList = 0;
    g_frameCounter = 3;
    g_delayDestroyHead = 0;
    g_delayDestroyTail = 0;
    g_numActiveSoldiers = 0;
    g_maxActiveSoldiers = *(int*)(TLT_FindNextResourceByType((TLTResourceID)13, 0)->data + 0x1c4);
    g_iNumGermansQueuedThisFrame = 0;
    g_iGermanCreationQueueHeadIndex = 0;
    g_iGermanCreationQueueTailIndex = 0;
    for (int j = 0; j < 15; j++) {
        g_pGermanCreationQueue[j].data00 = 0;
        g_pGermanCreationQueue[j].index = -1;
        g_pGermanCreationQueue[j].data08 = 0;
    }
    g_fDumpObjectCollisionRadius = *(float*)(TLT_FindNextResourceByType((TLTResourceID)11, 0)->data + 500);
    g_DumpObjectContainer.Clear();
    g_iNumberOfEnemiesSpawned = 0;
}
