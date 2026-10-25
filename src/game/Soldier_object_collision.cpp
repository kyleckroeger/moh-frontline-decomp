// A fragment of Soldier_object.cpp (0x800abc6c): CSoldierObject::OnCollision.
// After CAnimObject's handling, the other node of the collision is taken.
// For soldiers and players, both collision ids are mapped to a class
// (0 for ids 2 and 13, 1 for 1 and 5, 2 for 7, 25 and -1) and the pair of
// classes to a script event (188, 189 or 190), which is triggered on this
// soldier's script object with the other's script object when the other is
// not a player, or is a player whose AI status variable 5 is 1. A player
// is then pushed: the collision normals summed (turned towards this soldier
// when it is the second object), flattened, and when 30 times its vertical
// part exceeds 1 and the rest is not negligible, the normalized direction
// is applied to the player as a force of that size. Hitting a world object
// triggers event 169 with the collision; hitting a static or hierarchy
// object triggers 133 when a contact faces up (its normal's dot with +z
// above 0.8; the contacts are walked as an array) and 191 otherwise. The
// functions and classes are named by the mangled symbols and
// __vt__10ISceneNode; the collision and contact views, the tables' layout,
// the AI doodad chain to its CAIStatus, the vector helpers and the
// FindClasses inline (its own -1 locals) are inferred.
// The tables and constants are .rodata and .sdata2 pool references. The rest
// of the file is in other units.
#include <math.h>

class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
enum EClsnId {};

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

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
    virtual int GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual struct BSObject* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual class IMovingSceneNode* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual ISceneNode* AsPlayerObject();
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
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};
class IMovingSceneNode : public ISceneNode {
public:
    virtual void vf0dc();
    virtual void vf0e0();
    virtual void ApplyForceTo(CVector3, float);
};

struct BSObject;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

enum aistatus_var {};

class CAIStatus {
public:
    int GetVarValue(aistatus_var) const;
};

/* inferred: the AI doodad's filter object's AI object and its status */
struct AIObjectView {
    unsigned char unknown000[436];
    CAIStatus m_status;
};

struct AIFilterView {
    unsigned char unknown00[8];
    AIObjectView* m_object;
};

struct AIDoodadView {
    unsigned char unknown00[4];
    AIFilterView* m_filter;
};

/* inferred: a collision contact (48 bytes) */
struct ContactView {
    CVector3 m_normal;
    unsigned char unknown10[28];
    ContactView* m_next;
};

class CCollision {
public:
    ISceneNode* m_object0;
    ISceneNode* m_object1;
    unsigned char unknown08[16];
    ContactView* m_contacts;

    /* inferred */
    ISceneNode* GetOther(const ISceneNode* node) const {
        ISceneNode* other = m_object0;
        if (other == node)
            other = m_object1;
        return other;
    }
};

class CAnimObject : public ISceneNode {
public:
    void OnCollision(const CCollision&);
};

/* inferred: collision id to class, and class pair to script event */
struct CollisionClassView {
    int m_id;
    int m_class;
};

struct CollisionEventView {
    int m_class0;
    int m_class1;
    int m_event;
};

class CSoldierObject : public CAnimObject {
public:
    virtual ~CSoldierObject(); // declared first: defined in another unit (no vtable here)
    void OnCollision(const CCollision&);

    unsigned char unknown0004[9100];
    BSObject* m_script;
};

static inline void FindClasses(CollisionClassView* classes, int id, int otherId, int& myClass, int& otherClass) {
    int mine = -1;
    int theirs = -1;
    for (int i = 0; i < 7; i++) {
        if (id == classes[i].m_id)
            mine = classes[i].m_class;
        if (otherId == classes[i].m_id)
            theirs = classes[i].m_class;
    }
    myClass = mine;
    otherClass = theirs;
}
void CSoldierObject::OnCollision(const CCollision& collision) {
    CAnimObject::OnCollision(collision);
    ISceneNode* other = collision.m_object0 == this ? collision.m_object1 : collision.m_object0;
    if (other->AsSoldierObject() || other->AsPlayerObject()) {
        int id = GetCollisionId();
        int otherId = other->GetCollisionId();
        CollisionClassView classes[7] = {{2, 0}, {13, 0}, {1, 1}, {5, 1}, {7, 2}, {25, 2}, {-1, 2}};
        int otherClass;
        int myClass;
        FindClasses(classes, id, otherId, myClass, otherClass);
        CollisionEventView events[9] = {{0, 0, 189}, {0, 1, 188}, {0, 2, 190}, {1, 0, 188}, {1, 1, 189},
                                        {1, 2, 190}, {2, 0, 190}, {2, 1, 190}, {2, 2, 190}};
        int event = -1;
        for (int j = 0; j < 9; j++) {
            if (myClass == events[j].m_class0 && otherClass == events[j].m_class1) {
                event = events[j].m_event;
                break;
            }
        }
        if (!other->AsPlayerObject())
            BSObjectTriggerEvent(m_script, event, other->GetScriptObject(), 0, true);
        ISceneNode* player = other->AsPlayerObject();
        if (player) {
            if (player->GetAIDoodad()->m_filter->m_object->m_status.GetVarValue((aistatus_var)5) == 1)
                BSObjectTriggerEvent(m_script, event, other->GetScriptObject(), 0, true);
            bool first = collision.m_object0 == this;
            CVector3 normal;
            normal.x = normal.y = normal.z = 0.0f;
            for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
                normal.x += contact->m_normal.x;
                normal.y += contact->m_normal.y;
                normal.z += contact->m_normal.z;
            }
            if (!first) {
                normal.x *= -1.0f;
                normal.y *= -1.0f;
                normal.z *= -1.0f;
            }
            float force = 30.0f * (float)fabs(normal.z);
            normal.z = 0.0f;
            if (force > 1.0f && sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z) > 0.0001f) {
                float length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
                if (length != 0.0f) {
                    normal.x *= 1.0f / length;
                    normal.y *= 1.0f / length;
                    normal.z *= 1.0f / length;
                }
                other->AsMovingNode()->ApplyForceTo(normal, force);
            }
        }
    } else if (other->AsWorldObject()) {
        BSObjectTriggerEvent(m_script, 169, (void*)&collision, 0, true);
    } else if (other->AsStaticObject() || other->AsHierObject()) {
        bool up = false;
        CVector3 vertical;
        vertical.x = 0.0f;
        vertical.y = 0.0f;
        vertical.z = 1.0f;
        for (ContactView* contact = collision.m_contacts; contact; contact++) {
            if (contact->m_normal.x * vertical.x + contact->m_normal.y * vertical.y + contact->m_normal.z * vertical.z > 0.8f) {
                up = true;
                break;
            }
        }
        if (up)
            BSObjectTriggerEvent(m_script, 133, (void*)&collision, 0, true);
        else
            BSObjectTriggerEvent(m_script, 191, (void*)&collision, 0, true);
    }
}
