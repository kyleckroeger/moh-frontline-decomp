enum EDumpType {};
enum EClsnId {};

inline void* operator new(unsigned long, void* place) {
    return place;
}

void DebugMsg(const char*, ...);

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3& operator+=(const CVector3& o) {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;
    void SetPos(CVector3);

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    CQuaternion(const CQuaternion& o) : w(o.w), x(o.x), y(o.y), z(o.z) {}
    void GetMatrix(CMatrix&) const;

    float w;
    float x;
    float y;
    float z;
};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
};

class CVolSphere : public IVolume {
public:
    CVolSphere() {}
    virtual ~CVolSphere();
    CVolSphere& operator=(const CVolSphere&);
    void SetCenter(CVector3);
    void SetRadius(float);
};

class ISceneNode {
public:
    enum EVolumeType {};
};

/* inferred: a mesh's bounding box centre */
struct MeshBoxView {
    unsigned char unknown00[64];
    CVector3 m_center;
};

struct MeshDataView {
    unsigned char unknown00[32];
    MeshBoxView* m_box;
};

struct MeshView {
    unsigned char unknown00[4];
    MeshDataView* m_data;
};

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
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();

    void SetLocalBoundingVolume(ISceneNode::EVolumeType, IVolume*);
    void SetWorldBoundingVolume(ISceneNode::EVolumeType, IVolume*);

    unsigned char unknown004[44];
    MeshView* m_mesh;
    unsigned char unknown034[429];
    unsigned char m_flags1e1a : 1;
    unsigned char m_dumped : 1;
    unsigned char m_flags1e1b : 6;
};

class CScene {
public:
    void Add(ISceneNode&);
    class CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;
extern bool g_bInMultiplayerMode;
extern float g_fDumpObjectCollisionRadius;

class CSoundTable {
public:
    struct SSoundParms {
        unsigned char unknown00[16];
    };

    bool GetSoundEvent(int, int, int, SSoundParms*) const;

    unsigned char unknown00[4];
};

extern CSoundTable g_soundMapSingleton;

class BSObject;
void PlayScriptedSound(ISceneNode*, CVector3, const CSoundTable::SSoundParms&, int, BSObject*);

/* inferred: the weapon's properties (maximum ammo +18) */
struct WeaponPropertiesView {
    unsigned char unknown00[18];
    short m_maxAmmo;
};

class CWeapon {
public:
    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    short m_ammo;
    unsigned char unknown286[18];
    int m_dumpValue;
};

class CPlayerWeaponObject {
public:
    unsigned char unknown0000[12516];
    CWeapon* m_weapon;
};

/* inferred: the script data's health (first word) */
struct ScriptDataView {
    int m_health;
};


/* inferred: an object's proximity-trigger state, as in proxtrig_leafs.cpp */
struct _ProximityData {
    _ProximityData() {
        previousTriggers = triggers = 0;
        previousLeaf = leaf = 0;
    }

    unsigned int previousTriggers;
    unsigned int triggers;
    void* previousLeaf;
    void* leaf;
};

class BSGO_Basic {
public:
    void* m_object;
    int m_field4;
    int m_field8;

    BSGO_Basic() : m_object(0), m_field4(0), m_field8(0) {}
    virtual void Destroy();
    virtual ScriptDataView* GetScriptData();
};

class BSGO_Dummy : public BSGO_Basic {
public:
    BSGO_Dummy(CStaticObject* node) {
        m_object = 0;
        m_sceneNode = node;
    }
    virtual void Destroy();

    CStaticObject* m_sceneNode;
    _ProximityData m_proximity;
};

/* inferred: the pickup's properties (maximum health at +112) */
struct PickupPropertiesView {
    unsigned char unknown00[112];
    unsigned char m_maxHealth;
};

struct ScriptOwnerView {
    unsigned char unknown00[4];
    PickupPropertiesView* m_properties;
};

struct BSObjectView {
    unsigned char unknown00[8];
    ScriptOwnerView* m_owner;
    class BSGO_Basic* m_user;
};

class CPlayerObject {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void vf014();
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void vf024();
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void vf040();
    virtual void vf044();
    virtual void vf048();
    virtual void vf04c();
    virtual void vf050();
    virtual void vf054();
    virtual void vf058();
    virtual void vf05c();
    virtual BSObjectView* GetScriptObject() const;

    CPlayerWeaponObject* GetWeaponByCRC(int) const;
};

void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);

class CStaticObjectFactory {
public:
    static void DeallocateStaticObject(CStaticObject*);
    static CStaticObject* AllocateStaticObject(int, int);
};

class BSUtilObjectInstanceMemoryAllocator {
public:
    void FreeElement(void*);
    void* GetFreeElement(bool);

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
    DumpNode* m_nodes;
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

CStaticObject* CreateDumpObject(const CVector3& position, const CQuaternion& rotation, int type, int value) {
    CStaticObject* object = CStaticObjectFactory::AllocateStaticObject(type, -1);
    if (!object) {
        DebugMsg("\nERROR: Unable to create DumpObject type 0x%x.\n  CRP might not be in level file.\n", type);
        DebugMsg("Or it's possible the static object pool is all used up\n\n");
        MarkDumpObjectForDestruction(g_DumpObjectContainer.m_head);
        return 0;
    }
    CMatrix tm;
    CQuaternion q = rotation;
    q.GetMatrix(tm);
    tm.SetPos(position);
    object->SetTMLocalToWorld(tm);
    MeshView* mesh = object->m_mesh;
    object->unknown124();
    CVolSphere* local = new (g_pVolumeAllocator.GetFreeElement(true)) CVolSphere;
    CVector3 center(0.0f, 0.0f, g_fDumpObjectCollisionRadius * 0.5f);
    center += mesh->m_data->m_box->m_center;
    local->SetCenter(center);
    local->SetRadius(g_fDumpObjectCollisionRadius);
    CVolSphere* world = new (g_pVolumeAllocator.GetFreeElement(true)) CVolSphere;
    *world = *local;
    object->SetLocalBoundingVolume((ISceneNode::EVolumeType)3, local);
    object->SetLocalBoundingVolume((ISceneNode::EVolumeType)0, local);
    object->SetWorldBoundingVolume((ISceneNode::EVolumeType)3, world);
    object->SetWorldBoundingVolume((ISceneNode::EVolumeType)0, world);
    g_scene.Add(*(ISceneNode*)object);
    int kind;
    int extra;
    switch (type) {
    case 0x598e4105:
    case 0x10efeba2:
    case (int)0xf7baddb9:
    case 0x72441aa1: {
        BSObjectView* script = g_scene.GetPlayer(0)->GetScriptObject();
        PickupPropertiesView* properties = script->m_owner->m_properties;
        if (script->m_user->GetScriptData()->m_health == (int)properties->m_maxHealth)
            object->SetCollisionId((EClsnId)-1);
        else
            object->SetCollisionId((EClsnId)14);
        kind = 0;
        extra = 0;
        break;
    }
    default: {
        CPlayerObject* player = g_scene.GetPlayer(0);
        object->SetCollisionId((EClsnId)-1);
        CWeapon* weapon = player->GetWeaponByCRC(type)->m_weapon;
        if (g_bInMultiplayerMode || (weapon && (unsigned int)weapon->m_ammo < (unsigned int)weapon->m_properties->m_maxAmmo))
            object->SetCollisionId((EClsnId)14);
        CSoundTable::SSoundParms parms;
        if (g_soundMapSingleton.GetSoundEvent(457, -1, 87, &parms))
            PlayScriptedSound((ISceneNode*)object, position, parms, -1, 0);
        extra = weapon->m_dumpValue;
        kind = 1;
        break;
    }
    }
    DumpNode* node = g_DumpObjectContainer.m_nodes;
    int i;
    for (i = 0; i < 25 && node->m_value4 != 0; i++, node++) {
    }
    if (i == 25)
        node = 0;
    if (!node) {
        node = g_DumpObjectContainer.m_head;
        MarkDumpObjectForDestruction(node);
    }
    node->m_object = object;
    node->m_value4 = type;
    node->m_value8 = value;
    node->m_type = (EDumpType)kind;
    node->m_subtype = extra;
    node->m_next = 0;
    node->m_prev = g_DumpObjectContainer.m_tail;
    if (g_DumpObjectContainer.m_tail)
        g_DumpObjectContainer.m_tail->m_next = node;
    g_DumpObjectContainer.m_tail = node;
    if (!g_DumpObjectContainer.m_head)
        g_DumpObjectContainer.m_head = node;
    g_DumpObjectContainer.m_count++;
    object->m_dumped = 1;
    BSGO_Dummy dummy(object);
    ForceUpdateProximityTriggerStatus(&dummy, true);
    return object;
}
