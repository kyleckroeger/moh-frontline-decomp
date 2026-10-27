// A fragment of bsbifunc.cpp (0x80027e94): the script built-in
// GetWayPointInfo. Its arguments name a waypoint (kind 0: a trigger table
// index, 1: a script object's trigger) and the information wanted: 0 a
// quadrant code (3, 2, 0 or 1) from the normalized direction's dot products
// with the script object's node's rightward and forward vectors, 1 the
// distance to it, 2 the angle of its flat offset in the node's frame, 3 the
// calling AI's angle difference to its target, 4 false, 5 the angle of the
// waypoint's flat facing in the node's frame, 6 2 or 1 by its flag 0x04, 7
// whether its mounted gun is used, 8 its value, 9 the angle between the
// waypoint's reversed facing and the node, clamped to +-pi/2 and printed, 10
// a kind (4, 1, 2 or 0) from its flag bits. Floats are returned as their
// bit patterns below the popped arguments.
// The file name is this project's; the original record is bsbifunc.cpp and
// the built-ins around this one are in other units. The classes and
// functions are named by the symbols; the members, the trigger and AI views,
// the angle-difference inline and the two function-level floats shared by
// cases 2 and 9 are inferred (the shared floats reproduce the original's
// callee-saved float registers). The constants and printf strings are .sdata2
// and .rodata pool entries; the switch's jump table is this unit's.
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
    float Dot(const CVector3& o) const { return x * o.x + y * o.y + z * o.z; }
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


class CMatrix;

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
    CVector3 GetFront() const { return front; }
    static bool s_ClassInit;
} __attribute__((aligned(16)));

enum EClsnId {};
class CCollision;
class CDrawContext;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
struct BSObjectView;

class CAIFilterRealPosition;
class CAIFilterRealEulerDirection {
public:
    void CalculateAngleDiff(const CAIFilterRealPosition&, const CAIFilterRealPosition&, float*, float*) const;
};

struct AIFilterView {
    unsigned char unknown000[24];
    unsigned char position[72];
    unsigned char direction[568];
    unsigned char target[4];
};

struct AIObjectView {
    unsigned char unknown00[8];
    AIFilterView* filter;
};

class CAIDoodad {
public:
    unsigned char unknown00[4];
    AIObjectView* ai;
};

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
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    CQuaternion rotation;
    unsigned char unknown2c[14];
    short value;
    unsigned char unknown3c[52];
    unsigned char flag80 : 1;
    unsigned char flag40 : 1;
    unsigned char flag20 : 1;
    unsigned char flag10 : 1;
    unsigned char flag08 : 1;
    unsigned char flag04 : 1;
    unsigned char flag02 : 1;
    unsigned char flag01 : 1;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
    unsigned char unknown08[4];
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

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;
extern BSObjectView* g_pBSObject;
extern TriggerObject_struct g_pTriggerObjects[];

bool IsMGUsed(TriggerObject_struct*);
float MathFunAtan2F(float, float);
float MathFunNormalizeAngleNegativePiToPi(float);
extern "C" int printf(const char*, ...);

static inline float GetAngleDiff(AIFilterView* filter) {
    float diff;
    float unused;
    ((CAIFilterRealEulerDirection*)filter->direction)->CalculateAngleDiff(*(CAIFilterRealPosition*)filter->position, *(CAIFilterRealPosition*)filter->target, &diff, &unused);
    return diff;
}

void BIFunc_GetWayPointInfo(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int arg = *(*stack - (count - 1));
    int type = *(*stack - (count - 2));
    int info = *(*stack - (count - 3));
    TriggerCoreView* waypoint = 0;
    int result = 0;
    float v;
    float u;
    switch (type) {
    case 0:
        waypoint = g_pTriggerObjects[arg].core;
        break;
    case 1:
        waypoint = ((BSObjectView*)arg)->trigger->core;
        break;
    }
    switch (info) {
    case 0: {
        CVector3 front;
        CVector3 right;
        CVector3 position;
        g_pBSObject->user->GetSceneNode()->GetForward(front);
        g_pBSObject->user->GetSceneNode()->GetRightward(right);
        g_pBSObject->user->GetSceneNode()->GetPosition(position);
        CVector3 dir(waypoint->x, waypoint->y, waypoint->z);
        dir -= position;
        dir.Normalize();
        float ahead = front.Dot(dir);
        float side = right.Dot(dir);
        if (side < 0.7071f)
            result = 3;
        else if (side > -0.7071f)
            result = 2;
        else if (ahead >= 0.0f)
            result = 0;
        else
            result = 1;
        break;
    }
    case 1: {
        float x = waypoint->x;
        float y = waypoint->y;
        float z = waypoint->z;
        CVector3 position;
        g_pBSObject->user->GetSceneNode()->GetPosition(position);
        x -= position.x;
        y -= position.y;
        z -= position.z;
        float distance = sqrtf(x * x + y * y + z * z);
        result = *(int*)&distance;
        break;
    }
    case 2: {
        CVector3 front;
        CVector3 right;
        CVector3 position;
        g_pBSObject->user->GetSceneNode()->GetPosition(position);
        float x;
        float z = 0.0f;
        x = waypoint->x;
        u = waypoint->y;
        x -= position.x;
        u -= position.y;
        g_pBSObject->user->GetSceneNode()->GetForward(front);
        g_pBSObject->user->GetSceneNode()->GetRightward(right);
        float angle = MathFunAtan2F(x * right.x + u * right.y + z * right.z,
                                    x * front.x + u * front.y + z * front.z);
        result = *(int*)&angle;
        break;
    }
    case 3: {
        float angle = GetAngleDiff(((ISceneNode*)object)->GetAIDoodad()->ai->filter);
        result = *(int*)&angle;
        break;
    }
    case 4: {
        ((ISceneNode*)object)->GetAIDoodad();
        bool flag = false;
        result = *(int*)&flag;
        break;
    }
    case 5: {
        CMatrix matrix;
        waypoint->rotation.GetMatrix(matrix);
        CVector3 direction = matrix.GetFront();
        CVector3 front;
        CVector3 right;
        g_pBSObject->user->GetSceneNode()->GetForward(front);
        g_pBSObject->user->GetSceneNode()->GetRightward(right);
        direction.z = 0.0f;
        float x = direction.Dot(front);
        float y = direction.Dot(right);
        float angle = MathFunAtan2F(y, x);
        result = *(int*)&angle;
        break;
    }
    case 6:
        if (waypoint->flag04)
            result = 2;
        else
            result = 1;
        break;
    case 7:
        result = IsMGUsed(((BSObjectView*)arg)->trigger);
        break;
    case 8:
        if (waypoint)
            result = waypoint->value;
        else
            result = 0;
        break;
    case 9: {
        CMatrix matrix;
        waypoint->rotation.GetMatrix(matrix);
        CVector3 inverse;
        CVector3 position;
        v = waypoint->y;
        u = waypoint->x;
        g_pBSObject->user->GetSceneNode()->GetPosition(position);
        position.z = 0.0f;
        position -= CVector3(u, v, 0.0f);
        inverse = matrix.GetFront();
        inverse.x = -inverse.x;
        inverse.y = -inverse.y;
        inverse.z = 0.0f;
        float coverAngle = MathFunAtan2F(inverse.x, inverse.y);
        float targetAngle = MathFunAtan2F(position.x, position.y);
        float diff = MathFunNormalizeAngleNegativePiToPi(targetAngle - coverAngle);
        if (diff > 1.5707964f)
            diff = 1.5707964f;
        if (diff < -1.5707964f)
            diff = -1.5707964f;
        printf("******WAYPOINTINFO_TARGET_ANGLE_TO_COVER *****\n");
        printf("CoverInvAngle: %f, CoverToTargAngle: %f, AngleDiff: %f\n", coverAngle, targetAngle, diff);
        result = *(int*)&diff;
        break;
    }
    case 10:
        if (waypoint->flag04 || waypoint->flag08)
            result = 4;
        else if (waypoint->flag01 || waypoint->flag10 || waypoint->flag40)
            result = 1;
        else if (waypoint->flag02 || waypoint->flag20 || waypoint->flag80)
            result = 2;
        else
            result = 0;
        break;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&result;
}
