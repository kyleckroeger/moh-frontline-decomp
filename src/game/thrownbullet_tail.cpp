// A fragment of thrownbullet.cpp (0x800d4134): CThrownBullet's Destroy
// (returns the bullet to g_pBulletFactory), MarkForDestruction (ISubject's,
// then the attached light at +496 is marked too and its subscription
// cancelled through CPublisher, the attached effect at +500 released through
// its slot +28, bit 0x01 of the flag byte at +504 set, the bullet removed
// from g_scene and its collision id cleared) and Shutdown (CBullet's, then
// the light-volume manager at +212 reset). The classes, functions and
// globals are named by the mangled symbols; the members, flag name and
// effect view are inferred, and the bullet is passed as the IObserver at its
// start (the observer base is not modelled). The "&m_light" test reproduces
// the original's null check of the subscription slot's address.
// CThrownBullet declares Draw (defined elsewhere) first so its table is not
// emitted here. The rest of the file is not part of this unit.
class ISceneNode;
class CBullet;

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

class CThrownBullet : public CBullet {
public:
    virtual void Draw(); /* defined elsewhere; keeps the table out */
    virtual void Destroy();
    virtual void MarkForDestruction(int);
    void Shutdown();

    unsigned char unknown004[32];
    int m_collisionId;
    unsigned char unknown028[172];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown0f0[256];
    ISubject* m_light;
    AttachedEffectView* m_effect;
    unsigned char m_flags1f8 : 7;
    bool m_destroyed : 1;
};

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
