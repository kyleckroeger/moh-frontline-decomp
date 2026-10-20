// CAIObject::SetNonAITarget (drops the AI target as SetTarget does, then takes
// a script object's scene position as the target position, the point 1 below
// it as the aim point), SetTarget, the distance helpers,
// GetCoordinateAxisDirection and both SetCoordinateAxisDirection overloads (store the three
// axes, from vectors or nine floats, the angle of the first in the XY plane at +104, 0 at +100 (a .sdata2
// pool entry), and the angle of the second axis, rotated back by the first
// angle, at +96; the angle members are named by offset).
// The distance helpers (squared XY, squared XYZ to another object or to the
// target position, XYZ between AI objects' real positions) forward to
// CAIFilterRealPosition. The class names come from the mangled symbols; the
// position (+24), axis directions (+112), target (+196) and target position
// (+240) members, the position copy (z first) and the script object's
// scene-node view are inferred from offsets, the other members SetTarget uses
// are named by offset, and CAIObject is a non-virtual view. The rest of the
// file is not part of this unit.
class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    CAIFilterRealPosition& operator=(const CAIFilterRealPosition& other) {
        float x_, y_, z_;
        z_ = other.z;
        y_ = other.y;
        x_ = other.x;
        x = x_;
        y = y_;
        z = z_;
        return *this;
    }

    float x;
    float y;
    float z;
};

// The axis members' 16-byte stride and doubleword copies show an 8-byte
// aligned vector inside CAIFilterRealVector3 (inferred).
class CVector3 {
public:
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CAIFilterRealVector3 {
public:
    void RotateAboutZ(float);

    CVector3 v;
};

float MathFunAtan2F(float, float);

// Inferred: the scene object a script object refers to, with its position.
struct BSPositionView {
    unsigned char unknown00[16];
    CAIFilterRealPosition position;
};

struct BSUserView {
    unsigned char unknown00[4];
    BSPositionView* node;
};

class BSObject {
public:
    unsigned char unknown00[8];
    BSUserView* user;
};

class CAIObject {
public:
    void SetNonAITarget(BSObject*);
    void GetCoordinateAxisDirection(CAIFilterRealVector3*, CAIFilterRealVector3*, CAIFilterRealVector3*);
    void SetCoordinateAxisDirection(CAIFilterRealVector3&, CAIFilterRealVector3&, CAIFilterRealVector3&);
    void SetCoordinateAxisDirection(float, float, float, float, float, float, float, float, float);
    CAIObject* SetTarget(CAIObject*);
    void CalculateTargetPosition();
    float GetDistanceSquaredXYReal(CAIObject*);
    float GetDistanceSquaredXYZReal();
    float GetDistanceSquaredXYZReal(CAIObject*);
    float GetDistanceXYZReal(CAIObject*);

    unsigned char unknown000[24];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[60];
    float m_angle060;
    float m_value064;
    float m_angle068;
    unsigned char unknown06c[4];
    CAIFilterRealVector3 m_axis[3];
    unsigned char unknown0a0[32];
    bool m_unknown0c0;
    unsigned char unknown0c1[3];
    CAIObject* m_target;
    CAIFilterRealPosition m_nonAITarget;
    unsigned char unknown0d4[28];
    CAIFilterRealPosition m_targetPosition;
    unsigned char unknown0fc[84];
    CAIFilterRealPosition m_unknown150;
    unsigned char unknown15c[120];
    int m_unknown1d4;
    unsigned char unknown1d8[20];
    int m_unknown1ec;
    unsigned char unknown1f0[40];
    int m_unknown218;
    unsigned char unknown21c[188];
    int m_unknown2d8;
    unsigned char unknown2dc[1020];
    bool m_unknown6d8;
    unsigned char unknown6d9[88];
    bool m_unknown731;
};

// Inferred: the count at +492 and its value capped at 15 at +468, kept on
// the targeted object.
static inline void AdjustTargetedCount(CAIObject* object, int delta) {
    object->m_unknown1ec += delta;
    if (object->m_unknown1ec <= 15)
        object->m_unknown1d4 = object->m_unknown1ec;
    else
        object->m_unknown1d4 = 15;
}

void CAIObject::SetNonAITarget(BSObject* object) {
    m_unknown0c0 = false;
    if (m_target) {
        m_unknown6d8 = false;
        CAIObject* old = m_target;
        m_target = 0;
        if (m_unknown218 == 8 || m_unknown218 == 2) {
            if (m_unknown2d8 == 1 || m_unknown2d8 == 2)
                m_unknown2d8 = 3;
        }
        if (old)
            AdjustTargetedCount(old, -1);
    }
    m_unknown0c0 = true;
    if (object && object->user && object->user->node) {
        m_nonAITarget = object->user->node->position;
        m_targetPosition = m_nonAITarget;
        m_unknown150 = m_nonAITarget;
        m_unknown150.z -= 1.0f;
    }
}

CAIObject* CAIObject::SetTarget(CAIObject* target) {
    m_unknown0c0 = false;
    if (target == m_target)
        return m_target;
    m_unknown6d8 = false;
    CAIObject* old = m_target;
    m_target = target;
    if (target) {
        float z = target->m_position.z;
        float y = target->m_position.y;
        float x = target->m_position.x;
        m_unknown150.x = x;
        m_unknown150.y = y;
        m_unknown150.z = z;
        m_unknown6d8 = true;
        m_unknown731 = true;
        CalculateTargetPosition();
    } else if (m_unknown218 == 8 || m_unknown218 == 2) {
        if (m_unknown2d8 == 1 || m_unknown2d8 == 2)
            m_unknown2d8 = 3;
    }
    if (target)
        AdjustTargetedCount(target, 1);
    if (old)
        AdjustTargetedCount(old, -1);
    return old;
}

float CAIObject::GetDistanceSquaredXYReal(CAIObject* other) {
    return m_position.GetDistanceSquaredXYReal(other->m_position);
}

float CAIObject::GetDistanceSquaredXYZReal() {
    return m_position.GetDistanceSquaredXYZReal(m_targetPosition);
}

float CAIObject::GetDistanceSquaredXYZReal(CAIObject* other) {
    return m_position.GetDistanceSquaredXYZReal(other->m_position);
}

float CAIObject::GetDistanceXYZReal(CAIObject* other) {
    return m_position.GetDistanceXYZReal(other->m_position);
}

void CAIObject::GetCoordinateAxisDirection(CAIFilterRealVector3* forward, CAIFilterRealVector3* left,
                                           CAIFilterRealVector3* up) {
    *forward = m_axis[0];
    *left = m_axis[1];
    *up = m_axis[2];
}

void CAIObject::SetCoordinateAxisDirection(CAIFilterRealVector3& forward, CAIFilterRealVector3& left,
                                           CAIFilterRealVector3& up) {
    m_axis[0] = forward;
    m_axis[1] = left;
    m_axis[2] = up;
    m_angle068 = MathFunAtan2F(-m_axis[0].v.y, m_axis[0].v.x);
    CAIFilterRealVector3 rotated = m_axis[1];
    rotated.RotateAboutZ(-m_angle068);
    m_value064 = 0.0f;
    m_angle060 = MathFunAtan2F(-rotated.v.z, rotated.v.y);
}

void CAIObject::SetCoordinateAxisDirection(float forwardX, float forwardY, float forwardZ, float leftX, float leftY,
                                           float leftZ, float upX, float upY, float upZ) {
    m_axis[0].v.x = forwardX;
    m_axis[0].v.y = forwardY;
    m_axis[0].v.z = forwardZ;
    m_axis[1].v.x = leftX;
    m_axis[1].v.y = leftY;
    m_axis[1].v.z = leftZ;
    m_axis[2].v.x = upX;
    m_axis[2].v.y = upY;
    m_axis[2].v.z = upZ;
    m_angle068 = MathFunAtan2F(-m_axis[0].v.y, m_axis[0].v.x);
    CAIFilterRealVector3 rotated = m_axis[1];
    rotated.RotateAboutZ(-m_angle068);
    m_value064 = 0.0f;
    m_angle060 = MathFunAtan2F(-rotated.v.z, rotated.v.y);
}
