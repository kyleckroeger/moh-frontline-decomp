// A fragment of thrownbullet.cpp (0x800d3f5c): CThrownBullet's OnCollision
// (unless destroyed or already flagged: by thrown type, records whether a
// contact normal points up (z > 0.7, and for the bouncing types not along
// the velocity) and sums the normals, or marks the bullet to explode, then
// lets the other node handle the hit; an unknown type is reported), Destroy
// (returns the bullet to g_pBulletFactory), MarkForDestruction (ISubject's,
// then the attached light at +496 is marked too and its subscription
// cancelled through CPublisher, the attached effect at +500 released through
// its slot +28, bit 0x01 of the flag byte at +504 set, the bullet removed
// from g_scene and its collision id cleared) and Shutdown (CBullet's, then
// the light-volume manager at +212 reset). The classes, functions and
// globals are named by the mangled symbols; the members, flag names, the
// contact and effect views and the slot +104 name (HandleBulletCollision,
// from __vt__10ISceneNode) are inferred or placeholders, and the bullet is
// passed as the IObserver at its start (the observer base is not modelled). The "&m_light" test reproduces
// the original's null check of the subscription slot's address.
// CThrownBullet declares Draw (defined elsewhere) first so its table is not
// emitted here. The rest of the file is not part of this unit.
class CBullet;
class ISceneNode;

void DebugMsg(const char*, ...);

class ISubject {
public:
    virtual void MarkForDestruction(int);
};

class IObserver;

class CPublisher {
public:
    static void CancelSubscription(IObserver&, ISubject**);
};

class CScene {
public:
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
};

class CBullet : public ISubject {
public:
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

    unsigned char unknown004[32];
    int m_collisionId;
    unsigned char unknown028[124];
    unsigned int m_type;
    unsigned char unknown0a8[44];
    CLightVolumeManager m_lightVolumes;
    float m_velocityX;
    float m_velocityY;
    float m_velocityZ;
    unsigned char unknown0fc[20];
    float m_contactNormalX;
    float m_contactNormalY;
    float m_contactNormalZ;
    unsigned char unknown11c[212];
    ISubject* m_light;
    AttachedEffectView* m_effect;
    bool m_grounded : 1;
    bool m_touched : 1;
    bool m_flag20 : 1;
    bool m_flag10 : 1;
    bool m_flag08 : 1;
    unsigned char m_flags1f8 : 2;
    bool m_destroyed : 1;
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
            if (contact->m_normalZ > 0.7f && contact->m_normalX * m_velocityX + contact->m_normalY * m_velocityY + contact->m_normalZ * m_velocityZ <= 0.0f)
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
