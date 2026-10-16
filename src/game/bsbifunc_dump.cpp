// A fragment of bsbifunc.cpp (0x80026200): the script built-in that fires a
// projectile, then AdjustAim (file-local), which scatters an aim direction: a side vector (the direction crossed with +Z) and an up vector
// (the side crossed with the direction) are normalised, scaled by two random
// values within half the given spread either way, added to the direction,
// and the result is normalised again. Then script built-ins that create a
// dump object at the calling node's position and its script trigger's
// rotation (the dump type and flag read from the trigger for trigger types
// 2, 3 and 8; nothing for type 0 or -1) and set whether the current weapon
// of the node's soldier is drawn (a flag bit of the weapon), and make the
// calling player's current weapon stop shooting (a shot of type 1; the
// constants are entries of the file's .sdata2 pool). Each reads its argument
// below the script stack top and pops the built-in's arguments. The file
// name is this project's; the original record is bsbifunc.cpp and the
// built-ins around these are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (GetTMLocalToWorld at
// +60, GetPosition at +64, GetScriptObject at +96, AsStaticObject at +116,
// AsSoldierObject at +148, GetAIDoodad at +204); the vector view (four
// floats, 8-byte aligned, copied as two doublewords through a union, with
// inferred cross-product, normalisation and arithmetic helpers; the up
// vector is reused for the second cross product), the trigger position view
// (three floats the vector is built from), the matrix view (four vector
// rows, a class-initialising constructor), the quaternion view (built by an
// inline constructor), the trigger, script-object, soldier, weapon, weapon
// properties, static object, skeleton, bone, AI doodad and built-in record
// views are inferred (members at their offsets, names not original).
//
// BIFunc_FireProjectile reads a weapon CRC, an accuracy, a source and a
// target script object and a bone id. The weapon's properties give the
// bullet type, speed (default 5) and whether it is a projectile. The source
// position is the bone's matrix times the source node's world matrix when a
// bone id is given, else the node's position, else the trigger's position;
// the target position is the target node's position (with its AI doodad's
// velocity as a lead, scaled by the flight time) or the trigger's position.
// The aimed direction is scattered by AdjustAim, and the bullet factory
// creates the bullet with collision id 4 and the constant 512.
#include <math.h>

union CVector3Pair {
    double pair[2];
    float v[4];
};

struct TriggerPositionView {
    float x;
    float y;
    float z;
};

class CVector3 {
public:
    CVector3(const TriggerPositionView& v) : x(v.x), y(v.y), z(v.z) {}
    CVector3() {}
    CVector3(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
    }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    void SetCross(const CVector3& a, const CVector3& b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    float LengthSq() const { return x * x + y * y + z * z; }
    void Normalize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    CVector3& operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    CVector3& operator-=(const CVector3& o) {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        return *this;
    }
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

float MathFunRandomReal(float, float);

class CQuaternion {
public:
    CQuaternion(float ax, float ay, float az, float aw) : x(ax), y(ay), z(az), w(aw) {}

    float x;
    float y;
    float z;
    float w;
};

enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
class CAIDoodad;
struct BSObjectView;

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
    virtual BSObjectView* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual CPlayerObject* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
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
    virtual CAIDoodad* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};


struct TriggerCoreView {
    unsigned char unknown00[12];
    int type;
    TriggerPositionView position;
    float rx;
    float ry;
    float rz;
    float rw;
    unsigned char unknown2c[69];
    unsigned char dumpFlag;
    unsigned char unknown72[2];
    int dumpType;
    unsigned char unknown78[24];
    int dumpType3;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

enum EWeaponShootType {};

class CWeapon {
public:
    void Shoot(EWeaponShootType, float, float);

    unsigned char unknown000[480];
    unsigned char m_drawn : 1;
    unsigned char unknown1e0 : 7;
};

class CPlayerObject {
public:
    CWeapon* GetCurrentWeapon() const;

    unsigned char unknown0000[16400];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
};

void CreateDumpObject(const CVector3&, const CQuaternion&, int, int);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;


class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void Multiply(const CMatrix&, const CMatrix&);
    CVector3 GetPos() const { return pos; }

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
    static bool s_ClassInit;
} __attribute__((aligned(16)));

struct BoneView {
    CMatrix matrix;
    int id;
    unsigned char unknown44[12];
};

struct SkeletonView {
    unsigned char unknown00[40];
    BoneView* bones;
    int count;
};

struct MeshView {
    unsigned char unknown00[4];
    SkeletonView* skeleton;
};

class CStaticObject {
public:
    unsigned char unknown00[48];
    MeshView* mesh;
};

struct AITargetView {
    unsigned char unknown00[168];
    CVector3 velocity;
    CVector3 GetVelocity() const { return velocity; }
};

struct AIObjectView {
    unsigned char unknown00[8];
    AITargetView* target;
};

class CAIDoodad {
public:
    unsigned char unknown00[4];
    AIObjectView* ai;
};

struct WeaponProperties_struct {
    unsigned char unknown00[8];
    float speed;
    short bulletType;
    short projectile;
};

class CWeaponFactory {
public:
    WeaponProperties_struct* GetWeaponPropertiesByCRC(int, int*);
};

class CBullet;
class CBulletFactory {
public:
    CBullet* CreateBullet(int, bool, CVector3, CVector3, float, EClsnId, float, ISceneNode*);
};

extern CWeaponFactory* g_pWeaponFactory;
extern CBulletFactory* g_pBulletFactory;

static void AdjustAim(CVector3& dir, float accuracy);

void BIFunc_FireProjectile(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int crc = *(*stack - (count - 1));
    float accuracy = *(float*)(*stack - (count - 2));
    BSObjectView* from = *(BSObjectView**)(*stack - (count - 3));
    BSObjectView* to = *(BSObjectView**)(*stack - (count - 4));
    int boneId = *(*stack - (count - 5));
    int index = 0;
    int type = 0;
    float speed = 5.0f;
    bool projectile = true;
    WeaponProperties_struct* props = g_pWeaponFactory->GetWeaponPropertiesByCRC(crc, &index);
    if (props) {
        type = props->bulletType;
        speed = props->speed;
        projectile = props->projectile;
    }
    CVector3 start;
    CVector3 end;
    if (from && from->user && from->user->GetSceneNode()) {
        if (boneId) {
            SkeletonView* skeleton = from->user->GetSceneNode()->AsStaticObject()->mesh->skeleton;
            BoneView* bone = skeleton->bones;
            for (int i = 0; i < skeleton->count; i++) {
                if (bone->id == boneId)
                    break;
                bone++;
            }
            CMatrix matrix;
            from->user->GetSceneNode()->GetTMLocalToWorld(matrix);
            matrix.Multiply(bone->matrix, matrix);
            start = matrix.GetPos();
        } else {
            from->user->GetSceneNode()->GetPosition(start);
        }
    } else if (from && from->trigger && from->trigger->core) {
        TriggerCoreView* core = from->trigger->core;
        start = CVector3(core->position);
    }
    CVector3 lead(0.0f, 0.0f, 0.0f);
    if (to && to->user && to->user->GetSceneNode()) {
        ISceneNode* node = to->user->GetSceneNode();
        node->GetPosition(end);
        lead = node->GetAIDoodad()->ai->target->GetVelocity();
    } else if (to && to->trigger && to->trigger->core) {
        TriggerCoreView* core = to->trigger->core;
        end = CVector3(core->position);
    }
    end -= start;
    float time = sqrtf(end.LengthSq()) / speed;
    lead *= time;
    end += lead;
    AdjustAim(end, accuracy);
    g_pBulletFactory->CreateBullet(type, projectile, start, end, speed, (EClsnId)4, 512.0f, (ISceneNode*)object);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}


static void AdjustAim(CVector3& dir, float accuracy) {
    CVector3 up(0.0f, 0.0f, 1.0f);
    CVector3 side;
    side.SetCross(dir, up);
    side.Normalize();
    up.SetCross(side, dir);
    up.Normalize();
    float spread = 0.5f * accuracy;
    float a = MathFunRandomReal(-spread, spread);
    float b = MathFunRandomReal(-spread, spread);
    side *= a;
    up *= b;
    dir += side;
    dir += up;
    dir.Normalize();
}

void BIFunc_CreateDumpObject(int** stack, void* object) {
    ISceneNode* node = (ISceneNode*)object;
    TriggerCoreView* core = node->GetScriptObject()->trigger->core;
    int flag;
    int type;
    switch (core->type) {
    case 3:
        flag = core->dumpFlag;
        type = core->dumpType3;
        break;
    case 8:
        flag = core->dumpFlag;
        type = core->dumpType;
        break;
    case 2:
        flag = core->dumpFlag;
        type = core->dumpType;
        break;
    default:
        flag = 0;
        type = 0;
        break;
    }
    if (type != 0 && type != 0xffffffff) {
        CVector3 position;
        CQuaternion rotation(core->rx, core->ry, core->rz, core->rw);
        node->GetPosition(position);
        CreateDumpObject(position, rotation, type, flag);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetWeaponDraw(int** stack, void* object) {
    bool draw = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    CPlayerObject* soldier = ((ISceneNode*)object)->AsSoldierObject();
    CWeapon* weapon = soldier->m_weapons[soldier->m_currentWeapon];
    if (weapon)
        weapon->m_drawn = draw;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DisablePlayerShoot(int** stack, void* object) {
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    if (player->GetCurrentWeapon())
        player->GetCurrentWeapon()->Shoot((EWeaponShootType)1, 0.0f, -1.0f);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
