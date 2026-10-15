// A fragment of Soldier_object.cpp (0x800ac310): CSoldierObject's world and
// local bounding-volume getters: volume pointers for types 3 and 0, embedded
// volumes for types 1 and 2, none for other types; UpdateLocalBoundingVolume
// (type 3's volume box gets the new corners; the other types have nothing to
// update), CommitUpdate (CAnimObject's) and AttemptUpdate (CAnimObject's,
// then the type 1 and 2 world volumes become the local ones transformed by
// the soldier's matrix at +9024), UpdateAI (the aim matrix at +16224 placed
// at the soldier's position plus its rows scaled by the offset at +16288,
// then the AI doodad at +12512 updated) and BeginUpdate (CAnimObject's, the
// doodad's pre-update, the current and other weapons updated, a crouch timer
// and a countdown timer advanced in seconds at 60 per second).
// CSoldierObject, ISceneNode, IVolume, CVolBox, CAnimObject,
// CAISoldierDoodad, CWeapon and EVolumeType are named by the mangled
// symbols; the members and their names are inferred, CAnimObject declares
// placeholder virtuals up to GetPosition (+64, named by the node table), the
// CVector3 view copies through a double pair as in player_weapon.cpp, and the
// empty cases reproduce the original's compare tree. The rest of the file is
// not part of this unit.
class CMatrix;

union CVector3Pair {
    double pair[2];
    float v[4];
};

/* inferred: CVector3 copies move it as a doubleword pair */
class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    CVector3& operator+=(const CVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
};

class CVolBox : public IVolume {
public:
    void SetCorners(CVector3, CVector3);
};

/* inferred: the matrix rows */
class CMatrix {
public:
    CVector3 GetRight() const { return right; }
    CVector3 GetFront() const { return front; }
    CVector3 GetUp() const { return up; }
    void SetPos(CVector3);

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
};

class CWeapon {
public:
    void Update(float, bool);
};

class CAISoldierDoodad {
public:
    void Update(float);
    void PreUpdate();

    unsigned char unknown0000[3712];
};

/* inferred: storage of an embedded volume object (its class is not known) */
struct EmbeddedVolumeView {
    unsigned char data[32];
};

class ISceneNode {
public:
    enum EVolumeType {};
};

class CAnimObject {
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
    virtual void GetPosition(CVector3&) const;

    void CommitUpdate();
    void AttemptUpdate(float);
    void UpdateAI(float);
    void BeginUpdate(float);
};

class CSoldierObject : public CAnimObject {
public:
    IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    void CommitUpdate();
    void AttemptUpdate(float);
    void UpdateAI(float);
    void BeginUpdate(float);

    unsigned char unknown0004[9020];
    CMatrix m_matrix;
    IVolume* m_localVolume3;
    IVolume* m_localVolume0;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume0;
    unsigned char unknown2390[56];
    float m_impulseX;
    float m_impulseY;
    float m_impulseZ;
    unsigned char unknown23d4[3340];
    CAISoldierDoodad m_doodad;
    CMatrix m_aimMatrix;
    CVector3 m_aimOffset;
    unsigned char unknown3fb0[96];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
    unsigned char unknown4078[680];
    EmbeddedVolumeView m_localVolume1;
    EmbeddedVolumeView m_worldVolume1;
    EmbeddedVolumeView m_localVolume2;
    EmbeddedVolumeView m_worldVolume2;
    unsigned char unknown43a0[16];
    bool m_crouchTimerOn;
    float m_crouchTime;
    unsigned char unknown43b8[212];
    CWeapon* m_otherWeapon;
    float m_timer;
    int m_timerState;
};

IVolume* CSoldierObject::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_worldVolume3;
    case 0:
        return m_worldVolume0;
    case 1:
        return (IVolume*)&m_worldVolume1;
    case 2:
        return (IVolume*)&m_worldVolume2;
    case 4:
        return 0;
    }
    return 0;
}

IVolume* CSoldierObject::GetLocalBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_localVolume3;
    case 0:
        return m_localVolume0;
    case 1:
        return (IVolume*)&m_localVolume1;
    case 2:
        return (IVolume*)&m_localVolume2;
    case 4:
        return 0;
    }
    return 0;
}

void CSoldierObject::UpdateLocalBoundingVolume(ISceneNode::EVolumeType type, const CVector3& min, const CVector3& max) {
    switch (type) {
    case 3:
        {
            CVolBox* box = (CVolBox*)m_localVolume3;
            box->SetCorners(min, max);
        }
        break;
    case 0:
        break;
    case 1:
        break;
    case 4:
        break;
    }
}

void CSoldierObject::CommitUpdate() {
    CAnimObject::CommitUpdate();
}

void CSoldierObject::AttemptUpdate(float dt) {
    CAnimObject::AttemptUpdate(dt);
    GetWorldBoundingVolume((ISceneNode::EVolumeType)1)->TransformedCopy(*GetLocalBoundingVolume((ISceneNode::EVolumeType)1), m_matrix);
    GetWorldBoundingVolume((ISceneNode::EVolumeType)2)->TransformedCopy(*GetLocalBoundingVolume((ISceneNode::EVolumeType)2), m_matrix);
}

void CSoldierObject::UpdateAI(float dt) {
    CVector3 right = m_aimMatrix.GetRight();
    CVector3 front = m_aimMatrix.GetFront();
    CVector3 up = m_aimMatrix.GetUp();
    CVector3 pos;
    GetPosition(pos);
    right *= m_aimOffset.x;
    front *= m_aimOffset.y;
    up *= m_aimOffset.z;
    pos += right;
    pos += front;
    pos += up;
    m_aimMatrix.SetPos(pos);
    m_doodad.Update(dt);
}

void CSoldierObject::BeginUpdate(float dt) {
    float seconds = dt / 60.0f;
    CAnimObject::BeginUpdate(dt);
    m_doodad.PreUpdate();
    if (m_weapons[m_currentWeapon]) {
        m_weapons[m_currentWeapon]->Update(dt, false);
        m_otherWeapon->Update(dt, false);
    }
    if (m_crouchTimerOn) {
        m_impulseX = m_impulseY = m_impulseZ = 0.0f;
        m_crouchTime += seconds;
    }
    if (m_timer > 0.0f) {
        m_timer -= seconds;
    } else if (m_timer < 0.0f) {
        m_timer = 0.0f;
        m_timerState = 0;
    }
}
