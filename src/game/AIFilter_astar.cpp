// A fragment of AIFilter.cpp (0x8005d0e8): CAIFilterGlobal's
// GetClosestUsableCoverPoint walks the cover points (the leaf's own, then the
// level's cover-point waypoints; it starts from the level's first one) and
// returns the one whose free peek position is XY-closest to the position
// within the range, of no team or the AI's team, giving cover from the threat
// (when there is one) and, when a walk mode is asked for, reachable by
// CheckLineCollision (its mode and the peek type are returned). Then
// GetCoverPointIndex and GetCoverPointByIndex (a cover point is its trigger
// table entry, so these convert between the entry and its index; the result
// types are inferred), InvalidateCoverPointByID (invalidates every cover-point waypoint in the
// trigger table whose core id matches), then its A* and spline helpers. The
// path-node functions forward to the path-node list (created on demand when
// there are nodes); GetSplinePath finds the spline with an id
// (offset by 0x1000000 for the alternate set) in the spline manager's
// 12-byte entries; ResetSplinePathManager resets the search-node pool, clears
// the count in the file's recent_sounds record and resets the AI object
// list. The file name is this project's; the original record is AIFilter.cpp
// and CreateSplinePathManager after these is not reconstructed. The classes
// and globals are named by the mangled symbols; CAIFilterGlobal, the spline
// manager, the leaf and AI object views and recent_sounds are inferred views
// (members at their offsets, names not original), the cover-point walk and
// the spline lookup are inferred inline helpers, and the result types are
// inferred. recent_sounds is
// file-local in the original; it is declared extern so the fragment can refer
// to it.
class IVolume;
class CPathNode;
class CAIFilterRealPosition;
class CAIFilterRealVector3;
struct BPDPathFindingNode;
struct BPDPolyPath;

class CPathNodeList {
public:
    CPathNodeList(void*, BPDPathFindingNode*, int);
    void DisableCollidingNodes(const IVolume*);
    void EnableAll();
    CPathNode* GetClosestNode(const CAIFilterRealPosition&, const CAIFilterRealVector3&) const;
    CPathNode* GetPathNodeByID(int) const;

    unsigned char unknown00[8];
};

class CSearchNode {
public:
    static void Reset();
};

class CAIObject {
public:
    static void ResetGlobalList();
};

struct SplinePathEntryView {
    unsigned char unknown00[8];
    unsigned long id;
};

struct SplinePathManagerView {
    SplinePathEntryView* Find(unsigned long id) {
        unsigned long i;
        for (i = 0; i < count; i++) {
            if (id == paths[i].id)
                break;
        }
        return &paths[i];
    }

    unsigned long count;
    SplinePathEntryView* paths;
};

struct RecentSoundsView {
    unsigned char unknown00[8];
    int count;
    unsigned char unknown0c[12];
};

extern RecentSoundsView recent_sounds;

// Inferred views (named in propdat_bsp.cpp): a trigger table entry and the
// core record it points to (type +0x0c, team +0x35, id +0x38, class CRC +0x48).
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

enum COVERPOINT_WALK_MODE {};
enum COVERPOINT_TYPE {};

class CAIFilterRealPositionView {
public:
    float x;
    float y;
    float z;
};

class CCoverPoint {
public:
    void Invalidate();
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
    CCoverPoint* GetClosestUsableCoverPoint(const _PropBSPLeaf*, const CAIFilterRealPosition&,
                                            const CAIFilterRealPosition*, COVERPOINT_WALK_MODE*, COVERPOINT_TYPE*,
                                            CAIFilterRealPosition*, float, CAIObject*) const;
    int GetCoverPointIndex(CCoverPoint*) const;
    CCoverPoint* GetCoverPointByIndex(int) const;
    void InvalidateCoverPointByID(int);
    void DisableCollidingAStarPathNodes(const IVolume*);
    void EnableAllAStarPathNodes();
    CPathNode* GetClosestAStarPathNode(const CAIFilterRealPosition&, const CAIFilterRealVector3&) const;
    CPathNode* GetAStarPathNodeByIndex(int) const;
    void CreateAStarPathNodeList(void*, BPDPathFindingNode*, int);
    SplinePathEntryView* GetSplinePath(unsigned long, bool) const;
    void ResetSplinePathManager(void*, BPDPolyPath*, int);

    unsigned char unknown00[12];
    SplinePathManagerView* m_splines;
    CPathNodeList* m_pathNodes;
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

// Inferred inline: the same search from the entry after a given one.
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

// Inferred inline: advances the walk (the index starts at -1).
static inline CCoverPoint* NextCoverPoint(const _PropBSPLeaf* leaf, int& index, CCoverPoint* previous) {
    index++;
    return CoverPointAt(leaf, index, previous);
}


CCoverPoint* CAIFilterGlobal::GetClosestUsableCoverPoint(const _PropBSPLeaf* leaf, const CAIFilterRealPosition& from,
                                                         const CAIFilterRealPosition* threat, COVERPOINT_WALK_MODE* walkMode,
                                                         COVERPOINT_TYPE* type, CAIFilterRealPosition* walkPosition,
                                                         float range, CAIObject* ai) const {
    float distance;
    float closest;
    float dy;
    float dx;
    unsigned char team = ai ? ((AIObjectView*)ai)->team : 0;
    int index = -1;
    CCoverPoint* point = 0;
    point = NextCoverPoint(leaf, index, point);
    point = FindCoverPoint(g_pTriggerObjects); // the original discards the walk's first entry too
    closest = range;
    CCoverPoint* best = 0;
    for (; point; point = NextCoverPoint(leaf, index, point)) {
        COVERPOINT_TYPE peek = point->GetAvailablePeekPosition();
        unsigned char owner = ((TriggerObject_struct*)point)->core->team;
        if (peek && (owner == 0 || owner == team)) {
            CAIFilterRealPositionView position;
            point->GetModifiedPosition((CAIFilterRealVector3&)position, peek);
            const CAIFilterRealPositionView& origin = (const CAIFilterRealPositionView&)from;
            dx = position.x - origin.x;
            dy = position.y - origin.y;
            distance = dx * dx + dy * dy;
            if (distance < closest) {
                bool covered = threat ? point->ProvidesCover((const CAIFilterRealPosition&)position, threat, 0) : true;
                if (covered) {
                    if (walkMode) {
                        COVERPOINT_WALK_MODE mode = point->CheckLineCollision((const CAIFilterRealPosition&)position, from, walkPosition);
                        if (mode) {
                            *walkMode = mode;
                            closest = distance;
                            best = point;
                            *type = peek;
                        }
                    } else {
                        closest = distance;
                        best = point;
                    }
                }
            }
        }
    }
    return best;
}

int CAIFilterGlobal::GetCoverPointIndex(CCoverPoint* point) const {
    return (TriggerObject_struct*)point - g_pTriggerObjects;
}

CCoverPoint* CAIFilterGlobal::GetCoverPointByIndex(int index) const {
    TriggerObject_struct* point = g_pTriggerObjects;
    point += index;
    return (CCoverPoint*)point;
}

void CAIFilterGlobal::InvalidateCoverPointByID(int id) {
    for (CCoverPoint* point = FindCoverPoint(g_pTriggerObjects); point;
         point = FindCoverPoint((TriggerObject_struct*)point + 1)) {
        if (((TriggerObject_struct*)point)->core->field38 == id)
            point->Invalidate();
    }
}

void CAIFilterGlobal::DisableCollidingAStarPathNodes(const IVolume* volume) {
    m_pathNodes->DisableCollidingNodes(volume);
}

void CAIFilterGlobal::EnableAllAStarPathNodes() {
    m_pathNodes->EnableAll();
}

CPathNode* CAIFilterGlobal::GetClosestAStarPathNode(const CAIFilterRealPosition& position,
                                                     const CAIFilterRealVector3& direction) const {
    return m_pathNodes->GetClosestNode(position, direction);
}

CPathNode* CAIFilterGlobal::GetAStarPathNodeByIndex(int index) const {
    CPathNode* node = 0;
    if (m_pathNodes)
        node = m_pathNodes->GetPathNodeByID(index);
    return node;
}

void CAIFilterGlobal::CreateAStarPathNodeList(void* memory, BPDPathFindingNode* nodes, int count) {
    if (count)
        m_pathNodes = new CPathNodeList(memory, nodes, count);
}

SplinePathEntryView* CAIFilterGlobal::GetSplinePath(unsigned long id, bool alternate) const {
    if (alternate)
        id += 0x1000000;
    return m_splines->Find(id);
}

void CAIFilterGlobal::ResetSplinePathManager(void*, BPDPolyPath*, int) {
    CSearchNode::Reset();
    recent_sounds.count = 0;
    CAIObject::ResetGlobalList();
}
