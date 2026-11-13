// propdat.cpp: the level property data. The property-record byte-order
// conversions (EndianSwap for each record type; structure after Rising Sun's
// src/bpd/endian.cpp, CC0: every field converted with ChangeEndian; field
// types and names are views, offsets established by the loads and types by how
// each conversion is inlined, see docs/Game.md, "Endian conversions"), the
// machine-gun lookups and FindAllMGPointsInBPDFile, which pairs every
// machine-gun waypoint trigger with the nearest machine-gun mechanic trigger
// (a DEBUG warning instead when the nearest is more than 10 units away).
// FreePropertyMemory closes the BSP and property files; PatchUpPropBSPTreeLeaf
// and PatchUpPropBSPTreeNode convert a property BSP tree in place (byte order,
// then file offsets to pointers, recursing into both children);
// LoadPropBSPTree loads the tree and converts it unless it is already swapped;
// FindPlayerStartTriggerAndCreatePlayer finds the player-start trigger (or the
// alternate one) and creates the player there. RestartPlayer collects the
// level's player-start triggers once, marks the starts beyond the count and
// those within 10 units of a player as taken, then moves the given player to
// the first free start from a random one. PatchUpCore turns the core record's
// list offsets into pointers and converts each list's byte order;
// PatchUpAllPropertyData rebases and byte-swaps the property file's header
// lists and walks the property records, hanging each on the next trigger and
// dispatching on its name hash and type to the matching EndianSwap and
// PatchUpCore. LoadPropertyData loads the level's property file (unless
// reloading), converts and patches it, creates the spline-path manager and the
// path-node list (or resets the path manager on a reload), then initialises
// every trigger and clears the 32 machine-gun records. The weak EndianSwap
// functions follow their first out-of-line callers, and the weak offsetPtr<T>
// instances (each rebases a non-null pointer read from a loaded file) end the
// file in the reverse order of their first use. Function names are the
// symbols; struct views, field names, the record name hashes' meaning and the
// small inline helpers are inferred.

// Header-defined hash-table primes; every unit that includes the header carries a copy.
#include "game/DwiHashMap.h"

extern "C" int rand();
void DebugMsg(const char*, ...);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);
void TLT_CloseFile(void*);

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

template <class T> void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

class CMatrix;

#include "game/CVector3.h"

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;
    float x, y, z, w;
};

#include "game/CMatrix.h"

class CCollision;
class CDrawContext;
class CBullet;
class CLight;

#include "game/ISceneNode.h"

class IMovingSceneNode : public ISceneNode {
public:
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
};

// Only the size (328 bytes, from the g_scene symbol) and this method are established.
#include "game/CScene.h"

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

// Inferred: three floats swapped as one value (a vector type of its own).
struct PropVector3 {
    float x, y, z;
};

struct BPDLight {
    int field00;
    PropVector3 field04;
    PropVector3 field10;
    float field1c;
    float field20;
    float field24;
    unsigned long field28;
    int field2c;
};

struct BPDLightVolume {
    PropVector3 field00;
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
struct MOH_particle_Struct;
struct MOH_animatedLight_Struct;
struct BPDLightVolume;
struct BPDParticleTweaker;

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
    int field08;
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

void* g_pRawPropertyData;
unsigned int g_pNumTriggers;
BPDHeader* g_pBPDHeader;
PBSPHeader* g_pPBSPHeader;
_PropBSPNode* g_pPBSPHead;
BPDLightVolume* g_pLightVolumes;
int g_numPlayerStarts;

TriggerObject_struct g_pTriggerObjects[2000];
MGTriggerObjectView g_pMGTriggerObject[32];
int g_playerStartIndices[15];
bool g_takenPlayerStarts[15];

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

inline void EndianSwap(PropVector3& value) {
    EndianSwap(value.x, true);
    EndianSwap(value.y, true);
    EndianSwap(value.z, true);
}

// The two record swaps are too large to inline; the compiler emits them as
// weak functions after their first caller, PatchUpAllPropertyData.
inline void EndianSwap(BPDLight& value) {
    ChangeEndian(value.field00);
    EndianSwap(value.field04);
    EndianSwap(value.field10);
    ChangeEndian(value.field1c);
    ChangeEndian(value.field20);
    ChangeEndian(value.field24);
    ChangeEndian(value.field28);
    ChangeEndian(value.field2c);
}

inline void EndianSwap(BPDLightVolume& value) {
    EndianSwap(value.field00);
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

void EndianSwap(xyzProperty_Struct& value);

int SearchForClosestMachineGun(ISceneNode* node, float maxDistance);

bool IsMGUsed(TriggerObject_struct* trigger);

void MarkMGAsUsed(TriggerObject_struct* trigger, bool used);

void FindAllMGPointsInBPDFile();

int PatchUpAllPropertyData(BPDHeader* header);


static inline void InitTriggers() {
    TriggerObject_struct* trigger;
    unsigned int i;
    for (i = 0, trigger = g_pTriggerObjects; i < g_pNumTriggers; i++, trigger++) {
        trigger->flags = 0;
        if (trigger->core->field0c != 0) {
            if (trigger->core->field40) {
                CreateBSObject(trigger->core->field40, &trigger->bsObject, 0, trigger, 0);
                BSObjectTriggerEvent(trigger->bsObject, 44, 0, 0, true);
            } else {
                trigger->bsObject = 0;
            }
            if (trigger->core->field0c == 9 && trigger->core->field40 == 0)
                CParticleSystem::Create(trigger->core);
            if (trigger->core->field0c == 14 && trigger->core->field40 == 0)
                CLight::Create(trigger->core);
        }
    }
}

void FreePropertyMemory();

void PatchUpPropBSPTreeLeaf(char* base, _PropBSPLeaf* leaf);

void PatchUpPropBSPTreeNode(char* base, _PropBSPNode* node);

void LoadPropBSPTree(char* name, bool swapped);

void FindPlayerStartTriggerAndCreatePlayer(bool useAlternate, int player);

void RestartPlayer(int player);

void EndianSwap(MOH_fogParams_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field70);
    ChangeEndian(value.field72);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
    ChangeEndian(value.field7c);
}

void EndianSwap(MOH_animatedLight_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field72);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
}

void EndianSwap(MOH_playerPathController_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field72);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
    ChangeEndian(value.field7c);
}

void EndianSwap(MOH_projectileGeneratorTarget_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field72);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
    ChangeEndian(value.field7c);
    ChangeEndian(value.field80);
    ChangeEndian(value.field84);
    ChangeEndian(value.field88);
}

void EndianSwap(MOH_projectileGenerator_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field72);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
    ChangeEndian(value.field7c);
    ChangeEndian(value.field80);
    ChangeEndian(value.field84);
    ChangeEndian(value.field88);
    ChangeEndian(value.field8c);
    ChangeEndian(value.field90);
    ChangeEndian(value.field94);
}

void EndianSwap(MOH_particle_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field82);
    ChangeEndian(value.field84);
    ChangeEndian(value.field88);
    ChangeEndian(value.field8c);
    ChangeEndian(value.field90);
    ChangeEndian(value.field94);
    ChangeEndian(value.field98);
    ChangeEndian(value.field9c);
    ChangeEndian(value.fielda0);
    ChangeEndian(value.fielda4);
    ChangeEndian(value.fielda8);
    ChangeEndian(value.fieldac);
    ChangeEndian(value.fieldb0);
    ChangeEndian(value.fieldb4);
    ChangeEndian(value.fieldb8);
    ChangeEndian(value.fieldbc);
    ChangeEndian(value.fieldc0);
    ChangeEndian(value.fieldc4);
    ChangeEndian(value.fieldc8);
    ChangeEndian(value.fieldcc);
    ChangeEndian(value.fieldd0);
    ChangeEndian(value.fieldd4);
    ChangeEndian(value.fieldd8);
    ChangeEndian(value.fielddc);
    ChangeEndian(value.fielde0);
    ChangeEndian(value.fielde4);
    ChangeEndian(value.fielde8);
    ChangeEndian(value.fieldec);
}

void EndianSwap(MOH_mechanicEnvMod_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field74);
    ChangeEndian(value.field78);
}

void EndianSwap(MOH_enemy_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field7e);
    ChangeEndian(value.field80);
    ChangeEndian(value.field82);
    ChangeEndian(value.field84);
    ChangeEndian(value.field86);
    ChangeEndian(value.field88);
    ChangeEndian(value.field8a);
    ChangeEndian(value.field8c);
    ChangeEndian(value.field8e);
    ChangeEndian(value.field90);
    ChangeEndian(value.field94);
    ChangeEndian(value.field98);
    ChangeEndian(value.field9c);
    ChangeEndian(value.fielda0);
    ChangeEndian(value.fielda4);
    ChangeEndian(value.fielda8);
    ChangeEndian(value.fieldac);
}

void EndianSwap(MOH_mechanic_Struct& value) {
    EndianSwap(value.core);
    ChangeEndian(value.field74);
}

void EndianSwap(MOH_core_Struct& value) {
    ChangeEndian(value.field2c);
    ChangeEndian(value.field2e);
    ChangeEndian(value.field30);
    ChangeEndian(value.field32);
    ChangeEndian(value.field36);
    ChangeEndian(value.field38);
    ChangeEndian(value.field3a);
    ChangeEndian(value.field3c);
    ChangeEndian(value.field40);
    ChangeEndian(value.field44);
    ChangeEndian(value.field48);
    ChangeEndian(value.field4c);
    ChangeEndian(value.field50);
    ChangeEndian(value.field54);
    ChangeEndian(value.field58);
    ChangeEndian(value.field5c);
    ChangeEndian(value.field60);
    ChangeEndian(value.field64);
    ChangeEndian(value.field68);
}

void EndianSwap(xyzProperty_Struct& value) {
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
}

int SearchForClosestMachineGun(ISceneNode* node, float maxDistance) {
    float closest = 10000.0f;
    int found = -1;
    for (unsigned int i = 0; i < 32; i++) {
        TriggerObject_struct* mechanic = g_pMGTriggerObject[i].mechanic;
        if (mechanic) {
            CVector3 position;
            node->GetPosition(position);
            float dx = position.x - mechanic->core->position[0];
            float dy = position.y - mechanic->core->position[1];
            float dz = position.z - mechanic->core->position[2];
            float distance = sqrtf(dx * dx + dy * dy + dz * dz);
            if (distance < closest && distance < maxDistance) {
                closest = distance;
                found = i;
            }
        }
    }
    return found;
}

bool IsMGUsed(TriggerObject_struct* trigger) {
    return trigger->flags & 1;
}

void MarkMGAsUsed(TriggerObject_struct* trigger, bool used) {
    if (used)
        trigger->flags |= 1;
    else
        trigger->flags &= ~1;
}

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

void RestartPlayer(int player) {
    if (g_numPlayerStarts == 0) {
        TriggerObject_struct* trigger = g_pTriggerObjects;
        for (unsigned int i = 0; i < g_pNumTriggers; i++, trigger++) {
            MOH_core_Struct* core = trigger->core;
            if (core->field0c == 2 && core->field48 == 0x271BD574)
                g_playerStartIndices[g_numPlayerStarts++] = i;
        }
    }
    for (int i = 0; i < 15; i++) {
        if (i < g_numPlayerStarts)
            g_takenPlayerStarts[i] = false;
        else
            g_takenPlayerStarts[i] = true;
    }
    for (int i = 0; i < g_numPlayerStarts; i++) {
        MOH_core_Struct* core = g_pTriggerObjects[g_playerStartIndices[i]].core;
        CVector3 start(core->position[0], core->position[1], core->position[2]);
        for (int p = 0; p < g_NumPlayers; p++) {
            CVector3 position;
            ((IMovingSceneNode*)g_scene.GetPlayer(p))->GetPosition(position);
            CVector3 delta(position);
            delta -= start;
            if (delta.Length() < 10.0f) {
                g_takenPlayerStarts[i] = true;
                break;
            }
        }
    }
    int first = rand() % 15;
    for (int i = 0; i < 15; i++) {
        int index = (first + i) % 15;
        if (!g_takenPlayerStarts[index]) {
            TriggerObject_struct* trigger = &g_pTriggerObjects[g_playerStartIndices[index]];
            CMatrix matrix;
            trigger->core->field1c.GetMatrix(matrix);
            matrix.SetPos(CVector3(trigger->core->position[0], trigger->core->position[1], trigger->core->position[2]));
            ((IMovingSceneNode*)g_scene.GetPlayer(player))->SetTMLocalToWorld(matrix);
            return;
        }
    }
}

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

// A template so that its offsetPtr instances are requested where it is first
// used: as a plain inline function defined here it requests offsetPtr<float>
// and <unsigned long> first, and the image places those copies after the
// light volumes' (the instances are emitted in reverse request order).
template <class T> inline void PatchUpAnimatedLightArrays(T* light, int base) {
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

void LoadPropertyData(char* name, bool reload) {
    if (!reload) {
        int size;
        g_pBPDHeader = static_cast<BPDHeader*>(TLT_LoadFileFromLevelBigFile(name, &size));
        EndianSwap(*g_pBPDHeader);
        g_pNumTriggers = g_pBPDHeader->field2c;
        PatchUpAllPropertyData(g_pBPDHeader);
        g_aigAIFilterGlobalObject.CreateSplinePathManager(g_pBPDHeader, g_pBPDHeader->paths, g_pBPDHeader->pathCount);
        g_aigAIFilterGlobalObject.CreateAStarPathNodeList(g_pBPDHeader, g_pBPDHeader->nodes, g_pBPDHeader->nodeCount);
        g_pRawPropertyData = g_pBPDHeader->field28;
        g_pLightVolumes = g_pBPDHeader->field40;
    } else {
        g_aigAIFilterGlobalObject.ResetSplinePathManager(g_pBPDHeader, g_pBPDHeader->paths, g_pBPDHeader->pathCount);
    }
    InitTriggers();
    for (int i = 0; i < 32; i++) {
        g_pMGTriggerObject[i].waypoint = 0;
        g_pMGTriggerObject[i].mechanic = 0;
        g_pMGTriggerObject[i].field08 = 0;
    }
    FindAllMGPointsInBPDFile();
}
