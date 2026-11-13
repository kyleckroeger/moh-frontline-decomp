// A fragment of anim_obj.cpp (0x80093a98): CAnimObject's ApplyForceTo (the
// velocity becomes the force's direction scaled by the strength / 512 and
// the object is marked as forced, off the ground), Halt (clears the
// velocity and turn), OnCollision (for each contact, takes the normal -
// turned around when this object is the collision's first object - flat in
// z, and when it is not too short records it with the other object's moving
// node, if that is an animated object, or for collision ids 5 and 7 also a
// player), and ConstrainVelocity (removes the velocity component into the
// most opposing contact, then the next one; with two such contacts it solves
// the 2x2 system for both normals and backs off by 1.05, and finally zeroes
// any velocity still pushing into a contact or longer than 0.5). Its
// products go through an inferred compound-copy Mul inline (the original's
// colouring shows a return temporary). The file is compiled with deferred inlining (see
// anim_obj_draw.cpp), so the functions are written in reverse image order.
// The classes and functions are named by the symbols; ISceneNode declares
// the used virtual functions at their __vt__11CAnimObject slots
// (placeholders named by offset), the collision, contact and record views,
// the members and flag names are inferred, the record's copy constructor is
// inferred from the copy order, dwi::fast_vec follows anim_obj_ctor.cpp, and
// the CVector3 view copies through a double pair as in player_weapon.cpp with
// an inferred inline Normalize that divides per component (as in
// thrown_obj.cpp). The rest of the file is not part of this unit.
extern "C" {
#include <math.h>
}

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    CVector3& operator-=(const CVector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    void Normalize() {
        float length = sqrtf(x * x + y * y + z * z);
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }

    void Scale(const CVector3& o, float k) { x = k * o.x; y = k * o.y; z = k * o.z; }
    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; }
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CAnimObject;
class CPlayerObject;
/* inferred: only the world velocity slot (+276) is used */
class IMovingSceneNode {
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
    virtual CVector3 GetWorldLinearVelocity() const;
};

class ISceneNode {
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
    virtual int GetCollisionId() const;
    virtual void vf05c();
    virtual void vf060();
    virtual void vf064();
    virtual void vf068();
    virtual IMovingSceneNode* AsMovingNode();
    virtual void vf070();
    virtual void vf074();
    virtual void vf078();
    virtual void vf07c();
    virtual void vf080();
    virtual void vf084();
    virtual void vf088();
    virtual CAnimObject* AsAnimObject();
    virtual void vf090();
    virtual void vf094();
    virtual void vf098();
    virtual CPlayerObject* AsPlayerObject();
};

/* inferred: a contact of a collision: its normal and the next contact at +44 */
struct ContactView {
    CVector3 m_normal;
    unsigned char unknown10[28];
    ContactView* m_next;
};

/* inferred: the collision's two objects and its contact list at +24 */
class CCollision {
public:
    ISceneNode* m_object;
    ISceneNode* m_other;
    unsigned char unknown08[16];
    ContactView* m_contacts;
};

#include "game/DwiContainers.h"

class CAnimObject : public ISceneNode {
public:
    struct CClsnInfo {
        CClsnInfo() {}
        CClsnInfo(const CClsnInfo& o) {
            m_normal = o.m_normal;
            m_object = o.m_object;
        }

        CVector3 m_normal;
        IMovingSceneNode* m_object;
    };

    void ConstrainVelocity();
    void OnCollision(const CCollision&);
    void Halt();
    void ApplyForceTo(CVector3, float);

    unsigned char unknown0004[9156];
    CVector3 m_velocity;
    unsigned char unknown23d8[24];
    float m_turn;
    unsigned char unknown23f4[68];
    dwi::fast_vec<CClsnInfo> m_collisions;
    bool m_flag2450;
    bool m_flag2451;
    unsigned char unknown2452[2];
    bool m_onGround;
    unsigned char unknown2455;
    bool m_forced;
};

// inferred: compound-copy multiply helper (keeps the product in its first operand's register)
inline float Mul(float a, float b) { float r = a; r *= b; return r; }
void CAnimObject::ConstrainVelocity() {
    if (m_collisions.m_size == 0 || m_flag2450)
        return;
    {
        CClsnInfo* it;
        CClsnInfo* begin;
        CClsnInfo* end;
        CClsnInfo* first;
        CClsnInfo* second;
        CVector3 bv;
        float best;
        float best2;
        float b;
        float a;
        float zero;
        m_flag2450 = true;
        best = 1.0f;
        best2 = best;
        zero = 0.0f;
        begin = m_collisions.m_data;
        end = m_collisions.m_data + m_collisions.m_size;
        first = end;
        second = end;
        CVector3 velocity = m_velocity;
        for (it = begin; it != end; ++it) {
            CVector3 normal = it->m_normal;
            CVector3 v = m_velocity;
            if (it->m_object)
                v -= it->m_object->GetWorldLinearVelocity();
            float d = v.x * normal.x + v.y * normal.y + v.z * normal.z;
            if (d < zero && d < best) {
                best = d;
                first = it;
            }
        }
        bool found = first != end;
        if (found) {
            bv.Scale(first->m_normal, best);
            m_velocity.Sub(m_velocity, bv);
            for (it = begin; it != end; ++it) {
                if (it == first)
                    continue;
                CVector3 normal = it->m_normal;
                CVector3 v = m_velocity;
                if (it->m_object)
                    v -= it->m_object->GetWorldLinearVelocity();
                float d = v.x * normal.x + v.y * normal.y + v.z * normal.z;
                if (d < -1e-6f && d < best2) {
                    best2 = d;
                    second = it;
                }
            }
            found = second != end;
        }
        if (found) {
            float det;
            float e;
            float bd;
            CVector3& n2 = second->m_normal;
            float py;
            float px;
            CVector3& n1 = first->m_normal;
            float dc;
            float d2;
            float inv;
            dc = best * (n1.x * n2.x + n1.y * n2.y + n1.z * n2.z);
            d2 = best2 + dc;
            px = d2 * n2.x;
            float pz;
            py = d2 * n2.y;
            pz = d2 * n2.z;
            e = px * n1.x + py * n1.y + pz * n1.z;
            bd = best * d2;
            det = bd - e * dc;
            bool solved;
            if ((float)fabs(det) < 1e-5f) {
                solved = false;
            } else {
                inv = 1.0f / det;
                b = inv * (d2 * best - e * d2);
                a = inv * (bd - dc * best);
                solved = true;
            }
            if (solved) {
                float ty;
                float sx;
                float sy;
                sx = Mul(b, bv.x);
                float dx;
                float dz;
                sy = Mul(b, bv.y);
                float ux;
                float tz;
                float tx;
                tx = Mul(px, a);
                ux = sx + tx;
                dx = Mul(ux, 1.05f);
                float uz;
                ty = Mul(py, a);
                float sz;
                float uy;
                m_velocity.x = velocity.x - dx;
                uy = sy + ty;
                float dy;
                dy = Mul(uy, 1.05f);
                sz = Mul(b, bv.z);
                tz = Mul(pz, a);
                uz = sz + tz;
                dz = Mul(uz, 1.05f);
                m_velocity.y = velocity.y - dy;
                m_velocity.z = velocity.z - dz;
            }
            for (it = begin; found && it != end; ++it) {
                if (it == first || it == second)
                    continue;
                CVector3 normal = it->m_normal;
                if (m_velocity.x * normal.x + m_velocity.y * normal.y + m_velocity.z * normal.z < -1e-6f) {
                    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
                    found = false;
                }
            }
        }
        if (sqrtf(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y + m_velocity.z * m_velocity.z) > 0.5f)
            m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    }
}

void CAnimObject::OnCollision(const CCollision& collision) {
    m_flag2450 = false;
    bool first = collision.m_object == this;
    ISceneNode* other = collision.m_object == this ? collision.m_other : collision.m_object;
    for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
        CClsnInfo info;
        info.m_object = 0;
        if (m_flag2451) {
            int id = GetCollisionId();
            if ((id == 5 || id == 7) && (other->AsAnimObject() || other->AsPlayerObject()))
                info.m_object = other->AsMovingNode();
            else if (other->AsAnimObject())
                info.m_object = other->AsMovingNode();
        }
        if (first) {
            info.m_normal.x = -contact->m_normal.x;
            info.m_normal.y = -contact->m_normal.y;
            info.m_normal.z = -contact->m_normal.z;
        } else {
            info.m_normal = contact->m_normal;
        }
        info.m_normal.z = 0.0f;
        if (info.m_normal.x * info.m_normal.x + info.m_normal.y * info.m_normal.y + info.m_normal.z * info.m_normal.z > 0.001f) {
            info.m_normal.Normalize();
            m_collisions.push_back(info);
        }
    }
}

void CAnimObject::Halt() {
    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    m_turn = 0.0f;
}

void CAnimObject::ApplyForceTo(CVector3 force, float strength) {
    strength *= 0.001953125f;
    m_velocity = force;
    m_velocity.Normalize();
    m_velocity.x *= strength;
    m_velocity.y *= strength;
    m_velocity.z *= strength;
    m_forced = true;
    m_onGround = false;
}
