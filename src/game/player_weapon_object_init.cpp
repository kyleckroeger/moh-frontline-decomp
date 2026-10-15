// A fragment of player_weapon_object.cpp (0x800a8b7c):
// CPlayerWeaponObject::Shutdown destroys its weapon (+12516) through the
// weapon factory and removes the object from the scene; Init clears +12760,
// keeps the player (+12512), resets the matrix at +9024, four flag bits of
// +12754 and +12756 (-1) and the 80 matrices from +3904, creates the weapon
// of the given type for the player, marks +9298, adds the object to the
// scene and clears +12532, the vector at +12584 and +12720. The classes and
// functions are named by the symbols; this is a non-virtual view (the scene
// node base only provides the reference), and the members are inferred from
// offsets (the matrices as in anim_obj_ctor.cpp). The vector is assigned
// through a temporary copied as a double pair. The rest of the file is in
// other units.
enum EWeaponTypes {};

class CMatrix {
public:
    void Ident();

    float m[4][4];
};

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3(float ax, float ay, float az) {
        x = ax;
        y = ay;
        z = az;
    }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class ISceneNode {};
class CPlayerObject : public ISceneNode {};
class CWeapon;

class CWeaponFactory {
public:
    CWeapon* CreateWeapon(EWeaponTypes, ISceneNode*, int);
    void DestroyWeapon(CWeapon*);

    unsigned char unknown00[28];
};

class CScene {
public:
    void Add(ISceneNode&);
    void Remove(ISceneNode&);

    unsigned char unknown000[328];
};

extern CWeaponFactory g_WeaponFactory;
extern CScene g_scene;

class CPlayerWeaponObject : public ISceneNode {
public:
    void Shutdown();
    void Init(EWeaponTypes, int, CPlayerObject*);

    unsigned char unknown0000[3904];
    CMatrix m_matrices[80];
    CMatrix m_matrix2340;
    unsigned char unknown2380[210];
    bool m_flag2452;
    unsigned char unknown2453[3213];
    CPlayerObject* m_player;
    CWeapon* m_weapon;
    unsigned char unknown30e8[12];
    int m_unknown30f4;
    unsigned char unknown30f8[48];
    CVector3 m_vector3128;
    unsigned char unknown3138[120];
    int m_unknown31b0;
    unsigned char unknown31b4[30];
    unsigned char m_flag0 : 1;
    unsigned char m_flag1 : 1;
    unsigned char m_flag2 : 1;
    unsigned char m_flag3 : 1;
    unsigned char m_flags : 4;
    unsigned char unknown31d3;
    int m_unknown31d4;
    int m_unknown31d8;
};

void CPlayerWeaponObject::Shutdown() {
    if (m_weapon)
        g_WeaponFactory.DestroyWeapon(m_weapon);
    g_scene.Remove(*this);
}

void CPlayerWeaponObject::Init(EWeaponTypes type, int index, CPlayerObject* player) {
    m_unknown31d8 = 0;
    m_player = player;
    m_matrix2340.Ident();
    m_flag0 = 0;
    m_flag1 = 0;
    m_flag2 = 0;
    m_flag3 = 0;
    m_unknown31d4 = -1;
    for (int i = 0; i < 80; i++)
        m_matrices[i].Ident();
    m_weapon = g_WeaponFactory.CreateWeapon(type, player, index);
    m_flag2452 = true;
    g_scene.Add(*this);
    m_unknown30f4 = 0;
    m_vector3128 = CVector3(0.0f, 0.0f, 0.0f);
    m_unknown31b0 = 0;
}
