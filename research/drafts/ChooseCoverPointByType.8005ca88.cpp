class CAIFilterRealPosition;
class CAIFilterRealVector3;
class CAIObject;
enum COVERPOINT_WALK_MODE {};
enum COVERPOINT_TYPE {};
enum ECoverPointSelectionType {};
#include <math.h>

struct MOH_core_Struct {
    unsigned char unknown00[12];
    int field0c;
    unsigned char unknown10[37];
    unsigned char team;
    unsigned char unknown36[2];
    short field38;
    unsigned char unknown3a[14];
    unsigned long field48;
};

struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
    void* bsObject;
};

extern unsigned int g_pNumTriggers;
extern TriggerObject_struct g_pTriggerObjects[2000];

class CAIFilterRealPositionView {
public:
    float x;
    float y;
    float z;
};

class CCoverPoint {
public:
    COVERPOINT_TYPE GetAvailablePeekPosition() const;
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;
    bool ProvidesCover(const CAIFilterRealPosition&, const CAIFilterRealPosition*, const CAIObject*) const;
    COVERPOINT_WALK_MODE CheckLineCollision(const CAIFilterRealPosition&, const CAIFilterRealPosition&, CAIFilterRealPosition*) const;
};

struct _PropBSPLeaf {
    unsigned char unknown00[6];
    unsigned short coverCount;
    unsigned char unknown08[32];
    unsigned short* covers;
};

struct AIObjectView {
    unsigned char unknown00[20];
    unsigned char team;
};

class CAIFilterGlobal {
public:
    CCoverPoint* ChooseCoverPointByType(const _PropBSPLeaf*, const CAIFilterRealPosition&, const CAIFilterRealPosition*,
                                        ECoverPointSelectionType, float, float, CAIObject*) const;
    CCoverPoint* GetClosestUsableCoverPoint(const _PropBSPLeaf*, const CAIFilterRealPosition&,
                                            const CAIFilterRealPosition*, COVERPOINT_WALK_MODE*, COVERPOINT_TYPE*,
                                            CAIFilterRealPosition*, float, CAIObject*) const;

    unsigned char unknown00[24];
};

// Inferred inline: the next cover-point waypoint (type 5, CRC 0x9101c906)
// from a trigger entry on; the cover point is the trigger entry itself.
static inline CCoverPoint* FindCoverPoint(TriggerObject_struct* trigger) {
    TriggerObject_struct* end = g_pTriggerObjects + g_pNumTriggers;
    for (; trigger < end; trigger++) {
        if (trigger->core->field0c == 5 && trigger->core->field48 == 0x9101c906)
            return (CCoverPoint*)trigger;
    }
    return 0;
}

static inline CCoverPoint* FindNextCoverPoint(TriggerObject_struct* trigger) {
    TriggerObject_struct* end = g_pTriggerObjects + g_pNumTriggers;
    for (trigger++; trigger < end; trigger++) {
        if (trigger->core->field0c == 5 && trigger->core->field48 == 0x9101c906)
            return (CCoverPoint*)trigger;
    }
    return 0;
}

// Inferred inline: the cover point at a position of the walk through a leaf:
// its own cover points first, then the level's cover points from the start.
static inline CCoverPoint* CoverPointAt(const _PropBSPLeaf* leaf, int& index, CCoverPoint* previous) {
    if (index < leaf->coverCount)
        return (CCoverPoint*)&g_pTriggerObjects[leaf->covers[index]];
    if (index == leaf->coverCount)
        return FindCoverPoint(g_pTriggerObjects);
    return FindNextCoverPoint((TriggerObject_struct*)previous);
}


static inline CCoverPoint* NextCoverPoint(const _PropBSPLeaf* leaf, int& index, CCoverPoint* previous) {
    index++;
    return CoverPointAt(leaf, index, previous);
}

static inline bool IsBesideAndCloser(float dot, float distance, float toThreat, float threatDistance) {
    bool result;
    unsigned char inside;
    inside = result = false;
    if (fabs(dot) < 1.69 && distance < toThreat)
        inside = true;
    if (inside && threatDistance < distance)
        result = true;
    return result;
}

static inline bool IsBesideAndNearer(float dot, float distance, float toThreat, float threatDistance) {
    bool result;
    unsigned char inside;
    inside = result = false;
    if (fabs(dot) < 1.69 && distance < toThreat)
        inside = true;
    if (inside && distance < threatDistance)
        result = true;
    return result;
}

static inline float DistanceSquared(const CAIFilterRealPositionView& a, const CAIFilterRealPositionView& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

class DirectionView {
public:
    DirectionView(const CAIFilterRealPositionView& from, const CAIFilterRealPositionView& to)
        : x(to.x - from.x), y(to.y - from.y), z(to.z - from.z) {}
    void Normalize() {
        float length = sqrtf(x * x + y * y + z * z);
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    float x;
    float y;
    float z;
};

CCoverPoint* CAIFilterGlobal::ChooseCoverPointByType(const _PropBSPLeaf* leaf, const CAIFilterRealPosition& from,
                                                     const CAIFilterRealPosition* threat, ECoverPointSelectionType selection,
                                                     float range, float threatRange, CAIObject* ai) const {
    unsigned char team = ai ? ((AIObjectView*)ai)->team : 0;
    const CAIFilterRealPositionView& origin = (const CAIFilterRealPositionView&)from;
    const CAIFilterRealPositionView& enemy = *(const CAIFilterRealPositionView*)threat;
    float threatDistance;
    float distance;
    float dx;
    float dy;
    float dz;
    float closest;
    dy = enemy.y + -origin.y;
    dx = enemy.x + -origin.x;
    dz = enemy.z + -origin.z;
    threatDistance = dx * dx + dy * dy + dz * dz;
    if (threatDistance < 36.0f)
        return 0;
    float length = sqrtf(threatDistance);
    if (length != 0.0f) {
        float inverse = 1.0f / length;
        dx *= inverse;
        dy *= inverse;
        dz *= inverse;
    }
    int index = -1;
    CCoverPoint* point = NextCoverPoint(leaf, index, 0);
    closest = range;
    CCoverPoint* best = 0;
    for (; point; point = NextCoverPoint(leaf, index, point)) {
        COVERPOINT_TYPE peek = point->GetAvailablePeekPosition();
        unsigned char owner = ((TriggerObject_struct*)point)->core->team;
        if (peek && (owner == 0 || owner == team)) {
            CAIFilterRealPositionView position;
            point->GetModifiedPosition((CAIFilterRealVector3&)position, peek);
            float toThreat = DistanceSquared(enemy, position);
            if (toThreat < threatRange) {
                float fz = origin.z - position.z;
                float fx = origin.x - position.x;
                float fy = origin.y - position.y;
                distance = fx * fx + fy * fy + fz * fz;
                if (distance < closest && distance < toThreat) {
                    DirectionView direction(origin, position);
                    direction.Normalize();
                    float dot = dx * direction.x + dy * direction.y + dz * direction.z;
                    bool ok;
                    switch (selection) {
                    case 0:
                        ok = false;
                        if (dot > 0.38f && toThreat < distance && distance < threatDistance)
                            ok = true;
                        break;
                    case 1:
                        ok = false;
                        if (dot > 0.38f && distance < toThreat && distance < threatDistance)
                            ok = true;
                        break;
                    case 2:
                        ok = IsBesideAndCloser(dot, distance, toThreat, threatDistance);
                        break;
                    case 3:
                        ok = IsBesideAndNearer(dot, distance, toThreat, threatDistance);
                        break;
                    case 5:
                        ok = false;
                        if (dot < -0.38f && distance < toThreat && distance < threatDistance)
                            ok = true;
                        break;
                    case 4:
                        ok = false;
                        if (dot < -0.38f && distance < toThreat && threatDistance < distance)
                            ok = true;
                        break;
                    case 6:
                        ok = true;
                        break;
                    default:
                        ok = false;
                        break;
                    }
                    if (ok) {
                        bool covered = threat ? point->ProvidesCover((const CAIFilterRealPosition&)position, threat, ai) : true;
                        if (covered) {
                            closest = distance;
                            best = point;
                        }
                    }
                }
            }
        }
    }
    return best;
}
