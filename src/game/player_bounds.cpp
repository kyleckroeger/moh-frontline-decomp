// A fragment of player.cpp (0x800a50f8): CPlayerObject's world and local
// bounding-volume getters: a volume pointer for type 3, an embedded volume
// for types 0 to 2, none for other types. CPlayerObject, ISceneNode, IVolume
// and EVolumeType are named by the mangled symbols; the members are inferred
// and CPlayerObject is a non-virtual view. Draw after these and the rest of
// the file are not part of this unit.
class IVolume;

#include "game/ISceneNode.h"


/* inferred: storage of an embedded volume object (its class is not known) */
struct EmbeddedVolumeView {
    unsigned char data[80];
};

#include "game/CPlayerObject.h"


IVolume* CPlayerObject::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_worldVolume3;
    case 0:
    case 1:
    case 2:
        return (IVolume*)&m_worldCapsule;
    case 4:
        return 0;
    }
    return 0;
}

IVolume* CPlayerObject::GetLocalBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_localVolume3;
    case 0:
    case 1:
    case 2:
        return (IVolume*)&m_localCapsule;
    case 4:
        return 0;
    }
    return 0;
}
