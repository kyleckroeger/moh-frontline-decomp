// CThrownObject::OnCollision (a fragment of thrown_obj.cpp from 0x800b5bdc):
// while flying (state -1) and not hit by a bullet, each contact's normal
// (negated when this object is the collision's first node) that opposes the
// velocity is added to the push vector at +664 (flag 0x20), and a contact
// facing up (z > 0.7) sets the landed flag (0x40) and picks the impact sound
// from the contact's surface (sound 1 without one) when none is pending and the
// other node is a static object; in state 1 a world object or soldier gets
// script event 231 and the pair stops colliding; otherwise an upward contact
// fires event 232 once (state 2), stops the object and picks the sound the same
// way. CThrownObject::CleanUp: free the local and
// world bounding spheres (once when they are the same) and clear the sphere
// and collider pointers. The names come from the mangled symbols;
// ISceneNode's virtual functions follow __vt__13CThrownObject (slots through
// GetWorldBoundingVolume, +48 as a placeholder, the later slots these
// functions call) and CThrownObject declares Init first so this file does not
// emit its vtable. BSGO_Basic keeps
// its three words before its vtable pointer, as the constructors store them;
// its first virtual entry is a placeholder. The members are inferred from
// offsets (the mesh-constructor that sets the orientation reads constants
// pooled with the rest of the file and is not part of these fragments).
class CStaticMesh;
struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) {
        x = ax;
        y = ay;
        z = az;
    }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

/* inferred: a surface record, its impact sound index in the top 11 bits of the halfword at +12 */
struct SurfaceView {
    unsigned char unknown00[12];
    short m_sound : 11;
    short m_flags : 5;
};

/* inferred: a collision contact (normal first, surface at +36, next at +44) */
struct ContactView {
    CVector3 m_normal;
    unsigned char unknown10[20];
    SurfaceView* m_surface;
    unsigned char unknown28[4];
    ContactView* m_next;
};

class ISceneNode;

class CCollision {
public:
    ISceneNode* m_nodeA;
    ISceneNode* m_nodeB;
    unsigned char unknown08[16];
    ContactView* m_contacts;
};

class CSurfaceType {
public:
    static int GetSoundIndex(int);
};

class CScene {
public:
    void ExcludeCollision(const ISceneNode&, const ISceneNode&, int);

    unsigned char unknown00[328];
};

extern CScene g_scene;
struct LevelFileContentsStruct_;

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
    virtual void unknown30();
    virtual void* GetLocalBoundingVolume(EVolumeType) const;
    virtual void* GetWorldBoundingVolume(EVolumeType) const;
    virtual void unknown3c();
    virtual void unknown40();
    virtual void unknown44();
    virtual void unknown48();
    virtual void unknown4c();
    virtual void unknown50();
    virtual void unknown54();
    virtual void unknown58();
    virtual void unknown5c();
    virtual BSObject* GetScriptObject() const;
    virtual void unknown64();
    virtual void unknown68();
    virtual void unknown6c();
    virtual void unknown70();
    virtual void unknown74();
    virtual void unknown78();
    virtual void unknown7c();
    virtual void unknown80();
    virtual void* AsStaticObject();
    virtual void unknown88();
    virtual void* AsWorldObject();
    virtual void unknown90();
    virtual void unknown94();
    virtual void unknown98();
    virtual void* AsSoldierObject();
    virtual void unknowna0();
    virtual void unknowna4();
    virtual void unknowna8();
    virtual void unknownac();
    virtual void unknownb0();
    virtual void unknownb4();
    virtual void unknownb8();
    virtual void* AsBullet();
};

class CStaticObject : public ISceneNode {
public:
    struct MotionFrame;

    CStaticObject(CStaticMesh*, int, int, int, int, int, MotionFrame*);
    CStaticObject();
    virtual ~CStaticObject();
};

class BSGO_Basic {
    int m_field0;
    int m_field4;
    int m_field8;

public:
    BSGO_Basic() : m_field0(0), m_field4(0), m_field8(0) {}
    virtual void unknown08();
};

class BSGO_Dummy : public BSGO_Basic {
public:
    virtual void unknown08();
};

void FreeSphereVolume(void*);

class CThrownObject : public CStaticObject {
public:
    virtual void Init();
    void OnCollision(const CCollision&);
    void CleanUp();
    virtual ~CThrownObject();
    CThrownObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*, LevelFileContentsStruct_*);
    CThrownObject();

    unsigned char unknown004[264];
    void* m_sphere;
    void* m_collider;
    void* m_lastCollider;
    unsigned char unknown118[72];
    CVector3 m_velocity;
    unsigned char unknown170[296];
    CVector3 m_push;
    unsigned char unknown2a8[16];
    float m_orientationX;
    float m_orientationY;
    float m_orientationZ;
    float m_orientationW;
    LevelFileContentsStruct_* m_definition;
    int m_pendingSound;
    BSGO_Dummy m_scriptObject;
    unsigned char unknown2E0[20];
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char flag2 : 1;
    unsigned char flag3 : 1;
    unsigned char deleted : 1;
    unsigned char markedForDestruction : 1;
    unsigned char flag6 : 1;
    unsigned char flag7 : 1;
    unsigned char unknown2f5[3];
    int m_state;

    void PickImpactSound(ISceneNode* other, const ContactView* contact) {
        if (m_pendingSound == 0 && other->AsStaticObject()) {
            if (contact->m_surface && contact->m_surface->m_sound >= 0)
                m_pendingSound = CSurfaceType::GetSoundIndex(contact->m_surface->m_sound);
            else
                m_pendingSound = 1;
        }
    }
};

void CThrownObject::OnCollision(const CCollision& collision) {
    bool first = collision.m_nodeA == this;
    ISceneNode* other;
    if (collision.m_nodeA == this)
        other = collision.m_nodeB;
    else
        other = collision.m_nodeA;
    if (m_state == -1) {
        if (!other->AsBullet()) {
            for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
                CVector3 normal;
                if (first) {
                    normal.x = -contact->m_normal.x;
                    normal.y = -contact->m_normal.y;
                    normal.z = -contact->m_normal.z;
                } else
                    normal = contact->m_normal;
                if (!(m_velocity.x * normal.x + m_velocity.y * normal.y + m_velocity.z * normal.z >= 0.0f)) {
                    m_push.x += normal.x;
                    m_push.y += normal.y;
                    m_push.z += normal.z;
                    flag2 = 1;
                }
                if (contact->m_normal.z > 0.7f) {
                    flag1 = 1;
                    PickImpactSound(other, contact);
                }
            }
        }
    } else if (m_state != 0) {
        if (m_state == 1 && (other->AsWorldObject() || other->AsSoldierObject())) {
            BSObjectTriggerEvent(GetScriptObject(), 231, other->GetScriptObject(), 0, false);
            g_scene.ExcludeCollision(*this, *other, -1);
        } else {
            for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
                if (contact->m_normal.z > 0.7f) {
                    flag1 = 1;
                    if (m_state != 2) {
                        BSObjectTriggerEvent(GetScriptObject(), 232, 0, 0, false);
                        m_state = 2;
                    }
                    m_velocity = CVector3(0.0f, 0.0f, 0.0f);
                    PickImpactSound(other, contact);
                }
            }
        }
    }
}

void CThrownObject::CleanUp() {
    void* local = GetLocalBoundingVolume((EVolumeType)3);
    void* world;

    if (local)
        FreeSphereVolume(local);
    world = GetWorldBoundingVolume((EVolumeType)3);
    if (world && world != local)
        FreeSphereVolume(world);
    m_sphere = 0;
    m_lastCollider = 0;
}
