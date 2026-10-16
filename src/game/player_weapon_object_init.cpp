// A fragment of player_weapon_object.cpp (0x800a8638):
// CPlayerWeaponObject::BeginUpdate reports a change of the weapon's reserve
// (+644) outside multiplayer (dump collisions of type 1 for the weapon type,
// with whether the reserve is below the properties' value at +18, compared as
// unsigned words), and while the flag bit 0x40 of +12754 is set updates the
// animation and, unless the player's bit 0x80 of +919 is set, the weapon
// transforms and the weapon; with a muzzle flash (and the player's bit 0x40
// of +918 clear) it picks a random flash angle up to 6.28 * dt and, when the
// muzzle-flash object is set, spawns the flash effect (+12704, else
// 0x67d7fa88; not in multiplayer) at the fire attach matrix scaled by +12696,
// rotated by the angle, through the bone, the scale at +12568 and the matrix
// at +9024. The mesh pointer kept in a local between the two matrix
// constructions is inferred from the load order.
// CPlayerWeaponObject::UpdateWeaponTransforms takes the player's world
// transform into the matrix at +9024, applies the player's matrix at +192,
// then a local transform (the offset at +12536 raised by 1 in y, the
// rotations at +12552 with the player's angles at +1100 and +1104 added to x
// and z), copies a player flag bit (bit 3 of +916), gives the weapon the
// camera's (+2000) position and forward vector, and for the weapon types with
// a muzzle flash sets a flag bit and the muzzle-flash static object and finds
// the mesh's (+3300) "Fire" attach point (the first one when none is named
// so), keeping its index (+12712) and copying its matrix (+12608); with the
// flag set the attach point's position is built from the scale at +12568,
// the attach matrix, the bone's matrix, the matrix at +9024 and the camera's
// matrix at +128, and stored at +12584. The jump from the switch to the
// search after the default case and the inline search helper (which keeps
// the mesh pointer it searched) are inferred from the code layout.
// CPlayerWeaponObject::Shutdown destroys its weapon (+12516) through the
// weapon factory and removes the object from the scene; Init clears +12760,
// keeps the player (+12512), resets the matrix at +9024, four flag bits of
// +12754 and +12756 (-1) and the 80 matrices from +3904, creates the weapon
// of the given type for the player, marks +9298, adds the object to the
// scene and clears +12532, the vector at +12584 and +12720. The classes and
// functions are named by the symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode, so the object's
// members start after its table pointer, and the members are inferred from
// offsets (the matrices as in anim_obj_ctor.cpp). The camera, player, weapon
// and attach-point views are inferred (members at their offsets). Vectors are
// copied as double pairs. The rest of the file is in other units.
enum EWeaponTypes {};


union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
    }
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

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix(const CMatrix& other) { *this = other; }
    static void InitClass();
    CMatrix& operator=(const CMatrix&);
    void Ident();
    void Multiply(const CMatrix&, const CMatrix&);
    void BuildTrans(CVector3);
    void BuildScale(CVector3);
    void RotateX(float);
    void RotateY(float);
    void RotateZ(float);
    void BuildScale(float);
    CVector3 GetPos() const { return *(const CVector3*)m[3]; }

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

enum EClsnId {};
class CCollision;
class CDrawContext;
class CBullet;
class CLight;

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
    virtual void* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual void* AsPlayerObject();
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
    virtual void* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class CCamera : public ISceneNode {
public:
    unsigned char unknown004[124];
    CMatrix m_matrix80;
};

class CPlayerObject : public ISceneNode {
public:
    unsigned char unknown004[188];
    CMatrix m_matrixc0;
    unsigned char unknown100[660];
    unsigned char m_flag80 : 1;
    unsigned char m_flag40 : 1;
    unsigned char m_flag20 : 1;
    unsigned char m_flag10 : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_flag04 : 1;
    unsigned char m_flag02 : 1;
    unsigned char m_flag01 : 1;
    unsigned char unknown395;
    unsigned char m_flags396a : 1;
    unsigned char m_flag396 : 1;
    unsigned char m_flags396b : 6;
    unsigned char m_flag397 : 1;
    unsigned char m_flags397 : 7;
    unsigned char unknown398[180];
    float m_pitch;
    float m_roll;
    unsigned char unknown458[888];
    CCamera m_camera;
};

/* inferred: the weapon properties' value at +18 */
struct WeaponPropertiesView {
    unsigned char unknown00[18];
    short m_value12;
};

class CWeapon {
public:
    void Update(float, bool);
    unsigned char GetMuzzleFlashStatus();

    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    short m_reserve;
    unsigned char unknown286[18];
    int m_type;
    unsigned char unknown29c[4];
    CVector3 m_forward;
    CVector3 m_position;
};

extern "C" int stricmp(const char*, const char*);

struct AttachPointView {
    char name[16];
    int bone;
    CMatrix* matrix;
};

struct AttachMeshView {
    CMatrix* Find(const char* name, unsigned long& index) {
        unsigned long i;
        for (i = 0; i < count; i++) {
            if (stricmp(points[i].name, name) == 0)
                break;
        }
        if (i == count)
            i = 0;
        index = i;
        return points[i].matrix;
    }
    unsigned char unknown00[16];
    unsigned long count;
    AttachPointView* points;
};

extern int g_pStaticObjectMuzzleFlashPlayer;

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

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual void Render();
    virtual void Init();
    virtual void Link();
    virtual int IsUsed();
};

class CParticleSystem : public CRenderBin {
public:
    virtual ~CParticleSystem();
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
    virtual void SetLocalToWorld(const CMatrix&);
    static CParticleSystem* Create(unsigned long);
};

enum EDumpType {};
void EnableDumpCollisionsByType(EDumpType, int, bool);
float MathFunRandomReal(float, float);

extern bool g_bInMultiplayerMode;
extern CWeaponFactory g_WeaponFactory;
extern CScene g_scene;

class CPlayerWeaponObject : public ISceneNode {
public:
    virtual void vf0dc();
    virtual void vf0e0();
    virtual void vf0e4();
    virtual void vf0e8();
    virtual void vf0ec();
    virtual void vf0f0();
    virtual void vf0f4();
    virtual void vf0f8();
    virtual void vf0fc();
    virtual void vf100();
    virtual void vf104();
    virtual void vf108();
    virtual void vf10c();
    virtual void vf110();
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual void vf120();
    virtual void vf124();
    virtual void UpdateAnimation(float);
    virtual void BeginUpdate(float);
    void UpdateWeaponTransforms();
    void Shutdown();
    void Init(EWeaponTypes, int, CPlayerObject*);

    unsigned char unknown0004[3296];
    AttachMeshView* m_mesh;
    unsigned char unknown0ce8[600];
    CMatrix m_matrices[80];
    CMatrix m_matrix2340;
    unsigned char unknown2380[210];
    bool m_flag2452;
    unsigned char unknown2453[3213];
    CPlayerObject* m_player;
    CWeapon* m_weapon;
    unsigned char unknown30e8[12];
    unsigned int m_unknown30f4;
    CVector3 m_offset;
    float m_rotation[3];
    unsigned char unknown3114[4];
    float m_scale[3];
    unsigned char unknown3124[4];
    CVector3 m_vector3128;
    unsigned char unknown3138[8];
    CMatrix m_fireMatrix;
    unsigned char unknown3180[24];
    float m_flashScale;
    float m_flashAngle;
    long m_flashEffect;
    unsigned char unknown31a4[4];
    unsigned long m_firePoint;
    unsigned char unknown31ac[4];
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

void CPlayerWeaponObject::BeginUpdate(float dt) {
    if (m_weapon && !g_bInMultiplayerMode) {
        short reserve = m_weapon->m_reserve;
        if (reserve != m_unknown31d4) {
            EnableDumpCollisionsByType((EDumpType)1, m_weapon->m_type, (unsigned int)reserve < (unsigned int)m_weapon->m_properties->m_value12);
            m_unknown31d4 = reserve;
        }
    }
    if (m_flag1) {
        UpdateAnimation(dt);
        if (!m_player->m_flag397) {
            UpdateWeaponTransforms();
            m_weapon->Update(dt, false);
            if (m_weapon->GetMuzzleFlashStatus() && !m_player->m_flag396) {
                m_flashAngle = 0.0f;
                m_flashAngle = MathFunRandomReal(0.0f, 6.28f * dt);
                float scale = m_flashScale;
                if (m_unknown30f4) {
                    CMatrix world;
                    AttachMeshView* mesh = m_mesh;
                    CMatrix scaling;
                    scaling.BuildScale(scale);
                    CMatrix rotation(m_fireMatrix);
                    rotation.RotateY(m_flashAngle);
                    rotation.Multiply(scaling, rotation);
                    world.Multiply(rotation, m_matrices[mesh->points[m_firePoint].bone]);
                    scaling.BuildScale(*(const CVector3*)m_scale);
                    world.Multiply(world, scaling);
                    world.Multiply(world, m_matrix2340);
                    unsigned long effect = 0x67d7fa88;
                    if (m_flashEffect)
                        effect = m_flashEffect;
                    if (!g_bInMultiplayerMode) {
                        CParticleSystem* system = CParticleSystem::Create(effect);
                        if (system)
                            system->SetLocalToWorld(world);
                    }
                }
            }
        }
    }
}

void CPlayerWeaponObject::UpdateWeaponTransforms() {
    m_player->GetTMLocalToWorld(m_matrix2340);
    CMatrix head;
    head = m_player->m_matrixc0;
    m_matrix2340.Multiply(head, m_matrix2340);
    CMatrix local;
    CVector3 offset = m_offset;
    offset.y += 1.0f;
    local.BuildTrans(offset);
    local.RotateX(m_rotation[0] + m_player->m_pitch);
    local.RotateY(m_rotation[1]);
    local.RotateZ(m_rotation[2] + m_player->m_roll);
    m_matrix2340.Multiply(local, m_matrix2340);
    m_flag0 = m_player->m_flag08;
    CCamera* camera = &m_player->m_camera;
    CVector3 forward;
    CVector3 position;
    camera->GetForward(forward);
    camera->GetPosition(position);
    m_weapon->m_position = position;
    m_weapon->m_forward = forward;
    switch (m_weapon->m_type) {
    case 5:
    case 6:
    case 7:
    case 8:
    case 14:
    case 15:
    case 16:
    case 20:
    case 21:
    case 24:
    case 25:
    case 29:
    case 30:
    case 33:
    case 34:
    case 35:
    case 36:
        m_flag3 = 1;
        m_unknown30f4 = g_pStaticObjectMuzzleFlashPlayer;
        break;
    case 37:
    default:
        m_flag3 = 0;
        m_unknown30f4 = 0;
        goto done;
    }
    m_fireMatrix = *m_mesh->Find("Fire", m_firePoint);
done:
    if (m_flag3) {
        int bone = m_mesh->points[m_firePoint].bone;
        CMatrix matrix;
        matrix.BuildScale(CVector3(m_scale[0], m_scale[1], m_scale[2]));
        matrix.Multiply(matrix, m_fireMatrix);
        matrix.Multiply(matrix, m_matrices[bone]);
        matrix.Multiply(matrix, m_matrix2340);
        matrix.Multiply(matrix, camera->m_matrix80);
        m_vector3128 = matrix.GetPos();
    }
}

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
