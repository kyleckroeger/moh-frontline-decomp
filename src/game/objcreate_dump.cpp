// A fragment of objcreate.cpp (0x8003f8b8): the dump-object list functions:
// EnableDumpCollisionsByType (sets collision id 14, or -1 to disable, on the
// dumped objects of a type and, unless -1, subtype), DestroyOldestDumpObject
// (marks the head object for destruction and unlinks its node),
// DestroyDumpObject (returns the object to the static-object factory and
// its type-3 local and world volumes to the volume allocator),
// MarkDumpObjectForDestruction (the same for a given node) and HitDumpObject
// (finds an object's node and reports its values). The functions, classes
// and globals are named by the symbols (g_DumpObjectContainer and
// g_pVolumeAllocator are file-local in the original and declared extern
// here; their sizes are their symbols'); the list, node and object views and
// the unlink helper are inferred. The rest of the file is not part of this
// unit.
enum EDumpType {};
enum EClsnId {};
class IVolume;

class CStaticObject {
public:
    virtual void MarkForDestruction(int);
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual IVolume* GetLocalBoundingVolume(int) const;
    virtual IVolume* GetWorldBoundingVolume(int) const;
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void SetCollisionId(EClsnId);
};

class CStaticObjectFactory {
public:
    static void DeallocateStaticObject(CStaticObject*);
};

class BSUtilObjectInstanceMemoryAllocator {
public:
    void FreeElement(void*);

    unsigned char unknown00[28];
};

extern BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;

/* inferred: a dumped object's node */
struct DumpNode {
    CStaticObject* m_object;
    int m_value4;
    int m_value8;
    EDumpType m_type;
    int m_subtype;
    DumpNode* m_next;
    DumpNode* m_prev;
};

/* inferred: the list of dumped objects */
struct DumpObjectContainer {
    int m_value0;
    DumpNode* m_head;
    DumpNode* m_tail;
    int m_count;
};

extern DumpObjectContainer g_DumpObjectContainer;

/* inferred: unlinks a node from the dump list */
inline void UnlinkDumpNode(DumpNode* node) {
    if (g_DumpObjectContainer.m_head == node) {
        g_DumpObjectContainer.m_head = node->m_next;
        if (g_DumpObjectContainer.m_tail == node)
            g_DumpObjectContainer.m_tail = 0;
        else
            node->m_next->m_prev = 0;
    } else if (g_DumpObjectContainer.m_tail == node) {
        g_DumpObjectContainer.m_tail = node->m_prev;
        g_DumpObjectContainer.m_tail->m_next = 0;
    } else {
        node->m_next->m_prev = node->m_prev;
        node->m_prev->m_next = node->m_next;
    }
    node->m_value4 = 0;
    g_DumpObjectContainer.m_count--;
}

void EnableDumpCollisionsByType(EDumpType type, int subtype, bool enable) {
    EClsnId id;
    DumpNode* node = g_DumpObjectContainer.m_head;
    if (enable)
        id = (EClsnId)14;
    else
        id = (EClsnId)-1;
    for (; node; node = node->m_next) {
        if (node->m_type == type && (subtype == -1 || node->m_subtype == subtype))
            node->m_object->SetCollisionId(id);
    }
}

void DestroyOldestDumpObject() {
    if (g_DumpObjectContainer.m_count > 0) {
        DumpNode* node = g_DumpObjectContainer.m_head;
        node->m_object->MarkForDestruction(0);
        node->m_object = 0;
        UnlinkDumpNode(node);
    }
}

void DestroyDumpObject(CStaticObject* object) {
    CStaticObjectFactory::DeallocateStaticObject(object);
    if (object->GetLocalBoundingVolume(3))
        g_pVolumeAllocator.FreeElement(object->GetLocalBoundingVolume(3));
    if (object->GetWorldBoundingVolume(3))
        g_pVolumeAllocator.FreeElement(object->GetWorldBoundingVolume(3));
}

void MarkDumpObjectForDestruction(void* p) {
    DumpNode* node = (DumpNode*)p;
    node->m_object->MarkForDestruction(0);
    node->m_object = 0;
    UnlinkDumpNode(node);
}

DumpNode* HitDumpObject(CStaticObject* object, int* value8, int* value4, EDumpType* type, int* subtype) {
    DumpNode* node = g_DumpObjectContainer.m_head;
    while (node->m_object != object)
        node = node->m_next;
    *value8 = node->m_value8;
    *value4 = node->m_value4;
    *type = node->m_type;
    if (subtype)
        *subtype = node->m_subtype;
    return node;
}
