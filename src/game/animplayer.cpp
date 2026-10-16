// CAnimatedPlayer's update, bounding volumes and flocking parameters:
// AttemptUpdate is CAnimObject::AttemptUpdate (bone interpolation, yaw and
// position, world volume and scene extents widened by CONTACT_EPSILON) plus
// the current weapon's muzzle flash and a flag cleared for weapon types 33
// and 34 above 60 (both cleared while the player flag at bit 0x40 of +919 is
// set); BeginUpdate updates the light volumes, keeps the last position and
// yaw, takes the bone rotation and position from the second matrix, runs the
// animation and clears the velocity and a flag (CAnimObject::BeginUpdate
// without the falling physics); UpdateLocalBoundingVolume sets the type-3
// local box's corners (as CSoldierObject's does); the world and local volumes
// are chosen by volume type (types 0 to 2 share one volume, type 3 has its
// own, others have none), and the flocking parameters are a float and a long
// stored together. The classes, CONTACT_EPSILON and the called functions are
// named by the symbols; the members are inferred from offsets (shared with
// the CAnimObject view in anim_obj_update.cpp and the CWeapon view in
// weapon_head.cpp), the vector, matrix, volume and weapon views are
// inferred, and the virtual functions other than those used are placeholders
// in __vt__15CAnimatedPlayer's order (its virtual table is emitted with the
// rest of the file).
class IVolume;
class CMatrix;

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix(const CMatrix& other) { *this = other; }
    CMatrix& operator=(const CMatrix&);
    void SetPos(CVector3);
    void Translate(CVector3);
    void BuildRotZ(float);
    void Multiply(const CMatrix&, const CMatrix&);
    CVector3 GetPos() const { return position; }

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);
    void Slerp(CQuaternion, CQuaternion, float);
    void GetMatrix(CMatrix&) const;

    float w;
    float x;
    float y;
    float z;
};

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual void vf14();
    virtual void vf18();
    virtual void vf1c();
    virtual void vf20();
    virtual void vf24();
    virtual void vf28();
    virtual void vf2c();
    virtual void vf30();
    virtual void vf34();
    virtual void vf38();
    virtual void vf3c();
    virtual void vf40();
    virtual void vf44();
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CVolSphere {
public:
    void SetCenter(CVector3);
};

class CAnimated {
public:
    virtual ~CAnimated();
    virtual void UpdateAnimation(float);
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void SetRotation(long);
};

class CAnimSkinned : public CAnimated {
public:
    unsigned char unknown04[44];
};

/* inferred: a node of the scene's extent lists, its value at +8 */
struct ExtentNodeView {
    unsigned char unknown00[8];
    float m_value;
};

struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

extern const float CONTACT_EPSILON;

class CWeapon {
public:
    unsigned char GetMuzzleFlashStatus();

    unsigned char unknown000[480];
    unsigned char m_flag1e0 : 1;
    unsigned char unknown1e0 : 7;
    unsigned char unknown1e1[167];
    float m_value288;
    unsigned char unknown28c[12];
    int m_type;
    unsigned char unknown29c[36];
    unsigned char unknown2c0a : 5;
    unsigned char muzzleFlash : 1;
    unsigned char unknown2c0b : 2;
};

class CPlayerObject {
public:
    CWeapon* GetCurrentWeapon() const;

    unsigned char unknown000[919];
    unsigned char m_flag397a : 1;
    bool m_flag397b : 1;
    unsigned char m_flags397 : 6;
};

class CLightVolumeManager {
public:
    void Update(float);

    unsigned char unknown00[4];
};

class CVolBox {
public:
    void SetCorners(CVector3, CVector3);
};

class ISceneNode {
public:
    enum EVolumeType {};
};

class CAnimatedPlayer {
public:
    virtual void vf008();
    virtual void vf00c();
    virtual void vf010();
    virtual void BeginUpdate(float);
    virtual void vf018();
    virtual void vf01c();
    virtual void vf020();
    virtual void AttemptUpdate(float);
    virtual void vf028();
    virtual void vf02c();
    virtual void vf030();
    virtual void vf034();
    virtual void vf038();
    virtual void vf03c();
    virtual void GetPosition(CVector3&) const;
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
    virtual void vf114();
    virtual void vf118();
    virtual void vf11c();
    virtual void vf120();
    virtual void vf124();
    virtual void UpdateAnimation(float);

    void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    const IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    const IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    void GetFlocoParams(float&, long&);
    void SetFlocoParams(float, long);

    unsigned char unknown0004[8];
    ExtentNodeView* m_extentMinX;
    ExtentNodeView* m_extentMaxX;
    ExtentNodeView* m_extentMinY;
    ExtentNodeView* m_extentMaxY;
    unsigned char unknown001c[20];
    CAnimSkinned m_skinned;
    unsigned char unknown0060[3808];
    CMatrix m_matrices[80];
    CMatrix m_tm;
    IVolume* m_localVolume3;
    IVolume* m_localVolume;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume;
    BSObject* m_script;
    unsigned char unknown2394[4];
    CVolSphere* m_sphere;
    unsigned char unknown239c[20];
    long m_yaw;
    unsigned char unknown23b4[4];
    CVector3 m_lastPosition;
    CVector3 m_velocity;
    unsigned char unknown23d8[20];
    long m_lastYaw;
    float m_turn;
    unsigned char unknown23f4[4];
    CVector3 m_bonePosition;
    CQuaternion m_boneRotation;
    CVector3 m_nextBonePosition;
    CQuaternion m_nextBoneRotation;
    unsigned char unknown2438[8];
    int m_value2440;
    unsigned char unknown2444[12];
    bool m_flag2450;
    unsigned char unknown2451[3];
    bool m_onGround;
    bool m_falling;
    unsigned char unknown2456[4];
    bool m_frozen;
    bool m_useFixedTM;
    unsigned char unknown245c[4];
    CMatrix m_fixedTM;
    unsigned char unknown24a0[24];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown24bc[6996];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
    unsigned char unknown4078[1080];
    CPlayerObject* m_player;
    float m_flocoParam;
    long m_flocoCount;
};

void CAnimatedPlayer::AttemptUpdate(float dt) {
    CVector3 move;
    move.x = dt * m_velocity.x;
    move.y = dt * m_velocity.y;
    move.z = dt * m_velocity.z;
    float turn = m_turn * dt;
    CMatrix bone(m_matrices[1]);
    CQuaternion rotation;
    rotation.SetFromMatrix(bone);
    CVector3 position = bone.position;
    float pz = m_bonePosition.z;
    float py = m_bonePosition.y;
    float px = m_bonePosition.x;
    m_nextBonePosition.x = px + dt * (position.x - px);
    m_nextBonePosition.y = py + dt * (position.y - py);
    m_nextBonePosition.z = pz + dt * (position.z - pz);
    m_nextBoneRotation.Slerp(rotation, m_boneRotation, dt);
    m_nextBoneRotation.GetMatrix(bone);
    bone.SetPos(m_nextBonePosition);
    if (m_tm.position.z < -250.0f)
        BSObjectTriggerEvent(m_script, 45, 0, 0, false);
    if (!m_frozen) {
        m_yaw = (m_lastYaw + ((long)(2670176.8f * turn) & 0xffffff)) & 0xffffff;
        m_tm.BuildRotZ(6.2831855f * (float)m_yaw / 16777216.0f);
        m_tm.SetPos(m_lastPosition);
        m_tm.Translate(move);
        m_skinned.SetRotation(m_yaw);
    } else if (m_useFixedTM) {
        m_tm = m_fixedTM;
    }
    if (m_localVolume) {
        bone.Multiply(bone, m_tm);
        m_sphere->SetCenter(CVector3(0.0f, 0.0f, 0.0f));
        m_worldVolume->TransformedCopy(*m_localVolume, bone);
        CVector3 min;
        CVector3 max;
        m_worldVolume->GetExtents(min, max);
        m_extentMinX->m_value = min.x - CONTACT_EPSILON;
        m_extentMinY->m_value = min.y - CONTACT_EPSILON;
        m_extentMaxX->m_value = CONTACT_EPSILON + max.x;
        m_extentMaxY->m_value = CONTACT_EPSILON + max.y;
    } else {
        CVector3 p;
        GetPosition(p);
        m_extentMinX->m_value = p.x - CONTACT_EPSILON;
        m_extentMinY->m_value = p.y - CONTACT_EPSILON;
        m_extentMaxX->m_value = CONTACT_EPSILON + p.x;
        m_extentMaxY->m_value = CONTACT_EPSILON + p.y;
    }
    m_onGround = false;
    m_value2440 = 0;
    m_flag2450 = false;
    CWeapon* weapon = m_player->GetCurrentWeapon();
    if (m_player->m_flag397b) {
        m_weapons[m_currentWeapon]->muzzleFlash = 0;
        m_weapons[m_currentWeapon]->m_flag1e0 = 0;
    } else {
        m_weapons[m_currentWeapon]->muzzleFlash = weapon->GetMuzzleFlashStatus();
        if (weapon->m_value288 > 60.0f &&
            (m_weapons[m_currentWeapon]->m_type == 33 || m_weapons[m_currentWeapon]->m_type == 34))
            m_weapons[m_currentWeapon]->m_flag1e0 = 0;
        else
            m_weapons[m_currentWeapon]->m_flag1e0 = 1;
    }
}

void CAnimatedPlayer::BeginUpdate(float dt) {
    m_lightVolumes.Update(dt);
    m_lastPosition = m_tm.GetPos();
    m_lastYaw = m_yaw;
    CMatrix bone(m_matrices[1]);
    m_boneRotation.SetFromMatrix(bone);
    m_bonePosition = bone.GetPos();
    UpdateAnimation(dt);
    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
    m_flag2450 = false;
}

void CAnimatedPlayer::UpdateLocalBoundingVolume(ISceneNode::EVolumeType type, const CVector3& min, const CVector3& max) {
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

const IVolume* CAnimatedPlayer::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_worldVolume3;
    case 0:
    case 1:
    case 2:
        return m_worldVolume;
    case 4:
        return 0;
    }
    return 0;
}

const IVolume* CAnimatedPlayer::GetLocalBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_localVolume3;
    case 0:
    case 1:
    case 2:
        return m_localVolume;
    case 4:
        return 0;
    }
    return 0;
}

void CAnimatedPlayer::GetFlocoParams(float& param, long& count) {
    param = m_flocoParam;
    count = m_flocoCount;
}

void CAnimatedPlayer::SetFlocoParams(float param, long count) {
    m_flocoParam = param;
    m_flocoCount = count;
}
