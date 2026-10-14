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

class CStaticMesh {
public:
    virtual ~CStaticMesh();
};

struct MotionData;

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

struct StaticObjectTemplate {
    MESHSLOTVIEW m_meshSlot;
    int m_id;
    unsigned char unknown18[4];
    MotionData* m_motionData;
    unsigned char unknown20[4];
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

};

class CThrownObject {
public:
    void SetDeleted(bool);

    unsigned char unknown000[756];
    unsigned char unknown2f4 : 3;
    unsigned char m_flag2f4_3 : 1;
};

extern StaticObjectSlotView* g_pStaticObjectArray;
extern HierObjectSlotView* g_pHierObjectArray;
extern StaticAIObjectSlotView* g_pStaticAIObjectArray;
extern AttachableObjectSlotView* g_pAttachableObjectArray;
extern ThrownObjectSlotView* g_pThrownObjectArray;
extern StaticObjectSlotView* g_pStaticObjectFreeList;
extern HierObjectSlotView* g_pHierObjectFreeList;
extern StaticAIObjectSlotView* g_pStaticAIObjectFreeList;
extern AttachableObjectSlotView* g_pAttachableObjectFreeList;
extern ThrownObjectSlotView* g_pThrownObjectFreeList;
extern int g_iStaticObjectPoolSize;
extern int g_iHierObjectPoolSize;
extern int g_iAttachObjectPoolSize;
extern int g_iMaxThrownObjects;
extern int g_iStaticAIObjectPoolSize;
extern int g_iRecycledThrownObjectsIndex;
extern int g_currentAlternateAllocatorArrayIndex;
extern void** g_ppAllocatorFreeLists[];
extern CStaticObject* g_pStaticObjectGrenade;
extern CStaticObject* g_pStaticObjectStickGrenade;
extern CStaticObject* g_pStaticObjectKnife;
extern CStaticObject* g_pStaticObjectWrench;
extern CStaticObject* g_pStaticObjectBazooka;

class CStaticObjectFactory {
public:
    static void Reset();
    static void DeallocateStaticObject(CStaticObject*);
    static CStaticObject* AllocateStaticObject(int, int);
    static bool DoesHelmetTypeStopBullets(int);
    static MotionData* GetMotionData(int, int);
    static CStaticMesh* GetStaticMesh(int, int);
};

extern int g_numTemplates;
extern StaticObjectTemplate* g_pStaticObjectTemplates;

int CompareStaticObjectDefinitions(const void*, const void*);

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

void DebugMsg(const char*, ...);

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
