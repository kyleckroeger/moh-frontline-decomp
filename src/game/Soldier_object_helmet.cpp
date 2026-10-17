// A fragment of Soldier_object.cpp (0x800aac78): CSoldierObject::EjectHelmet
// throws the soldier's helmet off: when something is attached at the "head"
// node, a special thrown object of the soldier's helmet type (from its
// script object's data) is created at the head's world matrix (the head
// bone's matrix times the soldier's, then the node's hand matrix), given the
// direction scaled by a random 0.002..0.01 with a random upward speed of
// 0.005..0.065, and started; the head attachment is then cleared. It returns
// whether the helmet type stops bullets (false when nothing was thrown). The
// classes and functions are named by the mangled symbols; the members, the
// script data chain, the skeleton view and the inline helpers (the node
// search of Soldier_object_cleanup.cpp, the skeleton accessor of
// Soldier_object_look.cpp and the attachment clearing) are inferred. The
// string and the constants are .sdata2 pool references. The rest of the file
// is in other units.
extern "C" int stricmp(const char*, const char*);

float MathFunRandomReal(float, float);

class CVector3 {
public:
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
    void Multiply(const CMatrix&, const CMatrix&);
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

struct TriggerObject_struct;

class CThrownObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void BeginUpdate(float);
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

    void SetVelocity(CVector3&);
};

CThrownObject* CreateSpecialThrownObject(TriggerObject_struct*, int, int);

class CStaticObjectFactory {
public:
    static bool DoesHelmetTypeStopBullets(int);
};

/* inferred: the script object's data chain to the helmet type */
struct HelmetDataView {
    unsigned char unknown00[72];
    int m_helmetType;
};

struct ScriptOwnerView {
    unsigned char unknown00[4];
    HelmetDataView* m_data;
};

struct BSObject {
    unsigned char unknown00[8];
    ScriptOwnerView* m_owner;
};

/* inferred: a skeleton node (24 bytes): name, bone, own matrix */
struct SkeletonNodeView {
    char m_name[16];
    int m_bone;
    CMatrix* m_matrix;
};

struct SkeletonView {
    unsigned char unknown00[16];
    unsigned int m_count;
    SkeletonNodeView* m_nodes;

    /* inferred: the index of the node with the given name, or -1 */
    unsigned int Find(const char* name) const {
        for (unsigned int i = 0; i < m_count; i++) {
            if (stricmp(m_nodes[i].m_name, name) == 0)
                return i;
        }
        return (unsigned int)-1;
    }
};

class CStaticMesh;

class CSoldierObject {
public:
    bool EjectHelmet(CVector3*);

    /* inferred: clears the named node's attachment */
    void RemoveAttachment(const char* name) {
        SkeletonView* skeleton = GetSkeleton();
        unsigned int i;
        for (i = 0; i < skeleton->m_count; i++) {
            if (stricmp(skeleton->m_nodes[i].m_name, name) == 0)
                break;
        }
        if (i != skeleton->m_count) {
            m_attachments[i] = 0;
            m_attachTypes[i] = 0;
        }
    }

    unsigned char unknown0000[3300];
    SkeletonView* m_skeleton;
    SkeletonView* GetSkeleton() const { return m_skeleton; }
    unsigned char unknown0ce8[600];
    CMatrix m_bones[80];
    CMatrix m_matrix;
    unsigned char unknown2380[16];
    BSObject* m_script;
    unsigned char unknown2394[7328];
    CStaticMesh* m_attachments[8];
    unsigned int m_attachTypes[8];
    unsigned char unknown4074[12];
    CMatrix m_handMatrices[8];
};

bool CSoldierObject::EjectHelmet(CVector3* direction) {
    bool stops = true;
    SkeletonView* skeleton = GetSkeleton();
    unsigned int node = skeleton->Find("head");
    if (node == (unsigned int)-1)
        return false;
    if (!m_attachTypes[node])
        return false;
    int type = m_script->m_owner->m_data->m_helmetType;
    CThrownObject* helmet = CreateSpecialThrownObject(0, type, -1);
    if (!helmet)
        return false;
    if (!CStaticObjectFactory::DoesHelmetTypeStopBullets(type))
        stops = false;
    CMatrix matrix;
    matrix.Multiply(m_bones[skeleton->m_nodes[node].m_bone], m_matrix);
    matrix.Multiply(m_handMatrices[node], matrix);
    helmet->SetTMLocalToWorld(matrix);
    float scale = MathFunRandomReal(0.002f, 0.01f);
    direction->x *= scale;
    direction->y *= scale;
    direction->z *= scale;
    direction->z = MathFunRandomReal(0.005f, 0.065f);
    helmet->SetVelocity(*direction);
    helmet->BeginUpdate(1.0f);
    RemoveAttachment("head");
    return stops;
}
