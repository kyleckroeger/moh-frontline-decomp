// A fragment of bsbifunc.cpp (0x80023048): GetMPWeaponPrompt, which maps a
// script bullet type (0 to 25) to the text id of its multiplayer weapon prompt
// (-1 for other types) through a jump table. The cases are written in the order
// of their bodies in the original. It is followed by FireProjectileAtPath: a
// projectile of the weapon with the given CRC (bullet type, speed and
// projectile flag from its properties; type 0, speed 5 otherwise) fired from a
// script object (a bone of its node's skeleton, its node's position, or its
// trigger's position) towards a point of a spline path (the position along the
// path scaled by its segment count, expanded within that segment), with the
// aim adjusted by the accuracy. The file name is this project's; the original
// record is bsbifunc.cpp and the functions around these are not reconstructed.
// The functions, classes and EScriptBulletType are named by the mangled
// symbols; the enumerator names are unknown (the cases are written as numbers)
// and the int return type is inferred, as are the vector, matrix, skeleton,
// weapon, spline path, AI filter and script object views (members at their
// offsets, names not original) and the path accessors.
#include <math.h>

enum EScriptBulletType {};

int GetMPWeaponPrompt(EScriptBulletType type) {
    switch (type) {
    case 7:
        return 407;
    case 0:
        return 408;
    case 4:
        return 409;
    case 8:
        return 410;
    case 9:
        return 411;
    case 10:
        return 412;
    case 11:
        return 415;
    case 12:
        return 416;
    case 5:
        return 417;
    case 13:
        return 418;
    case 14:
        return 419;
    case 15:
        return 420;
    case 16:
        return 423;
    case 17:
        return 424;
    case 6:
        return 425;
    case 18:
        return 426;
    case 19:
        return 429;
    case 20:
        return 432;
    case 2:
        return 433;
    case 1:
        return 435;
    case 21:
        return 437;
    case 22:
        return 438;
    case 23:
        return 439;
    case 3:
        return 440;
    case 24:
        return 441;
    case 25:
        return 767;
    }
    return -1;
}

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

/* File-local in bsbifunc.cpp (defined with FireProjectile); declared non-static here. */
void AdjustAim(CVector3& dir, float accuracy);

class CAISplinePathSegment {
public:
    void Expand(float, CVector3&);

    unsigned char unknown00[72];
};

class CAISplinePath {
public:
    void GetPoint(float position, CVector3& point) {
        float t = position * m_count;
        int segment = (int)t;
        t -= segment;
        m_segments[segment].Expand(t, point);
    }
    CAISplinePathSegment* m_segments;
    unsigned long m_count;
    unsigned long m_id;
};

class CAISplinePathManager {
public:
    CAISplinePath* GetPath(unsigned long id) {
        unsigned long i;
        for (i = 0; i < m_count; i++) {
            if (id == m_paths[i].m_id)
                break;
        }
        return &m_paths[i];
    }
    unsigned long m_count;
    CAISplinePath* m_paths;
};

class CAIFilterGlobal {
public:
    unsigned char unknown00[12];
    CAISplinePathManager* m_splinePaths;
    unsigned char unknown10[8];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

void BIFunc_FireProjectileAtPath(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int crc = *(*stack - (count - 1));
    float accuracy = *(float*)(*stack - (count - 2));
    BSObjectView* from = *(BSObjectView**)(*stack - (count - 3));
    unsigned long pathId = *(*stack - (count - 4));
    float position = *(float*)(*stack - (count - 5));
    int boneId = *(*stack - (count - 6));
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
        start = CVector3(from->trigger->core->position);
    }
    CAISplinePath* path = g_aigAIFilterGlobalObject.m_splinePaths->GetPath(pathId);
    CVector3 end;
    path->GetPoint(position, end);
    end -= start;
    AdjustAim(end, accuracy);
    g_pBulletFactory->CreateBullet(type, projectile, start, end, speed, (EClsnId)4, 1.0f, (ISceneNode*)object);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
