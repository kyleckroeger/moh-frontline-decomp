// A fragment of Soldier_object.cpp (0x800ac310): CSoldierObject's world and
// local bounding-volume getters: volume pointers for types 3 and 0, embedded
// volumes for types 1 and 2, none for other types; UpdateLocalBoundingVolume
// (type 3's volume box gets the new corners; the other types have nothing to
// update), CommitUpdate (CAnimObject's) and AttemptUpdate (CAnimObject's,
// then the type 1 and 2 world volumes become the local ones transformed by
// the soldier's matrix at +9024). CSoldierObject, ISceneNode, IVolume,
// CVolBox, CAnimObject and EVolumeType are named by the mangled symbols; the
// members are inferred, CSoldierObject is a non-virtual view, the CVector3
// view copies through a double pair as in player_weapon.cpp, and the empty
// cases reproduce the original's compare tree. The rest of the file is not
// part of this unit.
class CMatrix;

union CVector3Pair {
    double pair[2];
    float v[4];
};

/* inferred: CVector3 copies move it as a doubleword pair */
class CVector3 {
public:
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }

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

/* inferred: the soldier's matrix storage at +9024 */
class CMatrix {
public:
    float m[4][4];
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
    void CommitUpdate();
    void AttemptUpdate(float);
};

class CSoldierObject : public CAnimObject {
public:
    IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    void CommitUpdate();
    void AttemptUpdate(float);

    unsigned char unknown0000[9024];
    CMatrix m_matrix;
    IVolume* m_localVolume3;
    IVolume* m_localVolume0;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume0;
    unsigned char unknown2390[8080];
    EmbeddedVolumeView m_localVolume1;
    EmbeddedVolumeView m_worldVolume1;
    EmbeddedVolumeView m_localVolume2;
    EmbeddedVolumeView m_worldVolume2;
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
