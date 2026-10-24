// A fragment of AIFilter.cpp; the file name is this project's. AI filter objects, the functions at the start of the file: the MMG
// queries forwarded to the AI object, the target guess position, the
// Euler-direction angle difference, Z rotation and the real-position
// distances (sqrtf is the SDK's inline; the XY distance squares a zero z). The
// class names come from the mangled symbols; the members are inferred from
// offsets, and the conversion from the game's CVector3 (a by-value copy, then
// the three coordinates) is written as an inferred inline constructor.
// CAIFilterPlayerObject::GetLookDirection comes first in the file: a weak copy
// of a header inline (defined __declspec(weak)) returning the owner's scene
// node forward vector and position through the inferred inline GetLookVectors
// (its locals and temporaries give the stack slots). UpdateFromGameObject on
// are not part of this unit.
extern "C" {
#include <math.h>
}

struct TriggerObject_struct;

class CVector3 {
public:
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CAIFilterRealVector3 {
public:
    CAIFilterRealVector3() {}
    CAIFilterRealVector3(CVector3 v) : x(v.x), y(v.y), z(v.z) {}
    void RotateAboutZ(float);

    float x;
    float y;
    float z;
};

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition() {}
    CAIFilterRealPosition(CVector3 v) : x(v.x), y(v.y), z(v.z) {}
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealEulerDirection {
public:
    void CalculateAngleDiff(const CAIFilterRealPosition&, const CAIFilterRealPosition&, float*, float*) const;

    float pitch;
    float roll;
    float yaw;
};

float MathFunAtan2F(float, float);
void MathFunRotateAboutZ(float*, float*, float);
float MathFunNormalizeAngleNegativePiToPi(float);

class CMatrix;
class CDrawContext;
class CCollision;

// The scene-node interface, flattened in the order of __vt__10ISceneNode;
// only GetPosition and GetForward are called here.
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
    virtual const void* GetLocalBoundingVolume(EVolumeType) const;
    virtual const void* GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
};

struct AIFilterGameObjectView {
    unsigned char unknown00[80];
    ISceneNode* sceneNode;
};

class CAIObject {
public:
    int ShouldILeaveMMG(TriggerObject_struct*);
    CAIObject* ChooseMMGPoint();

    unsigned char unknown000[8];
    void* m_gameObject;
    unsigned char unknown00c[228];
    CVector3 m_targetGuess;
};

struct BSObject {
    unsigned char unknown00[8];
    TriggerObject_struct* triggerObject;
};

class CAIFilterObject {
public:
    int ShouldILeaveMMG(BSObject*);
    void* ChooseMMGPoint();
    void GetTargetGuessPosition(CAIFilterRealPosition&);

    unsigned char unknown00[4];
    AIFilterGameObjectView* m_gameObject;
    CAIObject* m_aiObject;
};

class CAIFilterPlayerObject : public CAIFilterObject {
public:
    void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);
};

/* Inferred: the scene node's forward vector and position as real vectors
   (each through a by-value temporary). */
inline void GetLookVectors(AIFilterGameObjectView* object, CAIFilterRealVector3& direction, CAIFilterRealVector3& position) {
    CVector3 forward;
    CVector3 location;

    object->sceneNode->GetForward(forward);
    object->sceneNode->GetPosition(location);
    direction = CAIFilterRealVector3(forward);
    position = CAIFilterRealVector3(location);
}

__declspec(weak) void CAIFilterPlayerObject::GetLookDirection(CAIFilterRealVector3& direction, CAIFilterRealVector3& position) {
    GetLookVectors(m_gameObject, direction, position);
}

int CAIFilterObject::ShouldILeaveMMG(BSObject* script) {
    return m_aiObject->ShouldILeaveMMG(script->triggerObject);
}

void* CAIFilterObject::ChooseMMGPoint() {
    CAIObject* point = m_aiObject->ChooseMMGPoint();
    return point ? point->m_gameObject : 0;
}

void CAIFilterObject::GetTargetGuessPosition(CAIFilterRealPosition& position) {
    position = CAIFilterRealPosition(m_aiObject->m_targetGuess);
}

void CAIFilterRealEulerDirection::CalculateAngleDiff(const CAIFilterRealPosition& from, const CAIFilterRealPosition& to,
                                                     float* yawDiff, float* pitchDiff) const {
    float delta[3];
    delta[0] = to.x - from.x;
    delta[1] = to.y - from.y;
    delta[2] = to.z - from.z;
    float yawTo = MathFunAtan2F(delta[0], delta[1]);
    MathFunRotateAboutZ(&delta[0], &delta[1], -yawTo);
    float pitchTo = MathFunAtan2F(-delta[2], delta[1]);
    *yawDiff = MathFunNormalizeAngleNegativePiToPi(yawTo - yaw);
    *pitchDiff = MathFunNormalizeAngleNegativePiToPi(pitchTo - pitch);
}

void CAIFilterRealVector3::RotateAboutZ(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    float oy = y;
    float ox = x;
    x = ox * c - oy * s;
    y = ox * s + oy * c;
}

float CAIFilterRealPosition::GetDistanceSquaredXYReal(const CAIFilterRealPosition& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    return dx * dx + dy * dy;
}

float CAIFilterRealPosition::GetDistanceSquaredXYZReal(const CAIFilterRealPosition& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    return dx * dx + dy * dy + dz * dz;
}

float CAIFilterRealPosition::GetDistanceXYReal(const CAIFilterRealPosition& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = 0.0f;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

float CAIFilterRealPosition::GetDistanceXYZReal(const CAIFilterRealPosition& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}
