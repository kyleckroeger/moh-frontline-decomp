// A fragment of player.cpp (0x800a055c): CPlayerObject::ConstrainVelocity
// adjusts the player's velocity against this frame's collision contacts.
// With no contacts nothing happens; a (nearly) still player with no upward
// height speed has both velocities zeroed. Otherwise (unless the flag at bit
// 0x80 of +918 is set) every contact is visited: its object is notified
// (virtual slot +32), the most opposing contact normal is kept, the on-ground
// bit (0x20 of +916) survives only while every normal is floor-like
// (0.01 < z <= 0.642), and a ceiling contact while rising either stops the
// rise or, while crouch-jumping (bits 0x40 of +917 and 0x08 of +919), starts
// a fall (speed 0.0733, 0.0267 in multiplayer) when the normal is steeper
// than a limit from the contact depth and the travel direction. The
// velocity then loses its component along the most opposing normal (times
// 1.00001); a second opposing contact (beyond -0.0035 of the speed) limits
// it to the crease direction (the normalised cross product, when not
// degenerate), and a third stops the player. A changed velocity is turned
// into the local velocity (+616) through the inverse transform. The class,
// functions and globals are named by the symbols; the member names and
// views, the contact record (normal, depth, object; 24 bytes as in
// player_ctor.cpp's CClsnInfo), the bit names and the begin/data copy
// (found by the permuter) are inferred. CVector3 is the shared
// game/CVector3.h. The rest of the file is
// in other units.
extern bool g_bInMultiplayerMode;
extern const float CONTACT_EPSILON;



#include "game/CVector3.h"


class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;
    void FastInverse(const CMatrix&);
    void TransformVector(CVector3&, CVector3) const;

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

class ContactObjectView {
public:
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
};

namespace dwi {
template <class T>
class fast_vec {
public:
    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

class CPlayerObject {
public:
    struct CClsnInfo {
        CVector3 m_normal;
        float m_depth;
        ContactObjectView* m_object;
    };

    void ConstrainVelocity();

    unsigned char unknown000[128];
    CMatrix m_tm;
    unsigned char unknown0c0[408];
    CVector3 m_velocity;
    CVector3 m_localVelocity;
    unsigned char unknown278[44];
    float m_heightSpeed;
    dwi::fast_vec<CClsnInfo> m_collisions;
    unsigned char unknown2c0[212];
    unsigned char m_flag394_80 : 1;
    unsigned char m_flag394_40 : 1;
    bool m_onGround : 1;
    unsigned char m_flag394_10 : 1;
    unsigned char m_flag394_08 : 1;
    unsigned char m_flag394_04 : 1;
    unsigned char m_flag394_02 : 1;
    unsigned char m_flag394_01 : 1;
    unsigned char m_flag395_80 : 1;
    unsigned char m_flag395_40 : 1;
    unsigned char m_flag395_20 : 1;
    unsigned char m_flag395_10 : 1;
    unsigned char m_flag395_08 : 1;
    unsigned char m_flag395_04 : 1;
    unsigned char m_flag395_02 : 1;
    unsigned char m_flag395_01 : 1;
    unsigned char m_flag396_80 : 1;
    unsigned char m_flag396_40 : 1;
    unsigned char m_flag396_20 : 1;
    unsigned char m_flag396_10 : 1;
    unsigned char m_flag396_08 : 1;
    unsigned char m_flag396_04 : 1;
    unsigned char m_flag396_02 : 1;
    unsigned char m_flag396_01 : 1;
    unsigned char m_flag397_80 : 1;
    unsigned char m_flag397_40 : 1;
    unsigned char m_flag397_20 : 1;
    unsigned char m_flag397_10 : 1;
    unsigned char m_flag397_08 : 1;
    unsigned char m_flag397_04 : 1;
    unsigned char m_flag397_02 : 1;
    unsigned char m_flag397_01 : 1;
};

void CPlayerObject::ConstrainVelocity() {
    if (m_collisions.m_size == 0)
        return;
    if ((float)__fabs(m_velocity.LengthSq()) < 1e-06f && m_heightSpeed <= 0.0f) {
        m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
        m_localVelocity.x = m_localVelocity.y = m_localVelocity.z = 0.0f;
        return;
    }
    if (m_flag396_80)
        return;

    CClsnInfo* end;
    CClsnInfo* begin;
    CClsnInfo* data;
    CClsnInfo* it;
    CClsnInfo* first;
    CClsnInfo* second;
    bool changed = false;
    data = m_collisions.m_data;
    begin = data;
    end = begin + m_collisions.m_size;
    first = end;
    second = end;
    float best = 0.0f;
    float best2 = best;
    float zero = best;
    float length;
    CVector3 velocity = m_velocity;
    m_onGround = 1;
    for (it = begin; it != end; ++it) {
        if (it->m_object)
            it->m_object->vf20();
        CVector3 normal = it->m_normal;
        float d = velocity.x * normal.x + velocity.y * normal.y + velocity.z * normal.z;
        if (d < best) {
            best = d;
            first = it;
        }
        bool onFloor = false;
        if (it->m_normal.z <= 0.642f && it->m_normal.z > 0.01f)
            onFloor = true;
        m_onGround &= onFloor;
        if (m_heightSpeed > zero && it->m_normal.z < zero) {
            if (m_flag395_40 && m_flag397_08) {
                float depth = it->m_depth;
                if (depth > zero)
                    depth = zero;
                float scale = (CONTACT_EPSILON + depth) / CONTACT_EPSILON;
                CVector3 dir = velocity;
                dir.Normalize();
                float limit = -0.28f * scale * (0.5f * (dir.x * it->m_normal.x + dir.y * it->m_normal.y + dir.z * it->m_normal.z) + 0.5f);
                if (limit > -0.0f)
                    limit = -0.0f;
                else if (limit < -0.28f)
                    limit = -0.28f;
                if (it->m_normal.z < limit && m_flag397_08) {
                    m_flag395_80 = 1;
                    m_flag395_40 = 1;
                    m_heightSpeed = -(g_bInMultiplayerMode ? 0.02666667f : 0.07333334f);
                }
            } else if (it->m_normal.z < -0.0f) {
                m_heightSpeed = 0.0f;
            }
        }
    }
    bool found = first != end;
    if (found) {
        float k = 1.00001f * best;
        float bx = k * first->m_normal.x;
        float by = k * first->m_normal.y;
        float bz = k * first->m_normal.z;
        m_velocity.x = velocity.x - bx;
        m_velocity.y = velocity.y - by;
        m_velocity.z = velocity.z - bz;
        length = sqrtf(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y + m_velocity.z * m_velocity.z);
        changed = true;
        for (it = begin; it != end; ++it) {
            if (it == first)
                continue;
            CVector3 normal = it->m_normal;
            float d = m_velocity.x * normal.x + m_velocity.y * normal.y + m_velocity.z * normal.z;
            if (d < -0.0034906515f * length && d < best2) {
                best2 = d;
                second = it;
            }
        }
        found = second != end;
    }
    if (found) {
        CVector3& n1 = first->m_normal;
        CVector3& n2 = second->m_normal;
        CVector3 axis;
        axis.x = n1.y * n2.z - n1.z * n2.y;
        axis.y = n1.z * n2.x - n1.x * n2.z;
        axis.z = n1.x * n2.y - n1.y * n2.x;
        float len = axis.Normalize();
        found = len > 0.0034906515f;
        if (found) {
            float d = axis.x * velocity.x + axis.y * velocity.y + axis.z * velocity.z;
            m_velocity.x = d * axis.x;
            m_velocity.y = d * axis.y;
            m_velocity.z = d * axis.z;
            length = sqrtf(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y + m_velocity.z * m_velocity.z);
            changed = true;
        }
    }
    if (found) {
        for (; found && begin != end; ++begin) {
            if (begin == first || begin == second)
                continue;
            CVector3 normal = begin->m_normal;
            if (m_velocity.x * normal.x + m_velocity.y * normal.y + m_velocity.z * normal.z < -0.0034906515f * length) {
                changed = true;
                found = false;
                m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
            }
        }
    }
    if (changed) {
        CMatrix inverse;
        inverse.FastInverse(m_tm);
        inverse.TransformVector(m_localVelocity, m_velocity);
    }
}
