// A fragment of Soldier_object.cpp (0x800aba4c): CSoldierObject's
// GetLookMatrix (the soldier's matrix, then, when the skeleton has a
// "VisionCone" node, that node's bone matrix and its own matrix applied),
// AddWeapon (replaces the weapon in a slot: the old one is returned to
// g_WeaponFactory, an empty CRC (0xfb9630c9) leaves the slot empty, else the
// weapon is created by CRC; returns whether the slot holds a weapon) and
// SetAttachedLight (places the light at the soldier's matrix pre-translated
// by the offset). The classes, functions and globals are named by the
// mangled symbols; the members and the skeleton and light views are inferred
// (the node search is an inferred inline helper, as in
// Soldier_object_cleanup.cpp, and the skeleton is read through an inferred
// inline accessor, which keeps it in a register across the search),
// CSoldierObject declares placeholder virtuals up to GetTMLocalToWorld (+60,
// named by __vt__14CSoldierObject), and the CVector3 view copies through a double pair as in player_weapon.cpp. The
// rest of the file is not part of this unit.
class ISceneNode;
class CWeapon;

extern "C" int stricmp(const char*, const char*);

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }

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
    CMatrix& operator=(const CMatrix&);
    void Multiply(const CMatrix&, const CMatrix&);
    void PreTranslate(CVector3);
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

class CWeaponFactory {
public:
    void DestroyWeapon(CWeapon*);
    CWeapon* CreateWeaponByCRC(int, ISceneNode*, int);

    unsigned char unknown00[28];
};

extern CWeaponFactory g_WeaponFactory;

/* inferred: the light's SetTMLocalToWorld at +232 */
class CLight {
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

class CSoldierObject {
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
    virtual void GetTMLocalToWorld(CMatrix&) const;
    void GetLookMatrix(CMatrix&);
    bool AddWeapon(int, int);
    void SetAttachedLight(CLight*, CVector3);

    unsigned char unknown0004[3296];
    SkeletonView* m_skeleton;
    SkeletonView* GetSkeleton() const { return m_skeleton; }
    unsigned char unknown0ce8[600];
    CMatrix m_bones[80];
    CMatrix m_matrix;
    unsigned char unknown2380[7312];
    CWeapon* m_weapons[9];
};

void CSoldierObject::GetLookMatrix(CMatrix& matrix) {
    SkeletonView* skeleton = GetSkeleton();
    unsigned int node = skeleton->Find("VisionCone");
    matrix = m_matrix;
    if (node != (unsigned int)-1) {
        matrix.Multiply(m_bones[skeleton->m_nodes[node].m_bone], matrix);
        matrix.Multiply(*skeleton->m_nodes[node].m_matrix, matrix);
    }
}

bool CSoldierObject::AddWeapon(int slot, int crc) {
    if (m_weapons[slot])
        g_WeaponFactory.DestroyWeapon(m_weapons[slot]);
    if (crc == (int)0xfb9630c9) {
        m_weapons[slot] = 0;
        return true;
    }
    return (m_weapons[slot] = g_WeaponFactory.CreateWeaponByCRC(crc, (ISceneNode*)this, 999)) != 0;
}

void CSoldierObject::SetAttachedLight(CLight* light, CVector3 offset) {
    if (light) {
        CMatrix matrix;
        GetTMLocalToWorld(matrix);
        matrix.PreTranslate(offset);
        light->SetTMLocalToWorld(matrix);
    }
}
