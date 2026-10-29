// Soldier_object.cpp from SetHelmetOffset (0x800a9f1c) to OnCollision
// (0x800ac310) as one unit. The file is compiled with deferred inlining
// (functions are emitted in reverse source order, so the source runs from
// OnCollision back to SetHelmetOffset) and with RTTI: CSoldierObject's first
// declared virtual, AttachObject, is defined here, so the unit emits its
// vtable, the RTTI records and base lists of IDestructible <- ISubject <-
// IObserver <- ISceneNode <- IMovingSceneNode <- CAnimObject <- CSoldierObject
// and the class names. The file's .rodata therefore starts with the header's
// dwi_prime_list, OnCollision's two table initialisers and the class names,
// and FireWeapon reaches its strings from that start.
//
// SetHelmetOffset moves the "head" node's hand matrix by the offset turned
// into the matrix's frame. AttachMesh attaches a mesh at the named skeleton
// node when the node has nothing attached (the mesh is lit; the node's matrix
// becomes the node's own matrix or, given an attachment CRC, the inverse of
// the mesh's matching attachment point times it). The weak
// CStaticMesh::EnableLighting (an inline virtual) is emitted after its first
// use. DetachObject clears the named node's attachment (and the bullet
// emitter's validity when it was that node) and returns it; AttachObject is
// AttachMesh for a static object's mesh (attachment type 1). FireMMG fires
// the mounted gun's weapon from the gun's muzzle (its matrix times the
// attachment point 0xE6E804D5) toward the target, the aim turned from the
// gun's front by a slerp limited to a cosine of 0.9848. FireWeapon prints a
// warning (class name and trigger position) when the current weapon slot is
// empty; otherwise it shoots the given weapon from the bullet emitter's
// matrix (as GetBulletEmitterMatrix computes it) or, without an emitter, from
// the matrix at +3904+15*64 in world space, the direction constrained to the
// emitter's front (0.9848) or the soldier's forward vector (0.766).
// GetBulletEmitterMatrix: with the emitter valid and something attached at
// its node, the node's bone matrix times the soldier's matrix, the node's
// hand matrix and the emitter matrix; else the matrix at +16224.
// EjectHelmet throws the helmet off (a special thrown object of the
// soldier's helmet type at the head's world matrix, sent off with the
// direction scaled by a random 0.002..0.01 and a random upward speed of
// 0.005..0.065), clears the head attachment and returns whether the helmet
// type stops bullets. The empty CBullet::GetVelocity is a weak copy of a
// header inline. HandleBulletCollision fills a 24-byte script event block
// (type, damage, shooter's script object, side hit, body part, hit count),
// scales the damage by body part (0.5 for limbs, 3 for the head, where the
// helmet may be knocked off with the soldier's percentage chance and then
// scale it by the helmet factor), applies two cheats, fires script event
// 43, counts a player shooter's hits and shot locations in g_Shell, plays an
// impact sound and places a blood particle system at the contact, oriented
// to its normal; a thrown bullet explodes instead. GetLookMatrix applies the
// "VisionCone" node's bone and own matrices to the soldier's matrix.
// AddWeapon replaces the weapon in a slot (0xfb9630c9 = none). SetAttachedLight
// places the light at the soldier's matrix pre-translated by the offset.
// OnCollision maps both collision ids of soldier/player pairs to classes and
// the class pair to a script event (188-190), pushes a player along the
// summed, flattened contact normals, and triggers 169 for world objects and
// 133/191 for static or hierarchy objects (a contact facing up or not).
//
// The classes, functions and globals are named by the mangled symbols; the
// virtual functions follow __vt__14CSoldierObject; members, views, the event
// and collision tables' layout and the inline helpers are inferred. CVector3
// has no copy constructor (its implicit copy moves double pairs); its
// assignment copies through a double pair, and CMatrix::GetPos builds its
// result from one. The constants are .sdata2 pool references. The rest of the
// file is in other units.
extern "C" int stricmp(const char*, const char*);
extern "C" double acos(double);
extern "C" double sin(double);
void DebugMsg(const char*, ...);
float MathFunRandomReal(float, float);
long long MathFunRandomI64(long long, long long);

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

// Header-defined hash-table primes; every unit that includes the header carries a copy.
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    CVector3(const CVector3Pair& p) { *(CVector3Pair*)this = p; }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    CVector3& operator+=(const CVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    CVector3& operator-=(const CVector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    CVector3& operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    float Length() const { return sqrtf(x * x + y * y + z * z); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    float Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }
    void Cross(const CVector3& a, const CVector3& b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    void Constrain(const CVector3&, float);

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
    void Ident();
    void Multiply(const CMatrix&, const CMatrix&);
    void Inverse(const CMatrix&);
    void TransformVector(CVector3&, CVector3) const;
    void TransformPoint(CVector3&, CVector3) const;
    void PreTranslate(CVector3);
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    CVector3 GetPos() const { return CVector3(*(const CVector3Pair*)&pos); }
    CVector3 GetFront() const { return front; }
    static void InitClass();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

class CCollision;
class CDrawContext;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticMesh;
class CStaticObject;
class CWeapon;
class CProjectileBullet;
class IMovingSceneNode;
struct BSObject;
struct BPDLightVolume;
struct TriggerObject_struct;
struct AIDoodadView;
enum EClsnId {};

/* the scene-node interfaces and the animated object, with the virtual
   functions in the order of __vt__14CSoldierObject */
class IDestructible {
public:
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
    virtual void Destroy();
};

class ISubject : public IDestructible {
};

class IObserver : public ISubject {
};

class ISceneNode : public IObserver {
public:
    enum EVolumeType {};
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
    virtual int GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual IMovingSceneNode* AsMovingNode();
    virtual const IMovingSceneNode* AsMovingNode() const;
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
    virtual ISceneNode* AsPlayerObject();
    virtual const ISceneNode* AsPlayerObject() const;
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
    virtual AIDoodadView* GetAIDoodad();
    virtual const AIDoodadView* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class IMovingSceneNode : public ISceneNode {
public:
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();
    virtual CVector3 GetWorldLinearVelocity() const;
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual BPDLightVolume* GetLightVolume();
    virtual IMovingSceneNode* AsMovingNode();
    virtual const IMovingSceneNode* AsMovingNode() const;
};

class CAnimObject : public IMovingSceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual void ConstrainVelocity();
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual int GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual BSObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();
    virtual CVector3 GetWorldLinearVelocity() const;
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual BPDLightVolume* GetLightVolume();
    virtual void Init();
    virtual void UpdateAnimation(float);
    virtual void SetScript(BSObject*);
    virtual void UpdateLocalBoundingVolume(EVolumeType, const CVector3&, const CVector3&);

    void OnCollision(const CCollision&);
};

/* inferred: an attachment point of a mesh (80 bytes) */
struct AttachPointView {
    CMatrix m_matrix;
    int m_crc;
    unsigned char unknown44[12];
};

struct AttachListView {
    unsigned char unknown00[40];
    AttachPointView* m_points;
    int m_count;
};

class CStaticMesh {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void EnableLighting(bool on) { m_lit = on; }

    AttachListView* m_attachList;
    unsigned char m_lit : 1;
};

/* inferred: the static object's mesh at +48 */
class CStaticObject {
public:
    unsigned char unknown00[48];
    CStaticMesh* m_mesh;
    unsigned char unknown34[12];
    CMatrix m_matrix;
};

enum EWeaponShootType {};

/* inferred: the weapon's shot direction (+672) and origin (+688) */
class CWeapon {
public:
    void Shoot(EWeaponShootType, float, float);

    unsigned char unknown000[672];
    CVector3 m_direction;
    CVector3 m_origin;
    unsigned char unknown2c0 : 5;
    unsigned char m_scriptFired : 1;
};

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
    virtual void SetTMLocalToWorld(const CMatrix&);
};

void* BSEventGetEventMemoryBlock(unsigned int);
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void PlayImpactSound(unsigned short, unsigned short, CVector3, bool);

enum EShotLocations {};

class CShellMenu {
public:
    void Set_hits(unsigned int, bool);
    void Set_shotLocation(unsigned int, EShotLocations, bool);

    unsigned char unknown0000[6620];
};

extern CShellMenu g_Shell;

struct CheatsView {
    unsigned char unknown00[12];
    bool m_bigDamage;
    bool m_headShotsOnly;
};

extern CheatsView g_cheats;

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

class CThrownBullet {
public:
    void Explode();
};

class CBullet {
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
    virtual void GetForward(CVector3&) const;
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
    virtual float GetDamage() const;
    virtual int GetScriptBulletType() const;
    virtual ISceneNode* GetFiredBy() const;
    virtual CThrownBullet* AsThrown();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&);
};
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

    unsigned char unknown004[760];
    int m_helmetState;
};
CThrownObject* CreateSpecialThrownObject(TriggerObject_struct*, int, int);

class CStaticObjectFactory {
public:
    static bool DoesHelmetTypeStopBullets(int);
};

/* inferred: the script object's chain to the soldier's data: the class
   record (name at +8) and the trigger's data (position at +16, kind at +53,
   helmet type at +72) */
struct SoldierClassView {
    unsigned char unknown00[8];
    char m_name[1];
};

struct SoldierDataView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    unsigned char unknown1c[25];
    unsigned char m_kind;
    unsigned char unknown36[18];
    int m_helmetType;
};

struct ScriptOwnerView {
    unsigned char unknown00[4];
    SoldierDataView* m_data;
};

struct BSObject {
    SoldierClassView* m_class;
    unsigned char unknown04[4];
    ScriptOwnerView* m_owner;
};

/* inferred: a collision contact (48 bytes): normal, position, body part */
struct ContactView {
    CVector3 m_normal;
    CVector3 m_position;
    unsigned char unknown20[4];
    int m_part;
    unsigned char unknown28[4];
    ContactView* m_next;
};

class CCollision {
public:
    ISceneNode* m_object0;
    ISceneNode* m_object1;
    unsigned char unknown08[16];
    ContactView* m_contacts;

    ContactView* GetContact() const { return m_contacts; }
};

/* inferred: the bullet event block (24 bytes) passed with script event 43 */
struct BulletHitEvent {
    BSObject* m_shooter;
    int m_damage;
    int m_side;
    int m_type;
    int m_part;
    int m_hitCount;
};

enum aistatus_var {};

class CAIStatus {
public:
    int GetVarValue(aistatus_var) const;
};

/* inferred: the AI doodad's filter object's AI object and its status */
struct AIObjectView {
    unsigned char unknown000[436];
    CAIStatus m_status;
};

struct AIFilterView {
    unsigned char unknown00[8];
    AIObjectView* m_object;
};

struct AIDoodadView {
    unsigned char unknown00[4];
    AIFilterView* m_filter;
};

/* inferred: collision id to class, and class pair to script event */
struct CollisionClassView {
    int m_id;
    int m_class;
};

struct CollisionEventView {
    int m_class0;
    int m_class1;
    int m_event;
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

class CSoldierObject : public CAnimObject {
public:
    virtual bool AttachObject(CStaticObject*, char*, int);
    virtual CStaticMesh* DetachObject(char*);
    virtual void GetAttachPointMatrix(char*, CMatrix*);
    virtual void AttachFloatingBone();
    virtual void DetachFloatingBone();
    virtual void InitAI();
    virtual ~CSoldierObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual AIDoodadView* GetAIDoodad();
    virtual const AIDoodadView* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual void Init();
    virtual void UpdateAnimation(float);
    virtual void UpdateLocalBoundingVolume(EVolumeType, const CVector3&, const CVector3&);

    void SetHelmetOffset(CVector3&);
    bool AttachMesh(CStaticMesh*, char*, int);
    bool FireMMG(CVector3&, CStaticObject*);
    bool FireWeapon(int, CVector3&, CVector3&, float);
    void GetBulletEmitterMatrix(CMatrix&);
    bool EjectHelmet(CVector3*);
    void GetLookMatrix(CMatrix&);
    bool AddWeapon(int, int);

    SkeletonView* GetSkeleton() const { return m_skeleton; }

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

    /* inferred: knocks the helmet off with the bullet's velocity; whether
       the helmet type stops bullets */
    bool KnockOffHelmet(CBullet* bullet) {
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
        RemoveAttachment("head");
        if (!CStaticObjectFactory::DoesHelmetTypeStopBullets(type)) {
            helmet->m_helmetState = 2;
            stops = false;
        } else {
            helmet->m_helmetState = 1;
        }
        CMatrix matrix;
        matrix.Multiply(m_bones[skeleton->m_nodes[node].m_bone], m_matrix);
        matrix.Multiply(m_handMatrices[node], matrix);
        helmet->SetTMLocalToWorld(matrix);
        CVector3 velocity;
        bullet->GetVelocity(velocity);
        float scale = MathFunRandomReal(0.002f, 0.01f);
        velocity.x *= scale;
        velocity.y *= scale;
        velocity.z *= scale;
        velocity.z = MathFunRandomReal(0.005f, 0.065f);
        helmet->SetVelocity(velocity);
        helmet->BeginUpdate(1.0f);
        return stops;
    }

    unsigned char unknown0004[3296];
    SkeletonView* m_skeleton;
    unsigned char unknown0ce8[600];
    CMatrix m_bones[80];
    CMatrix m_matrix;
    unsigned char unknown2380[16];
    BSObject* m_script;
    unsigned char unknown2394[7116];
    CMatrix m_matrix3f60;
    unsigned char unknown3fa0[112];
    CWeapon* m_weapons[9];
    CStaticMesh* m_attachments[8];
    unsigned int m_attachTypes[8];
    int m_currentWeapon;
    unsigned char unknown4078[8];
    CMatrix m_handMatrices[8];
    bool m_emitterValid;
    unsigned int m_emitterIndex;
    unsigned char unknown4288[8];
    CMatrix m_emitterMatrix;
    unsigned char unknown42d0[216];
    int m_helmetChance;
    float m_helmetDamageScale;
    unsigned char unknown43b0[212];
    bool m_firingMMG;
    CStaticObject* m_mmg;
    CWeapon* m_mmgWeapon;
    float m_hitTimer;
    int m_hitCount;
};

static inline void FindClasses(CollisionClassView* classes, int id, int otherId, int& myClass, int& otherClass) {
    int mine = -1;
    int theirs = -1;
    for (int i = 0; i < 7; i++) {
        if (id == classes[i].m_id)
            mine = classes[i].m_class;
        if (otherId == classes[i].m_id)
            theirs = classes[i].m_class;
    }
    myClass = mine;
    otherClass = theirs;
}
void CSoldierObject::OnCollision(const CCollision& collision) {
    CAnimObject::OnCollision(collision);
    ISceneNode* other = collision.m_object0 == this ? collision.m_object1 : collision.m_object0;
    if (other->AsSoldierObject() || other->AsPlayerObject()) {
        int id = GetCollisionId();
        int otherId = other->GetCollisionId();
        CollisionClassView classes[7] = {{2, 0}, {13, 0}, {1, 1}, {5, 1}, {7, 2}, {25, 2}, {-1, 2}};
        int otherClass;
        int myClass;
        FindClasses(classes, id, otherId, myClass, otherClass);
        CollisionEventView events[9] = {{0, 0, 189}, {0, 1, 188}, {0, 2, 190}, {1, 0, 188}, {1, 1, 189},
                                        {1, 2, 190}, {2, 0, 190}, {2, 1, 190}, {2, 2, 190}};
        int event = -1;
        for (int j = 0; j < 9; j++) {
            if (myClass == events[j].m_class0 && otherClass == events[j].m_class1) {
                event = events[j].m_event;
                break;
            }
        }
        if (!other->AsPlayerObject())
            BSObjectTriggerEvent(m_script, event, other->GetScriptObject(), 0, true);
        ISceneNode* player = other->AsPlayerObject();
        if (player) {
            if (player->GetAIDoodad()->m_filter->m_object->m_status.GetVarValue((aistatus_var)5) == 1)
                BSObjectTriggerEvent(m_script, event, other->GetScriptObject(), 0, true);
            bool first = collision.m_object0 == this;
            CVector3 normal;
            normal.x = normal.y = normal.z = 0.0f;
            for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
                normal.x += contact->m_normal.x;
                normal.y += contact->m_normal.y;
                normal.z += contact->m_normal.z;
            }
            if (!first) {
                normal.x *= -1.0f;
                normal.y *= -1.0f;
                normal.z *= -1.0f;
            }
            float force = 30.0f * (float)__fabs(normal.z);
            normal.z = 0.0f;
            if (force > 1.0f && sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z) > 0.0001f) {
                float length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
                if (length != 0.0f) {
                    normal.x *= 1.0f / length;
                    normal.y *= 1.0f / length;
                    normal.z *= 1.0f / length;
                }
                other->AsMovingNode()->ApplyForceTo(normal, force);
            }
        }
    } else if (other->AsWorldObject()) {
        BSObjectTriggerEvent(m_script, 169, (void*)&collision, 0, true);
    } else if (other->AsStaticObject() || other->AsHierObject()) {
        bool up = false;
        CVector3 vertical;
        vertical.x = 0.0f;
        vertical.y = 0.0f;
        vertical.z = 1.0f;
        for (ContactView* contact = collision.m_contacts; contact; contact++) {
            if (contact->m_normal.x * vertical.x + contact->m_normal.y * vertical.y + contact->m_normal.z * vertical.z > 0.8f) {
                up = true;
                break;
            }
        }
        if (up)
            BSObjectTriggerEvent(m_script, 133, (void*)&collision, 0, true);
        else
            BSObjectTriggerEvent(m_script, 191, (void*)&collision, 0, true);
    }
}

void CSoldierObject::SetAttachedLight(CLight* light, CVector3 offset) {
    if (light) {
        CMatrix matrix;
        GetTMLocalToWorld(matrix);
        matrix.PreTranslate(offset);
        light->SetTMLocalToWorld(matrix);
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

void CSoldierObject::GetLookMatrix(CMatrix& matrix) {
    SkeletonView* skeleton = GetSkeleton();
    unsigned int node = skeleton->Find("VisionCone");
    matrix = m_matrix;
    if (node != (unsigned int)-1) {
        matrix.Multiply(m_bones[skeleton->m_nodes[node].m_bone], matrix);
        matrix.Multiply(*skeleton->m_nodes[node].m_matrix, matrix);
    }
}

void CSoldierObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    CProjectileBullet* projectile = bullet->AsProjectile();
    CThrownBullet* thrown = bullet->AsThrown();
    if (projectile) {
        BulletHitEvent* event = (BulletHitEvent*)BSEventGetEventMemoryBlock(24);
        event->m_type = bullet->GetScriptBulletType();
        event->m_damage = (int)bullet->GetDamage();
        if (event->m_type == 1 && m_hitTimer == 0.0f)
            m_hitTimer = 0.25f;
        if (m_hitTimer > 0.0f)
            event->m_hitCount = ++m_hitCount;
        else
            event->m_hitCount = 1;
        ISceneNode* shooter = bullet->GetFiredBy();
        if (shooter)
            event->m_shooter = shooter->GetScriptObject();
        else
            event->m_shooter = 0;
        CVector3 direction;
        bullet->GetForward(direction);
        CVector3 forward;
        GetForward(forward);
        CVector3 right;
        GetRightward(right);
        float front = forward.x * direction.x + forward.y * direction.y + forward.z * direction.z;
        float side = right.x * direction.x + right.y * direction.y + right.z * direction.z;
        if (side < -0.7071f)
            event->m_side = 3;
        else if (side > 0.7071f)
            event->m_side = 2;
        else if (front < 0.0f)
            event->m_side = 0;
        else
            event->m_side = 1;
        ContactView* contact = collision.GetContact();
        bool stopped = false;
        event->m_part = contact->m_part;
        switch (event->m_part) {
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            event->m_damage = (int)(0.5f * event->m_damage);
            break;
        case 0:
        case 1: {
            float scale = 1.0f;
            if (MathFunRandomI64(0, 100) < m_helmetChance) {
                stopped = KnockOffHelmet(bullet);
                if (stopped)
                    scale = m_helmetDamageScale;
            }
            event->m_damage = (int)(scale * (3.0f * event->m_damage));
            break;
        }
        case 2:
        case 9:
        case 10:
        case 11:
        case 12:
            event->m_damage = (int)(1.0f * event->m_damage);
            break;
        }
        if (g_cheats.m_bigDamage && m_script->m_owner->m_data->m_kind == 2)
            event->m_damage = 1000;
        if (g_cheats.m_headShotsOnly && event->m_part != 0 && event->m_part != 1)
            event->m_damage = 0;
        BSObjectTriggerEvent(m_script, 43, event, 0, false);
        if (shooter && shooter->AsPlayerObject() && m_script->m_owner->m_data->m_kind == 2 && event->m_hitCount < 2) {
            g_Shell.Set_hits(1, false);
            switch (event->m_part) {
            case 3:
            case 4:
            case 5:
                g_Shell.Set_shotLocation(1, (EShotLocations)1, false);
                break;
            case 6:
            case 7:
            case 8:
                g_Shell.Set_shotLocation(1, (EShotLocations)2, false);
                break;
            case 13:
            case 14:
            case 15:
                g_Shell.Set_shotLocation(1, (EShotLocations)4, false);
                break;
            case 16:
            case 17:
            case 18:
                g_Shell.Set_shotLocation(1, (EShotLocations)5, false);
                break;
            case 19:
            case 20:
                g_Shell.Set_shotLocation(1, (EShotLocations)6, false);
                break;
            case 0:
            case 1:
                g_Shell.Set_shotLocation(1, (EShotLocations)0, false);
                break;
            case 2:
            case 9:
            case 10:
            case 11:
            case 12:
                g_Shell.Set_shotLocation(1, (EShotLocations)3, false);
                break;
            }
        }
        int sound = 14;
        if (stopped)
            sound = 15;
        PlayImpactSound(sound, 1, contact->m_position, false);
        CParticleSystem* blood = CParticleSystem::Create(0x6da45fe8);
        if (blood) {
            CMatrix matrix;
            matrix.Ident();
            CVector3 right;
            CVector3 front;
            if ((float)__fabs(contact->m_normal.Dot(CVector3(1.0f, 0.0f, 0.0f))) <= 0.9999f) {
                front.Cross(contact->m_normal, CVector3(1.0f, 0.0f, 0.0f));
                front.Normalize();
                right.Cross(front, contact->m_normal);
                right.Normalize();
            } else {
                right.Cross(contact->m_normal, CVector3(0.0f, 0.0f, 1.0f));
                right.Normalize();
                front.Cross(contact->m_normal, right);
                front.Normalize();
            }
            matrix.SetRight(right);
            matrix.SetFront(front);
            matrix.SetUp(contact->m_normal);
            matrix.SetPos(contact->m_position);
            blood->SetLocalToWorld(matrix);
        }
    } else if (thrown) {
        thrown->Explode();
    }
}

__declspec(weak) void CBullet::GetVelocity(CVector3&) {
}

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

void CSoldierObject::GetBulletEmitterMatrix(CMatrix& matrix) {
    if (m_emitterValid && m_attachTypes[m_emitterIndex]) {
        matrix.Multiply(m_bones[m_skeleton->m_nodes[m_emitterIndex].m_bone], m_matrix);
        matrix.Multiply(m_handMatrices[m_emitterIndex], matrix);
        matrix.Multiply(m_emitterMatrix, matrix);
    } else {
        matrix = m_matrix3f60;
    }
}

bool CSoldierObject::FireWeapon(int weapon, CVector3&, CVector3& target, float speed) {
    if (!m_weapons[m_currentWeapon]) {
        DebugMsg("WEAPON WARNING WEAPON WARNING WEAPON WARNING\n");
        DebugMsg("WEAPON WARNING WEAPON WARNING WEAPON WARNING\n");
        DebugMsg("WEAPON WARNING WEAPON WARNING WEAPON WARNING\n");
        DebugMsg("A person is trying to fire a weapon from script although the person's property\n");
        DebugMsg("data says that the person is not supposed to have a weapon!!\n");
        DebugMsg("Persons Class: %s", m_script->m_class->m_name);
        SoldierDataView* placement = m_script->m_owner->m_data;
        DebugMsg("Persons Trigger Location: x: %d, y: %d, z: %d\n", (int)placement->x, (int)placement->y, (int)placement->z);
        return false;
    }
    if (m_emitterValid) {
        CMatrix matrix;
        m_weapons[m_currentWeapon]->m_scriptFired = true;
        if (m_emitterValid && m_attachTypes[m_emitterIndex]) {
            matrix.Multiply(m_bones[m_skeleton->m_nodes[m_emitterIndex].m_bone], m_matrix);
            matrix.Multiply(m_handMatrices[m_emitterIndex], matrix);
            matrix.Multiply(m_emitterMatrix, matrix);
        } else {
            matrix = m_matrix3f60;
        }
        CVector3 origin;
        CVector3 front;
        front = matrix.front;
        origin = matrix.pos;
        CVector3 direction;
        direction = target;
        direction.Normalize();
        direction.Constrain(front, 0.9848f);
        m_weapons[weapon]->m_direction = direction;
        m_weapons[weapon]->m_origin = origin;
        m_weapons[weapon]->Shoot((EWeaponShootType)2, speed, -1.0f);
        return true;
    }
    CMatrix matrix = m_bones[15];
    CVector3 origin;
    origin = matrix.pos;
    m_matrix.TransformPoint(origin, origin);
    CVector3 direction;
    direction = target;
    direction.Normalize();
    CVector3 forward;
    GetForward(forward);
    direction.Constrain(forward, 0.766f);
    m_weapons[weapon]->m_direction = direction;
    m_weapons[weapon]->m_origin = origin;
    m_weapons[weapon]->Shoot((EWeaponShootType)2, speed, -1.0f);
    return true;
}

bool CSoldierObject::FireMMG(CVector3& target, CStaticObject* gun) {
    AttachListView* list = gun->m_mesh->m_attachList;
    int count = list->m_count;
    AttachPointView* point = list->m_points;
    CMatrix matrix;
    matrix = gun->m_matrix;
    for (int i = 0; i < count; i++, point++) {
        if (point->m_crc == 0xE6E804D5) {
            matrix.Multiply(point->m_matrix, matrix);
            break;
        }
    }
    CVector3 origin;
    origin = matrix.pos;
    CVector3 forward;
    forward = matrix.front;
    CVector3 direction;
    direction = target;
    direction -= origin;
    CVector3 aim;
    aim = direction;
    aim.Normalize();
    float cosine = forward.Dot(aim);
    const float limit = 0.9848f;
    if (cosine < limit) {
        float maxAngle = acos(limit);
        float angle = acos(cosine);
        float sinAngle = sin(angle);
        float a = (float)sin(angle - maxAngle) / sinAngle;
        float b = (float)sin(maxAngle) / sinAngle;
        aim *= b;
        forward *= a;
        aim += forward;
    }
    m_mmgWeapon->m_direction = aim;
    m_mmgWeapon->m_origin = origin;
    m_mmgWeapon->Shoot((EWeaponShootType)2, 0.0f, -1.0f);
    m_firingMMG = true;
    m_mmg = gun;
    return true;
}

bool CSoldierObject::AttachObject(CStaticObject* object, char* name, int crc) {
    SkeletonView* skeleton = GetSkeleton();
    if (!object)
        return false;
    unsigned int node = skeleton->Find(name);
    if (node == (unsigned int)-1)
        return false;
    if (m_attachTypes[node])
        return false;
    m_attachments[node] = (CStaticMesh*)object;
    m_attachTypes[node] = 1;
    CStaticMesh* mesh = object->m_mesh;
    if (mesh)
        mesh->EnableLighting(true);
    CMatrix inverse;
    if (crc) {
        int i;
        AttachListView* list = mesh->m_attachList;
        int count = list->m_count;
        AttachPointView* point = list->m_points;
        for (i = 0; i < count; i++, point++) {
            if (point->m_crc == crc) {
                inverse.Inverse(point->m_matrix);
                break;
            }
        }
        m_handMatrices[node] = *skeleton->m_nodes[node].m_matrix;
        if (i < count)
            m_handMatrices[node].Multiply(inverse, m_handMatrices[node]);
    } else {
        m_handMatrices[node] = *skeleton->m_nodes[node].m_matrix;
    }
    return true;
}

CStaticMesh* CSoldierObject::DetachObject(char* name) {
    SkeletonView* skeleton = GetSkeleton();
    unsigned int i;
    for (i = 0; i < skeleton->m_count; i++) {
        if (stricmp(skeleton->m_nodes[i].m_name, name) == 0)
            break;
    }
    if (i == skeleton->m_count)
        return 0;
    CStaticMesh* mesh = m_attachments[i];
    m_attachments[i] = 0;
    m_attachTypes[i] = 0;
    if (m_emitterIndex == i)
        m_emitterValid = false;
    return mesh;
}

bool CSoldierObject::AttachMesh(CStaticMesh* mesh, char* name, int crc) {
    SkeletonView* skeleton = GetSkeleton();
    if (!mesh)
        return false;
    unsigned int node = skeleton->Find(name);
    if (node == (unsigned int)-1)
        return false;
    if (m_attachTypes[node])
        return false;
    m_attachments[node] = mesh;
    m_attachTypes[node] = 2;
    m_attachments[node]->EnableLighting(true);
    CMatrix inverse;
    if (crc) {
        int i;
        AttachListView* list = mesh->m_attachList;
        int count = list->m_count;
        AttachPointView* point = list->m_points;
        for (i = 0; i < count; i++, point++) {
            if (point->m_crc == crc) {
                i = count;
                inverse.Inverse(point->m_matrix);
            }
        }
        m_handMatrices[node].Multiply(inverse, *skeleton->m_nodes[node].m_matrix);
    } else {
        m_handMatrices[node] = *skeleton->m_nodes[node].m_matrix;
    }
    return true;
}

void CSoldierObject::SetHelmetOffset(CVector3& offset) {
    unsigned int head = m_skeleton->Find("head");
    CMatrix& matrix = m_handMatrices[head];
    CVector3 pos;
    pos = matrix.GetPos();
    CVector3 move;
    matrix.TransformVector(move, offset);
    pos += move;
    matrix.SetPos(pos);
}
