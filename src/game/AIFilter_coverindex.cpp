// A fragment of AIFilter.cpp (0x8005d470): CAIFilterGlobal::GetCoverPointIndex
// returns a cover point's index in the trigger table (a cover point is its
// trigger table entry). The class, function and table are named by the
// symbols; the trigger record view (12 bytes) and the result type are
// inferred. GetCoverPointByIndex after it is drafted in
// scratch/game/AIFilter_astar_getters_wip.cpp; the rest of the file is in
// other units.
struct MOH_core_Struct;

struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
    void* bsObject;
};

extern TriggerObject_struct g_pTriggerObjects[2000];

class CCoverPoint;

class CAIFilterGlobal {
public:
    int GetCoverPointIndex(CCoverPoint*) const;
};

int CAIFilterGlobal::GetCoverPointIndex(CCoverPoint* point) const {
    return (TriggerObject_struct*)point - g_pTriggerObjects;
}
