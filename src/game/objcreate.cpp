// Script game-object creation: CreateObjectGetMemoryRequirements (the
// memory the file's allocators need), the dump container's destructor and the
// accessors of the script game-object classes that follow it. The class names
// come from the mangled symbols; the members are inferred from offsets and the
// accessors' result types are left incomplete. The accessors are inline in
// the original (weak symbols), so they are defined __declspec(weak). The unit
// ends with the file's static initialisation: the two object allocators and
// the volume, animated-volume and environment-mechanism allocators (their
// destructors registered; the array through a generated array destructor) and
// the dump container (700 bytes allocated and cleared, the rest of its words
// zero). The allocator and container members and the container's
// constructor are inferred; the file's .bss block is defined here. The
// creators before them are not part of this unit.
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

    unsigned char unknown00[28];
};

static BSUtilObjectInstanceMemoryAllocator g_pObjectAllocators[2];
static BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;
static BSUtilObjectInstanceMemoryAllocator g_pAnimatedVolumeAllocator;
static BSUtilObjectInstanceMemoryAllocator g_pEnvModMechAllocator;
static CDumpObjectContainer g_DumpObjectContainer;

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

CDumpObjectContainer::~CDumpObjectContainer() {
    delete[] m_objects;
    m_objects = 0;
}

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

__declspec(weak) ISceneNode* BSGO_Dummy::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ProximityDataView* BSGO_Dummy::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) ScriptDataView* BSGO_Destructable::GetScriptData() {
    return (ScriptDataView*)m_script;
}

__declspec(weak) ISceneNode* BSGO_Destructable::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ProximityDataView* BSGO_Destructable::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) ScriptDataView* BSGO_Tank::GetScriptData() {
    return (ScriptDataView*)m_script;
}

__declspec(weak) ISceneNode* BSGO_Tank::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ProximityDataView* BSGO_Tank::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) ScriptDataView* BSGO_Soldier::GetScriptData() {
    return (ScriptDataView*)m_script;
}

__declspec(weak) ISceneNode* BSGO_Soldier::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ProximityDataView* BSGO_Soldier::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) HitReactionInfoView* BSGO_Soldier::GetEmbeddedHitReactionInfo() {
    return (HitReactionInfoView*)m_hitReaction;
}

