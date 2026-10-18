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
class MotionData;
class CStaticObject;
class CHierObject;
class BSObject;

inline void* operator new(unsigned long, void* place) {
    return place;
}

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
        return y;
    }
    return x;
}

class CVector3 {
public:
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3(const CVector3& other) : d(other.d) {}
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            float scale = 1.0f / length;
            d.v[0] *= scale;
            d.v[1] *= scale;
            d.v[2] *= scale;
        }
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class BSUtilObjectInstanceMemoryAllocator {
public:
    void* GetFreeElement(bool);

    unsigned char unknown00[40];
};

extern BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;

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

class ISceneNode {
public:
    enum EVolumeType {};
};

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
    virtual void unknown0e8();
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
    virtual CHierObject* AsHierTankObject();
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
    CHierObject* tank = parent->AsHierTankObject();
    CVolBox* box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;
    BoxAxesView* source = object->m_mesh->m_data->m_box;
    CVector3 a(source->m_axes[0]);
    CVector3 b(source->m_axes[1]);
    CVector3 c(source->m_axes[2]);
    CVector3 center(source->m_center);
    if (tank) {
        if (g_Shell.Get_currentMission() == 4 && g_Shell.Get_currentStage() == 2 && !g_bInMultiplayerMode) {
            center.d.v[0] += 0.0f * a.Length();
            center.d.v[1] += 0.0f * c.Length();
            center.d.v[2] += 0.0f * b.Length();
            box->SetWidth(2.4f * a.Length());
            box->SetHeight(2.4f * c.Length());
            box->SetDepth(2.4f * b.Length());
        } else {
            center.d.v[0] += 0.0f * a.Length();
            center.d.v[1] += 0.1f * c.Length();
            center.d.v[2] += -0.1f * b.Length();
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
