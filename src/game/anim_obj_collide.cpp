// A fragment of anim_obj.cpp (0x80093a98): CAnimObject's ApplyForceTo (the
// velocity becomes the force's direction scaled by the strength / 512 and
// the object is marked as forced, off the ground), Halt (clears the
// velocity and turn) and OnCollision (for each contact, takes the normal -
// turned around when this object is the collision's first object - flat in
// z, and when it is not too short records it with the other object's moving
// node, if that is an animated object, or for collision ids 5 and 7 also a
// player). The file is compiled with deferred inlining (see
// anim_obj_draw.cpp), so the functions are written in reverse image order.
// The classes and functions are named by the symbols; ISceneNode declares
// the used virtual functions at their __vt__11CAnimObject slots
// (placeholders named by offset), the collision, contact and record views,
// the members and flag names are inferred, the record's copy constructor is
// inferred from the copy order, dwi::fast_vec follows anim_obj_ctor.cpp, and
// the CVector3 view copies through a double pair as in player_weapon.cpp with
// an inline Normalize matching the weak CVector3::Normalize. The rest of the
// file is not part of this unit.
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
    void Normalize() {
        float length = sqrtf(x * x + y * y + z * z);
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CAnimObject;
class CPlayerObject;
class IMovingSceneNode;

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

namespace dwi {
template <class T>
class fast_vec {
public:
    void push_back(T value) { m_data[m_size++] = value; }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

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
