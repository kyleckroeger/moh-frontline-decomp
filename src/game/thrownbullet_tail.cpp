// A fragment of thrownbullet.cpp (0x800d3f5c): CThrownBullet's OnCollision
// (unless destroyed or already flagged: by thrown type, records whether a
// contact normal points up (z > 0.7, and for the bouncing types not along
// the velocity) and sums the normals, or marks the bullet to explode, then
// lets the other node handle the hit; an unknown type is reported), Destroy
// (returns the bullet to g_pBulletFactory), MarkForDestruction (ISubject's,
// then the attached light at +496 is marked too and its subscription
// cancelled through CPublisher, the attached effect at +500 released through
// its slot +28, bit 0x01 of the flag byte at +504 set, the bullet removed
// from g_scene and its collision id cleared), Shutdown (CBullet's, then
// the light-volume manager at +212 reset) and Init (CBullet's, then stores
// the type, properties, fuse, position and collision id, clears the flags,
// drops the light and subscribes to the thrower; scales the throw power by
// 1/512 into the function-static [minPower, maxPower] range, or, when the
// thrower has an owner, to a quarter (types 0-2) or half (type 10) of
// maxPower or maxPower times the scaled power, and sets the velocity
// along the normalised direction from it, the speed and the properties' speed;
// builds the matrix from the position and a frame around the direction, adds
// the bullet to g_scene, sets its fiber from the origin to minus the speed
// along y, and by type
// sets a random or fixed spin or creates the attached light and smoke effect
// (an unknown type is reported); then resets the counters, triggers event
// 264 on the German whose node shares the thrower's owner, and the rubber
// grenade cheats zero the friction and make the bounce 1). The classes,
// functions and globals are named by the mangled symbols; the members, flag
// names, the contact, effect and property views and the slot names
// (HandleBulletCollision from __vt__10ISceneNode, the others by guess) are
// inferred or placeholders, and the bullet is passed as the IObserver at its
// start (the observer base is not modelled). The "&m_light" test reproduces
// the original's null check of the subscription slot's address.
// CThrownBullet declares Draw (defined elsewhere) first so its table is not
// emitted here. The rest of the file is not part of this unit.
class CBullet;
class ISceneNode;

void DebugMsg(const char*, ...);

struct BSObject;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
extern "C" int rand();

enum ESubjectEvent {};
enum EThrownBulletTypes {};
enum EClsnId {};

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
        return y;
    }
    return x;
}

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    void Set(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    float Length() const { return sqrtf(d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]); }
    void Normalize() {
        float length = Length();
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }
    void operator*=(float s) {
        d.v[0] *= s;
        d.v[1] *= s;
        d.v[2] *= s;
    }
    void Cross(const CVector3& a, const CVector3& b) {
        d.v[0] = a.d.v[1] * b.d.v[2] - a.d.v[2] * b.d.v[1];
        d.v[1] = a.d.v[2] * b.d.v[0] - a.d.v[0] * b.d.v[2];
        d.v[2] = a.d.v[0] * b.d.v[1] - a.d.v[1] * b.d.v[0];
    }

    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    void BuildTrans(CVector3);
    void SetFront(CVector3);
    void SetRight(CVector3);
    void SetUp(CVector3);

    float m[4][4];
} __attribute__((aligned(16)));

class CVolFiber {
public:
    void Set(CVector3, CVector3);
    virtual void vf008();

    unsigned char unknown04[76];
};

class ISubject {
public:
    virtual void MarkForDestruction(int);
};

class IObserver;

typedef void (*SubjectCallback)(IObserver&, ISubject**, ESubjectEvent);

class CPublisher {
public:
    static void CancelSubscription(IObserver&, ISubject**);
    static void AddSubscription(IObserver&, ISubject**, SubjectCallback);
};

void SECB_NullifyPointer(IObserver&, ISubject**, ESubjectEvent);
void SECB_NullifyPointerOnMarked(IObserver&, ISubject**, ESubjectEvent);

class CScene {
public:
    void Add(ISceneNode&);
    void Remove(ISceneNode&);

    unsigned char unknown000[328];
};

extern CScene g_scene;

class CBulletFactory {
public:
    void DestroyBullet(CBullet*);
};

extern CBulletFactory* g_pBulletFactory;

class CLightVolumeManager {
public:
    void Reset();

    unsigned char unknown00[28];
};

/* inferred: an attached effect whose table is at +28 */
class AttachedEffectView {
    unsigned char unknown00[28];

public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void SetLocalToWorld(const CMatrix&);
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void SetVelocity(CVector3);
};

class CParticleSystem {
public:
    static AttachedEffectView* Create(unsigned long);
};

class CLight : public ISubject {
public:
    static CLight* Create(unsigned long);
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void SetTMLocalToWorld(const CMatrix&);
};

class CBullet : public ISubject {
public:
    void Init();
    void Shutdown();
};

class CCollision;

class ISceneNode {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual ISceneNode* GetOwner();
};

class BSGO_Basic {
public:
    BSObject* m_object;
    BSGO_Basic* m_next;
    BSGO_Basic* m_prev;

    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

extern BSGO_Basic* g_pGermanList;
extern bool g_bIsRubberGrenadeCheatEnabled;

/* inferred: the cheat flag read here */
struct CheatsView {
    unsigned char unknown00[10];
    unsigned char m_keepGrenadeSpeed;
    bool m_bigExplosions;
    unsigned char unknown0c[8];
};

extern CheatsView g_cheats;

/* inferred: the thrown bullet's properties */
struct ThrownBulletProperties_struct {
    float m_speed;
    unsigned char unknown04[52];
    float m_friction;
    float m_bounce;
    unsigned char unknown40[8];
    short m_fuse;
    signed char m_bounces;
};

/* inferred: a contact of the collision, its normal first, chained at +44 */
struct ContactView {
    float m_normalX;
    float m_normalY;
    float m_normalZ;
    unsigned char unknown0c[32];
    ContactView* m_next;
};

class CCollision {
public:
    ISceneNode* m_nodeA;
    ISceneNode* m_nodeB;
    unsigned char unknown08[16];
    ContactView* m_contacts;
};

class CThrownBullet : public CBullet {
public:
    virtual void Draw(); /* defined elsewhere; keeps the table out */
    virtual void OnCollision(const CCollision&);
    virtual void Destroy();
    virtual void MarkForDestruction(int);
    void Shutdown();
    void Init(EThrownBulletTypes, ThrownBulletProperties_struct*, const CVector3&, const CVector3&, float, EClsnId, float, ISceneNode*);
    void SetSubject(ISubject** slot, ISubject* subject, SubjectCallback callback) {
        if (subject != *slot) {
            if (slot)
                CPublisher::CancelSubscription(*(IObserver*)this, slot);
            *slot = subject;
            if (subject)
                CPublisher::AddSubscription(*(IObserver*)this, slot, callback);
        }
    }

    unsigned char unknown004[32];
    int m_collisionId;
    unsigned char unknown028[8];
    CVector3 m_position;
    unsigned char unknown040[16];
    CMatrix m_tm;
    float m_fuse;
    unsigned char unknown094[16];
    unsigned int m_type;
    ThrownBulletProperties_struct* m_properties;
    ISceneNode* m_firedBy;
    int m_value0b0;
    unsigned char unknown0b4[12];
    CThrownBullet* m_self;
    int m_value0c4;
    int m_value0c8;
    int m_value0cc;
    int m_value0d0;
    CLightVolumeManager m_lightVolumes;
    CVector3 m_velocity;
    CVector3 m_previousVelocity;
    float m_contactNormalX;
    float m_contactNormalY;
    float m_contactNormalZ;
    unsigned char unknown11c[4];
    CVector3 m_rotation;
    CVector3 m_previousRotation;
    CVector3 m_spin;
    CVolFiber m_fiber;
    unsigned char unknown1a0[80];
    ISubject* m_light;
    AttachedEffectView* m_effect;
    bool m_grounded : 1;
    bool m_touched : 1;
    bool m_flag20 : 1;
    bool m_flag10 : 1;
    bool m_flag08 : 1;
    bool m_flag04 : 1;
    bool m_flag02 : 1;
    bool m_destroyed : 1;
    int m_value1fc;
};

void CThrownBullet::OnCollision(const CCollision& collision) {
    if (m_destroyed || m_flag10)
        return;
    switch (m_type) {
    case 5:
        m_flag20 = true;
    case 3:
    case 4:
    case 9:
        m_contactNormalX = m_contactNormalY = m_contactNormalZ = 0.0f;
        for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
            if (contact->m_normalZ > 0.7f && contact->m_normalX * m_velocity.d.v[0] + contact->m_normalY * m_velocity.d.v[1] + contact->m_normalZ * m_velocity.d.v[2] <= 0.0f)
                m_grounded = true;
            m_contactNormalX += contact->m_normalX;
            m_contactNormalY += contact->m_normalY;
            m_contactNormalZ += contact->m_normalZ;
            m_touched = true;
        }
        break;
    case 0:
    case 1:
    case 2:
    case 8:
    case 10:
        for (ContactView* contact = collision.m_contacts; contact; contact = contact->m_next) {
            if (contact->m_normalZ > 0.7f)
                m_grounded = true;
            m_touched = true;
        }
        if (m_touched)
            m_flag10 = !m_flag08;
        break;
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in OnCollision.  Type %d\n", m_type);
        break;
    }
    if (m_touched) {
        ISceneNode* other = collision.m_nodeA;
        if (other == (ISceneNode*)this)
            other = collision.m_nodeB;
        other->HandleBulletCollision(this, collision);
    }
}

void CThrownBullet::Destroy() {
    g_pBulletFactory->DestroyBullet(this);
}

void CThrownBullet::MarkForDestruction(int flag) {
    ISubject::MarkForDestruction(flag);
    if (m_light) {
        m_light->MarkForDestruction(flag);
        if (m_light) {
            if (&m_light)
                CPublisher::CancelSubscription(*(IObserver*)this, &m_light);
            m_light = 0;
        }
    }
    if (m_effect) {
        m_effect->unknown01c();
        m_effect = 0;
    }
    m_destroyed = true;
    g_scene.Remove(*(ISceneNode*)this);
    m_collisionId = -1;
}

void CThrownBullet::Shutdown() {
    CBullet::Shutdown();
    m_lightVolumes.Reset();
}

inline double fabs(double x) { return __fabs(x); }
#define RANDF ((float)rand() / 32767.0f)

void CThrownBullet::Init(EThrownBulletTypes type, ThrownBulletProperties_struct* properties, const CVector3& position,
                         const CVector3& direction, float speed, EClsnId clsnId, float power, ISceneNode* firedBy) {
    CBullet::Init();
    m_type = type;
    m_properties = properties;
    m_fuse = properties->m_fuse;
    m_position = position;
    m_collisionId = clsnId;
    m_grounded = false;
    m_touched = false;
    m_flag10 = false;
    m_flag04 = false;
    m_flag02 = false;
    m_destroyed = false;
    m_flag08 = !properties->m_bounces;
    SetSubject(&m_light, 0, SECB_NullifyPointerOnMarked);
    m_effect = 0;
    m_value1fc = 0;
    SetSubject((ISubject**)&m_firedBy, (ISubject*)firedBy, SECB_NullifyPointer);
    power *= 0.001953125f;
    static float maxPower = 1.0f;
    static float minPower = 0.1f;
    if (m_firedBy && m_firedBy->GetOwner()) {
        switch (type) {
        case 0:
        case 1:
        case 2:
            power = 0.25f * maxPower;
            break;
        case 10:
            power = 0.5f * maxPower;
            break;
        default:
            power = maxPower * power;
            break;
        }
    } else {
        power = minPower + power * (maxPower - minPower);
    }
    m_velocity = direction;
    m_velocity.Normalize();
    m_velocity *= power * (speed * m_properties->m_speed);
    m_previousVelocity = m_velocity;
    m_tm.BuildTrans(position);
    CVector3 front = direction;
    CVector3 right;
    CVector3 up;
    front.Normalize();
    if (fabs(front.d.v[2]) < fabs(front.d.v[0])) {
        up.Set(0.0f, 0.0f, 1.0f);
        right.Cross(front, up);
        up.Cross(right, front);
    } else {
        if (front.d.v[1] < 0.0f)
            right.Set(-1.0f, 0.0f, 0.0f);
        else
            right.Set(1.0f, 0.0f, 0.0f);
        up.Cross(right, front);
        right.Cross(front, up);
    }
    m_tm.SetFront(front);
    m_tm.SetRight(right);
    m_tm.SetUp(up);
    g_scene.Add(*(ISceneNode*)this);
    CVector3 top(0.0f, -m_velocity.Length(), 0.0f);
    CVector3 bottom(0.0f, 0.0f, 0.0f);
    m_fiber.Set(top, bottom);
    switch (m_type) {
    case 5:
        m_flag20 = false;
        m_rotation.Set(0.0f, 0.0f, 0.0f);
        {
            float x, y, z;
            z = RANDF;
            y = RANDF;
            x = RANDF;
            m_spin.Set(-1.3962634f * x, -1.3962634f * y, -1.3962634f * z);
        }
        break;
    case 4:
        m_rotation.Set(0.0f, 0.0f, 0.0f);
        m_spin = CVector3(0.0f, 1.3962634f, 1.3962634f);
        break;
    case 3:
    case 6:
    case 9:
        m_rotation.Set(0.0f, 0.0f, 0.0f);
        {
            float x, y, z;
            z = RANDF;
            y = RANDF;
            x = RANDF;
            m_spin.Set(-0.41887903f * x, -0.41887903f * y, -0.41887903f * z);
        }
        break;
    case 0:
    case 1:
    case 2:
    case 10: {
        m_flag20 = false;
        m_rotation.Set(0.0f, 0.0f, 0.0f);
        m_spin.Set(0.0f, 0.0f, 0.0f);
        CLight* light = CLight::Create(0xA2401755);
        if (light) {
            light->SetTMLocalToWorld(m_tm);
            SetSubject(&m_light, light, SECB_NullifyPointerOnMarked);
        }
        m_effect = CParticleSystem::Create(0xA23FD6BD);
        if (m_effect) {
            CVector3 velocity(60.0f * m_velocity.d.v[0], 60.0f * m_velocity.d.v[1], 60.0f * m_velocity.d.v[2]);
            m_effect->SetVelocity(velocity);
            m_effect->SetLocalToWorld(m_tm);
        }
        break;
    }
    case 8: {
        m_flag20 = false;
        m_rotation.Set(0.0f, 0.0f, 0.0f);
        m_spin.Set(0.0f, 0.0f, 0.0f);
        CLight* light = CLight::Create(0xA2401755);
        if (light) {
            light->SetTMLocalToWorld(m_tm);
            SetSubject(&m_light, light, SECB_NullifyPointerOnMarked);
        }
        m_effect = CParticleSystem::Create(0xA23FD6BD);
        if (m_effect) {
            CVector3 velocity(60.0f * m_velocity.d.v[0], 60.0f * m_velocity.d.v[1], 60.0f * m_velocity.d.v[2]);
            m_effect->SetVelocity(velocity);
            m_effect->SetLocalToWorld(m_tm);
        }
        break;
    }
    default:
        DebugMsg("WARNING: Unknown thrown bullet type in Init.  Type %d\n", m_type);
        break;
    }
    m_previousRotation = m_rotation;
    m_value0c8 = 0;
    m_value0c4 = 0;
    m_value0d0 = 0;
    m_value0cc = 0;
    m_value0b0 = 0;
    m_self = this;
    ISceneNode* owner = m_firedBy ? m_firedBy->GetOwner() : 0;
    if (owner) {
        for (BSGO_Basic* german = g_pGermanList; german; german = german->m_next) {
            if (german->GetSceneNode()->GetOwner() == owner) {
                BSObjectTriggerEvent(german->m_object, 264, this, 0, true);
                break;
            }
        }
    }
    if (g_bIsRubberGrenadeCheatEnabled || g_cheats.m_keepGrenadeSpeed) {
        m_properties->m_friction = 0.0f;
        m_properties->m_bounce = 1.0f;
    }
}
