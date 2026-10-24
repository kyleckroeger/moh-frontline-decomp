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
// (found by the permuter) are inferred. The CVector3 view is a union of two
// doubles and a named float struct (CW cannot look up members of an
// anonymous struct inside the class's own methods). The rest of the file is
// in other units.
extern bool g_bInMultiplayerMode;
extern const float CONTACT_EPSILON;

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
        return y ;
    }
    return x;
}


class CVector3 {
public:
    union {
        double pair[2];
        struct {
            float x;
            float y;
            float z;
            float w;
        } c;
    };

    float LengthSq() const { return this->c.x * this->c.x + this->c.y * this->c.y + this->c.z * this->c.z; }
    float Normalize() {
        float length = sqrtf(this->c.x * this->c.x + this->c.y * this->c.y + this->c.z * this->c.z);
        if (length != 0.0f) {
            float inv = 1.0f / length;
            this->c.x *= inv;
            this->c.y *= inv;
            this->c.z *= inv;
        }
        return length;
    }
} __attribute__((aligned(8)));

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
        m_velocity.c.x = m_velocity.c.y = m_velocity.c.z = 0.0f;
        m_localVelocity.c.x = m_localVelocity.c.y = m_localVelocity.c.z = 0.0f;
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
        float d = velocity.c.x * normal.c.x + velocity.c.y * normal.c.y + velocity.c.z * normal.c.z;
        if (d < best) {
            best = d;
            first = it;
        }
        bool onFloor = false;
        if (it->m_normal.c.z <= 0.642f && it->m_normal.c.z > 0.01f)
            onFloor = true;
        m_onGround &= onFloor;
        if (m_heightSpeed > zero && it->m_normal.c.z < zero) {
            if (m_flag395_40 && m_flag397_08) {
                float depth = it->m_depth;
                if (depth > zero)
                    depth = zero;
                float scale = (CONTACT_EPSILON + depth) / CONTACT_EPSILON;
                CVector3 dir = velocity;
                dir.Normalize();
                float limit = -0.28f * scale * (0.5f * (dir.c.x * it->m_normal.c.x + dir.c.y * it->m_normal.c.y + dir.c.z * it->m_normal.c.z) + 0.5f);
                if (limit > -0.0f)
                    limit = -0.0f;
                else if (limit < -0.28f)
                    limit = -0.28f;
                if (it->m_normal.c.z < limit && m_flag397_08) {
                    m_flag395_80 = 1;
                    m_flag395_40 = 1;
                    m_heightSpeed = -(g_bInMultiplayerMode ? 0.02666667f : 0.07333334f);
                }
            } else if (it->m_normal.c.z < -0.0f) {
                m_heightSpeed = 0.0f;
            }
        }
    }
    bool found = first != end;
    if (found) {
        float k = 1.00001f * best;
        float bx = k * first->m_normal.c.x;
        float by = k * first->m_normal.c.y;
        float bz = k * first->m_normal.c.z;
        m_velocity.c.x = velocity.c.x - bx;
        m_velocity.c.y = velocity.c.y - by;
        m_velocity.c.z = velocity.c.z - bz;
        length = sqrtf(m_velocity.c.x * m_velocity.c.x + m_velocity.c.y * m_velocity.c.y + m_velocity.c.z * m_velocity.c.z);
        changed = true;
        for (it = begin; it != end; ++it) {
            if (it == first)
                continue;
            CVector3 normal = it->m_normal;
            float d = m_velocity.c.x * normal.c.x + m_velocity.c.y * normal.c.y + m_velocity.c.z * normal.c.z;
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
        axis.c.x = n1.c.y * n2.c.z - n1.c.z * n2.c.y;
        axis.c.y = n1.c.z * n2.c.x - n1.c.x * n2.c.z;
        axis.c.z = n1.c.x * n2.c.y - n1.c.y * n2.c.x;
        float len = axis.Normalize();
        found = len > 0.0034906515f;
        if (found) {
            float d = axis.c.x * velocity.c.x + axis.c.y * velocity.c.y + axis.c.z * velocity.c.z;
            m_velocity.c.x = d * axis.c.x;
            m_velocity.c.y = d * axis.c.y;
            m_velocity.c.z = d * axis.c.z;
            length = sqrtf(m_velocity.c.x * m_velocity.c.x + m_velocity.c.y * m_velocity.c.y + m_velocity.c.z * m_velocity.c.z);
            changed = true;
        }
    }
    if (found) {
        for (; found && begin != end; ++begin) {
            if (begin == first || begin == second)
                continue;
            CVector3 normal = begin->m_normal;
            if (m_velocity.c.x * normal.c.x + m_velocity.c.y * normal.c.y + m_velocity.c.z * normal.c.z < -0.0034906515f * length) {
                changed = true;
                found = false;
                m_velocity.c.x = m_velocity.c.y = m_velocity.c.z = 0.0f;
            }
        }
    }
    if (changed) {
        CMatrix inverse;
        inverse.FastInverse(m_tm);
        inverse.TransformVector(m_localVelocity, m_velocity);
    }
}
