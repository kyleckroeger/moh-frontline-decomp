// best WIP (harvest1, 2026-11-12): 2 real lines (was 44; index said 44), from
// scratch/loop3/mid2/CRH_BEST2.cpp: world volume through the file's own AllocateBoxVolume() (attested,
// same body as src/game/objcreate_volumes.cpp), box cast from the allocator then 'new (box) CVolBox'.
// Left: the placement-new knot (State question 7, scratch/loop3/pnew1/notes.md).
// A fragment of objcreate.cpp (0x8003d130): DestroySubHierObject calls the
// virtual function at slot 0x164 (a placeholder named by offset) of a
// hierarchical object's sub-object; SwapHierObject swaps a
// sub-object of a hierarchical object to the static mesh and motion data
// of another index of the object's type. The functions and classes are
// named by the symbols; the virtual function at slot 0x60 (a placeholder
// named by offset) and the chain from its result to the type (+8, +4,
// +72) are inferred, and the parameter meanings are inferred from the calls.
// CreateSubHierObject follows: it allocates the sub-object for the parent's
// type, builds its box volume in a pool element from the mesh's box (axis
// lengths give width, height and depth: 2.4 for a tank part in mission 4
// stage 2 outside multiplayer, 1.8/1.9/1.6 with the centre nudged by 0.1 and
// -0.1 for other tank parts, else 2.0; the normalised axes and the centre
// become its basis and centre), installs it as the local volumes and a copy
// as the world volumes, hands over the parent's script object and initialises
// the sub-object under the parent with a zero offset. The mesh and box views,
// the parameter names and the scale meanings are inferred; the vector view
// copies through a double pair (an init-list union copy, so the squared
// lengths stay in registers as in the original).
// The rest of the file is not part of this unit.
class CStaticMesh;
class CTankObject;
class MotionData;
class CStaticObject;
class CHierObject;
class BSObject;

inline void* operator new(unsigned long, void* place) {
    return place;
}


#include "game/CVector3.h"


class BSUtilObjectInstanceMemoryAllocator {
public:
    void* GetFreeElement(bool);

    unsigned char unknown00[28];
};

extern BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;
extern BSUtilObjectInstanceMemoryAllocator g_pObjectAllocators[2];

#include "game/CMatrix.h"


class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

/* inferred: the trigger's core record */
struct TriggerCoreView {
    unsigned char unknown00[16];
    float position[3];
    CQuaternion rotation;
    unsigned char unknown2c[28];
    int type;
    char* scriptName;
    unsigned char unknown50[32];
    unsigned char field70;
    unsigned char unknown71[39];
    int weapons[3];
};

struct TriggerObject_struct {
    unsigned int flags;
    TriggerCoreView* core;
};

class BSGO_Basic {
public:
    BSGO_Basic() : m_object(0), m_next(0), m_prev(0) {}

    BSObject* m_object;
    BSGO_Basic* m_next;
    BSGO_Basic* m_prev;

    virtual void Destroy();
};

class BSGO_Tank : public BSGO_Basic {
public:
    static void* operator new(unsigned long) { return g_pObjectAllocators[1].GetFreeElement(true); }
    BSGO_Tank() {}
    virtual void Destroy();

    int m_field10;
    unsigned char m_script[208];
    CHierObject* m_sceneNode;
    int m_proximity[4];
};

extern BSGO_Basic* g_pTankList;

int CreateBSObject(const char*, BSObject**, BSGO_Basic*, TriggerObject_struct*, void**);
int BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);

class CTankObject {
public:
    virtual void v008();
    virtual void v00c();
    virtual void v010();
    virtual void v014();
    virtual void v018();
    virtual void v01c();
    virtual void v020();
    virtual void v024();
    virtual void v028();
    virtual void v02c();
    virtual void v030();
    virtual void v034();
    virtual void v038();
    virtual void v03c();
    virtual void v040();
    virtual void v044();
    virtual void v048();
    virtual void v04c();
    virtual void v050();
    virtual void v054();
    virtual void v058();
    virtual void SetCollisionId(int);
    virtual void v060();
    virtual void v064();
    virtual void v068();
    virtual void v06c();
    virtual void v070();
    virtual void v074();
    virtual void v078();
    virtual void v07c();
    virtual void v080();
    virtual void v084();
    virtual void v088();
    virtual void v08c();
    virtual void v090();
    virtual void v094();
    virtual void v098();
    virtual void v09c();
    virtual void v0a0();
    virtual void v0a4();
    virtual void v0a8();
    virtual void v0ac();
    virtual void v0b0();
    virtual void v0b4();
    virtual void v0b8();
    virtual void v0bc();
    virtual void v0c0();
    virtual void v0c4();
    virtual void v0c8();
    virtual void v0cc();
    virtual void v0d0();
    virtual void v0d4();
    virtual void v0d8();
    virtual void v0dc();
    virtual void v0e0();
    virtual void v0e4();
    virtual void v0e8();
    virtual void v0ec();
    virtual void v0f0();
    virtual void v0f4();
    virtual void v0f8();
    virtual void v0fc();
    virtual void v100();
    virtual void v104();
    virtual void v108();
    virtual void v10c();
    virtual void v110();
    virtual void v114();
    virtual void v118();
    virtual void v11c();
    virtual void v120();
    virtual void v124();
    virtual void v128();
    virtual void v12c();
    virtual void v130();
    virtual void v134();
    virtual void v138();
    virtual void v13c();
    virtual void v140();
    virtual void v144();
    virtual void v148();
    virtual void v14c();
    virtual void v150();
    virtual void v154();
    virtual void v158();
    virtual void v15c();
    virtual void v160();
    virtual void v164();
    virtual void v168();
    virtual void v16c();
    virtual void InitWeapons();
    void AddWeapon(int, int);
};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    CVolBox& operator=(const CVolBox&);
    void SetWidth(float);
    void SetHeight(float);
    void SetDepth(float);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetCenter(CVector3);

    unsigned char unknown04[76];
};

/* inferred: the box's axes (+16) and centre (+64) */
struct BoxAxesView {
    unsigned char unknown00[16];
    CVector3 m_axes[3];
    CVector3 m_center;
};

/* inferred: the mesh's data record and its box (+32) */
struct MeshDataView {
    unsigned char unknown00[32];
    BoxAxesView* m_box;
};

struct StaticMeshView {
    unsigned char unknown00[4];
    MeshDataView* m_data;
};

#include "game/ISceneNode.h"


#include "game/CScene.h"


extern CScene g_scene;

class CStaticObject {
public:
    void SetLocalBoundingVolume(ISceneNode::EVolumeType, IVolume*);
    void SetWorldBoundingVolume(ISceneNode::EVolumeType, IVolume*);
};

class CStaticObjectFactory {
public:
    static CStaticMesh* GetStaticMesh(int, int);
    static MotionData* GetMotionData(int, int);
    static CHierObject* AllocateStaticObject(int, int);
};

enum DWI_ATTACHMENT_CRC_ENUM {};

class CShellMenu {
public:
    unsigned int Get_currentMission();
    unsigned int Get_currentStage();

    unsigned char unknown0000[6620];
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;

/* inferred: the views from the slot-0x60 result to the type */
struct FactoryOwnerView {
    unsigned char unknown00[72];
    int m_type;
};

struct FactoryLinkView {
    unsigned char unknown0[4];
    FactoryOwnerView* m_owner;
};

struct FactoryRefView {
    unsigned char unknown0[8];
    FactoryLinkView* m_link;
};

class CHierObject {
public:
    virtual void unknown008();
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
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual FactoryRefView* unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual CHierObject* AsHierObject();
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
    virtual void SetScript(BSObject*);
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual void unknown140();
    virtual void unknown144();
    virtual CTankObject* AsHierTankObject();
    virtual void unknown14c();
    virtual void unknown150();
    virtual void unknown154();
    virtual void unknown158();
    virtual void unknown15c();
    virtual void unknown160();
    virtual void unknown164(); /* result type not known */

    CHierObject* GetSubObject(int);
    void SwapOut(CStaticMesh*, MotionData*);
    void Init(CHierObject*, int, int, int, CVector3&);

    unsigned char unknown004[44];
    StaticMeshView* m_mesh;
};

void* AllocateBoxVolume() {
    return new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;
}

void DestroySubHierObject(CHierObject* object, int sub) {
    object->GetSubObject(sub)->unknown164();
}

void SwapHierObject(CHierObject* object, int sub, int index) {
    int type = object->unknown060()->m_link->m_owner->m_type;
    CStaticMesh* mesh = CStaticObjectFactory::GetStaticMesh(type, index);
    MotionData* motion = CStaticObjectFactory::GetMotionData(type, index);
    object->GetSubObject(sub)->SwapOut(mesh, motion);
}

void CreateSubHierObject(CHierObject* parent, int sub, int index, DWI_ATTACHMENT_CRC_ENUM attachment) {
    int type = parent->unknown060()->m_link->m_owner->m_type;
    CHierObject* object = CStaticObjectFactory::AllocateStaticObject(type, sub)->AsHierObject();
    CTankObject* tank = parent->AsHierTankObject();
    CVolBox* box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;
    BoxAxesView* source = object->m_mesh->m_data->m_box;
    CVector3 a(source->m_axes[0]);
    CVector3 b(source->m_axes[1]);
    CVector3 c(source->m_axes[2]);
    CVector3 center(source->m_center);
    if (tank) {
        if (g_Shell.Get_currentMission() == 4 && g_Shell.Get_currentStage() == 2 && !g_bInMultiplayerMode) {
            center.x += 0.0f * a.Length();
            center.y += 0.0f * c.Length();
            center.z += 0.0f * b.Length();
            box->SetWidth(2.4f * a.Length());
            box->SetHeight(2.4f * c.Length());
            box->SetDepth(2.4f * b.Length());
        } else {
            center.x += 0.0f * a.Length();
            center.y += 0.1f * c.Length();
            center.z += -0.1f * b.Length();
            box->SetWidth(1.8f * a.Length());
            box->SetHeight(1.9f * c.Length());
            box->SetDepth(1.6f * b.Length());
        }
    } else {
        box->SetWidth(2.0f * a.Length());
        box->SetHeight(2.0f * c.Length());
        box->SetDepth(2.0f * b.Length());
    }
    a.Normalize();
    b.Normalize();
    c.Normalize();
    box->SetBasis(a, b, c);
    box->SetCenter(center);
    ((CStaticObject*)object)->SetLocalBoundingVolume((ISceneNode::EVolumeType)3, box);
    ((CStaticObject*)object)->SetLocalBoundingVolume((ISceneNode::EVolumeType)0, box);
    CVolBox* world = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;
    *world = *box;
    ((CStaticObject*)object)->SetWorldBoundingVolume((ISceneNode::EVolumeType)3, world);
    ((CStaticObject*)object)->SetWorldBoundingVolume((ISceneNode::EVolumeType)0, world);
    object->SetScript((BSObject*)parent->unknown060());
    CVector3 offset(0.0f, 0.0f, 0.0f);
    object->Init(parent, sub, index, attachment, offset);
}

static inline void AttachScript(TriggerObject_struct* trigger, int flag, TriggerCoreView* core, BSGO_Tank* go, CHierObject* object, CTankObject* tank) {
    CreateBSObject(core->scriptName, &go->m_object, go, trigger, (void**)go->m_script);
    object->SetScript(go->m_object);
    BSObjectTriggerEvent(go->m_object, 44, (void*)flag, 0, false);
    if (tank) {
        tank->SetCollisionId(23);
        tank->AddWeapon(0, core->weapons[0]);
        tank->AddWeapon(1, core->weapons[1]);
        tank->AddWeapon(2, core->weapons[2]);
        tank->InitWeapons();
    }
}

BSGO_Basic* CreateRootHierObject(TriggerObject_struct* trigger, int flag) {
    BSGO_Tank* go;
    CHierObject* object;
    CTankObject* tank;
    CVolBox* box;
    TriggerCoreView* core;
    CVolBox* world;
    core = trigger->core;
    go = new BSGO_Tank;
    if (g_pTankList)
        g_pTankList->m_prev = go;
    go->m_next = g_pTankList;
    go->m_prev = 0;
    g_pTankList = go;
    object = CStaticObjectFactory::AllocateStaticObject(core->type, 0)->AsHierObject();
    tank = object->AsHierTankObject();
    CMatrix matrix;
    CVector3 position(core->position[0], core->position[1], core->position[2]);
    core->rotation.GetMatrix(matrix);
    matrix.SetPos(position);
    object->SetTMLocalToWorld(matrix);
    box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true);
    new (box) CVolBox;
    BoxAxesView* source = object->m_mesh->m_data->m_box;
    CVector3 a(source->m_axes[0]);
    CVector3 b(source->m_axes[1]);
    CVector3 c(source->m_axes[2]);
    CVector3 center(source->m_center);
    if (tank) {
        if (g_Shell.Get_currentMission() == 4 && g_Shell.Get_currentStage() == 2 && !g_bInMultiplayerMode) {
            center.x += 0.0f * a.Length();
            center.y += 0.0f * c.Length();
            center.z += 0.0f * b.Length();
            box->SetWidth(2.4f * a.Length());
            box->SetHeight(2.4f * c.Length());
            box->SetDepth(2.4f * b.Length());
        } else {
            center.x += 0.0f * a.Length();
            center.y += 0.1f * c.Length();
            center.z += -0.1f * b.Length();
            box->SetWidth(1.8f * a.Length());
            box->SetHeight(1.9f * c.Length());
            box->SetDepth(1.6f * b.Length());
        }
    } else {
        box->SetWidth(2.0f * a.Length());
        box->SetHeight(2.0f * c.Length());
        box->SetDepth(2.0f * b.Length());
    }
    a.Normalize();
    b.Normalize();
    c.Normalize();
    box->SetBasis(a, b, c);
    box->SetCenter(center);
    ((CStaticObject*)object)->SetLocalBoundingVolume((ISceneNode::EVolumeType)3, box);
    ((CStaticObject*)object)->SetLocalBoundingVolume((ISceneNode::EVolumeType)0, box);
    world = (CVolBox*)AllocateBoxVolume();
    *world = *box;
    ((CStaticObject*)object)->SetWorldBoundingVolume((ISceneNode::EVolumeType)3, world);
    ((CStaticObject*)object)->SetWorldBoundingVolume((ISceneNode::EVolumeType)0, world);
    matrix.Ident();
    object->Init(object, 0, 0, 0, position);
    g_scene.Add(*(ISceneNode*)object);
    go->m_sceneNode = object;
    go->m_proximity[1] = 0;
    go->m_proximity[0] = 0;
    go->m_proximity[3] = 0;
    go->m_proximity[2] = 0;
    go->m_field10 = core->field70;
    AttachScript(trigger, flag, core, go, object, tank);
    ForceUpdateProximityTriggerStatus(go, true);
    return go;
}
