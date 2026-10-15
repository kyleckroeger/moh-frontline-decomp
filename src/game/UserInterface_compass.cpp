// A fragment of UserInterface.cpp (0x800bcbcc): UserInterface::
// UpdateCompassRotation damps the compass's angular velocity (+2040; by 0.95
// per whole time step), takes the player's normalised forward direction, the
// angle between it and the reference direction (+8; half a turn further when
// the flag at +2068 is set, mirrored by the sign of a cross-product term), the
// shortest signed difference to the shown angle (+2044), and integrates the
// velocity towards it, keeping the shown angle within one half turn. The
// classes and functions are named by the symbols; the members, the
// direction view and the player's GetForward slot (its table order) are
// inferred.
extern "C" {
#include <math.h>
double acos(double);
}

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CPlayerObject {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
};

class UserInterface {
public:
    void UpdateCompassRotation(CPlayerObject*, float);

    unsigned char unknown000[8];
    float m_referenceX;
    float m_referenceY;
    float m_referenceZ;
    unsigned char unknown014[2020];
    float m_compassVelocity;
    float m_compassAngle;
    unsigned char unknown800[20];
    bool m_compassFlipped;
};

void UserInterface::UpdateCompassRotation(CPlayerObject* player, float time) {
    for (int i = 0; i < time; i++)
        m_compassVelocity *= 0.95f;
    CVector3 forward;
    player->GetForward(forward);
    float length = sqrtf(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
    if (length != 0.0f) {
        float scale = 1.0f / length;
        forward.x *= scale;
        forward.y *= scale;
        forward.z *= scale;
    }
    float cosine = acos(m_referenceX * forward.x + m_referenceY * forward.y + m_referenceZ * forward.z);
    float angle = cosine;
    if (m_compassFlipped)
        angle += 3.1415927f;
    if (forward.y * m_referenceZ - forward.z * m_referenceY > forward.x)
        angle *= -1.0f;
    float difference = 0.0f;
    if (angle > m_compassAngle) {
        difference = angle - m_compassAngle;
        if (difference > 3.1415927f)
            difference -= 6.2831855f;
    } else if (angle < m_compassAngle) {
        difference = angle - m_compassAngle;
        if (difference < -3.1415927f)
            difference = 6.2831855f + difference;
    }
    if (difference < 0.0f) {
        m_compassVelocity += difference;
        m_compassAngle += 0.016683352f * (m_compassVelocity * time);
    } else if (difference > 0.0f) {
        m_compassVelocity += difference;
        m_compassAngle += 0.016683352f * (m_compassVelocity * time);
    }
    float shown = m_compassAngle;
    if (shown > 3.1415927f)
        shown = -6.2831855f + shown;
    else if (shown < -3.1415927f)
        shown = 6.2831855f + shown;
    m_compassAngle = shown;
}
