// CAnimatedPlayer's bounding-volume update and getters and flocking
// parameters: UpdateLocalBoundingVolume sets the type-3 local box's corners
// (as CSoldierObject's does; other types are ignored), the
// world and local volumes are chosen by volume type (types 0 to 2 share one
// volume, type 3 has its own, others have none), and the flocking parameters
// are a float and a long stored together. CAnimatedPlayer, ISceneNode and
// EVolumeType are named by the mangled symbols; the members are inferred
// from offsets, the volumes are opaque IVolume pointers, and the class is a
// non-virtual view (its virtual table is emitted with the rest of the file).
// The vector view (16 bytes, 8-byte aligned, copied as two doubles) and the
// box view are inferred as in Soldier_object_bounds.cpp.
class IVolume;

class CVector3 {
public:
    double pair[2];
} __attribute__((aligned(8)));

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
    void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    const IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    const IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    void GetFlocoParams(float&, long&);
    void SetFlocoParams(float, long);

    unsigned char unknown0000[9088];
    IVolume* m_localVolume3;
    IVolume* m_localVolume;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume;
    unsigned char unknown2390[8484];
    float m_flocoParam;
    long m_flocoCount;
};

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
