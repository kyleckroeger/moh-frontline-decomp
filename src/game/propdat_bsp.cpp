// A second fragment of propdat.cpp (0x800421e0-0x80042f68; src/game/propdat.cpp
// holds the endian conversions and machine-gun lookups before it). The file
// name is this project's; the original record is propdat.cpp.
// FindAllMGPointsInBPDFile pairs every machine-gun waypoint trigger with the
// nearest machine-gun mechanic trigger in the machine-gun table (a DEBUG
// warning instead when the nearest is more than 10 units away); the table is
// written by index and the trigger list walked with a pointer, and its
// messages follow the header's prime list in the file's .rodata block.
// FreePropertyMemory closes the BSP and property files; PatchUpPropBSPTreeLeaf
// and PatchUpPropBSPTreeNode convert a property BSP tree in place (byte order,
// then file offsets to pointers, recursing into both children);
// LoadPropBSPTree loads the tree and converts it unless it is already swapped;
// FindPlayerStartTriggerAndCreatePlayer finds the player-start trigger (or the
// alternate one) and creates the player there. The weak EndianSwap functions
// in this range are the out-of-line copies of the inline conversions. Function
// names are the symbols; struct views, field names and the conversion helpers
// are inferred (see docs/Game.md, "Endian conversions"). The file's globals
// are declared extern: their data is not reconstructed.

// Header-defined hash-table primes; every unit that includes the header carries a copy.
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

extern "C" int rand();
void DebugMsg(const char*, ...);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);
void TLT_CloseFile(void*);

extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y ;
    }
    return x;
}

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
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

inline void ChangeEndian(unsigned int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<unsigned int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<unsigned int*>(bytes);
}

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

inline void EndianSwap(float& value, bool) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

inline void ChangeEndian(float& value) {
    float& f = value;
    EndianSwap(f, true);
}

inline void EndianSwapList(unsigned long* values) {
    ChangeEndian(values[0]);
    for (int i = 1; i <= values[0]; ++i)
        ChangeEndian(values[i]);
}

template <class T> void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

class CMatrix;

class CVector3 {
public:
    float x, y, z, w;
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    // Frontline's out-of-line copy constructor copies two doublewords.
    CVector3(const CVector3& o) {
        reinterpret_cast<double*>(this)[0] = reinterpret_cast<const double*>(&o)[0];
        reinterpret_cast<double*>(this)[1] = reinterpret_cast<const double*>(&o)[1];
    }
    CVector3& operator-=(const CVector3& o) {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        return *this;
    }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
} __attribute__((aligned(8)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;
    float x, y, z, w;
};

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetPos(CVector3);
    static bool s_ClassInit;
    float m[4][4];
} __attribute__((aligned(16)));

class CCollision;
class CDrawContext;
class CBullet;
class CLight;
enum EClsnId { ClsnIdUnknown = 0 };

class ISceneNode {
public:
    enum EVolumeType { VolumeTypeUnknown = 0 };
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual void* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual void* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class IMovingSceneNode : public ISceneNode {
public:
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
};

// Only the size (328 bytes, from the g_scene symbol) and this method are established.
class CScene {
public:
    IMovingSceneNode* GetPlayer(int) const;

private:
    unsigned char unknown00[328];
};

extern CScene g_scene;
extern int g_NumPlayers;
extern bool g_bInMultiplayerMode;

struct MOH_core_Struct {
    unsigned char unknown00[4];
    unsigned long field04;
    unsigned char unknown08[4];
    int field0c;
    float position[3];
    CQuaternion field1c;
    short field2c;
    short field2e;
    short field30;
    short field32;
    unsigned char unknown34[2];
    short field36;
    short field38;
    short field3a;
    short field3c;
    unsigned char unknown3e[2];
    char* field40;
    unsigned long* field44;
    int field48;
    char* field4c;
    unsigned long field50;
    char* field54;
    unsigned long* field58;
    unsigned long* field5c;
    unsigned long* field60;
    unsigned long* field64;
    unsigned long* field68;
    void* field6c;
};

struct MOH_fogParams_Struct {
    MOH_core_Struct core;
    short field70;
    short field72;
    short field74;
    unsigned char unknown76[2];
    int field78;
    int field7c;
};

struct MOH_animatedLight_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[2];
    short field72;
    unsigned long* field74;
    float* field78;
};

struct MOH_playerPathController_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[2];
    short field72;
    short field74;
    unsigned char unknown76[2];
    unsigned long field78;
    int field7c;
};

struct MOH_projectileGeneratorTarget_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[2];
    short field72;
    unsigned long field74;
    unsigned long field78;
    unsigned long field7c;
    unsigned long field80;
    unsigned long field84;
    int field88;
};

struct MOH_projectileGenerator_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[2];
    short field72;
    unsigned long field74;
    unsigned long field78;
    unsigned long field7c;
    unsigned long field80;
    unsigned long field84;
    unsigned long field88;
    unsigned long field8c;
    int field90;
    int field94;
};

struct MOH_particle_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[18];
    short field82;
    short field84;
    unsigned char unknown86[2];
    int field88;
    int field8c;
    int field90;
    int field94;
    int field98;
    int field9c;
    int fielda0;
    int fielda4;
    int fielda8;
    int fieldac;
    int fieldb0;
    int fieldb4;
    int fieldb8;
    int fieldbc;
    int fieldc0;
    int fieldc4;
    int fieldc8;
    int fieldcc;
    int fieldd0;
    int fieldd4;
    int fieldd8;
    int fielddc;
    int fielde0;
    int fielde4;
    int fielde8;
    void* fieldec;
};

struct MOH_mechanicEnvMod_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[4];
    int field74;
    int field78;
};

struct MOH_enemy_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[14];
    short field7e;
    short field80;
    short field82;
    short field84;
    short field86;
    short field88;
    short field8a;
    short field8c;
    short field8e;
    int field90;
    int field94;
    int field98;
    int field9c;
    int fielda0;
    int fielda4;
    int fielda8;
    int fieldac;
};

struct MOH_mechanic_Struct {
    MOH_core_Struct core;
    unsigned char unknown70[4];
    int field74;
};

struct xyzProperty_Struct {
    int field00;
    unsigned long field04;
    int field08;
    int field0c;
    float field10;
    float field14;
    float field18;
    unsigned long field1c;
    float field20;
    float field24;
    float field28;
};

// The record type that swaps eight halfwords at +0x2c; its name is unknown.
struct PropertyShortsView {
    unsigned char unknown00[44];
    short field2c;
    short field2e;
    short field30;
    short field32;
    short field34;
    short field36;
    short field38;
    short field3a;
};

struct _PropBSPLeaf {
    unsigned int field00;
    unsigned short field04;
    unsigned short field06;
    unsigned short field08;
    unsigned short field0a;
    int field0c;
    int field10;
    int field14;
    int field18;
    int field1c;
    int field20;
    int field24;
    int field28;
    int field2c;
};

// Element of the leaf array at +0x1c: two halfwords swapped by one inline call.
struct PropBSPLeafPairView {
    short field00;
    short field02;
};

struct _PropBSPNode;

union PBSPNodeOrLeaf {
    _PropBSPNode* node;
    _PropBSPLeaf* leaf;
    int offset;
};

struct _PropBSPNode {
    float field00;
    PBSPNodeOrLeaf front;
    PBSPNodeOrLeaf back;
    bool frontIsLeaf;
    bool backIsLeaf;
};

struct PBSPHeader {
    unsigned char unknown00[32];
    int field20;
    int field24;
};

struct PropPlane4 {
    float x, y, z, d;
};

struct BPDLight {
    int field00;
    float field04;
    float field08;
    float field0c;
    float field10;
    float field14;
    float field18;
    float field1c;
    float field20;
    float field24;
    unsigned long field28;
    int field2c;
};

struct BPDLightVolume {
    float field00;
    float field04;
    float field08;
    float field0c;
    float field10;
    float field14;
    int planeCount;
    PropPlane4* planes;
    int lightCount;
    BPDLight* lights;
    int field28;
    int field2c;
};

struct BPDPolyPath {
    int field00;
    int field04;
    float field08;
};

struct BPDPathFindingNode;

struct BPDParticleTweaker {
    char* field00;
    void* field04;
};

struct BPDHeader {
    int field00;
    int field04;
    int field08;
    int field0c;
    BPDPolyPath* paths;
    int pathCount;
    BPDPathFindingNode* nodes;
    int nodeCount;
    void** field20;
    int field24;
    void* field28;
    int field2c;
    MOH_particle_Struct* field30;
    int field34;
    MOH_animatedLight_Struct* field38;
    int field3c;
    BPDLightVolume* field40;
    int field44;
    BPDParticleTweaker* field48;
    int field4c;
    BPDParticleTweaker* field50;
    int field54;
};

class BSObject;
class BSGO_Basic;
class ShapeFile;

struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
    BSObject* bsObject;
};

struct MGTriggerObjectView {
    TriggerObject_struct* waypoint;
    TriggerObject_struct* mechanic;
    unsigned char unknown08[4];
};

int CreateBSObject(const char*, BSObject**, BSGO_Basic*, TriggerObject_struct*, void**);
int BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void CreateObject(TriggerObject_struct*, int, void*);

class CParticleSystem {
public:
    static void Register(ShapeFile*);
    static void Register(void*, int);
    static void Create(void*);
};

class CLight {
public:
    static void Register(void*, int);
    static void Create(void*);
};

class CAIFilterGlobal {
public:
    void CreateSplinePathManager(void*, BPDPolyPath*, int);
    void ResetSplinePathManager(void*, BPDPolyPath*, int);
    void CreateAStarPathNodeList(void*, BPDPathFindingNode*, int);

private:
    // Only the size (24 bytes, from the g_aigAIFilterGlobalObject symbol) is established.
    unsigned char unknown00[24];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

extern void* g_pRawPropertyData;
extern unsigned int g_pNumTriggers;
extern BPDHeader* g_pBPDHeader;
extern PBSPHeader* g_pPBSPHeader;
extern _PropBSPNode* g_pPBSPHead;
extern BPDLightVolume* g_pLightVolumes;
extern int g_numPlayerStarts;

extern TriggerObject_struct g_pTriggerObjects[2000];
extern MGTriggerObjectView g_pMGTriggerObject[32];
extern int g_playerStartIndices[15];
extern bool g_takenPlayerStarts[15];


inline void EndianSwap(_PropBSPLeaf& value) {
    ChangeEndian(value.field00);
    ChangeEndian(value.field04);
    ChangeEndian(value.field06);
    ChangeEndian(value.field08);
    ChangeEndian(value.field0a);
    ChangeEndian(value.field0c);
    ChangeEndian(value.field10);
    ChangeEndian(value.field14);
    ChangeEndian(value.field18);
    ChangeEndian(value.field1c);
    ChangeEndian(value.field20);
    ChangeEndian(value.field24);
    ChangeEndian(value.field28);
    ChangeEndian(value.field2c);
}

inline void EndianSwap(PropBSPLeafPairView& value) {
    ChangeEndian(value.field00);
    ChangeEndian(value.field02);
}

inline void EndianSwap(PBSPNodeOrLeaf& value) {
    ChangeEndian(value.offset);
}

inline void EndianSwap(_PropBSPNode& value) {
    EndianSwap(value.field00, false);
    EndianSwap(value.front);
    EndianSwap(value.back);
}

inline void EndianSwap(BPDPolyPath& value) {
    ChangeEndian(value.field00);
    ChangeEndian(value.field04);
    ChangeEndian(value.field08);
}

inline void EndianSwap(BPDParticleTweaker& value) {
    ChangeEndian(value.field00);
    ChangeEndian(value.field04);
}

inline void EndianSwap(BPDLight& value) {
    ChangeEndian(value.field00);
    ChangeEndian(value.field04);
    ChangeEndian(value.field08);
    ChangeEndian(value.field0c);
    ChangeEndian(value.field10);
    ChangeEndian(value.field14);
    ChangeEndian(value.field18);
    ChangeEndian(value.field1c);
    ChangeEndian(value.field20);
    ChangeEndian(value.field24);
    ChangeEndian(value.field28);
    ChangeEndian(value.field2c);
}

inline void EndianSwap(BPDLightVolume& value) {
    ChangeEndian(value.field00);
    ChangeEndian(value.field04);
    ChangeEndian(value.field08);
    ChangeEndian(value.field0c);
    ChangeEndian(value.field10);
    ChangeEndian(value.field14);
    ChangeEndian(value.planeCount);
    ChangeEndian(value.planes);
    ChangeEndian(value.lightCount);
    ChangeEndian(value.lights);
    ChangeEndian(value.field28);
    ChangeEndian(value.field2c);
}

inline void EndianSwap(BPDHeader& value) {
    ChangeEndian(value.field00);
    ChangeEndian(value.field04);
    ChangeEndian(value.field08);
    ChangeEndian(value.field0c);
    ChangeEndian(value.paths);
    ChangeEndian(value.pathCount);
    ChangeEndian(value.nodes);
    ChangeEndian(value.nodeCount);
    ChangeEndian(value.field20);
    ChangeEndian(value.field24);
    ChangeEndian(value.field28);
    ChangeEndian(value.field2c);
    ChangeEndian(value.field30);
    ChangeEndian(value.field34);
    ChangeEndian(value.field38);
    ChangeEndian(value.field3c);
    ChangeEndian(value.field40);
    ChangeEndian(value.field44);
    ChangeEndian(value.field48);
    ChangeEndian(value.field4c);
    ChangeEndian(value.field50);
    ChangeEndian(value.field54);
}

void EndianSwap(MOH_core_Struct& value);

void EndianSwap(MOH_fogParams_Struct& value);

void EndianSwap(MOH_animatedLight_Struct& value);

void EndianSwap(MOH_playerPathController_Struct& value);

void EndianSwap(MOH_projectileGeneratorTarget_Struct& value);

void EndianSwap(MOH_projectileGenerator_Struct& value);

void EndianSwap(MOH_particle_Struct& value);

void EndianSwap(MOH_mechanicEnvMod_Struct& value);

void EndianSwap(MOH_enemy_Struct& value);

void EndianSwap(MOH_mechanic_Struct& value);

void EndianSwap(MOH_core_Struct& value);

void EndianSwap(xyzProperty_Struct& value);


int SearchForClosestMachineGun(ISceneNode* node, float maxDistance);

bool IsMGUsed(TriggerObject_struct* trigger);

void MarkMGAsUsed(TriggerObject_struct* trigger, bool used);

void FindAllMGPointsInBPDFile();

void FindAllMGPointsInBPDFile() {
    int count = 0;
    TriggerObject_struct* trigger = g_pTriggerObjects;
    for (unsigned int i = 0; i < g_pNumTriggers; i++, trigger++) {
        MOH_core_Struct* core = trigger->core;
        if (trigger->core->field0c == 5 && core->field48 == 0x5b607813) {
            float best = 1e37f;
            TriggerObject_struct* other = g_pTriggerObjects;
            TriggerObject_struct* closest = 0;
            for (unsigned int j = 0; j < g_pNumTriggers; j++, other++) {
                if (other->core->field0c == 2 &&
                    (other->core->field48 == 0x29f985d6 || other->core->field48 == (int)0xf6798fe8 ||
                     other->core->field48 == 0x6e39be8f)) {
                    float dx = other->core->position[0];
                    float dy = other->core->position[1];
                    float dz = other->core->position[2];
                    dx -= core->position[0];
                    dy -= core->position[1];
                    dz -= core->position[2];
                    float distance = dx * dx + dy * dy + dz * dz;
                    if (distance < best) {
                        best = distance;
                        closest = other;
                    }
                }
            }
            if (closest) {
                if (best > 10.0f) {
                    DebugMsg("++++++++++++++++++++++++ START MMG WARNING ++++++++++++++++++++++++");
                    DebugMsg("WARNING:  Possible bad waypoint placement at x: %d  y: %d  z: %d\n",
                             (int)trigger->core->position[0], (int)trigger->core->position[1],
                             (int)trigger->core->position[2]);
                    DebugMsg("          Closest machine gun mechanic is %d away\n", (int)best);
                    DebugMsg("          We are going to ignore the waypoint\n");
                    DebugMsg("+++++++++++++++++++++++++ END MMG WARNING +++++++++++++++++++++++++");
                } else {
                    g_pMGTriggerObject[count].waypoint = trigger;
                    g_pMGTriggerObject[count].mechanic = closest;
                    count++;
                }
            }
        }
    }
}

void FreePropertyMemory() {
    if (g_pPBSPHeader) {
        TLT_CloseFile(g_pPBSPHeader);
        g_pPBSPHeader = 0;
    }
    if (g_pBPDHeader) {
        TLT_CloseFile(g_pBPDHeader);
        g_pBPDHeader = 0;
    }
}

void PatchUpPropBSPTreeLeaf(char* base, _PropBSPLeaf* leaf) {
    EndianSwap(*leaf);
    int offset = leaf->field20;
    if (leaf->field00) {
        leaf->field20 = reinterpret_cast<int>(base) + offset;
        for (unsigned int i = 0; i < leaf->field00; i++)
            ChangeEndian(reinterpret_cast<short*>(leaf->field20)[i]);
    } else {
        leaf->field20 = reinterpret_cast<int>(&leaf->field20);
    }
    if (leaf->field2c > 0) {
        leaf->field2c = reinterpret_cast<int>(base) + leaf->field2c;
        for (int i = 0; i < leaf->field0a; i++)
            ChangeEndian(reinterpret_cast<short*>(leaf->field2c)[i]);
    } else {
        leaf->field2c = reinterpret_cast<int>(&leaf->field2c);
    }
    offset = leaf->field28;
    if (leaf->field06) {
        leaf->field28 = reinterpret_cast<int>(base) + offset;
        for (int i = 0; i < leaf->field06; i++)
            ChangeEndian(reinterpret_cast<short*>(leaf->field28)[i]);
    } else {
        leaf->field28 = reinterpret_cast<int>(&leaf->field28);
    }
    offset = leaf->field1c;
    if (leaf->field08) {
        leaf->field1c = reinterpret_cast<int>(base) + offset;
        for (int i = 0; i < leaf->field08; i++)
            EndianSwap(reinterpret_cast<PropBSPLeafPairView*>(leaf->field1c)[i]);
    } else {
        leaf->field1c = reinterpret_cast<int>(&leaf->field1c);
    }
    offset = leaf->field24;
    if (leaf->field04) {
        leaf->field24 = reinterpret_cast<int>(base) + offset;
        for (int i = 0; i < leaf->field04; i++)
            ChangeEndian(reinterpret_cast<short*>(leaf->field24)[i]);
    } else {
        leaf->field24 = reinterpret_cast<int>(&leaf->field24);
    }
}

void PatchUpPropBSPTreeNode(char* base, _PropBSPNode* node) {
    EndianSwap(*node);
    node->front.offset = reinterpret_cast<int>(base) + node->front.offset;
    node->back.offset = reinterpret_cast<int>(base) + node->back.offset;
    if (node->frontIsLeaf)
        PatchUpPropBSPTreeLeaf(base, node->front.leaf);
    else
        PatchUpPropBSPTreeNode(base, node->front.node);
    if (node->backIsLeaf)
        PatchUpPropBSPTreeLeaf(base, node->back.leaf);
    else
        PatchUpPropBSPTreeNode(base, node->back.node);
}

void LoadPropBSPTree(char* name, bool swapped) {
    PBSPHeader* header = static_cast<PBSPHeader*>(TLT_LoadFileFromLevelBigFile(name, 0));
    g_pPBSPHeader = header;
    if (!swapped) {
        ChangeEndian(header->field20);
        ChangeEndian(header->field24);
    }
    g_pPBSPHead = reinterpret_cast<_PropBSPNode*>(reinterpret_cast<char*>(header) + g_pPBSPHeader->field24);
    if (!swapped)
        PatchUpPropBSPTreeNode(reinterpret_cast<char*>(header), g_pPBSPHead);
}

void FindPlayerStartTriggerAndCreatePlayer(bool useAlternate, int player) {
    TriggerObject_struct* trigger = g_pTriggerObjects;
    TriggerObject_struct* start = 0;
    TriggerObject_struct* alternate = 0;
    for (unsigned int i = 0; i < g_pNumTriggers; i++, trigger++) {
        MOH_core_Struct* core = trigger->core;
        if (core->field0c == 2) {
            if (!start && core->field48 == 0x271BD574) {
                if (g_bInMultiplayerMode && core->field38 != player)
                    continue;
                start = trigger;
                if (!useAlternate)
                    break;
            } else if (!alternate && core->field48 == 0xFD092133) {
                alternate = trigger;
                if (useAlternate)
                    break;
            }
        }
    }
    if (useAlternate && alternate)
        start = alternate;
    if (start)
        CreateObject(start, 0xffff, reinterpret_cast<void*>(player));
}

void RestartPlayer(int player);

void PatchUpCore(MOH_core_Struct* core);

// Inferred inline helper: case 14's swaps sit one inline level deeper than the
// header's animated-light loop. Its original name is unknown.
static void PatchUpAnimatedLightArrays(MOH_animatedLight_Struct* light, int base);

int PatchUpAllPropertyData(BPDHeader* header);

void LoadPropertyData(char* name, bool reload);
