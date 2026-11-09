// objcreate.cpp: removing soldiers and destructable objects, as partial source.
// Views are inferred from these functions. Virtual functions not yet identified
// are called through their table offset, marked "vtable +N".
void DebugMsg(const char*, ...);

class CVector3 {
public:
    float x, y, z;
};

class ISceneNode {
public:
    // vtable +0x34, +0x38: the two bounding volumes; +0x40: GetPosition; +0x60: the script object.
    void** m_table;
};
class BSObject;
typedef void (*GetPositionFn)(ISceneNode*, CVector3*);
typedef BSObject* (*GetScriptObjectFn)(ISceneNode*);
typedef void* (*GetVolumeFn)(ISceneNode*, int);
static inline void* Slot(void* object, int offset) { return *(void**)(*(char**)object + offset); }

class CScene {
public:
    ISceneNode* GetPlayer(int) const;
    void Remove(ISceneNode&);
    unsigned char unknown[0x1000];
};
extern CScene g_scene;
bool BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

// The German list links through +4; a thunk at table +0x10 gives the scene node.
struct BSGO_German {
    void* unknown00;
    BSGO_German* m_next;
    unsigned char unknown08[4];
    void** m_table;
};
typedef ISceneNode* (*GetNodeFn)(BSGO_German*);
extern BSGO_German* g_pGermanList;
extern int g_maxActiveSoldiers;
extern int g_numActiveSoldiers;

// Asks the farthest soldiers the player can't see to destroy themselves, when
// creating 'count' more would pass the limit. The event is 45.
void NonVisDestructFarthestSoldiers(int count) {
    count -= g_maxActiveSoldiers - g_numActiveSoldiers;
    if (count <= 0)
        return;
    DebugMsg("-----The maximum number of soldiers in existance has been reached.\n");
    DebugMsg("-----This number is currently %d.  It is controled by commondata.crp\n", g_maxActiveSoldiers);
    DebugMsg("-----Attempting to remove %d soldiers\n", count);

    ISceneNode* player = g_scene.GetPlayer(0);
    CVector3 playerPos;
    ((GetPositionFn)Slot(player, 0x40))(player, &playerPos);

    ISceneNode* nodes[128];
    float distance[128];
    int found = 0;
    for (BSGO_German* g = g_pGermanList; g; g = g->m_next) {
        ISceneNode* node = ((GetNodeFn)g->m_table[4])(g);
        if (!*((char*)node + 0x3160)) {
            CVector3 pos;
            ((GetPositionFn)Slot(node, 0x40))(node, &pos);
            pos.x -= playerPos.x;
            pos.y -= playerPos.y;
            pos.z -= playerPos.z;
            nodes[found] = node;
            distance[found] = pos.z * pos.z + pos.x * pos.x + pos.y * pos.y;
            found++;
        }
    }

    float limit = 1.0e35f;
    int removed = 0;
    while (removed < count) {
        float best = 0.0f;
        ISceneNode* farthest = 0;
        for (int i = 0; i < found; i++) {
            if (best < distance[i] && distance[i] < limit) {
                farthest = nodes[i];
                best = distance[i];
            }
        }
        if (!farthest) {
            DebugMsg("WARNING:WARNING: I tried to NonVisDestruct %d soldiers because there\n", count);
            DebugMsg("WARNING:WARNING: are too many in existance, but I was only able to\n");
            DebugMsg("WARNING:WARNING: NonVisDestruct %d soldiers.  I'll try again later\n", removed);
            return;
        }
        limit = best;
        BSObject* script = ((GetScriptObjectFn)Slot(farthest, 0x60))(farthest);
        if (BSObjectTriggerEvent(script, 45, 0, 0, true)) {
            removed++;
            DebugMsg("-----I found a suitable candidate.\n");
        }
    }
}

class CStaticObject;
class CStaticObjectFactory {
public:
    static void DeallocateStaticObject(CStaticObject*);
};
class BSUtilObjectInstanceMemoryAllocator {
public:
    void FreeElement(void*);
    unsigned char unknown[16];
};
extern BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;
extern BSUtilObjectInstanceMemoryAllocator g_pEnvModMechAllocator;
extern BSUtilObjectInstanceMemoryAllocator g_pObjectAllocators;
void DestroyBSObject(BSObject*);

struct DestructableClass {
    unsigned char unknown00[4];
    struct {
        unsigned char unknown00[12];
        int m_type;
    }* m_info;
};
struct DestructableScript {
    unsigned char unknown00[8];
    DestructableClass* m_class;
};

class BSGO_Destructable {
public:
    void Destroy();
    DestructableScript* m_script;
    BSGO_Destructable* m_next;
    BSGO_Destructable* m_prev;
    unsigned char unknown0c[16];
    ISceneNode* m_node;
};
extern BSGO_Destructable* g_pBoxList;

void BSGO_Destructable::Destroy() {
    if (m_prev)
        m_prev->m_next = m_next;
    if (m_next)
        m_next->m_prev = m_prev;
    if (g_pBoxList == this)
        g_pBoxList = m_next;
    g_scene.Remove(*m_node);
    void* local = ((GetVolumeFn)Slot(m_node, 0x34))(m_node, 3);
    if (local)
        g_pVolumeAllocator.FreeElement(local);
    void* world = ((GetVolumeFn)Slot(m_node, 0x38))(m_node, 3);
    if (world && world != local)
        g_pVolumeAllocator.FreeElement(world);
    switch (m_script->m_class->m_info->m_type) {
    case 2:
    case 4:
        CStaticObjectFactory::DeallocateStaticObject((CStaticObject*)m_node);
        break;
    case 8:
        g_pEnvModMechAllocator.FreeElement(m_node);
        break;
    }
    DestroyBSObject((BSObject*)m_script);
    m_script = 0;
    m_node = 0;
    g_pObjectAllocators.FreeElement(this);
}
