// A fragment of static_obj.cpp (0x800b1bd0): CStaticObject's world and local
// bounding-volume getters (the volume pointers kept for type 3 and for types
// 0 to 2, none for other types) and setters for types 3 and 0 (the world
// setter transforms a new volume by the object's transform unless it is the
// other world volume, and for type 0 calls two of the object's virtual
// functions). CStaticObject, ISceneNode, IVolume, CMatrix and EVolumeType are
// named by the mangled symbols; the members and the object's virtual slots
// (+88, +92) are inferred, and IVolume's first slots are as in
// animated_volume_dispatch.cpp. The rest of the file is not part of this unit.
#include "game/CMatrix.h"


class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
};

class ISceneNode {
public:
    enum EVolumeType {};
};

class CStaticObject {
public:
    IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    void SetWorldBoundingVolume(ISceneNode::EVolumeType, IVolume*);
    void SetLocalBoundingVolume(ISceneNode::EVolumeType, IVolume*);

    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual void unknown34();
    virtual void unknown38();
    virtual void unknown3c();
    virtual void unknown40();
    virtual void unknown44();
    virtual void unknown48();
    virtual void unknown4c();
    virtual void unknown50();
    virtual void unknown54();
    virtual int unknown58();
    virtual void unknown5c(int);

    unsigned char unknown004[60];
    CMatrix m_tm;
    unsigned char unknown080[136];
    IVolume* m_localVolume3;
    IVolume* m_localVolume;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume;
};

IVolume* CStaticObject::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
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

IVolume* CStaticObject::GetLocalBoundingVolume(ISceneNode::EVolumeType type) const {
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

void CStaticObject::SetWorldBoundingVolume(ISceneNode::EVolumeType type, IVolume* volume) {
    switch (type) {
    case 3:
        if (volume != m_worldVolume)
            volume->TransformedCopy(*volume, m_tm);
        m_worldVolume3 = volume;
        break;
    case 0:
        if (volume != m_worldVolume3)
            volume->TransformedCopy(*volume, m_tm);
        m_worldVolume = volume;
        if (unknown58() == -1)
            unknown5c(0);
        break;
    case 4:
        break;
    }
}

void CStaticObject::SetLocalBoundingVolume(ISceneNode::EVolumeType type, IVolume* volume) {
    switch (type) {
    case 3:
        m_localVolume3 = volume;
        break;
    case 0:
        m_localVolume = volume;
        break;
    case 4:
        break;
    }
}
