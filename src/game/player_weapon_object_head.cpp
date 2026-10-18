// A fragment of player_weapon_object.cpp; the file name is this project's. CPlayerWeaponObject, the functions at the start of the file: initialisation,
// identity casts, visibility (the draw-enabled virtual), the sleeve texture
// from the level's texture pack, destruction through the animated-object
// factory, the player's muzzle flag and the weapon name. The class names come
// from the mangled symbols. CPlayerWeaponObject is an inferred, non-virtual
// view whose CAnimObject base (at the start of the object) is reached through
// casts; CAnimObject is declared with the ISceneNode virtual functions in the
// order of __vt__10ISceneNode, flattened. Only the members these functions
// touch are declared, at their offsets (their names are not original). The
// weapon's flag byte is an inferred bit-field view. CreateEjectedShell, the
// last of them, throws a spent casing of the given type from the weapon's
// "Casing" node (else its "Fire" node; neither: nothing): the casing starts
// at the node's world position (the node's matrix scaled by the weapon's
// scale and placed by its bone, through the weapon's matrix) with a random
// velocity per casing type (two type CRCs and a default; in units per frame,
// times 0.016683) turned by the weapon's matrix plus half the owner's
// velocity, and a random spin applied to the weapon's orientation; it is then
// started. The skeleton, casing and owner views and the node search are
// inferred; the constants and node names are .sdata2 pool entries. The rest
// of the file is not reconstructed.
extern "C" char* strcpy(char*, const char*);
extern "C" int stricmp(const char*, const char*);

float MathFunRandomReal(float, float);

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { d = other.d; }
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }

    void Scale(const CVector3& a, float s) {
        d.v[0] = s * a.d.v[0];
        d.v[1] = s * a.d.v[1];
        d.v[2] = s * a.d.v[2];
    }
    CVector3& operator+=(const CVector3& v) {
        d.v[0] += v.d.v[0];
        d.v[1] += v.d.v[1];
        d.v[2] += v.d.v[2];
        return *this;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

inline CVector3 operator*(float f, const CVector3& v) { return CVector3(f * v.d.v[0], f * v.d.v[1], f * v.d.v[2]); }

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix(const CMatrix& other) { *this = other; }
    static void InitClass();
    CMatrix& operator=(const CMatrix&);
    void BuildScale(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);
    void TransformPoint(CVector3&, CVector3) const;
    void TransformVector(CVector3&, CVector3) const;
    void RotateX(float);
    void RotateY(float);
    void RotateZ(float);

    CVector3 m_right;
    CVector3 m_front;
    CVector3 m_up;
    CVector3 m_pos;
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);

    float x;
    float y;
    float z;
    float w;
};

struct TriggerObject_struct;

class CThrownObject {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void BeginUpdate(float);
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
    virtual void SetCollisionId(int);
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual void vf0cc();
    virtual void vf0d0();
    virtual void vf0d4();
    virtual void vf0d8();
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
    virtual void SetPosition(CVector3);
    void SetVelocity(CVector3&);

    unsigned char unknown004[692];
    CQuaternion m_rotation;
};

CThrownObject* CreateSpecialThrownObject(TriggerObject_struct*, int, int);

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

    /* inferred: the index of the named node, or the count */
    unsigned int Find(const char* name) const {
        unsigned int i;
        for (i = 0; i < m_count; i++) {
            if (stricmp(m_nodes[i].m_name, name) == 0)
                break;
        }
        return i;
    }
};

/* inferred: the weapon's owner, its velocity through slot +276 */
class WeaponOwnerView {
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
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual void vf06c();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual void vf08c();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual void vf09c();
    virtual void vf0a0();
    virtual void vf0a4();
    virtual void vf0a8();
    virtual void vf0ac();
    virtual void vf0b0();
    virtual void vf0b4();
    virtual void vf0b8();
    virtual void vf0bc();
    virtual void vf0c0();
    virtual void vf0c4();
    virtual void vf0c8();
    virtual void vf0cc();
    virtual void vf0d0();
    virtual void vf0d4();
    virtual void vf0d8();
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
    virtual CVector3 GetVelocity() const;
};

class CDrawContext;
class CCollision;
class CMatrix;
class CVector3;
class CTexture;
struct LevelFileContentsStruct_;
enum TLTResourceID {};

LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFromRepository(const char*, int*, bool*);

struct LevelResourceView {
    unsigned char unknown00[228];
    const char* name;
};

struct LevelFileContentsView {
    unsigned char unknown00[8];
    LevelResourceView* resource;
};

class CTexturePack {
public:
    CTexture* FindTexture(char*);
};

class CWeaponFlagsView {
public:
    unsigned char unknown000[704];
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char flag2 : 1;
    unsigned char flag3 : 1;
    unsigned char flag4 : 1;
    unsigned char muzzle : 1;
    unsigned char flag6 : 1;
    unsigned char flag7 : 1;
};

class CPlayerObject {
public:
    CWeaponFlagsView* GetCurrentWeapon() const;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    char data[328];
};

extern CScene g_scene;

struct BSObject;

class CAnimObject {
public:
    enum EVolumeType {};

    virtual void MarkForDestruction(int);
    virtual ~CAnimObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual const void* GetLocalBoundingVolume(EVolumeType) const;
    virtual const void* GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;

    void Init();
};

class CAnimObjectFactory {
public:
    void ReleaseObject(CAnimObject*);

    unsigned char data[16];
};

extern CAnimObjectFactory g_AnimObjFactory;

class CPlayerWeaponObject {
public:
    void Init();
    const CPlayerWeaponObject* AsPlayerWeaponObject() const;
    CPlayerWeaponObject* AsPlayerWeaponObject();
    BSObject* GetScriptObject() const;
    bool IsVisible(CDrawContext&) const;
    void SetSleeveTexture(char*);
    void Destroy();
    static void InitClass();
    void SetWeaponName(const char*);
    void CreateEjectedShell(int);

    unsigned char unknown0000[3300];
    SkeletonView* m_skeleton;
    SkeletonView* GetSkeleton() const { return m_skeleton; }
    unsigned char unknown0ce8[600];
    CMatrix m_bones[80];
    CMatrix m_matrix;
    unsigned char unknown2380[3424];
    WeaponOwnerView* m_owner;
    unsigned char unknown30e4[52];
    float m_scaleX;
    float m_scaleY;
    float m_scaleZ;
    unsigned char unknown3124[132];
    int m_attachNode;
    unsigned char unknown31ac[8];
    char m_weaponName[36];
    CTexture* m_sleeveTexture;
};

__declspec(weak) void CPlayerWeaponObject::Init() {
    // The CAnimObject base is at the start of the object.
    ((CAnimObject*)this)->Init();
}

__declspec(weak) const CPlayerWeaponObject* CPlayerWeaponObject::AsPlayerWeaponObject() const {
    return this;
}

__declspec(weak) CPlayerWeaponObject* CPlayerWeaponObject::AsPlayerWeaponObject() {
    return this;
}

__declspec(weak) BSObject* CPlayerWeaponObject::GetScriptObject() const {
    return 0;
}

bool CPlayerWeaponObject::IsVisible(CDrawContext&) const {
    return ((const CAnimObject*)this)->IsDrawEnabled();
}

void CPlayerWeaponObject::SetSleeveTexture(char* name) {
    bool loaded = false;
    LevelFileContentsView* contents = (LevelFileContentsView*)TLT_FindNextResourceByType((TLTResourceID)13, 0);
    CTexturePack* pack = (CTexturePack*)TLT_LoadFromRepository(contents->resource->name, 0, &loaded);
    m_sleeveTexture = pack->FindTexture(name);
}

void CPlayerWeaponObject::Destroy() {
    g_AnimObjFactory.ReleaseObject((CAnimObject*)this);
}

void CPlayerWeaponObject::InitClass() {
}

void SetMuzzleStatusForPlayer(bool) {
    CWeaponFlagsView* weapon = g_scene.GetPlayer(0)->GetCurrentWeapon();
    weapon->muzzle = true;
}

void CPlayerWeaponObject::SetWeaponName(const char* name) {
    if (name)
        strcpy(m_weaponName, name);
    else
        m_weaponName[0] = 0;
}

void CPlayerWeaponObject::CreateEjectedShell(int type) {
    SkeletonView* skeleton = GetSkeleton();
    CThrownObject* shell = CreateSpecialThrownObject(0, type, -1);
    if (!shell)
        return;
    unsigned int node;
    for (node = 0; node < skeleton->m_count; node++) {
        if (stricmp(skeleton->m_nodes[node].m_name, "Casing") == 0)
            break;
    }
    if (node == skeleton->m_count) {
        for (node = 0; node < skeleton->m_count; node++) {
            if (stricmp(skeleton->m_nodes[node].m_name, "Fire") == 0)
                break;
        }
        if (node == skeleton->m_count)
            return;
    }
    CMatrix local(*skeleton->m_nodes[node].m_matrix);
    CMatrix world;
    world.BuildScale(CVector3(m_scaleX, m_scaleY, m_scaleZ));
    world.Multiply(world, local);
    world.Multiply(world, m_bones[skeleton->m_nodes[m_attachNode].m_bone]);
    CVector3 position;
    CVector3 point;
    m_matrix.TransformPoint(point, world.m_pos);
    position = point;
    float vx;
    float vy;
    float vz;
    float rx;
    float ry;
    float rz;
    switch (type) {
    case (int)0xf2c8fb78:
        vx = 0.016683351f * MathFunRandomReal(-2.0f, 0.0f);
        vy = 0.016683351f * MathFunRandomReal(2.0f, 4.0f);
        vz = 0.016683351f * MathFunRandomReal(2.0f, 3.0f);
        rx = MathFunRandomReal(-0.5f, 0.5f);
        ry = MathFunRandomReal(-0.5f, 0.5f);
        rz = MathFunRandomReal(-0.5f, 0.5f);
        break;
    case (int)0xb2134d62:
        vx = 0.016683351f * MathFunRandomReal(0.5f, 1.0f);
        vy = 0.016683351f * MathFunRandomReal(0.0f, 1.0f);
        vz = 0.016683351f * MathFunRandomReal(3.5f, 4.5f);
        rx = MathFunRandomReal(-0.1f, 0.1f);
        ry = MathFunRandomReal(-0.1f, 0.1f);
        rz = MathFunRandomReal(-0.1f, 0.1f);
        break;
    default:
        vx = 0.016683351f * MathFunRandomReal(2.0f, 2.5f);
        vy = 0.016683351f * MathFunRandomReal(0.0f, 0.5f);
        vz = 0.016683351f * MathFunRandomReal(1.0f, 2.0f);
        rx = MathFunRandomReal(-0.5f, 0.5f);
        ry = MathFunRandomReal(-0.5f, 0.5f);
        rz = MathFunRandomReal(-0.5f, 0.5f);
        break;
    }
    CVector3 velocity(vx, vy, vz);
    m_matrix.TransformVector(velocity, velocity);
    shell->SetPosition(position);
    CVector3 scaled;
    scaled.Scale(m_owner->GetVelocity(), 0.5f);
    velocity += scaled;
    shell->SetCollisionId(-1);
    shell->SetVelocity(velocity);
    CMatrix spin(m_matrix);
    spin.RotateX(rx);
    spin.RotateY(ry);
    spin.RotateZ(rz);
    shell->m_rotation.SetFromMatrix(spin);
    shell->BeginUpdate(1.0f);
}
