// A fragment of Soldier_object.cpp (0x800ac9e4): CSoldierObject::Draw.
// Nothing is drawn while either of two configuration flags is set. The
// model's and the head's textures are bound; while the soldier fades out,
// past half the fade its weapons are removed once (after three quarters;
// RemoveWeapons' body, inlined) and both materials get an alpha override of
// 128 times the remaining fraction (also stored at +17464), which a cheat
// forces to 0. The object and the head are drawn with a copy of the draw
// context, then each skeleton node's attachment at the node's world matrix
// (static objects: their mesh, lit by the context's light volume, and the
// muzzle flash when it is the current weapon's mesh; meshes directly), the
// floating-bone object at the soldier's matrix offset and rotated by the
// head angles (with the mounted gun's muzzle flash while it fires, after
// which the firing state is cleared), and the shadow with the original
// context. The classes, functions and globals are named by the mangled
// symbols; the members, the draw-context layout (its implicit copy uses
// CMatrix's copy constructor), the material views and the virtual slots'
// names are inferred. The strings and constants are .sdata2 pool
// references. The rest of the file is in other units.
class CTexture;
class CWeapon;
struct BPDLightVolume;

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
    CMatrix(const CMatrix& other) { *this = other; }
    CMatrix& operator=(const CMatrix&);
    void Multiply(const CMatrix&, const CMatrix&);
    void PreTranslate(CVector3);
    void RotateX(long);
    void RotateY(long);
    void RotateZ(long);
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

/* inferred layout */
class CDrawContext {
public:
    CMatrix m_matrix0;
    CMatrix m_matrix1;
    CMatrix m_matrix2;
    CMatrix* m_object;
    int m_value0c4;
    int m_value0c8;
    int m_value0cc;
    int m_value0d0;
    int m_value0d4;
    int m_value0d8;
    CVector3 m_vector;
    int m_value0f0;
    int m_value0f4;
    BPDLightVolume* m_lightVolume;
    int m_value0fc;
    int m_value100;
    float m_value104;
    float m_value108;
    float m_value10c;
    float m_value110;
    int m_value114;
};

class CStaticMesh {
public:
    virtual void vf008();
    virtual void Draw(CDrawContext&, int);
    virtual void EnableLighting(bool);
    virtual bool IsLightingEnabled();
    static void SetLightVolume(BPDLightVolume*);
};

class CStaticObject {
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
    virtual bool IsVisible();
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
    virtual void SetTMLocalToWorld(const CMatrix&);

    unsigned char unknown04[44];
    CStaticMesh* m_mesh;
};

class CAnimated {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void Draw(CDrawContext&);
    void BindTexture(CTexture*, char);
};

class CShadow {
public:
    virtual void vf008();
    virtual void Draw(CDrawContext&);
};

/* inferred: a material with an alpha override flag and value */
struct MaterialView {
    void SetAlphaOverride() { m_flags = (m_flags & ~0x100) | 0x100; }
    unsigned int m_flags;
    unsigned char m_alpha;
};

class ISceneNode;

class CScene {
public:
    bool IsNodeInScene(const ISceneNode*) const;
    void Remove(ISceneNode&);

    unsigned char unknown000[328];
};

extern CScene g_scene;

class CWeaponFactory {
public:
    void DestroyWeapon(CWeapon*);

    unsigned char unknown00[28];
};

extern CWeaponFactory g_WeaponFactory;

struct ConfigView {
    unsigned char unknown00[3];
    bool m_flag3;
    bool m_flag4;
    unsigned char unknown05[15];
};

extern ConfigView g_config;

struct CheatsView {
    unsigned char unknown00[15];
    bool m_flag15;
};

extern CheatsView g_cheats;

extern "C" int stricmp(const char*, const char*);

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
};

class CAnimObject {
public:
    void Draw(CDrawContext&);
};

class CWeaponView {
public:
    unsigned char unknown00[48];
    void* m_mesh;
};

class CSoldierObject : public CAnimObject {
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
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual void vf120();
    virtual void vf124();
    virtual void vf128();
    virtual void vf12c();
    virtual void vf130();
    virtual void vf134();
    virtual void* DetachObject(char*);

    void Draw(CDrawContext&);
    void DrawMuzzleFlash(CDrawContext&, CWeapon*, bool);
    /* RemoveWeapons' body (inlined here) */
    void RemoveWeapons() {
        DetachObject("lt_hand");
        DetachObject("rt_hand");
        int i;
        for (i = 0; i < 9; i++) {
            if (m_weapons[i]) {
                if (g_scene.IsNodeInScene((ISceneNode*)m_weapons[i]))
                    g_scene.Remove(*(ISceneNode*)m_weapons[i]);
                g_WeaponFactory.DestroyWeapon(m_weapons[i]);
                m_weapons[i] = 0;
            }
        }
        if (m_mmgWeapon) {
            g_WeaponFactory.DestroyWeapon(m_mmgWeapon);
            m_mmgWeapon = 0;
        }
    }
    SkeletonView* GetSkeleton() const { return m_skeleton; }

    unsigned char unknown0004[44];
    CAnimated m_model;
    unsigned char unknown0034[2612];
    MaterialView m_modelMaterial;
    unsigned char unknown0a70[628];
    SkeletonView* m_skeleton;
    unsigned char unknown0ce8[48];
    CVector3 m_headOffset;
    unsigned char unknown0d28[4];
    long m_headAngleX;
    long m_headAngleY;
    long m_headAngleZ;
    unsigned char unknown0d38[520];
    CMatrix m_bones[80];
    CMatrix m_matrix;
    unsigned char unknown2380[3508];
    CTexture* m_modelTexture;
    CTexture* m_headTexture;
    char m_modelTextureFlag;
    unsigned char unknown313d[51];
    CAnimated m_head;
    unsigned char unknown3174[2612];
    MaterialView m_headMaterial;
    unsigned char unknown3bb0[1040];
    bool m_floatingBoneAttached;
    unsigned char unknown3fc1[3];
    CStaticObject* m_floatingObject;
    unsigned char unknown3fc8[8];
    CMatrix m_floatingMatrix;
    CWeapon* m_weapons[9];
    void* m_attachments[8];
    unsigned int m_attachTypes[8];
    int m_currentWeapon;
    unsigned char unknown4078[8];
    CMatrix m_handMatrices[8];
    unsigned char unknown4280[304];
    bool m_fading;
    unsigned char unknown43b1[3];
    float m_fadeTime;
    float m_fadeDuration;
    unsigned char unknown43bc[4];
    bool m_weaponsRemoved;
    unsigned char unknown43c1[15];
    CShadow m_shadow;
    unsigned char unknown43d4[100];
    float m_alpha;
    unsigned char unknown443c[72];
    bool m_firingMMG;
    unsigned char unknown4485[3];
    CStaticObject* m_mmg;
    CWeapon* m_mmgWeapon;
};

inline void SetMeshLight(BPDLightVolume* v) { CStaticMesh::SetLightVolume(v); }
inline void DrawAnimObject(CSoldierObject* o, CDrawContext& c) { o->CAnimObject::Draw(c); }

void CSoldierObject::Draw(CDrawContext& context) {
    CDrawContext local = context;
    if (g_config.m_flag3 || g_config.m_flag4)
        return;
    m_model.BindTexture(m_modelTexture, m_modelTextureFlag);
    m_head.BindTexture(m_headTexture, -1);
    if (m_fading && m_fadeTime > 0.5f * m_fadeDuration) {
        if (!m_weaponsRemoved && m_fadeTime > 0.75f * m_fadeDuration) {
            m_weaponsRemoved = true;
            RemoveWeapons();
        }
        float t = (m_fadeTime - 0.5f * m_fadeDuration) / (0.5f * m_fadeDuration);
        if (t >= 1.0f)
            t = 1.0f;
        float alpha = 1.0f - t;
        int value = 128.0f * alpha;
        m_modelMaterial.SetAlphaOverride();
        m_modelMaterial.m_alpha = value;
        m_headMaterial.SetAlphaOverride();
        m_headMaterial.m_alpha = value;
        m_alpha = alpha;
    }
    if (g_cheats.m_flag15) {
        m_modelMaterial.SetAlphaOverride();
        m_modelMaterial.m_alpha = 0;
        m_headMaterial.SetAlphaOverride();
        m_headMaterial.m_alpha = 0;
    }
    DrawAnimObject(this, local);
    m_head.Draw(local);
    SkeletonView* skeleton = GetSkeleton();
    CMatrix matrix;
    for (unsigned int i = 0; i < skeleton->m_count; i++) {
        if (m_attachTypes[i])
        switch (m_attachTypes[i]) {
        case 1: {
            matrix.Multiply(m_bones[skeleton->m_nodes[i].m_bone], m_matrix);
            matrix.Multiply(m_handMatrices[i], matrix);
            CStaticObject* object = (CStaticObject*)m_attachments[i];
            object->SetTMLocalToWorld(matrix);
            CStaticMesh* mesh = ((CStaticObject*)m_attachments[i])->m_mesh;
            if (mesh->IsLightingEnabled())
                SetMeshLight(local.m_lightVolume);
            local.m_object = &matrix;
            if (((CStaticObject*)m_attachments[i])->IsVisible())
                mesh->Draw(local, 0);
            CWeaponView* weapon = (CWeaponView*)m_weapons[m_currentWeapon];
            if (weapon && mesh == weapon->m_mesh)
                DrawMuzzleFlash(local, (CWeapon*)weapon, false);
            break;
        }
        case 2:
            matrix.Multiply(m_bones[skeleton->m_nodes[i].m_bone], m_matrix);
            matrix.Multiply(m_handMatrices[i], matrix);
            local.m_object = &matrix;
            if (((CStaticMesh*)m_attachments[i])->IsLightingEnabled())
                SetMeshLight(local.m_lightVolume);
            ((CStaticMesh*)m_attachments[i])->Draw(local, 0);
            break;
        }
    }
    if (m_floatingBoneAttached && m_floatingObject) {
        CMatrix floating;
        floating = m_matrix;
        floating.PreTranslate(m_headOffset);
        floating.RotateZ(-m_headAngleZ);
        floating.RotateY(-m_headAngleY);
        floating.RotateX(-m_headAngleX);
        floating.Multiply(m_floatingMatrix, floating);
        m_floatingObject->SetTMLocalToWorld(floating);
        if (m_firingMMG)
            DrawMuzzleFlash(local, m_mmgWeapon, true);
    }
    m_firingMMG = false;
    m_mmg = 0;
    m_shadow.Draw(context);
}
