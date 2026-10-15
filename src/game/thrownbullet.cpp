// CThrownBullet: the weak bounding-volume, collision, light, damage and cast
// accessors. The class names come from the mangled symbols; the members and
// the weapon-record view are inferred from offsets, the result types are
// inferred, and the class view declares only the first virtual
// (MarkForDestruction, defined elsewhere, so the table is not emitted). The
// accessors are inline in the original (weak symbols), so they are defined
// __declspec(weak). The weak velocity getter, the light-volume forwards (to
// the manager at +212) and SomebodyCaughtYou follow; the latter, unless bit
// 0x10 or bit 0x01 of the flag byte at +504 is set, clears bit 0x10, sets bit
// 0x08 and marks the object for destruction (the bit-fields are views named
// by mask). The rest of the file is not part of this unit.
class CLight;
struct BPDLightVolume;

// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: the velocity
// is copied as two lfd/stfd pairs, which an implicit copy through the double
// pair reproduces.
class CVector3 {
public:
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        double pair[2];
    };
} __attribute__((aligned(8)));

class CLightVolumeManager {
public:
    void* GetVolume();
    void RemoveVolume(BPDLightVolume*);
    void AddVolume(BPDLightVolume*);

    unsigned char unknown00[28];
};

class ISceneNode {
public:
    enum EVolumeType {};
};

struct ThrownWeaponView {
    unsigned char unknown00[64];
    float damage;
};

class CThrownBullet {
public:
    virtual void MarkForDestruction(int); /* defined elsewhere; keeps the table out */
    const void* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    const void* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    bool IsCollisionEnabled() const;
    CLight* GetAttachedLight() const;
    float GetDamage() const;
    CThrownBullet* AsThrown();
    void GetVelocity(CVector3&);
    void* GetLightVolume();
    void ExitLightVolume(BPDLightVolume*);
    void EnterLightVolume(BPDLightVolume*);
    void SomebodyCaughtYou();

    unsigned char unknown004[164];
    ThrownWeaponView* m_weapon;
    unsigned char unknown0ac[40];
    CLightVolumeManager m_lightVolumes;
    CVector3 m_velocity;
    unsigned char unknown100[80];
    unsigned char m_localVolume[80];
    unsigned char m_worldVolume[80];
    CLight* m_attachedLight;
    unsigned char unknown1f4[4];
    unsigned char m_flags1f8 : 3;
    unsigned char m_flag10 : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_flags1f8b : 2;
    unsigned char m_flag01 : 1;
};

__declspec(weak) const void* CThrownBullet::GetLocalBoundingVolume(ISceneNode::EVolumeType) const {
    return m_localVolume;
}

__declspec(weak) const void* CThrownBullet::GetWorldBoundingVolume(ISceneNode::EVolumeType) const {
    return m_worldVolume;
}

__declspec(weak) bool CThrownBullet::IsCollisionEnabled() const {
    return true;
}

__declspec(weak) CLight* CThrownBullet::GetAttachedLight() const {
    return m_attachedLight;
}

__declspec(weak) float CThrownBullet::GetDamage() const {
    return m_weapon->damage;
}

__declspec(weak) CThrownBullet* CThrownBullet::AsThrown() {
    return this;
}

__declspec(weak) void CThrownBullet::GetVelocity(CVector3& v) {
    v = m_velocity;
}

void* CThrownBullet::GetLightVolume() {
    return m_lightVolumes.GetVolume();
}

void CThrownBullet::ExitLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.RemoveVolume(volume);
}

void CThrownBullet::EnterLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.AddVolume(volume);
}

void CThrownBullet::SomebodyCaughtYou() {
    if (!m_flag10 && !m_flag01) {
        m_flag10 = 0;
        m_flag08 = 1;
        MarkForDestruction(0);
    }
}
