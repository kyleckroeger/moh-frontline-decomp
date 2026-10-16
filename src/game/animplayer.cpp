// CAnimatedPlayer's update start, bounding volumes and flocking parameters:
// BeginUpdate updates the light volumes, keeps the last position and yaw,
// takes the bone rotation and position from the second matrix, runs the
// animation and clears the velocity and a flag (CAnimObject::BeginUpdate
// without the falling physics); UpdateLocalBoundingVolume sets the type-3
// local box's corners (as CSoldierObject's does); the world and local volumes
// are chosen by volume type (types 0 to 2 share one volume, type 3 has its
// own, others have none), and the flocking parameters are a float and a long
// stored together. CAnimatedPlayer, ISceneNode, EVolumeType and the called
// functions are named by the mangled symbols; the members are inferred from
// offsets (shared with the CAnimObject view in anim_obj_update.cpp), the
// volumes are opaque IVolume pointers, the vector, matrix and box views are
// inferred, and the virtual functions before UpdateAnimation are placeholders
// in __vt__15CAnimatedPlayer's order (its virtual table is emitted with the
// rest of the file).
class IVolume;

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
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
    CVector3 GetPos() const { return position; }

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);

    float w;
    float x;
    float y;
    float z;
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

    unsigned char unknown0004[3900];
    CMatrix m_matrices[80];
    CMatrix m_tm;
    IVolume* m_localVolume3;
    IVolume* m_localVolume;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume;
    unsigned char unknown2390[32];
    long m_yaw;
    unsigned char unknown23b4[4];
    CVector3 m_lastPosition;
    CVector3 m_velocity;
    unsigned char unknown23d8[20];
    long m_lastYaw;
    unsigned char unknown23f0[8];
    CVector3 m_bonePosition;
    CQuaternion m_boneRotation;
    unsigned char unknown2418[56];
    bool m_flag2450;
    unsigned char unknown2451[103];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown24bc[8184];
    float m_flocoParam;
    long m_flocoCount;
};

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
