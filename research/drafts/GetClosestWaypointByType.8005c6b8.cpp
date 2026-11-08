class CAIFilterRealPosition {
public:
    float x;
    float y;
    float z;
};

struct MOH_core_Struct {
    unsigned char unknown00[12];
    int type;
    CAIFilterRealPosition position;
    unsigned char unknown1c[44];
    int crc;
};

struct BSObject;

struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
    BSObject* bsObject;
};

extern TriggerObject_struct g_pTriggerObjects[2000];
extern unsigned int g_pNumTriggers;

struct _PropBSPLeaf {
    unsigned char unknown00[4];
    unsigned short triggerCount;
    unsigned char unknown06[30];
    unsigned short* triggers;
};

enum DWI_OBJECT_CRC_ENUM {};

bool MathFunCloseToZero(float, float);

/* inferred: the next waypoint of a type, from the leaf's trigger list and
   then, when asked, from every type-5 trigger */
inline TriggerObject_struct* FindWaypoint(const _PropBSPLeaf* leaf, int& index, TriggerObject_struct* current,
                                          DWI_OBJECT_CRC_ENUM type, bool all) {
    for (; index < leaf->triggerCount; index++) {
        TriggerObject_struct* trigger = &g_pTriggerObjects[leaf->triggers[index]];
        if (trigger->core->crc == type)
            return trigger;
    }
    if (index == leaf->triggerCount) {
        if (all) {
            for (TriggerObject_struct* trigger = g_pTriggerObjects; trigger < g_pTriggerObjects + g_pNumTriggers;
                 trigger++) {
                if (trigger->core->type == 5 && trigger->core->crc == type)
                    return trigger;
            }
        }
        return 0;
    }
    TriggerObject_struct* trigger = current;
    for (trigger++; trigger < g_pTriggerObjects + g_pNumTriggers; trigger++) {
        if (trigger->core->type == 5 && trigger->core->crc == type)
            return trigger;
    }
    return 0;
}

inline TriggerObject_struct* NextWaypoint(const _PropBSPLeaf* leaf, int& index, TriggerObject_struct* current,
                                          DWI_OBJECT_CRC_ENUM type, bool all) {
    index++;
    return FindWaypoint(leaf, index, current, type, all);
}

class CAIFilterGlobal {
public:
    TriggerObject_struct* GetClosestWaypointByType(const _PropBSPLeaf*, const CAIFilterRealPosition&,
                                                   DWI_OBJECT_CRC_ENUM, bool, float) const;
};

static inline TriggerObject_struct* Closest(float distance, bool all, DWI_OBJECT_CRC_ENUM type, const _PropBSPLeaf* leaf, const CAIFilterRealPosition& position) {
    int index = -1;
    TriggerObject_struct* waypoint = NextWaypoint(leaf, index, 0, type, all);
    float nearest = distance;
    TriggerObject_struct* closest = 0;
    while (waypoint) {
        CAIFilterRealPosition point = waypoint->core->position;
        if (MathFunCloseToZero(point.z - position.z, 2.0f)) {
            float dy = point.y - position.y;
            float dx = point.x - position.x;
            float squared = dx * dx + dy * dy;
            if (squared < nearest) {
                nearest = squared;
                closest = waypoint;
            }
        }
        waypoint = NextWaypoint(leaf, index, waypoint, type, all);
    }
    return closest;
}

TriggerObject_struct* CAIFilterGlobal::GetClosestWaypointByType(const _PropBSPLeaf* leaf,
                                                                const CAIFilterRealPosition& position,
                                                                DWI_OBJECT_CRC_ENUM type, bool all,
                                                                float distance) const {
    return Closest(distance, all, type, leaf, position);
}
