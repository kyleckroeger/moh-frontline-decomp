// A fragment of AIObject.cpp (0x8004b8fc): CAIObject::WalkAStarPath updates
// the A* path walk, then, when within the stopping distance (+732) of the
// walk's destination (+632; in 3D for walk mode 4, else in XY) less 0.5,
// marks the walk arrived (+652) and fires script event 152 on the object's
// script. CAIObject::UpdateAStarPathWalk refreshes the destination 0.4 above
// the followed object (+624) or, when the next check (+540) is due, above the
// arbitrary point (refreshed from the threat or +336 by walk mode, as in
// AIObject_patrolwalk.cpp), takes the unit direction from the destination to
// the raised position, and then walks the path nodes (+612 start, +616 end,
// +620 next): every 0.3 seconds it re-aims at the node closest to the
// destination, moves the move target (+80) to the next node (or the
// destination), and advances once the next node is within 0.25 (XY, squared)
// or within 4 and already passed; a move away from the target retries once
// and then gives up on the node (script event 193). Without a path (or with a
// lost next node) it rebuilds one from the closest nodes once a second
// (+648). The classes and functions are named by the symbols; the members
// follow AIObject_patrolwalk.cpp and are otherwise inferred from offsets, as
// are the path node's passable flag (top bit of byte 17), the position copy
// and move-target helpers (inline, copying z, y, x) and the start node's
// position copy (kept in memory, as the original's dead stores show). The
// rest of the file is in other units.
struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition() {}
    CAIFilterRealPosition(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    CAIFilterRealVector3() {}
    float w;
} __attribute__((aligned(8)));

class CSearchNode;

/* inferred: an A* path node, its position first and a flag in byte 17 */
class CPathNode {
public:
    CAIFilterRealPosition GetPosition() const { return CAIFilterRealPosition(m_position[0], m_position[1], m_position[2]); }

    float m_position[3];
    CSearchNode* m_pSearchNode;
    unsigned char unknown10;
    unsigned char m_passable : 1;
    unsigned char m_flags17 : 7;
};

/* inferred: a position copy through the three components (z, y, x) */
inline void Copy(CAIFilterRealPosition& d, const CAIFilterRealPosition& s) {
    float y;
    float z;
    float x;
    z = s.z;
    y = s.y;
    x = s.x;
    d.x = x;
    d.y = y;
    d.z = z;
}

class CAIFilterGlobal {
public:
    CPathNode* GetClosestAStarPathNode(const CAIFilterRealPosition&, const CAIFilterRealVector3&) const;

    unsigned char unknown00[4];
    float m_time;
    unsigned char unknown08[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

/* inferred: the AI's game object, its script at +12 */
struct AIGameObjectView {
    unsigned char unknown00[12];
    BSObject* m_script;
};

class CAIObject {
public:
    void WalkAStarPath();
    void UpdateAStarPathWalk();
    CPathNode* GetNextPathNode(CPathNode*, CPathNode*);
    /* inferred */
    void MoveTo(const CAIFilterRealPosition& p) {
        float y;
        float z;
        float x;
        z = p.z;
        y = p.y;
        x = p.x;
        m_moveTarget.x = x;
        m_moveTarget.y = y;
        m_moveTarget.z = z;
    }

    unsigned char unknown000[8];
    AIGameObjectView* m_object;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[44];
    CAIFilterRealPosition m_moveTarget;
    unsigned char unknown05c[104];
    CAIObject* m_threat;
    unsigned char unknown0c8[136];
    CAIFilterRealPosition m_unknown150;
    unsigned char unknown15c[192];
    float m_nextNodeCheck;
    float m_distanceSquared;
    bool m_closing;
    bool m_retried;
    unsigned char unknown226[62];
    CPathNode* m_startNode;
    CPathNode* m_endNode;
    CPathNode* m_nextNode;
    CAIObject* m_followObject;
    unsigned char unknown274[4];
    CAIFilterRealPosition m_walkDestination;
    unsigned char unknown284[4];
    float m_nextRepath;
    bool m_walkArrived;
    unsigned char unknown28d[59];
    CAIFilterRealPosition m_arbitraryPoint;
    unsigned char unknown2d4[4];
    int m_walkMode;
    float m_walkStopDistance;
    CAIFilterRealPosition* m_unknown2e0;
    unsigned char unknown2e4[1012];
    bool m_unknown6d8;
};

void CAIObject::WalkAStarPath() {
    UpdateAStarPathWalk();
    float distance;
    if (m_walkMode == 4)
        distance = m_position.GetDistanceXYZReal(m_walkDestination);
    else
        distance = m_position.GetDistanceXYReal(m_walkDestination);
    if (distance - m_walkStopDistance < 0.5f) {
        m_walkArrived = true;
        BSObjectTriggerEvent(m_object->m_script, 152, 0, 0, false);
    }
}

void CAIObject::UpdateAStarPathWalk() {
    CAIFilterRealPosition position;
    Copy(position, m_position);
    position.z += 0.4f;
    if (m_followObject) {
        float z = m_followObject->m_position.z;
        float y = m_followObject->m_position.y;
        float x = m_followObject->m_position.x;
        m_walkDestination.x = x;
        m_walkDestination.y = y;
        m_walkDestination.z = z;
        m_walkDestination.z += 0.4f;
    } else if (g_aigAIFilterGlobalObject.m_time > m_nextNodeCheck) {
        switch (m_walkMode) {
        case 0:
        case 1:
        case 2:
        default:
            if (m_threat && m_unknown6d8) {
                float tz = m_threat->m_position.z;
                float ty = m_threat->m_position.y;
                float tx = m_threat->m_position.x;
                m_arbitraryPoint.x = tx;
                m_arbitraryPoint.y = ty;
                m_arbitraryPoint.z = tz;
            } else {
                float tz = m_unknown150.z;
                float ty = m_unknown150.y;
                float tx = m_unknown150.x;
                m_arbitraryPoint.x = tx;
                m_arbitraryPoint.y = ty;
                m_arbitraryPoint.z = tz;
            }
            m_unknown2e0->x = m_arbitraryPoint.x;
            m_unknown2e0->y = m_arbitraryPoint.y;
            m_unknown2e0->z = m_arbitraryPoint.z;
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            break;
        }
        float z = m_arbitraryPoint.z;
        float y = m_arbitraryPoint.y;
        float x = m_arbitraryPoint.x;
        m_walkDestination.x = x;
        m_walkDestination.y = y;
        m_walkDestination.z = z;
        m_walkDestination.z += 0.4f;
    }
    CAIFilterRealVector3 direction;
    direction.x = position.x - m_walkDestination.x;
    direction.y = position.y - m_walkDestination.y;
    direction.z = position.z - m_walkDestination.z;
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (length != 0.0f) {
        direction.x *= 1.0f / length;
        direction.y *= 1.0f / length;
        direction.z *= 1.0f / length;
    }
    if (m_startNode && m_endNode && !m_walkArrived) {
        CAIFilterGlobal* global = &g_aigAIFilterGlobalObject;
        if (!(global->m_time < m_nextNodeCheck)) {
            CPathNode* node = global->GetClosestAStarPathNode(m_walkDestination, direction);
            if (!node)
                node = m_endNode;
            if (node != m_endNode && m_startNode != node)
                GetNextPathNode(m_startNode, node);
            else if (m_startNode == node)
                m_endNode = node;
            m_nextNodeCheck = 0.3f + global->m_time;
        }
        if (m_startNode == m_endNode) {
            MoveTo(m_walkDestination);
        } else {
            if (m_nextNode && !m_nextNode->m_passable)
                GetNextPathNode(m_startNode, 0);
            if (m_nextNode) {
                CPathNode* start = m_startNode;
                CAIFilterRealPosition from = start->GetPosition();
                MoveTo(m_nextNode->GetPosition());
                const CAIFilterRealPosition* f = &from;
                float sx = m_moveTarget.x - f->x;
                float sy = m_moveTarget.y - f->y;
                float sz = m_moveTarget.z - f->z;
                float px = m_position.x - m_moveTarget.x;
                float py = m_position.y - m_moveTarget.y;
                float pz = m_position.z - m_moveTarget.z;
                float distance = m_position.GetDistanceSquaredXYReal(m_moveTarget);
                if (distance <= 0.25f || (distance < 4.0f && px * sx + py * sy + pz * sz > 0.0f)) {
                    m_startNode = m_nextNode;
                    bool last = m_nextNode == m_endNode;
                    GetNextPathNode(m_startNode, m_endNode);
                    if (m_nextNode) {
                        MoveTo(m_nextNode->GetPosition());
                    } else {
                        m_walkArrived = true;
                        MoveTo(m_walkDestination);
                        if (!last)
                            BSObjectTriggerEvent(m_object->m_script, 193, 0, 0, true);
                    }
                    m_distanceSquared = m_position.GetDistanceSquaredXYReal(m_moveTarget);
                    m_closing = false;
                    m_retried = false;
                    return;
                }
                if (m_closing) {
                    if (distance > m_distanceSquared) {
                        if (!m_retried) {
                            m_startNode = m_nextNode;
                            GetNextPathNode(m_startNode, m_endNode);
                            if (m_nextNode) {
                                MoveTo(m_nextNode->GetPosition());
                            } else {
                                MoveTo(m_walkDestination);
                                BSObjectTriggerEvent(m_object->m_script, 193, 0, 0, true);
                            }
                            m_distanceSquared = m_position.GetDistanceSquaredXYReal(m_moveTarget);
                            m_closing = false;
                            m_retried = true;
                        } else {
                            MoveTo(m_walkDestination);
                            BSObjectTriggerEvent(m_object->m_script, 193, 0, 0, true);
                        }
                    }
                } else if (distance < m_distanceSquared) {
                    m_closing = true;
                }
                m_distanceSquared = distance;
            } else if (global->m_time >= m_nextRepath) {
                CPathNode* node = g_aigAIFilterGlobalObject.GetClosestAStarPathNode(m_walkDestination, direction);
                CAIFilterRealVector3 zero;
                zero.x = zero.y = zero.z = 0.0f;
                m_startNode = g_aigAIFilterGlobalObject.GetClosestAStarPathNode(position, zero);
                if (!m_startNode || !node) {
                    MoveTo(m_walkDestination);
                    BSObjectTriggerEvent(m_object->m_script, 193, 0, 0, true);
                } else {
                    m_endNode = 0;
                    GetNextPathNode(m_startNode, node);
                    if (m_nextNode) {
                        MoveTo(m_nextNode->GetPosition());
                    } else {
                        MoveTo(m_walkDestination);
                        BSObjectTriggerEvent(m_object->m_script, 193, 0, 0, true);
                    }
                    m_distanceSquared = m_position.GetDistanceSquaredXYReal(m_moveTarget);
                    m_closing = false;
                    m_retried = false;
                }
                m_nextRepath = 1.0f + global->m_time;
            }
        }
    } else if (!m_walkArrived && g_aigAIFilterGlobalObject.m_time >= m_nextRepath) {
        CAIFilterGlobal* global = &g_aigAIFilterGlobalObject;
        {
            CPathNode* node = global->GetClosestAStarPathNode(m_walkDestination, direction);
            CAIFilterRealVector3 zero;
            zero.x = zero.y = zero.z = 0.0f;
            m_startNode = g_aigAIFilterGlobalObject.GetClosestAStarPathNode(position, zero);
            if (!m_startNode || !node) {
                MoveTo(m_walkDestination);
            } else {
                m_endNode = 0;
                GetNextPathNode(m_startNode, node);
                if (m_nextNode) {
                    MoveTo(m_nextNode->GetPosition());
                } else {
                    MoveTo(m_walkDestination);
                    m_startNode = 0;
                    m_endNode = 0;
                }
                m_distanceSquared = m_position.GetDistanceSquaredXYReal(m_moveTarget);
                m_closing = false;
                m_retried = false;
            }
            m_nextRepath = 1.0f + global->m_time;
        }
    } else {
        MoveTo(m_walkDestination);
    }
}
