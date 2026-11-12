// A fragment of AIFilter.cpp (0x8005c9c8): CAIFilterObject::FindRunAwayPoint
// searches the A* path nodes greedily away from a scene node's position,
// starting at the AI object's position, and stores the found node's
// position. The classes and functions are named by the symbols; the scene
// node's GetPosition slot follows the ISceneNode views
// (bsbifunc_soundbank.cpp); the position's inline assignment (its copy
// order from the code), the path node list at +16 of the global filter and the AI object's
// position (+24 of the object at +8) are inferred, as are the result types
// (found or not; the node's position first). The rest of the file is in
// other units.
#include "game/CVector3.h"

class CMatrix;
class CDrawContext;
class CCollision;

#include "game/ISceneNode.h"


class CAIFilterRealPosition {
public:
    CAIFilterRealPosition& operator=(const CAIFilterRealPosition& other) {
        float z_ = other.z;
        float y_ = other.y;
        x = other.x;
        y = y_;
        z = z_;
        return *this;
    }

    float x;
    float y;
    float z;
};

/* inferred: a path node, its position first */
class CPathNode {
public:
    CAIFilterRealPosition m_position;
};

class CPathNodeList {
public:
    CPathNode* GreedySearchAwayFromPoint(const CAIFilterRealPosition&, const CAIFilterRealPosition&, float) const;
};

class CAIFilterGlobal {
public:
    unsigned char unknown00[16];
    CPathNodeList* m_pathNodes;
    unsigned char unknown14[4];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

/* inferred: the AI object, its position at +24 */
struct AIObjectView {
    unsigned char unknown00[24];
    CAIFilterRealPosition m_position;
};

class CAIFilterObject {
public:
    bool FindRunAwayPoint(const ISceneNode*, float, CAIFilterRealPosition*);

    unsigned char unknown00[8];
    AIObjectView* m_object;
};

bool CAIFilterObject::FindRunAwayPoint(const ISceneNode* node, float distance, CAIFilterRealPosition* point) {
    AIObjectView* object = m_object;
    CAIFilterRealPosition away;
    CVector3 position;
    node->GetPosition(position);
    away.x = position.x;
    away.y = position.y;
    away.z = position.z;
    CPathNode* found = g_aigAIFilterGlobalObject.m_pathNodes->GreedySearchAwayFromPoint(object->m_position, away, distance);
    if (found) {
        *point = found->m_position;
        return true;
    }
    return false;
}
