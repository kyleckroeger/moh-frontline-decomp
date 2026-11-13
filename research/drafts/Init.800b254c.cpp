// forms1 (2026-11-12): the variant motion names are read through the LevelField view (accepted
// pointer-cast spelling, as AnimObjFactory_head.cpp) instead of the declined byte-offset spelling;
// same 138 real lines, the DELPTRCOND condition/object register ranks remain the knot.
/* CStaticObjectFactory::Reset, GetMotionData, GetStaticMesh and
   DeallocateStaticObject (a fragment of staticobjfactory.cpp). Reset links each object pool into its free list
   (static, hierarchy, attachable, thrown and static AI objects; the next
   pointer at +36 of each slot; the thrown and static AI loops run to the pool
   size), clears the recycling and allocator indices, points the allocator
   free-list table at three of the free lists and allocates the shared grenade,
   stick grenade, knife, wrench and bazooka objects by id. GetMotionData and
   GetStaticMesh binary-search the sorted templates by id and variant
   and return the template's motion data or its mesh, if loaded.
   DeallocateStaticObject reports the object and returns it to the free list
   of its kind by object type: a plain static object to the pool whose array
   holds it, an attachable object, a thrown object (or marks it deleted when
   its flag at +0x2f4 bit 4 is set) and a hierarchy object (the static AI
   list when it is a hierarchy tank object). The virtual-table views follow
   the original tables (GetObjectType at +300, AsThrownObject at +320,
   AsHierTankObject at +328); the flag, the message's local and the case
   order are inferred, and the message is a .rodata pool entry.
   StaticObjectTemplate, CStaticObjectFactory and CStaticMesh are named by the
   mangled symbols; the template's members are inferred from offsets (a mesh
   slot whose index is -1 when empty, the object id at +20, motion data at +28,
   definition data at +36 and the variant at +40; the embedded mesh's size is
   not known). g_numTemplates and g_pStaticObjectTemplates are file-local in
   the original and are declared here as externals. The weak template
   destructor compiled here is a duplicate: the original linker kept the copy
   that follows DoesHelmetTypeStopBullets. The pool slot views (their sizes
   from the strides, the link at +36) and the globals' types are inferred. */
extern "C" void* bsearch(const void*, const void*, unsigned long, unsigned long, int (*)(const void*, const void*));

inline void* operator new(unsigned long, void* p) { return p; }
class CDrawContext;
class CStaticMesh {
public:
    CStaticMesh() {
        m_data = 0;
        m_lighting = true;
        m_fogging = true;
    }
    virtual ~CStaticMesh();
    virtual void Draw(CDrawContext&, int);
    virtual void EnableLighting(bool enable) { m_lighting = enable; }
    virtual bool IsLightingEnabled() { return m_lighting; }
    virtual void EnableFogging(bool enable) { m_fogging = enable; }
    virtual bool IsFoggingEnabled() { return m_fogging; }
    void Init(void*, bool);

    void* m_data;
    bool m_lighting : 1;
    bool m_fogging : 1;
};

class CStaticObject;

struct MotionFrameView;

/* inferred: motion data with its frames inline from +16 */
struct MotionData {
    int data00;
    int data04;
    int data08;
    int data0c;
    unsigned char m_frames[16];
};

/* inferred views */
struct HELMETVIEW {
    unsigned char unknown000[164];
    int field164;
};

struct TEMPLATEDATAVIEW {
    unsigned char unknown0[8];
    HELMETVIEW* helmet;
};

/* inferred: the template's mesh slot (an index, -1 when empty, and storage
   for the mesh); its own name is not known */
struct MESHSLOTVIEW {
    MESHSLOTVIEW() { m_index = -1; }
    ~MESHSLOTVIEW() {
        if (Get())
            Get()->~CStaticMesh();
        m_index = -1;
    }
    CStaticMesh* Get() { return m_index >= 0 ? (CStaticMesh*)m_mesh : 0; }

    int m_index;
    unsigned char m_mesh[16];
};

class CStaticAnimMesh : public CStaticMesh {
public:
    CStaticAnimMesh() { m_anim = 0; }
    virtual ~CStaticAnimMesh();
    virtual void Draw(CDrawContext&, int);
    void Init(void*, bool);
    int numFrames() const;
    int ticksPerFrame() const;

    void* m_anim;
};

/* inferred: the object definition the template's level contents point at */
struct ObjectDefinitionView {
    unsigned char unknown00[20];
    int m_hasAI;
    unsigned char unknown18[52];
    int m_recycled;
};

struct LevelFileContentsStruct_ {
    int data00;
    int data04;
    ObjectDefinitionView* m_definition;
};

struct StaticObjectTemplate {
    CStaticMesh* GetMesh() { if (m_meshSlot.m_index >= 0) return (CStaticMesh*)m_meshSlot.m_mesh; return 0; }
    bool IsAnimated() const { return m_meshSlot.m_index >= 0 && (m_meshSlot.m_index & 2); }
    int NumFrames() { if (m_meshSlot.m_index >= 0 && (m_meshSlot.m_index & 2)) return ((CStaticAnimMesh*)&m_meshSlot.m_mesh)->numFrames(); return 0; }
    int TicksPerFrame() { if (m_meshSlot.m_index >= 0 && (m_meshSlot.m_index & 2)) return ((CStaticAnimMesh*)&m_meshSlot.m_mesh)->ticksPerFrame(); return 0; }
    void CreateAnimMesh(void* data, bool flag) {
        CStaticAnimMesh* mesh = (CStaticAnimMesh*)m_meshSlot.m_mesh;
        m_meshSlot.m_index = 2;
        new (mesh) CStaticAnimMesh;
        mesh->Init(data, flag);
    }
    void CreateStaticMesh(void* data, bool flag) {
        CStaticMesh* mesh = (CStaticMesh*)m_meshSlot.m_mesh;
        m_meshSlot.m_index = 0;
        new (mesh) CStaticMesh;
        mesh->Init(data, flag);
    }

    MESHSLOTVIEW m_meshSlot;
    int m_id;
    int m_flags;
    MotionData* m_motionData;
    LevelFileContentsStruct_* m_contents;
    TEMPLATEDATAVIEW* m_data;
    int m_variant;
};

struct StaticObjectSlotView {
    unsigned char unknown000[36];
    StaticObjectSlotView* m_next;
    unsigned char unknown028[600];
};

struct HierObjectSlotView {
    unsigned char unknown000[36];
    HierObjectSlotView* m_next;
    unsigned char unknown028[1816];
};

struct AttachableObjectSlotView {
    unsigned char unknown000[36];
    AttachableObjectSlotView* m_next;
    unsigned char unknown028[1368];
};

struct ThrownObjectSlotView {
    unsigned char unknown000[36];
    ThrownObjectSlotView* m_next;
    unsigned char unknown028[728];
};

struct StaticAIObjectSlotView {
    unsigned char unknown000[36];
    StaticAIObjectSlotView* m_next;
    unsigned char unknown028[4056];
};

class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CPlayerObject;
class CLight;
class BSObject;
class BPDLightVolume;
struct AIDoodadView;
enum EClsnId {};
enum EBSEventEnum {};

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CThrownObject;
class CStaticObject;

/* inferred virtual-table views (slot order from the original tables) */
class ISceneNode {
public:
    enum EVolumeType {};
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
    virtual CStaticObject* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
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
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class IMovingSceneNode : public ISceneNode {
public:
    virtual ~IMovingSceneNode();
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
};

class CStaticObject : public IMovingSceneNode {
public:
    struct MotionFrame;
    CStaticObject();
    CStaticObject(CStaticMesh*, int, int, int, int, int, MotionFrame*);
    virtual ~CStaticObject();
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();
    virtual int GetWorldLinearVelocity() const;
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual int GetLightVolume();
    virtual void Init();
    virtual void SetScript(BSObject*);
    virtual int GetObjectType() const;
    virtual void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void ScaleFromScript(float, float, EBSEventEnum);
    virtual void StartMotionPlayback(int, int, EBSEventEnum);
    virtual CThrownObject* AsThrownObject();
    virtual void* AsWeaponObject();
    virtual void* AsHierTankObject();
    virtual void SetCollisionId(EClsnId);

    unsigned char unknown004[636];
};

class CThrownObject {
public:
    CThrownObject();
    ~CThrownObject();
    CThrownObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*, LevelFileContentsStruct_*);
    void SetDeleted(bool);

    unsigned char unknown000[756];
    unsigned char unknown2f4 : 3;
    unsigned char m_flag2f4_3 : 1;
    unsigned char m_deleted : 1;
    unsigned char unknown2f5[11];
};

/* inferred: a recycled thrown object (768 bytes, the deleted flag at +756) */
class RecycledThrownView {
public:
    virtual void MarkForDestruction(int);

    unsigned char unknown004[752];
    unsigned char unknown2f4 : 4;
    unsigned char m_deleted : 1;
    unsigned char unknown2f5[11];
};

class CStaticMesh;
class CTankObject {
public:
    CTankObject();
    ~CTankObject();
    unsigned char unknown[4096];
    CTankObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
};
class CHierObject {
public:
    CHierObject();
    ~CHierObject();
    unsigned char unknown[1856];
    CHierObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
};
class CAttachableObject {
public:
    CAttachableObject();
    ~CAttachableObject();
    unsigned char unknown[1408];
    CAttachableObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
};

void DestroyOldestDumpObject();

extern CStaticObject* g_pStaticObjectGrenade;
extern CStaticObject* g_pStaticObjectStickGrenade;
extern CStaticObject* g_pStaticObjectKnife;
extern CStaticObject* g_pStaticObjectWrench;
extern CStaticObject* g_pStaticObjectBazooka;

class CStaticObjectFactory {
public:
    static void Init();
    static void PatchUpMVDFile(MotionData*);
    static void Reset();
    static void DeallocateStaticObject(CStaticObject*);
    static CStaticObject* AllocateStaticObject(int, int);
    static bool DoesHelmetTypeStopBullets(int);
    static MotionData* GetMotionData(int, int);
    static CStaticMesh* GetStaticMesh(int, int);
};


static int g_iStaticAIObjectPoolSize;
static int g_iStaticObjectPoolSize;
static int g_iAttachObjectPoolSize;
static int g_iHierObjectPoolSize;
static int g_numTemplates;
static int g_iMaxThrownObjects;
static int g_iMaxRecycledThrownObjects;
static int g_iRecycledThrownObjectsIndex;
static StaticObjectTemplate* g_pStaticObjectTemplates;
static int g_currentAlternateAllocatorArrayIndex;
static StaticObjectSlotView* g_pStaticObjectArray;
static HierObjectSlotView* g_pHierObjectArray;
static HierObjectSlotView* g_pHierObjectFreeList;
static StaticAIObjectSlotView* g_pStaticAIObjectArray;
static StaticAIObjectSlotView* g_pStaticAIObjectFreeList;
static AttachableObjectSlotView* g_pAttachableObjectArray;
static ThrownObjectSlotView* g_pThrownObjectArray;
static RecycledThrownView* g_pRecycledThrownObjectArray;
static StaticObjectSlotView* g_pStaticObjectFreeList;
static AttachableObjectSlotView* g_pAttachableObjectFreeList;
static ThrownObjectSlotView* g_pThrownObjectFreeList;
static void** g_ppAllocatorFreeLists[3];

int CompareStaticObjectDefinitions(const void*, const void*);

namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}
void DebugMsg(const char*, ...);


enum TLTResourceID {};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
LevelFileContentsStruct_* TLT_FindNextResourceByTypeAndVal(TLTResourceID, int, int, LevelFileContentsStruct_*);
void* TLT_LoadFromRepository(const char*, int*, bool*);
extern "C" void qsort(void*, unsigned long, unsigned long, int (*)(const void*, const void*));

struct StaticDefView {
    int data00;
    const char* m_meshName;
    int data08;
    int m_id;
    int data10;
    int m_flags;
    int data18;
    int m_noLighting;
    int data20;
    int m_noFog;
    int data28;
    const char* m_motionName;
};

struct LevelDefView {
    int data00;
    int data04;
    int data08;
    int data0c;
    int data10;
    const char* m_meshName;
    unsigned char unknown18[36];
    int m_id;
};

struct AIDefView {
    unsigned char unknown00[60];
    const char* m_meshName;
    unsigned char unknown40[52];
    int m_id;
};

struct VariantEntryView {
    const char* m_meshName;
    int data04;
    int m_flags;
    int data0c;
    const char* m_motionName;
    int data14;
};

struct VariantDefView {
    int data00;
    int m_count;
    int data08;
    int m_id;
    int data10;
    int data14;
    int data18;
    int m_noLighting;
    int data20;
    int m_noFog;
    int data28;
    VariantEntryView m_entries[1];
};

struct ThrownSettingsView {
    unsigned char unknown000[468];
    int m_maxRecycledThrown;
};

struct PoolSettingsView {
    unsigned char unknown000[484];
    int m_staticPool;
    int data1e8;
    int m_attachPool;
    int data1f0;
    int m_hierPool;
    int data1f8;
    int m_staticAIPool;
    int data200;
    int m_thrownPool;
};

/* Inferred: a level contents field (a tag word, then the value), as in
   AnimObjFactory_head.cpp; a variant's motion names are read through it. */
struct LevelField {
    int tag;
    union {
        int i;
        float f;
        const char* s;
    };
};

struct LevelFileInfo {
    LevelField fields[27];
};

#define DEF(type, r) ((type*)(r)->m_definition)

void CStaticObjectFactory::Init() {
    LevelFileContentsStruct_* r;
    for (r = TLT_FindNextResourceByType((TLTResourceID)10, 0); r; r = TLT_FindNextResourceByType((TLTResourceID)10, r))
        if (DEF(StaticDefView, r)->m_meshName)
            g_numTemplates++;
    for (r = TLT_FindNextResourceByType((TLTResourceID)19, 0); r; r = TLT_FindNextResourceByType((TLTResourceID)19, r))
        g_numTemplates++;
    for (r = TLT_FindNextResourceByType((TLTResourceID)9, 0); r; r = TLT_FindNextResourceByType((TLTResourceID)9, r))
        if (DEF(AIDefView, r)->m_meshName)
            g_numTemplates++;
    for (r = TLT_FindNextResourceByType((TLTResourceID)21, 0); r; r = TLT_FindNextResourceByType((TLTResourceID)21, r))
        g_numTemplates += DEF(VariantDefView, r)->m_count;
    if (g_numTemplates > 0) {
        g_pStaticObjectTemplates = new StaticObjectTemplate[g_numTemplates];
        int count = 0;
        for (r = TLT_FindNextResourceByType((TLTResourceID)10, 0); r; r = TLT_FindNextResourceByType((TLTResourceID)10, r)) {
            if (DEF(StaticDefView, r)->m_meshName) {
                g_pStaticObjectTemplates[count].m_id = DEF(StaticDefView, r)->m_id;
                g_pStaticObjectTemplates[count].m_flags = DEF(StaticDefView, r)->m_flags;
                g_pStaticObjectTemplates[count].m_contents = r;
                g_pStaticObjectTemplates[count].m_variant = -1;
                if (DEF(StaticDefView, r)->m_motionName) {
                    int size = 0;
                    bool cached = false;
                    g_pStaticObjectTemplates[count].m_motionData =
                        (MotionData*)TLT_LoadFromRepository(DEF(StaticDefView, r)->m_motionName, &size, &cached);
                    if (!cached)
                        PatchUpMVDFile(g_pStaticObjectTemplates[count].m_motionData);
                } else {
                    g_pStaticObjectTemplates[count].m_motionData = 0;
                }
                int size;
                bool cached;
                void* data = TLT_LoadFromRepository(DEF(StaticDefView, r)->m_meshName, &size, &cached);
                if (g_pStaticObjectTemplates[count].m_flags & 2)
                    g_pStaticObjectTemplates[count].CreateAnimMesh(data, !cached);
                else
                    g_pStaticObjectTemplates[count].CreateStaticMesh(data, !cached);
                if (DEF(StaticDefView, r)->m_noLighting)
                    g_pStaticObjectTemplates[count].GetMesh()->EnableLighting(false);
                if (DEF(StaticDefView, r)->m_noFog)
                    g_pStaticObjectTemplates[count].GetMesh()->EnableFogging(false);
                count++;
            }
        }
        r = TLT_FindNextResourceByType((TLTResourceID)19, 0);
        while (r) {
            g_pStaticObjectTemplates[count].m_id = DEF(LevelDefView, r)->m_id;
            g_pStaticObjectTemplates[count].m_flags = 0;
            g_pStaticObjectTemplates[count].m_motionData = 0;
            g_pStaticObjectTemplates[count].m_contents = r;
            g_pStaticObjectTemplates[count].m_variant = -1;
            int size;
            bool cached;
            void* data = TLT_LoadFromRepository(DEF(LevelDefView, r)->m_meshName, &size, &cached);
            int flags = g_pStaticObjectTemplates[count].m_flags;
            DebugMsg("Loading static mesh file %s\n", DEF(LevelDefView, r)->m_meshName);
            if (flags & 2)
                g_pStaticObjectTemplates[count].CreateAnimMesh(data, !cached);
            else
                g_pStaticObjectTemplates[count].CreateStaticMesh(data, !cached);
            r = TLT_FindNextResourceByType((TLTResourceID)19, r);
            count++;
        }
        LevelFileContentsStruct_* thrown = TLT_FindNextResourceByTypeAndVal((TLTResourceID)10, 1, 0x7d06eb90, 0);
        for (r = TLT_FindNextResourceByType((TLTResourceID)9, 0); r; r = TLT_FindNextResourceByType((TLTResourceID)9, r)) {
            if (DEF(AIDefView, r)->m_meshName) {
                g_pStaticObjectTemplates[count].m_id = DEF(AIDefView, r)->m_id;
                g_pStaticObjectTemplates[count].m_flags = 4;
                g_pStaticObjectTemplates[count].m_motionData = 0;
                g_pStaticObjectTemplates[count].m_contents = thrown;
                g_pStaticObjectTemplates[count].m_data = (TEMPLATEDATAVIEW*)r;
                g_pStaticObjectTemplates[count].m_variant = -1;
                int size;
                bool cached;
                void* data = TLT_LoadFromRepository(DEF(AIDefView, r)->m_meshName, &size, &cached);
                g_pStaticObjectTemplates[count].CreateStaticMesh(data, !cached);
                count++;
            }
        }
        for (r = TLT_FindNextResourceByType((TLTResourceID)21, 0); r; r = TLT_FindNextResourceByType((TLTResourceID)21, r)) {
            for (int i = 0; i < DEF(VariantDefView, r)->m_count; i++) {
                g_pStaticObjectTemplates[count].m_id = DEF(VariantDefView, r)->m_id;
                g_pStaticObjectTemplates[count].m_flags = DEF(VariantDefView, r)->m_entries[i].m_flags;
                g_pStaticObjectTemplates[count].m_flags |= 8;
                g_pStaticObjectTemplates[count].m_contents = r;
                g_pStaticObjectTemplates[count].m_variant = i;
                if (((const char**)&((LevelFileInfo*)(r)->m_definition)->fields[i * 3 + 7])[1]) {
                    int size = 0;
                    bool cached = false;
                    g_pStaticObjectTemplates[count].m_motionData =
                        (MotionData*)TLT_LoadFromRepository(((const char**)&((LevelFileInfo*)(r)->m_definition)->fields[i * 3 + 7])[1], &size, &cached);
                    if (!cached)
                        PatchUpMVDFile(g_pStaticObjectTemplates[count].m_motionData);
                } else {
                    g_pStaticObjectTemplates[count].m_motionData = 0;
                }
                int size;
                bool cached;
                void* data = TLT_LoadFromRepository(DEF(VariantDefView, r)->m_entries[i].m_meshName, &size, &cached);
                if (g_pStaticObjectTemplates[count].m_flags & 2)
                    g_pStaticObjectTemplates[count].CreateAnimMesh(data, !cached);
                else
                    g_pStaticObjectTemplates[count].CreateStaticMesh(data, !cached);
                if (DEF(VariantDefView, r)->m_noLighting)
                    g_pStaticObjectTemplates[count].GetMesh()->EnableLighting(false);
                if (DEF(VariantDefView, r)->m_noFog)
                    g_pStaticObjectTemplates[count].GetMesh()->EnableFogging(false);
                count++;
            }
        }
    }
    g_iMaxRecycledThrownObjects = DEF(ThrownSettingsView, TLT_FindNextResourceByType((TLTResourceID)11, 0))->m_maxRecycledThrown;
    LevelFileContentsStruct_* pools = TLT_FindNextResourceByType((TLTResourceID)13, 0);
    g_iStaticObjectPoolSize = DEF(PoolSettingsView, pools)->m_staticPool;
    g_iAttachObjectPoolSize = DEF(PoolSettingsView, pools)->m_attachPool;
    g_iHierObjectPoolSize = DEF(PoolSettingsView, pools)->m_hierPool;
    g_iStaticAIObjectPoolSize = DEF(PoolSettingsView, pools)->m_staticAIPool;
    g_iMaxThrownObjects = DEF(PoolSettingsView, pools)->m_thrownPool;
    g_pStaticObjectArray = (StaticObjectSlotView*)new CStaticObject[g_iStaticObjectPoolSize];
    g_pHierObjectArray = (HierObjectSlotView*)new CHierObject[g_iHierObjectPoolSize];
    g_pStaticAIObjectArray = (StaticAIObjectSlotView*)new CTankObject[g_iStaticAIObjectPoolSize];
    g_pAttachableObjectArray = (AttachableObjectSlotView*)new CAttachableObject[g_iAttachObjectPoolSize];
    g_pThrownObjectArray = (ThrownObjectSlotView*)new CThrownObject[g_iMaxThrownObjects];
    g_pRecycledThrownObjectArray = (RecycledThrownView*)new CThrownObject[g_iMaxRecycledThrownObjects];
    g_pStaticObjectFreeList = g_pStaticObjectArray;
    g_pHierObjectFreeList = g_pHierObjectArray;
    g_pStaticAIObjectFreeList = g_pStaticAIObjectArray;
    g_pAttachableObjectFreeList = g_pAttachableObjectArray;
    g_pThrownObjectFreeList = g_pThrownObjectArray;
    for (int i = 0; i < g_iStaticObjectPoolSize - 1; i++)
        g_pStaticObjectArray[i].m_next = &g_pStaticObjectArray[i + 1];
    g_pStaticObjectArray[g_iStaticObjectPoolSize - 1].m_next = 0;
    for (int i = 0; i < g_iHierObjectPoolSize - 1; i++)
        g_pHierObjectArray[i].m_next = &g_pHierObjectArray[i + 1];
    g_pHierObjectArray[g_iHierObjectPoolSize - 1].m_next = 0;
    for (int i = 0; i < g_iAttachObjectPoolSize - 1; i++)
        g_pAttachableObjectArray[i].m_next = &g_pAttachableObjectArray[i + 1];
    g_pAttachableObjectArray[g_iAttachObjectPoolSize - 1].m_next = 0;
    for (int i = 0; i < g_iMaxThrownObjects; i++)
        g_pThrownObjectArray[i].m_next = &g_pThrownObjectArray[i + 1];
    g_pThrownObjectArray[g_iMaxThrownObjects - 1].m_next = 0;
    for (int i = 0; i < g_iStaticAIObjectPoolSize; i++)
        g_pStaticAIObjectArray[i].m_next = &g_pStaticAIObjectArray[i + 1];
    g_pStaticAIObjectArray[g_iStaticAIObjectPoolSize - 1].m_next = 0;
    qsort(g_pStaticObjectTemplates, g_numTemplates, sizeof(StaticObjectTemplate), CompareStaticObjectDefinitions);
    g_currentAlternateAllocatorArrayIndex = 0;
    g_ppAllocatorFreeLists[0] = (void**)&g_pHierObjectFreeList;
    g_ppAllocatorFreeLists[1] = (void**)&g_pAttachableObjectFreeList;
    g_ppAllocatorFreeLists[2] = (void**)&g_pThrownObjectFreeList;
    g_pStaticObjectGrenade = AllocateStaticObject(0xa55dda2d, -1);
    g_pStaticObjectStickGrenade = AllocateStaticObject(0x83674487, -1);
    g_pStaticObjectKnife = AllocateStaticObject(0x7b81b72c, -1);
    g_pStaticObjectWrench = AllocateStaticObject(0x60531a56, -1);
    g_pStaticObjectBazooka = AllocateStaticObject(0xd23d51cf, -1);
}

void CStaticObjectFactory::Reset() {
    g_pStaticObjectFreeList = g_pStaticObjectArray;
    g_pHierObjectFreeList = g_pHierObjectArray;
    g_pStaticAIObjectFreeList = g_pStaticAIObjectArray;
    g_pAttachableObjectFreeList = g_pAttachableObjectArray;
    g_pThrownObjectFreeList = g_pThrownObjectArray;
    for (int i = 0; i < g_iStaticObjectPoolSize - 1; i++)
        g_pStaticObjectArray[i].m_next = &g_pStaticObjectArray[i + 1];
    g_pStaticObjectArray[g_iStaticObjectPoolSize - 1].m_next = 0;
    for (int i = 0; i < g_iHierObjectPoolSize - 1; i++)
        g_pHierObjectArray[i].m_next = &g_pHierObjectArray[i + 1];
    g_pHierObjectArray[g_iHierObjectPoolSize - 1].m_next = 0;
    for (int i = 0; i < g_iAttachObjectPoolSize - 1; i++)
        g_pAttachableObjectArray[i].m_next = &g_pAttachableObjectArray[i + 1];
    g_pAttachableObjectArray[g_iAttachObjectPoolSize - 1].m_next = 0;
    for (int i = 0; i < g_iMaxThrownObjects; i++)
        g_pThrownObjectArray[i].m_next = &g_pThrownObjectArray[i + 1];
    g_pThrownObjectArray[g_iMaxThrownObjects - 1].m_next = 0;
    for (int i = 0; i < g_iStaticAIObjectPoolSize; i++)
        g_pStaticAIObjectArray[i].m_next = &g_pStaticAIObjectArray[i + 1];
    g_pStaticAIObjectArray[g_iStaticAIObjectPoolSize - 1].m_next = 0;
    g_iRecycledThrownObjectsIndex = 0;
    g_currentAlternateAllocatorArrayIndex = 0;
    g_ppAllocatorFreeLists[0] = (void**)&g_pHierObjectFreeList;
    g_ppAllocatorFreeLists[1] = (void**)&g_pAttachableObjectFreeList;
    g_ppAllocatorFreeLists[2] = (void**)&g_pThrownObjectFreeList;
    g_pStaticObjectGrenade = AllocateStaticObject(0xa55dda2d, -1);
    g_pStaticObjectStickGrenade = AllocateStaticObject(0x83674487, -1);
    g_pStaticObjectKnife = AllocateStaticObject(0x7b81b72c, -1);
    g_pStaticObjectWrench = AllocateStaticObject(0x60531a56, -1);
    g_pStaticObjectBazooka = AllocateStaticObject(0xd23d51cf, -1);
}

MotionData* CStaticObjectFactory::GetMotionData(int id, int variant) {
    StaticObjectTemplate key;
    StaticObjectTemplate* found;

    key.m_id = id;
    key.m_variant = variant;
    found = (StaticObjectTemplate*)bsearch(&key, g_pStaticObjectTemplates, g_numTemplates, sizeof(StaticObjectTemplate), CompareStaticObjectDefinitions);
    if (!found)
        return 0;
    return found->m_motionData;
}

CStaticMesh* CStaticObjectFactory::GetStaticMesh(int id, int variant) {
    StaticObjectTemplate key;
    StaticObjectTemplate* found;

    key.m_id = id;
    key.m_variant = variant;
    found = (StaticObjectTemplate*)bsearch(&key, g_pStaticObjectTemplates, g_numTemplates, sizeof(StaticObjectTemplate), CompareStaticObjectDefinitions);
    if (!found)
        return 0;
    return found->m_meshSlot.Get();
}

void CStaticObjectFactory::DeallocateStaticObject(CStaticObject* object) {
    void* address = object;
    DebugMsg("Deallocating static object %x <<<d %x>>>\n", address, address);
    switch (object->GetObjectType()) {
    case 8:
        if (object->AsHierTankObject()) {
            ((StaticAIObjectSlotView*)object)->m_next = g_pStaticAIObjectFreeList;
            g_pStaticAIObjectFreeList = (StaticAIObjectSlotView*)object;
        } else {
            ((HierObjectSlotView*)object)->m_next = g_pHierObjectFreeList;
            g_pHierObjectFreeList = (HierObjectSlotView*)object;
        }
        break;
    case 0:
        if ((StaticObjectSlotView*)object >= g_pStaticObjectArray &&
            (StaticObjectSlotView*)object < g_pStaticObjectArray + g_iStaticObjectPoolSize) {
            ((StaticObjectSlotView*)object)->m_next = g_pStaticObjectFreeList;
            g_pStaticObjectFreeList = (StaticObjectSlotView*)object;
        } else if ((AttachableObjectSlotView*)object >= g_pAttachableObjectArray &&
                   (AttachableObjectSlotView*)object < g_pAttachableObjectArray + g_iAttachObjectPoolSize) {
            ((AttachableObjectSlotView*)object)->m_next = g_pAttachableObjectFreeList;
            g_pAttachableObjectFreeList = (AttachableObjectSlotView*)object;
        } else if ((ThrownObjectSlotView*)object >= g_pThrownObjectArray &&
                   (ThrownObjectSlotView*)object < g_pThrownObjectArray + g_iMaxThrownObjects) {
            ((ThrownObjectSlotView*)object)->m_next = g_pThrownObjectFreeList;
            g_pThrownObjectFreeList = (ThrownObjectSlotView*)object;
        } else if ((HierObjectSlotView*)object >= g_pHierObjectArray &&
                   (HierObjectSlotView*)object < g_pHierObjectArray + g_iHierObjectPoolSize) {
            ((HierObjectSlotView*)object)->m_next = g_pHierObjectFreeList;
            g_pHierObjectFreeList = (HierObjectSlotView*)object;
        }
        break;
    case 1:
        ((AttachableObjectSlotView*)object)->m_next = g_pAttachableObjectFreeList;
        g_pAttachableObjectFreeList = (AttachableObjectSlotView*)object;
        break;
    case 4: {
        CThrownObject* thrown = object->AsThrownObject();
        if (thrown->m_flag2f4_3) {
            thrown->SetDeleted(true);
        } else {
            ((ThrownObjectSlotView*)object)->m_next = g_pThrownObjectFreeList;
            g_pThrownObjectFreeList = (ThrownObjectSlotView*)object;
        }
        break;
    }
    }
}


static inline StaticObjectSlotView* PopFreeSlot() {
    StaticObjectSlotView** list;
    if (g_pStaticObjectFreeList) {
        list = &g_pStaticObjectFreeList;
    } else {
        int index = g_currentAlternateAllocatorArrayIndex;
        if (*g_ppAllocatorFreeLists[index])
            list = (StaticObjectSlotView**)g_ppAllocatorFreeLists[index];
        else if (*g_ppAllocatorFreeLists[(index + 1) % 3])
            list = (StaticObjectSlotView**)g_ppAllocatorFreeLists[(index + 1) % 3];
        else if (*g_ppAllocatorFreeLists[(index + 2) % 3])
            list = (StaticObjectSlotView**)g_ppAllocatorFreeLists[(index + 2) % 3];
        else {
            DestroyOldestDumpObject();
            if (g_pStaticObjectFreeList)
                list = &g_pStaticObjectFreeList;
            else
                return 0;
        }
    }
    StaticObjectSlotView* slot = *list;
    *list = slot->m_next;
    g_currentAlternateAllocatorArrayIndex++;
    g_currentAlternateAllocatorArrayIndex = g_currentAlternateAllocatorArrayIndex >= 3 ? 0 : g_currentAlternateAllocatorArrayIndex;
    return slot;
}

CStaticObject* CStaticObjectFactory::AllocateStaticObject(int id, int variant) {
    StaticObjectTemplate key;
    StaticObjectTemplate* t;
    CStaticObject* object;

    key.m_id = id;
    key.m_variant = variant;
    t = (StaticObjectTemplate*)bsearch(&key, g_pStaticObjectTemplates, g_numTemplates, sizeof(StaticObjectTemplate), CompareStaticObjectDefinitions);
    if (t->m_flags & 8) {
        if (variant == 0 && t->m_contents->m_definition->m_hasAI != 0) {
            StaticAIObjectSlotView* slot = g_pStaticAIObjectFreeList;
            if (!slot) {
                DebugMsg("Static object factory memory is used up.  Unable to allocate hierobject with AI 0x%x\n", id);
                return 0;
            }
            g_pStaticAIObjectFreeList = slot->m_next;
            CTankObject* mem = (CTankObject*)slot;
            DebugMsg("Allocating static object CRC: %x, obj num %d, returned %x <<<a AIObj %x,%x>>>\n", id, variant, slot, id, slot);
            if (t->m_motionData)
                object = (CStaticObject*)new (mem) CTankObject(t->GetMesh(), t->NumFrames(), t->TicksPerFrame(), t->m_motionData->data08, t->m_motionData->data04, t->m_motionData->data0c, (CStaticObject::MotionFrame*)t->m_motionData->m_frames);
            else
                object = (CStaticObject*)new (mem) CTankObject(t->GetMesh(), t->NumFrames(), t->TicksPerFrame(), 0, 0, 0, 0);
        } else {
            if (!g_pHierObjectFreeList) {
                DebugMsg("Static object factory memory is used up.  Unable to allocate hierobject without AI 0x%x\n", id);
                return 0;
            }
            HierObjectSlotView* slot = g_pHierObjectFreeList;
            g_pHierObjectFreeList = slot->m_next;
            DebugMsg("Allocating static object CRC: %x, obj num %d, returned %x <<<a Hier %x,%x>>>\n", id, variant, slot, id, slot);
            if (t->m_motionData)
                object = (CStaticObject*)new (slot) CHierObject(t->GetMesh(), t->NumFrames(), t->TicksPerFrame(), t->m_motionData->data08, t->m_motionData->data04, t->m_motionData->data0c, (CStaticObject::MotionFrame*)t->m_motionData->m_frames);
            else
                object = (CStaticObject*)new (slot) CHierObject(t->GetMesh(), t->NumFrames(), t->TicksPerFrame(), 0, 0, 0, 0);
        }
    } else if (t->m_flags & 1) {
        if (!g_pAttachableObjectFreeList) {
            DebugMsg("Attachable static object factory memory is used up.  Unable to allocate object 0x%x\n", id);
            return 0;
        }
        AttachableObjectSlotView* slot = g_pAttachableObjectFreeList;
        g_pAttachableObjectFreeList = slot->m_next;
        DebugMsg("Allocating static object CRC: %x, obj num %d, returned %x <<<a Attach %x,%x>>>\n", id, variant, slot, id, slot);
        if (t->m_motionData)
            object = (CStaticObject*)new (slot) CAttachableObject(t->GetMesh(), t->NumFrames(), t->TicksPerFrame(), t->m_motionData->data08, t->m_motionData->data04, t->m_motionData->data0c, (CStaticObject::MotionFrame*)t->m_motionData->m_frames);
        else
            object = (CStaticObject*)new (slot) CAttachableObject(t->GetMesh(), t->NumFrames(), t->TicksPerFrame(), 0, 0, 0, 0);
    } else if (t->m_flags & 4) {
        void* slot;
        if (t->m_contents->m_definition->m_recycled != 0) {
            int index = g_iRecycledThrownObjectsIndex % g_iMaxRecycledThrownObjects;
            if (g_iRecycledThrownObjectsIndex >= g_iMaxRecycledThrownObjects) {
                int last = index > 0 ? index - 1 : g_iMaxRecycledThrownObjects - 1;
                while (index != last) {
                    if (g_pRecycledThrownObjectArray[index].m_deleted)
                        break;
                    index = index == g_iMaxRecycledThrownObjects - 1 ? 0 : index + 1;
                    g_iRecycledThrownObjectsIndex++;
                }
                if (index == last) {
                    g_pRecycledThrownObjectArray[index].MarkForDestruction(0);
                    return 0;
                }
            } else {
                g_iRecycledThrownObjectsIndex++;
            }
            slot = &g_pRecycledThrownObjectArray[index];
        } else {
            slot = g_pThrownObjectFreeList;
            g_pThrownObjectFreeList = ((ThrownObjectSlotView*)slot)->m_next;
        }
        DebugMsg("Allocating static object CRC: %x, obj num %d, returned %x <<<a Thrown %x,%x>>>\n", id, variant, slot, id, slot);
        object = (CStaticObject*)new (slot) CThrownObject(t->GetMesh(), t->NumFrames(), t->TicksPerFrame(), 0, 0, 0, 0, t->m_contents);
    } else {
        StaticObjectSlotView* slot;
        slot = PopFreeSlot();
        if (!slot) {
            DebugMsg("Static object factory memory is used up.  Unable to allocate object 0x%x\n", id);
            return 0;
        }
        DebugMsg("Allocating static object CRC: %x, obj num %d, returned %x <<<a Normal %x,%x>>>\n", id, variant, slot, id, slot);
        if (t->m_motionData)
            object = (CStaticObject*)new (slot) CStaticObject(t->GetMesh(), t->NumFrames(), t->TicksPerFrame(), t->m_motionData->data08, t->m_motionData->data04, t->m_motionData->data0c, (CStaticObject::MotionFrame*)t->m_motionData->m_frames);
        else
            object = (CStaticObject*)new (slot) CStaticObject(t->GetMesh(), t->NumFrames(), t->TicksPerFrame(), 0, 0, 0, 0);
    }
    return object;
}

int CompareStaticObjectDefinitions(const void* a, const void* b) {
    const StaticObjectTemplate* x = (const StaticObjectTemplate*)a;
    const StaticObjectTemplate* y = (const StaticObjectTemplate*)b;

    if (x->m_id < y->m_id)
        return -1;
    if (x->m_id > y->m_id)
        return 1;
    if (x->m_variant < y->m_variant)
        return -1;
    return x->m_variant > y->m_variant;
}
