// A third fragment of propdat.cpp: PatchUpCore (0x80043340, 1,004 bytes), which
// turns the core record's list offsets into pointers and converts each list's
// byte order, turns its string offsets into pointers and clears the word at +0x6c,
// and PatchUpAllPropertyData (0x8004372c, 2,620 bytes), which rebases and
// byte-swaps the property file's header lists (particle shapes and records,
// animated lights, light volumes with their planes and lights, the two particle
// tweaker lists, the polygon paths), then walks the property records, hanging
// each on the next trigger object and dispatching on its name hash and then on
// its type to the matching EndianSwap and PatchUpCore, and returns the size of
// the records. The file name is this project's; the original record is
// propdat.cpp and RestartPlayer before it is not reconstructed (its .sdata2
// constants are pooled with the file's). The header's dwi_prime_list copy
// (.rodata) belongs to the propdat_bsp.cpp fragment and is left out here. The
// offsetPtr templates and the weak EndianSwap copies for BPDLight and
// BPDLightVolume (emitted after PatchUpAllPropertyData in the original, as
// out-of-line inlines) are declared only. Struct views, field names, the
// record name hashes' meaning and the small inline helpers are inferred; the
// file's globals are declared extern.


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

template <class T> void offsetPtr(T*& pointer, int base);

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

inline void EndianSwap(PropPlane4& value) {
    EndianSwap(value.x, true);
    EndianSwap(value.y, true);
    EndianSwap(value.z, true);
    EndianSwap(value.d, true);
}

void EndianSwap(BPDLight& value);

void EndianSwap(BPDLightVolume& value);

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

void FreePropertyMemory();

void PatchUpPropBSPTreeLeaf(char* base, _PropBSPLeaf* leaf);

void PatchUpPropBSPTreeNode(char* base, _PropBSPNode* node);

void LoadPropBSPTree(char* name, bool swapped);

void FindPlayerStartTriggerAndCreatePlayer(bool useAlternate, int player);

void RestartPlayer(int player);

void PatchUpCore(MOH_core_Struct* core) {
    if (core->field58) {
        core->field58 = reinterpret_cast<unsigned long*>(reinterpret_cast<int>(core) + reinterpret_cast<int>(core->field58));
        EndianSwapList(core->field58);
    }
    if (core->field5c) {
        core->field5c = reinterpret_cast<unsigned long*>(reinterpret_cast<int>(core) + reinterpret_cast<int>(core->field5c));
        EndianSwapList(core->field5c);
    }
    if (core->field60) {
        core->field60 = reinterpret_cast<unsigned long*>(reinterpret_cast<int>(core) + reinterpret_cast<int>(core->field60));
        EndianSwapList(core->field60);
    }
    if (core->field64) {
        core->field64 = reinterpret_cast<unsigned long*>(reinterpret_cast<int>(core) + reinterpret_cast<int>(core->field64));
        EndianSwapList(core->field64);
    }
    if (core->field44) {
        core->field44 = reinterpret_cast<unsigned long*>(reinterpret_cast<int>(core) + reinterpret_cast<int>(core->field44));
        EndianSwapList(core->field44);
    }
    if (core->field68) {
        core->field68 = reinterpret_cast<unsigned long*>(reinterpret_cast<int>(core) + reinterpret_cast<int>(core->field68));
        EndianSwapList(core->field68);
    }
    if (core->field40)
        core->field40 = reinterpret_cast<char*>(reinterpret_cast<int>(core) + reinterpret_cast<int>(core->field40));
    if (core->field4c)
        core->field4c = reinterpret_cast<char*>(reinterpret_cast<int>(core) + reinterpret_cast<int>(core->field4c));
    if (core->field54)
        core->field54 = reinterpret_cast<char*>(reinterpret_cast<int>(core) + reinterpret_cast<int>(core->field54));
    if (core->field6c)
        core->field6c = 0;
}

inline void PatchUpAnimatedLightArrays(MOH_animatedLight_Struct* light, int base) {
    offsetPtr(light->field74, base);
    offsetPtr(light->field78, base);
    for (int i = 0; i < light->field72; i++)
        ChangeEndian(light->field78[i]);
}

int PatchUpAllPropertyData(BPDHeader* header) {
    offsetPtr(header->field28, (int)header);
    TriggerObject_struct* trigger = g_pTriggerObjects;
    char* start = (char*)header->field28;
    unsigned int count = header->field2c;
    char* cursor = start;
    int base = (int)header;
    if (header->field24) {
        offsetPtr(header->field20, (int)header);
        for (int i = 0; i < header->field24; i++) {
            ChangeEndian(header->field20[i]);
            offsetPtr(header->field20[i], base);
            CParticleSystem::Register((ShapeFile*)header->field20[i]);
        }
    }
    if (header->field30) {
        MOH_particle_Struct* particle = header->field30;
        offsetPtr(particle, (int)header);
        for (int i = 0; i < header->field34; i++) {
            EndianSwap(*(xyzProperty_Struct*)particle);
            EndianSwap(*particle);
            offsetPtr(particle->fieldec, base);
            particle++;
        }
        char* data = (char*)header->field30;
        offsetPtr(data, (int)header);
        CParticleSystem::Register(data, header->field34);
    }
    if (header->field38) {
        MOH_animatedLight_Struct* light = header->field38;
        offsetPtr(light, (int)header);
        for (int i = 0; i < header->field3c; i++) {
            EndianSwap(*(xyzProperty_Struct*)light);
            EndianSwap(*light);
            offsetPtr(light->field74, base);
            offsetPtr(light->field78, base);
            for (int j = 0; j < light->field72; j++)
                EndianSwap(light->field78[j], true);
            light++;
        }
        char* data = (char*)header->field38;
        offsetPtr(data, (int)header);
        CLight::Register(data, header->field3c);
    }
    if (header->field40) {
        offsetPtr(header->field40, (int)header);
        for (int i = 0; i < header->field44; i++) {
            EndianSwap(header->field40[i]);
            if (header->field40[i].planeCount > 0) {
                offsetPtr(header->field40[i].planes, base);
                for (int j = 0; j < header->field40[i].planeCount; j++)
                    EndianSwap(header->field40[i].planes[j]);
            }
            if (header->field40[i].lightCount > 0) {
                offsetPtr(header->field40[i].lights, base);
                for (int j = 0; j < header->field40[i].lightCount; j++)
                    EndianSwap(header->field40[i].lights[j]);
            }
        }
    }
    if (header->field48) {
        offsetPtr(header->field48, (int)header);
        for (int i = 0; i < header->field4c; i++) {
            EndianSwap(header->field48[i]);
            if (header->field48[i].field00)
                offsetPtr(header->field48[i].field00, base);
            if (header->field48[i].field04)
                offsetPtr(header->field48[i].field04, base);
        }
    }
    if (header->field50) {
        offsetPtr(header->field50, (int)header);
        for (int i = 0; i < header->field54; i++) {
            EndianSwap(header->field50[i]);
            if (header->field50[i].field00)
                offsetPtr(header->field50[i].field00, base);
            if (header->field50[i].field04)
                offsetPtr(header->field50[i].field04, base);
        }
    }
    if (header->paths) {
        offsetPtr(header->paths, (int)header);
        for (int i = 0; i < header->pathCount; i++)
            EndianSwap(header->paths[i]);
    }
    for (unsigned int n = 0; n < count; n++, trigger++) {
        trigger->core = (MOH_core_Struct*)cursor;
        EndianSwap(*(xyzProperty_Struct*)trigger->core);
        bool handled = false;
        switch (((xyzProperty_Struct*)trigger->core)->field00) {
        case 0x19fcee8c:
            break;
        case (int)0xb6f8df6e:
        case (int)0x9fb57f53:
        case 0x040294b8:
        case (int)0xfac270e1:
        case 0x49821e11:
        case 0x3602da6b:
        case 0x56bb6353:
            break;
        case 0x4d078f23: {
            handled = true;
            PropertyShortsView* shorts = (PropertyShortsView*)trigger->core;
            ChangeEndian(shorts->field2c);
            ChangeEndian(shorts->field2e);
            ChangeEndian(shorts->field30);
            ChangeEndian(shorts->field32);
            ChangeEndian(shorts->field34);
            ChangeEndian(shorts->field36);
            ChangeEndian(shorts->field38);
            ChangeEndian(shorts->field3a);
            break;
        }
        case (int)0xd4717f38:
            EndianSwap(*(MOH_playerPathController_Struct*)trigger->core);
            PatchUpCore(trigger->core);
            handled = true;
            break;
        case 0x35fb0439:
            EndianSwap(*(MOH_fogParams_Struct*)trigger->core);
            PatchUpCore(trigger->core);
            handled = true;
            break;
        }
        if (!handled) {
            switch (trigger->core->field0c) {
            case 0:
                break;
            case 1:
            case 6:
                EndianSwap(*trigger->core);
                PatchUpCore(trigger->core);
                break;
            case 7:
                EndianSwap(*trigger->core);
                PatchUpCore(trigger->core);
                break;
            case 2:
                EndianSwap(*(MOH_mechanic_Struct*)trigger->core);
                PatchUpCore(trigger->core);
                break;
            case 8:
                EndianSwap(*(MOH_mechanicEnvMod_Struct*)trigger->core);
                PatchUpCore(trigger->core);
                break;
            case 3:
                EndianSwap(*(MOH_enemy_Struct*)trigger->core);
                PatchUpCore(trigger->core);
                break;
            case 4:
                EndianSwap(*trigger->core);
                PatchUpCore(trigger->core);
                break;
            case 5:
                EndianSwap(*trigger->core);
                PatchUpCore(trigger->core);
                break;
            case 9:
                EndianSwap(*(MOH_particle_Struct*)trigger->core);
                PatchUpCore(trigger->core);
                offsetPtr(((MOH_particle_Struct*)trigger->core)->fieldec, (int)header);
                break;
            case 14:
                EndianSwap(*(MOH_animatedLight_Struct*)trigger->core);
                PatchUpCore(trigger->core);
                PatchUpAnimatedLightArrays((MOH_animatedLight_Struct*)trigger->core, (int)cursor);
                break;
            case 10:
                EndianSwap(*(MOH_projectileGenerator_Struct*)trigger->core);
                PatchUpCore(trigger->core);
                break;
            case 11:
                EndianSwap(*(MOH_projectileGeneratorTarget_Struct*)trigger->core);
                PatchUpCore(trigger->core);
                break;
            default:
                if (trigger->core->field0c < 16) {
                    EndianSwap(*trigger->core);
                    PatchUpCore(trigger->core);
                } else {
                    DebugMsg("Unknown property type\n");
                }
                break;
            }
        }
        cursor += ((xyzProperty_Struct*)trigger->core)->field04;
    }
    return cursor - start;
}

void LoadPropertyData(char* name, bool reload);

