// A fragment of AIObject.cpp (0x8004e9f8): CAIObject::TestGrenadeShot rates
// a grenade throw at the target (+240): too short a throw (below 10 from the
// flat distance plus 3.464 per unit the target is above the hand, 0.5 above
// the position) or a blocked path from the hand to the throw's apex (half
// the throw out along the facing +104, raised by the arc) or from the apex
// down to the target gives -1, else 600 times the throw speed with a random
// spread (+508). The parameter is unused, as in the code. The classes and
// functions are named by the symbols; the members, the throw's constants'
// meanings and the vector views (as in AIObject_visionseek.cpp) are
// inferred. The rest of the file is in other units.
extern "C" {
#include <math.h>
}

float MathFunRandomRealSigned(float, float);

class CAIFilterRealPosition {
public:
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    void RotateAboutZ(float);

    float w;
} __attribute__((aligned(8)));

class CAIFilterObject;

class CAIFilterGlobal {
public:
    static bool CheckCollisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                                CAIFilterObject*);
};

class CAIObject {
public:
    float TestGrenadeShot(float);

    unsigned char unknown000[8];
    CAIFilterObject* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[68];
    float m_facing;
    unsigned char unknown06c[132];
    CAIFilterRealPosition m_target;
    unsigned char unknown0fc[256];
    float m_spread;
};

float CAIObject::TestGrenadeShot(float) {
    CAIFilterRealVector3 hand;
    CAIFilterRealVector3 apex;
    float x;
    float y;
    float z;
    float distance = m_position.GetDistanceXYReal(m_target);
    float rise = m_target.z - (0.5f + m_position.z);
    rise = 3.464f * rise;
    float throwDistance = distance + rise;
    if (throwDistance < 10.0f)
        return -1.0f;
    float reach = 0.95f * throwDistance;
    float speedSquared = reach;
    speedSquared *= 0.0062994817f;
    float speed = sqrtf(speedSquared);
    z = m_position.z;
    y = m_position.y;
    x = m_position.x;
    hand.x = x;
    hand.y = y;
    hand.z = z;
    hand.z += 0.5f;
    float half = 0.5f * speed;
    apex.x = 0.0f;
    apex.y = 0.5f * reach;
    apex.z = 91.65313f * (half * half);
    apex.RotateAboutZ(m_facing);
    apex.x = apex.x + hand.x;
    apex.y = apex.y + hand.y;
    apex.z = apex.z + hand.z;
    if (CAIFilterGlobal::CheckCollisionLineToEnvironment(hand, apex, m_filter))
        return -1.0f;
    z = m_target.z;
    y = m_target.y;
    x = m_target.x;
    hand.x = x;
    hand.y = y;
    hand.z = z;
    if (CAIFilterGlobal::CheckCollisionLineToEnvironment(apex, hand, m_filter))
        return -1.0f;
    return 600.0f * (speed + MathFunRandomRealSigned(0.0f, 0.05f * (speed * m_spread)));
}
