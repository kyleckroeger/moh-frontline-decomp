// A fragment of AIFilter.cpp (0x8005d470): CAIFilterGlobal's
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
// manager and recent_sounds are inferred views (members at their offsets,
// names not original), the spline lookup is an inferred inline helper, and
// the result types are inferred. recent_sounds is
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
// core record it points to (type +0x0c, id +0x38, class CRC +0x48).
struct MOH_core_Struct {
    unsigned char unknown00[12];
    int field0c;
    unsigned char unknown10[40];
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

class CCoverPoint {
public:
    void Invalidate();
};

class CAIFilterGlobal {
public:
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
