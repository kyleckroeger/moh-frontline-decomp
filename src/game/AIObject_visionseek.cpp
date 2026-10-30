// A fragment of AIObject.cpp: 0x80048134..0x8004cd34 as one unit, the
// CAIObject vision, line-of-sight, field-of-view, cover-point and path-walk
// functions (the former AIObject_visionsetup, _lineofsight_obj,
// _lookdirection, _lineofsight_mode, _lineofsight, _fovcorpse, _obstacle,
// _arbitrary, _coverwalkto, _coveruseful, _coverwalk, _setcover,
// _storecover, _searchnodes, _walkastar and _patrolwalk fragments) with
// SetupAStarPathWalk and StartAStarPathWalk(CAIObject*), which reach their
// warnings from the start of the file's .rodata block. AIObject.cpp is
// compiled with deferred inlining (functions emitted in reverse source order)
// and RTTI, so that block holds the header's dwi_prime_list, the "CAIObject"
// type name (the class's table and type record come from its key function,
// the destructor, which AIObject_ctor places: here it is a file duplicate)
// and then the strings of these functions in code order. Each function keeps
// the description and the inferences of its former fragment:
//
// CheckVisionSeekingMode makes two tries at seeing the sought position
// (+1712) through random offsets (+1744, +1748) along +1728 and returns the
// sought object (+1692) when the line from 1.5 above the position is clear
// within the vision distance (+1760; 50 with a warning when not above 0.001);
// SetupVisionSeekingMode stores the sought object, position and distance and
// the flat, normalised, quarter-turned seeking direction (the raised position
// is kept in memory: it is read through a pointer). IsObjectInLineOfSight
// checks the vision distance and two vision lines from the filter object's
// look position (the weak, empty CAIFilterObject::GetLookDirection follows
// it) to the position raised by the standing or crouched heights; it is
// auto-inlined where the file calls it, which needs CAIFilterRealPosition
// and CAIFilterRealVector3 without constructors. The line-of-sight modes
// (watch list +1780/+1828, watched object +1692, eye position +688) use the
// inferred inline FindListObject, IsThreatInLineOfSight and an inline
// IsObjectInLineOfSight(CAIObject*) with the same body; the field-of-view
// modes, the corpse check (with the weak GetAIStatusVar and
// CAIStatus::GetVarValue, which inline the ComputeStatus functions placed by
// AIObject_threat and AIObject_enemy: file duplicates here; CanSee is an
// inferred wrapper) and obstacle avoidance (CObstacleAvoidance at +744). The
// arbitrary-point walk (the weak, empty CAIFilterObject::SetMoveTarget); the
// cover-point walks (cover point +656, type +660, position +664, the matrix
// row through inferred CMatrix inlines); SetCoverPoint, ChooseCoverPoint and
// StoreClosestCoverPoint; the A* search (ComputeAStarPath, GetNextPathNode,
// CleanupAStarPathWalk, AddChildrenToSearchNodeQueue; its catch (...)
// blocks have listed extab padding patches); WalkAStarPath and
// UpdateAStarPathWalk; the patrol walks; SetupAStarPathWalk (start and
// destination 0.4 above the position and the arbitrary point, the closest
// path nodes in the given directions, the path, the first node as move
// target, obstacle avoidance disengaged) and StartAStarPathWalk(CAIObject*)
// (the same toward another object, with the directions zero), each printing
// a warning when a node or the path is missing. The classes are views
// inferred from offsets and the code: one CAIObject view merges the members
// of the former fragments (names by offset where unknown), the move-target
// and position-copy helpers (MoveTo, Copy, the three-component overloads for
// path-node positions) are inferred, and the list head, file-local in the
// original, is declared extern. The rest of the file is in other units.
// Header-defined hash-table primes; every unit that includes the header carries a copy.
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

extern "C" {
#include <math.h>
int printf(const char*, ...);
void* memcpy(void*, const void*, unsigned long);
}

struct BSObject;
struct BS_STRUCT_Vector_struct;
struct _PropBSPLeaf;
class CAIObject;
class CSearchNode;
class CMatrix;
class BSGO_Basic;
struct ProximityDataView;

enum COVERPOINT_TYPE {};
enum ECoverPointSelectionType {};
enum COVERPOINT_WALK_MODE {};
enum aifilter_target_mode {};
enum EMovePointType {};
enum aistatus_var {};

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
void DebugMsg(const char*, ...);
float MathFunRandomRealSigned(float, float);
long long MathFunRandomI64(long long, long long);
bool MathFunCloseToZero(float, float);

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition& operator=(const CAIFilterRealPosition& other) {
        float x_, y_, z_;
        z_ = other.z;
        y_ = other.y;
        x_ = other.x;
        x = x_;
        y = y_;
        z = z_;
        return *this;
    }
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    CAIFilterRealVector3& operator=(const CAIFilterRealVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    void Normalize() {
        float length = sqrtf(x * x + y * y + z * z);
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    float Dot(const CAIFilterRealVector3& v) const { return x * v.x + y * v.y + z * v.z; }

    float w;
} __attribute__((aligned(8)));

struct aistatus_match {
    int value[3];
};

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual void ChooseTarget(float);
    virtual void SetTargetMode(aifilter_target_mode);
    virtual void ResetTargetMatchList(aifilter_target_mode);
    virtual void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    virtual void SetMoveTarget(const CAIFilterRealPosition&) {}
    virtual void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);

    unsigned char unknown04[8];
    BSObject* m_script;
    BSGO_Basic* m_user;
};

class CAIFilterGlobal {
public:
    static bool CheckVisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                             CAIFilterObject*);
    static bool CheckCollisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                                CAIFilterObject*);
    class CCoverPoint* GetCoverPointByIndex(int) const;
    class CCoverPoint* ChooseCoverPointByType(const _PropBSPLeaf*, const CAIFilterRealPosition&,
                                        const CAIFilterRealPosition*, ECoverPointSelectionType, float, float,
                                        CAIObject*) const;
    class CCoverPoint* GetClosestUsableCoverPoint(const _PropBSPLeaf*, const CAIFilterRealPosition&,
                                            const CAIFilterRealPosition*, COVERPOINT_WALK_MODE*, COVERPOINT_TYPE*,
                                            CAIFilterRealPosition*, float, CAIObject*) const;
    class CPathNode* GetAStarPathNodeByIndex(int) const;
    class CPathNode* GetClosestAStarPathNode(const CAIFilterRealPosition&, const CAIFilterRealVector3&) const;

    unsigned char unknown00[4];
    float m_time;
    unsigned char unknown08[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;
extern CAIObject* g_paioAIObjectListStart; // file-local in the original

class CLinkedListElement {
public:
    CLinkedListElement* m_next;
};

class CLinkedList {
public:
    bool IsEmpty();
    void RemoveElement(CLinkedListElement*);
    void AddElementAtEnd(CLinkedListElement*);
    void AddElementAtBeginning(CLinkedListElement*);

    CLinkedListElement* m_head;
};

class CPathNode;

class CSearchNode : public CLinkedListElement {
public:
    static void* operator new(unsigned long);
    static void operator delete(void*, unsigned long);
    CSearchNode();
    ~CSearchNode();
    CSearchNode* Setup(CSearchNode*, CPathNode*, const CPathNode*);
    float GetCostValue(const CSearchNode*);

    CSearchNode* m_parent;
    CPathNode* m_pathNode;
    float m_unknown0c;
    float m_unknown10;
    float m_cost;
    bool m_closed;

    /* inferred inline getter (the test reads like an inlined bool return) */
    bool IsClosed() const { return m_closed; }
};

class CPathNode {
public:
    CAIFilterRealPosition m_position;
    CSearchNode* m_searchNode;
    unsigned char m_childCount;
    unsigned char m_enabled : 1;
    unsigned char m_flags17 : 7;
    unsigned short* m_children;
};

class CSearchNodeQueue {
public:
    void Clear();
    CSearchNode* PopFirst();
    void ReplaceElement(CSearchNode*, CSearchNode*);
    void Insert(CSearchNode*);

    int m_count;
};

class CSearchNodeList : public CLinkedList {
public:
    ~CSearchNodeList();
};

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    CAIFilterRealVector3 GetRow(int i) const { return *(const CAIFilterRealVector3*)m[i]; }
    void GetFront(CAIFilterRealVector3& v) const { v = GetRow(1); }

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

class CVector3 {
public:
    float x;
    float y;
    float z;
};

/* ISceneNode's virtuals up to GetPosition, in table order */
class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const void*);
    virtual void CommitUpdate();
    virtual void Draw(void*);
    virtual int GetLocalBoundingVolume(int) const;
    virtual int GetWorldBoundingVolume(int) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
};

struct ProximityDataView {
    unsigned char unknown00[12];
    _PropBSPLeaf* leaf;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual void* GetSceneNode();
    virtual ProximityDataView* GetProximityData();
};

/* the cover point's object, oriented by the quaternion at +28 */
struct CoverPointObjectView {
    unsigned char unknown00[28];
    CQuaternion orientation;
};

class CCoverPoint {
public:
    void FlagAsUnoccupied(COVERPOINT_TYPE);
    bool FlagAsOccupied(COVERPOINT_TYPE);
    COVERPOINT_TYPE GetAvailablePeekPosition() const;
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;
    CAIFilterRealVector3 GetPeekLocation();
    bool ProvidesCover(const CAIFilterRealPosition&, const CAIFilterRealPosition*, const CAIObject*) const;

    unsigned char unknown00[4];
    CoverPointObjectView* m_object;
};

class CObstacleAvoidance {
public:
    void RunEnvironmentCollisionChecks();
    void DetermineNewObjectivePosition(CAIFilterRealPosition&);
    void PrepareForAvoidance(CAIFilterRealPosition&);
    ~CObstacleAvoidance();

    unsigned char unknown000[872];
    CAIFilterRealPosition m_unknown368;
    unsigned char unknown374[20];
};

class CAIStatus {
public:
    ~CAIStatus();
    bool MatchStatusList(aistatus_match*, int);
    int GetVarValue(aistatus_var) const;

    int m_values[14];
    static CAIObject* s_paioAttacker;
    static CAIObject* s_paioTarget;
};

/* inferred: a position set from three components (a path node's position) */
inline void Copy(CAIFilterRealPosition& d, float x, float y, float z) {
    d.x = x;
    d.y = y;
    d.z = z;
}

static inline CAIObject* GetListStart() { return g_paioAIObjectListStart; }

class CAISplinePathTraversal {
public:
    ~CAISplinePathTraversal();

    unsigned char unknown00[40];
};

class CAIObject {
public:
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);
    virtual bool IsCrouching() const; /* result type not known */
    virtual bool IsPlayer() const; /* result type not known */

    CAIObject* CheckVisionSeekingMode(); /* result type inferred */
    void SetupVisionSeekingMode(CAIObject*, CAIFilterRealPosition&, float);
    bool IsObjectInLineOfSight(CAIFilterRealPosition&, float, bool);
    bool IsObjectInLineOfSight(CAIObject*, float, bool);
    bool IsObjectInLineOfSightFromEye(CAIFilterRealPosition&, float, bool);
    bool IsTargetInLineOfSight(float);
    CAIObject* CheckLineOfSightModeList();
    CAIObject* CheckLineOfSightModeObject(); /* result type inferred */
    void SetupLineOfSightModeList(aistatus_match*, int, float);
    void SetupLineOfSightModeObject(CAIObject*, float);
    bool IsObjectInFieldOfView(CAIFilterRealPosition&, float, float, bool);
    CAIObject* CheckFieldOfViewCorpse(); /* result type inferred */
    int GetAIStatusVar(aistatus_var) const;
    int ComputeStatusIsEnemy(CAIObject*);
    int ComputeStatusLOSValue(CAIObject*);
    int ComputeStatusThreatValue(CAIObject*);
    int ComputeStatusExponentialDistance(CAIObject*);
    int ComputeStatusDistance(CAIObject*);
    int ComputeStatusVisibility(CAIObject*);
    CAIObject* CheckFieldOfViewModeList();
    CAIObject* CheckFieldOfViewModeObject(); /* result type inferred */
    void SetupFieldOfViewCorpse(float, float);
    void SetupFieldOfViewModeList(aistatus_match*, int, float, float);
    void SetupFieldOfViewModeObject(CAIObject*, float, float);
    void ProcessObstacleAvoidance();
    void EngageObstacleAvoidance(bool, float, float, float);
    void SetupArbitraryPointUpdate(EMovePointType, BS_STRUCT_Vector_struct*, float);
    void StartWalkToArbitraryPoint(CAIFilterRealPosition&);
    void WalkToArbitraryPoint();
    void CheckCoverpointUsefulness();
    void WalkToCoverPoint();
    void WalkToCoverPointDirect();
    void WalkToCoverPointIndirect();
    bool StartCoverPointWalk(bool);
    void StartCoverPointWalkToStored(bool);
    void CommitCoverPointWalkToStored();
    void SetCoverPoint(int, bool);
    CCoverPoint* StoreClosestCoverPoint();
    void ChooseCoverPoint(ECoverPointSelectionType, const ISceneNode*, float, float);
    CPathNode* GetNextPathNode(CPathNode*, CPathNode*);
    CSearchNode* AddChildrenToSearchNodeQueue(CSearchNode*, const CPathNode*, CSearchNodeQueue&, CSearchNodeList&);
    bool ComputeAStarPath(CPathNode*, const CPathNode*, float, CSearchNodeQueue&, CSearchNodeList&);
    void CleanupAStarPathWalk();
    void WalkAStarPath();
    void UpdateAStarPathWalk();
    bool SetupAStarPathWalk(const CAIFilterRealVector3&, const CAIFilterRealVector3&, bool);
    bool SetupAStarPathWalkForPatrol(const CAIFilterRealVector3&);
    bool StartAStarPathWalk();
    bool StartAStarPathWalk(CAIObject*);
    void RemoveFromGlobalList();

    /* inferred: the next object of the global list matching the watch list */
    CAIObject* FindListObject() {
        CAIObject* first = GetListStart();
        CAIObject* object;
        if (m_listObject)
            object = m_listObject;
        else
            object = first;
        for (;;) {
            object = object->m_next;
            if (!object)
                object = first;
            if (object != this && object->m_status.MatchStatusList(m_list, m_listCount)) {
                m_listObject = object;
                return m_listObject;
            }
            if (m_listObject) {
                if (object == m_listObject)
                    return 0;
            } else if (object == first) {
                return 0;
            }
        }
    }
    /* inferred: the next object of the global list matching the watch list */
    CAIObject* SearchList(CAIObject* object, CAIObject* first) {
        for (;;) {
            object = object->m_next;
            if (!object)
                object = first;
            if (object != this && object->m_status.MatchStatusList(m_list, m_listCount)) {
                m_listObject = object;
                return m_listObject;
            }
            if (m_listObject) {
                if (object == m_listObject)
                    return 0;
            } else if (object == first) {
                return 0;
            }
        }
    }
    bool IsThreatInLineOfSight(float distance) {
        CAIObject* threat = m_threat;
        float range = distance;
        if (threat->m_unknown730)
            range *= 0.5f;
        if (threat->IsCrouching())
            range *= 0.75f;
        return IsObjectInLineOfSight(threat, range, threat->IsCrouching());
    }
    // inferred wrapper (see the file comment)
    bool CanSee(CAIObject* other, float distance) { return IsObjectInLineOfSight(other->m_position, distance, other->IsCrouching()); }
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
    /* inferred: the move target set from three components (a path node's position) */
    void MoveTo(float x, float y, float z) {
        m_moveTarget.x = x;
        m_moveTarget.y = y;
        m_moveTarget.z = z;
    }

    CAIObject* m_next;
    CAIFilterObject* m_filter;
    unsigned char unknown00c[8];
    unsigned char m_team;
    unsigned char unknown015[3];
    CAIFilterRealVector3 m_position;
    unsigned char unknown028[40];
    CAIFilterRealVector3 m_moveTarget;
    unsigned char unknown060[32];
    CAIFilterRealVector3 m_facing;
    unsigned char unknown090[48];
    bool m_unknown0c0;
    unsigned char unknown0c1[3];
    CAIObject* m_threat;
    unsigned char unknown0c8[136];
    CAIFilterRealPosition m_unknown150;
    unsigned char unknown15c[76];
    unsigned long m_flags1a8;
    int m_unknown1ac;
    unsigned long m_flags;
    CAIStatus m_status;
    int m_unknown1ec;
    unsigned char unknown1f0[24];
    float m_sightDistance;
    unsigned char unknown20c[12];
    int m_moveMode;
    float m_nextNodeCheck;
    float m_distanceSquared;
    bool m_closing;
    bool m_retried;
    unsigned char unknown226[2];
    unsigned long m_state;
    unsigned char unknown22c[4];
    CAISplinePathTraversal m_spline;
    CSearchNodeList m_searchNodes;
    unsigned char unknown25c[8];
    CPathNode* m_startNode;
    CPathNode* m_endNode;
    CPathNode* m_nextNode;
    CAIObject* m_followObject;
    unsigned char unknown274[4];
    CAIFilterRealPosition m_walkDestination;
    unsigned char unknown284[4];
    float m_nextRepath;
    bool m_walkArrived;
    unsigned char unknown28d[3];
    CCoverPoint* m_coverPoint;
    COVERPOINT_TYPE m_coverType;
    CAIFilterRealVector3 m_coverPosition;
    CCoverPoint* m_chosenCoverPoint;
    bool m_checkCover;
    unsigned char unknown2ad[3];
    CAIFilterRealPosition m_peekLocation;
    unsigned char unknown2bc[4];
    bool m_atCoverPoint;
    unsigned char unknown2c1[3];
    float m_nextCoverCheck;
    CAIFilterRealVector3 m_arbitraryPoint;
    int m_arbitraryPointType;
    float m_walkStopDistance;
    CAIFilterRealPosition* m_unknown2e0;
    bool m_unknown2e4;
    unsigned char unknown2e5[3];
    CObstacleAvoidance m_obstacleAvoidance;
    CAIObject* m_unknown670;
    unsigned char unknown674[8];
    float m_unknown67c;
    unsigned char unknown680[8];
    CAIFilterRealPosition m_unknown688;
    unsigned char unknown694[4];
    CAIObject* m_listObject;
    CAIObject* m_seekObject;
    unsigned char unknown6a0[16];
    CAIFilterRealPosition m_seekPosition;
    unsigned char unknown6bc[4];
    CAIFilterRealPosition m_seekDirection;
    unsigned char unknown6cc[4];
    float m_seekSideways;
    float m_seekUp;
    bool m_unknown6d8;
    unsigned char unknown6d9[3];
    float m_listDistance;
    float m_visionDistance;
    float m_corpseDistance;
    float m_fovAngle;
    float m_listAngle;
    float m_corpseAngle;
    aistatus_match m_list[4];
    int m_listCount;
    unsigned char unknown728[8];
    bool m_unknown730;
    bool m_unknown731;
    unsigned char unknown732[2];
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

extern void* SPR_BEGIN;

static inline CCoverPoint* Select(CAIObject* self, const CAIFilterRealVector3& position,
                                  const CAIFilterRealPosition* from, int type, float distance, float range) {
    if (from)
        return g_aigAIFilterGlobalObject.ChooseCoverPointByType(
            self->m_filter->m_user->GetProximityData()->leaf, position, from, (ECoverPointSelectionType)type, distance,
            range, self);
    return g_aigAIFilterGlobalObject.GetClosestUsableCoverPoint(
        self->m_filter->m_user->GetProximityData()->leaf, position, 0, 0, 0, 0, distance, self);
}

static inline CAIObject* Me(CAIObject* p) { return p; }
static inline int TypeOf(ECoverPointSelectionType p) { return p; }

inline float PathCost(CSearchNode* node) {
    if (!node)
        return 0.0f;
    float cost = 0.0f;
    for (CSearchNode* next = (CSearchNode*)node->m_next; next; next = (CSearchNode*)next->m_next) {
        cost += next->GetCostValue(node);
        node = next;
    }
    return cost;
}

static inline CAIObject* Search2(CAIObject* self, CAIObject* first, CAIObject* object) {
    for (;;) {
        object = object->m_next;
        if (!object)
            object = first;
        if (object != self && object->m_status.MatchStatusList(self->m_list, self->m_listCount)) {
            self->m_listObject = object;
            return self->m_listObject;
        }
        if (self->m_listObject) {
            if (object == self->m_listObject)
                return 0;
        } else if (object == first) {
            return 0;
        }
    }
}

static inline bool AreEnemyTeams(unsigned char team, unsigned char other) {
    switch (team) {
    case 1:
        switch (other) {
        case 0:
        case 1:
            return false;
        case 2:
            return true;
        }
        break;
    case 2:
        switch (other) {
        case 0:
        case 2:
            return false;
        case 1:
            return true;
        }
        break;
    case 0:
        switch (other) {
        case 0:
            return false;
        case 1:
        case 2:
            return true;
        }
        break;
    }
    return false;
}

// CAIObject's key function: its destructor (placed by AIObject_ctor, compiled here as a file duplicate so that this
// part of the file also emits the class's virtual table and type information, as the original file did).
static inline void AdjustTargetedCount(CAIObject* object, int delta) {
    object->m_unknown1ec += delta;
    if (object->m_unknown1ec <= 15)
        object->m_status.m_values[8] = object->m_unknown1ec;
    else
        object->m_status.m_values[8] = 15;
}

CAIObject::~CAIObject() {
    m_unknown0c0 = false;
    if (m_threat) {
        m_unknown6d8 = false;
        CAIObject* old = m_threat;
        m_threat = 0;
        if (m_moveMode == 8 || m_moveMode == 2) {
            if (m_arbitraryPointType == 1 || m_arbitraryPointType == 2)
                m_arbitraryPointType = 3;
        }
        if (old)
            AdjustTargetedCount(old, -1);
    }
    RemoveFromGlobalList();
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
}

inline bool CAIObject::IsObjectInLineOfSight(CAIObject* other, float distance, bool low) {
    if ((m_flags & 8) == 8)
        return false;
    float range = distance;
    if (range <= 0.001f) {
        printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
        range = 50.0f;
    }
    if (!(m_position.GetDistanceXYZReal(other->m_position) <= range))
        return false;
    CAIFilterRealVector3 unused;
    CAIFilterRealVector3 eye;
    m_filter->GetLookDirection(unused, eye);
    float standing[2] = {0.8f, 1.2f};
    float crouching[2] = {0.8f, 0.1f};
    int count = 2;
    float* heights = standing;
    if (low) {
        count = 2;
        heights = crouching;
    }
    for (int i = 0; i < count; i++) {
        CAIFilterRealPosition target;
        float y;
        float z = other->m_position.z;
        y = other->m_position.y;
        float x = other->m_position.x;
        target.x = x;
        target.y = y;
        target.z = z;
        target.z += heights[i];
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(eye, target, m_filter))
            return true;
    }
    return false;
}

inline bool CAIObject::IsObjectInLineOfSightFromEye(CAIFilterRealPosition& position, float distance, bool low) {
    if ((m_flags & 8) == 8)
        return false;
    float range = distance;
    if (range <= 0.001f) {
        printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
        range = 50.0f;
    }
    if (!(m_position.GetDistanceXYZReal(position) <= range))
        return false;
    float standing[2] = {0.8f, 1.2f};
    float crouching[2] = {0.8f, 0.1f};
    int count = 2;
    float* heights = standing;
    if (low) {
        count = 2;
        heights = crouching;
    }
    for (int i = 0; i < count; i++) {
        CAIFilterRealVector3 target;
        float y;
        float z = position.z;
        y = position.y;
        float x = position.x;
        target.x = x;
        target.y = y;
        target.z = z;
        target.z += heights[i];
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(m_peekLocation, target, m_filter))
            return true;
    }
    return false;
}

bool CAIObject::StartAStarPathWalk(CAIObject* object) {
    m_moveMode = 0;
    m_startNode = 0;
    m_endNode = 0;
    m_nextNode = 0;
    MoveTo(m_arbitraryPoint);
    m_distanceSquared = m_position.GetDistanceSquaredXYZReal(m_moveTarget);
    m_closing = false;
    m_retried = false;
    m_moveMode = 0;
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
    m_followObject = 0;
    CAIFilterRealVector3 start(*(const CAIFilterRealVector3*)&m_position);
    start.z += 0.4f;
    CAIFilterRealVector3 end(*(const CAIFilterRealVector3*)&object->m_position);
    end.z += 0.4f;
    CAIFilterRealVector3 zero;
    zero.x = zero.y = zero.z = 0.0f;
    m_startNode = g_aigAIFilterGlobalObject.GetClosestAStarPathNode(start, zero);
    if (!m_startNode) {
        DebugMsg("StartAStarPathWalk: Could not find closest path node to start pos (%f,%f,%f), start dir (%f,%f,%f)\n",
                 start.x, start.y, start.z, zero.x, zero.y, zero.z);
        return false;
    }
    CAIFilterRealVector3 direction;
    direction.x = start.x - end.x;
    direction.y = start.y - end.y;
    direction.z = start.z - end.z;
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (length != 0.0f) {
        direction.x *= 1.0f / length;
        direction.y *= 1.0f / length;
        direction.z *= 1.0f / length;
    }
    CPathNode* endNode = g_aigAIFilterGlobalObject.GetClosestAStarPathNode(end, direction);
    if (!endNode) {
        DebugMsg("StartAStarPathWalk: Could not find closest path node to end pos (%f,%f,%f), end dir (%f,%f,%f)\n",
                 end.x, end.y, end.z, direction.x, direction.y, direction.z);
        return false;
    }
    m_endNode = 0;
    GetNextPathNode(m_startNode, endNode);
    if (!m_nextNode) {
        DebugMsg("StartAStartPathWalk: Could not compute AStar path from node (%f,%f,%f), to node (%f,%f,%f)\n",
                 m_startNode->m_position.x, m_startNode->m_position.y, m_startNode->m_position.z,
                 endNode->m_position.x, endNode->m_position.y, endNode->m_position.z);
        return false;
    }
    m_moveMode = 2;
    m_followObject = object;
    Copy(m_walkDestination, object->m_position);
    m_walkDestination.z += 0.4f;
    CAIFilterRealVector3 target;
    if (m_nextNode)
        Copy(target, m_nextNode->m_position.x, m_nextNode->m_position.y, m_nextNode->m_position.z);
    else
        Copy(target, m_walkDestination);
    m_distanceSquared = m_position.GetDistanceSquaredXYReal(target);
    m_closing = false;
    m_retried = false;
    EngageObstacleAvoidance(false, 0.0f, 0.0f, 0.0f);
    return true;
}

bool CAIObject::SetupAStarPathWalk(const CAIFilterRealVector3& startDir, const CAIFilterRealVector3& endDir, bool engage) {
    CAIFilterRealVector3 start;
    CAIFilterRealVector3 end;
    Copy(start, m_position);
    start.z += 0.4f;
    Copy(end, m_arbitraryPoint);
    end.z += 0.4f;
    m_followObject = 0;
    Copy(m_walkDestination, m_arbitraryPoint);
    m_walkDestination.z += 0.4f;
    m_startNode = g_aigAIFilterGlobalObject.GetClosestAStarPathNode(start, startDir);
    if (!m_startNode) {
        DebugMsg("SetupAStarPathWalk: Could not find closest path node to start pos (%f,%f,%f), start dir (%f,%f,%f)\n",
                 start.x, start.y, start.z, startDir.x, startDir.y, startDir.z);
        return false;
    }
    CAIFilterRealVector3 direction(endDir);
    if (m_arbitraryPointType == 4)
        direction.x = direction.y = direction.z = 0.0f;
    CPathNode* node = g_aigAIFilterGlobalObject.GetClosestAStarPathNode(end, direction);
    if (!node) {
        DebugMsg("SetupAStarPathWalk: Could not find closest path node to end pos (%f,%f,%f), end dir (%f,%f,%f)\n",
                 end.x, end.y, end.z, endDir.x, endDir.y, endDir.z);
        return false;
    }
    m_endNode = 0;
    GetNextPathNode(m_startNode, node);
    if (!m_nextNode) {
        DebugMsg("SetupAStartPathWalk: Could not compute AStar path from node (%f,%f,%f), to node (%f,%f,%f)\n",
                 m_startNode->m_position.x, m_startNode->m_position.y, m_startNode->m_position.z,
                 node->m_position.x, node->m_position.y, node->m_position.z);
        return false;
    }
    if (m_nextNode) {
        m_walkArrived = false;
        MoveTo(m_nextNode->m_position.x, m_nextNode->m_position.y, m_nextNode->m_position.z);
    } else {
        m_walkArrived = true;
        MoveTo(m_walkDestination);
    }
    if (engage) {
        m_distanceSquared = m_position.GetDistanceSquaredXYReal(m_moveTarget);
        m_closing = false;
        m_retried = false;
        m_nextRepath = g_aigAIFilterGlobalObject.m_time + 1.0f;
        m_nextNodeCheck = g_aigAIFilterGlobalObject.m_time + 0.3f;
        EngageObstacleAvoidance(false, 0.0f, 0.0f, 0.0f);
    }
    return true;
}

bool CAIObject::SetupAStarPathWalkForPatrol(const CAIFilterRealVector3& point) {
    CAIFilterRealVector3 direction;
    CAIFilterRealVector3 back;
    m_startNode = 0;
    m_endNode = 0;
    m_nextNode = 0;
    float z = m_arbitraryPoint.z;
    float y = m_arbitraryPoint.y;
    float x = m_arbitraryPoint.x;
    m_moveTarget.x = x;
    m_moveTarget.y = y;
    m_moveTarget.z = z;
    m_distanceSquared = m_position.GetDistanceSquaredXYZReal(m_moveTarget);
    m_closing = false;
    m_retried = false;
    m_moveMode = 0;
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
    m_arbitraryPointType = (EMovePointType)9;
    CAIFilterRealVector3 patrol(point);
    m_arbitraryPoint.x = patrol.x;
    m_arbitraryPoint.y = patrol.y;
    m_arbitraryPoint.z = patrol.z;
    switch (m_arbitraryPointType) {
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
    direction.x = m_arbitraryPoint.x - m_position.x;
    direction.y = m_arbitraryPoint.y - m_position.y;
    direction.z = m_arbitraryPoint.z - m_position.z;
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (length != 0.0f) {
        direction.x *= 1.0f / length;
        direction.y *= 1.0f / length;
        direction.z *= 1.0f / length;
    }
    back.x = -1.0f * direction.x;
    back.y = -1.0f * direction.y;
    back.z = -1.0f * direction.z;
    if (!SetupAStarPathWalk(direction, back, true))
        return false;
    return true;
}

bool CAIObject::StartAStarPathWalk() {
    CAIFilterRealVector3 zero;
    CAIFilterRealVector3 direction;
    m_moveMode = 0;
    m_startNode = 0;
    m_endNode = 0;
    m_nextNode = 0;
    float z = m_arbitraryPoint.z;
    float y = m_arbitraryPoint.y;
    float x = m_arbitraryPoint.x;
    m_moveTarget.x = x;
    m_moveTarget.y = y;
    m_moveTarget.z = z;
    m_distanceSquared = m_position.GetDistanceSquaredXYZReal(m_moveTarget);
    m_closing = false;
    m_retried = false;
    m_moveMode = 0;
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
    if (m_unknown2e0) {
        m_arbitraryPoint.x = m_unknown2e0->x;
        m_arbitraryPoint.y = m_unknown2e0->y;
        m_arbitraryPoint.z = m_unknown2e0->z;
    }
    switch (m_arbitraryPointType) {
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
    zero.x = zero.y = zero.z = 0.0f;
    direction.x = m_position.x - m_arbitraryPoint.x;
    direction.y = m_position.y - m_arbitraryPoint.y;
    direction.z = m_position.z - m_arbitraryPoint.z;
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    if (length != 0.0f) {
        direction.x *= 1.0f / length;
        direction.y *= 1.0f / length;
        direction.z *= 1.0f / length;
    }
    if (!SetupAStarPathWalk(zero, direction, true)) {
        m_moveMode = 0;
        return false;
    }
    m_moveMode = 2;
    return true;
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
        switch (m_arbitraryPointType) {
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
            if (m_nextNode && !m_nextNode->m_enabled)
                GetNextPathNode(m_startNode, 0);
            if (m_nextNode) {
                CPathNode* start = m_startNode;
                CAIFilterRealPosition from;
                Copy(from, start->m_position.x, start->m_position.y, start->m_position.z);
                MoveTo(m_nextNode->m_position.x, m_nextNode->m_position.y, m_nextNode->m_position.z);
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
                        MoveTo(m_nextNode->m_position.x, m_nextNode->m_position.y, m_nextNode->m_position.z);
                    } else {
                        m_walkArrived = true;
                        MoveTo(m_walkDestination);
                        if (!last)
                            BSObjectTriggerEvent(m_filter->m_script, 193, 0, 0, true);
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
                                MoveTo(m_nextNode->m_position.x, m_nextNode->m_position.y, m_nextNode->m_position.z);
                            } else {
                                MoveTo(m_walkDestination);
                                BSObjectTriggerEvent(m_filter->m_script, 193, 0, 0, true);
                            }
                            m_distanceSquared = m_position.GetDistanceSquaredXYReal(m_moveTarget);
                            m_closing = false;
                            m_retried = true;
                        } else {
                            MoveTo(m_walkDestination);
                            BSObjectTriggerEvent(m_filter->m_script, 193, 0, 0, true);
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
                    BSObjectTriggerEvent(m_filter->m_script, 193, 0, 0, true);
                } else {
                    m_endNode = 0;
                    GetNextPathNode(m_startNode, node);
                    if (m_nextNode) {
                        MoveTo(m_nextNode->m_position.x, m_nextNode->m_position.y, m_nextNode->m_position.z);
                    } else {
                        MoveTo(m_walkDestination);
                        BSObjectTriggerEvent(m_filter->m_script, 193, 0, 0, true);
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
                    MoveTo(m_nextNode->m_position.x, m_nextNode->m_position.y, m_nextNode->m_position.z);
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

void CAIObject::WalkAStarPath() {
    UpdateAStarPathWalk();
    float distance;
    if (m_arbitraryPointType == 4)
        distance = m_position.GetDistanceXYZReal(m_walkDestination);
    else
        distance = m_position.GetDistanceXYReal(m_walkDestination);
    if (distance - m_walkStopDistance < 0.5f) {
        m_walkArrived = true;
        BSObjectTriggerEvent(m_filter->m_script, 152, 0, 0, false);
    }
}

CSearchNode* CAIObject::AddChildrenToSearchNodeQueue(CSearchNode* parent, const CPathNode* goal,
                                                     CSearchNodeQueue& queue, CSearchNodeList& list) {
    CPathNode* node = parent->m_pathNode;
    unsigned short* child = node->m_children;
    unsigned short* end = child + node->m_childCount;
    for (; child < end; child++) {
        CPathNode* next = g_aigAIFilterGlobalObject.GetAStarPathNodeByIndex(*child);
        if (!(next->m_enabled))
            continue;
        if (parent->m_parent && parent->m_parent->m_pathNode == next)
            continue;
        CSearchNode* search = new CSearchNode;
        CSearchNode* existing = search->Setup(parent, next, goal);
        if (existing && !existing->IsClosed()) {
            if (search->m_cost < existing->m_cost) {
                search->m_pathNode->m_searchNode = search;
                queue.ReplaceElement(existing, search);
                delete existing;
            } else {
                delete search;
            }
        } else if (existing) {
            delete search;
        } else if (goal == next) {
            return search;
        } else {
            queue.Insert(search);
        }
    }
    return 0;
}

void CAIObject::CleanupAStarPathWalk() {
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
}

CPathNode* CAIObject::GetNextPathNode(CPathNode* start, CPathNode* end) {
    CSearchNode* first = (CSearchNode*)m_searchNodes.m_head;
    if (end == m_endNode && first) {
        m_searchNodes.RemoveElement(first);
        delete first;
        CSearchNode* head = (CSearchNode*)m_searchNodes.m_head;
        if (head)
            m_nextNode = head->m_pathNode;
        else
            m_nextNode = 0;
        return m_nextNode;
    }
    if (end)
        m_endNode = end;
    bool found = ComputeAStarPath(start, m_endNode, 100.0f, *(CSearchNodeQueue*)SPR_BEGIN,
                                  *(CSearchNodeList*)&m_searchNodes);
    first = (CSearchNode*)m_searchNodes.m_head;
    CSearchNode* next = (CSearchNode*)first->m_next;
    if (next) {
        CAIFilterRealVector3 to;
        CAIFilterRealVector3 from;
        float z = m_position.z;
        float y = m_position.y;
        float x = m_position.x;
        from.x = x;
        from.y = y;
        from.z = z;
        from.z += 0.4f;
        CPathNode* node = next->m_pathNode;
        float ny;
        float nz = node->m_position.z;
        ny = node->m_position.y;
        float nx = node->m_position.x;
        to.x = nx;
        to.y = ny;
        to.z = nz;
        to.z += 0.4f;
        if (!CAIFilterGlobal::CheckCollisionLineToEnvironment(from, to, m_filter)) {
            m_searchNodes.RemoveElement(first);
            delete first;
            m_nextNode = next->m_pathNode;
            return m_nextNode;
        }
    }
    m_nextNode = first->m_pathNode;
    if (found)
        return m_nextNode;
    return 0;
}

bool CAIObject::ComputeAStarPath(CPathNode* start, const CPathNode* goal, float maxCost,
                                 CSearchNodeQueue& queue, CSearchNodeList& list) {
    queue.m_count = 1;
    CSearchNode* node;
    CSearchNode* found = 0;
    while (!list.IsEmpty()) {
        CSearchNode* node = (CSearchNode*)list.m_head;
        list.RemoveElement(node);
        delete node;
    }
    if (start == goal) {
        found = new CSearchNode;
        found->Setup(0, start, goal);
    } else {
        CSearchNode* node = new CSearchNode;
        node->Setup(0, start, goal);
        queue.Insert(node);
        float limit = 0.25f * maxCost;
        while (queue.m_count > 1) {
            CSearchNode* best = queue.PopFirst();
            if (best->m_cost > maxCost && best->m_unknown0c > limit) {
                delete best;
                break;
            }
            best->m_closed = true;
            list.AddElementAtEnd(best);
            found = AddChildrenToSearchNodeQueue(best, goal, queue, list);
            if (found)
                break;
        }
        queue.Clear();
    }
    bool result = found != 0;
    if (found) {
        if (!found->IsClosed())
            found->m_closed = true;
        list.AddElementAtEnd(found);
    } else {
        CSearchNode* nearestNode = 0;
        float distance;
        float nearest = 0.0f;
        for (CSearchNode* node = (CSearchNode*)list.m_head; node; node = (CSearchNode*)node->m_next) {
            distance = node->m_unknown10;
            if (!nearestNode || distance < nearest) {
                nearestNode = node;
                nearest = distance;
            }
        }
        found = nearestNode;
    }
    if (found) {
        for (node = found; node; node = node->m_parent) {
            node->m_pathNode->m_searchNode = 0;
            node->m_closed = !node->m_closed;
        }
        CSearchNode* cur = (CSearchNode*)list.m_head;
        while (cur) {
            node = (CSearchNode*)cur->m_next;
            list.RemoveElement(cur);
            if (cur->m_closed) {
                cur->m_closed = !cur->m_closed;
                delete cur;
            }
            cur = node;
        }
        for (node = found; node; node = node->m_parent)
            list.AddElementAtBeginning(node);
    }
    return result;
}

CCoverPoint* CAIObject::StoreClosestCoverPoint() {
    const CAIFilterRealPosition* threat;
    if (m_threat)
        threat = &m_threat->m_position;
    else
        threat = 0;
    CAIFilterRealVector3 position;
    float z = m_position.z;
    float y = m_position.y;
    float x = m_position.x;
    position.x = x;
    position.y = y;
    position.z = z;
    position.z += 0.5f;
    float range = 0.5f * m_sightDistance;
    range *= range;
    MathFunRandomI64(0, 1);
    COVERPOINT_WALK_MODE mode;
    m_coverPoint = g_aigAIFilterGlobalObject.GetClosestUsableCoverPoint(
        m_filter->m_user->GetProximityData()->leaf, position, threat, &mode, &m_coverType, &m_coverPosition, range,
        this);
    if (m_coverPoint) {
        if (!m_coverPoint->FlagAsOccupied(m_coverType))
            BSObjectTriggerEvent(m_filter->m_script, 62, 0, 0, false);
        const CAIFilterRealVector3& peek = m_coverPoint->GetPeekLocation();
        float pz = peek.z;
        float py = peek.y;
        float px = peek.x;
        m_peekLocation.x = px;
        m_peekLocation.y = py;
        m_peekLocation.z = pz;
        return m_coverPoint;
    }
    return 0;
}

void CAIObject::ChooseCoverPoint(ECoverPointSelectionType type, const ISceneNode* node, float radius,
                                 float range) {
    CAIObject* const self = this;
    float z;
    float y;
    float x;
    float distance;
    const CAIFilterRealPosition* from;
    int t = type;
    CAIFilterRealVector3 position;
    CVector3 nodePosition;
    if (node) {
        from = (const CAIFilterRealPosition*)&nodePosition;
        node->GetPosition(*(CVector3*)from);
    } else if (self->m_threat) {
        from = &self->m_threat->m_position;
    } else {
        from = 0;
    }
    z = m_position.z;
    y = m_position.y;
    x = m_position.x;
    position.x = x;
    position.y = y;
    position.z = z;
    position.z += 0.5f;
    float half = 0.5f * radius;
    distance = half * half;
    range *= range;
    m_chosenCoverPoint = Select(self, position, from, t, distance, range);
}

void CAIObject::SetCoverPoint(int index, bool flag) {
    if (m_coverPoint) {
        m_flags1a8 &= ~2;
        if (m_coverPoint) {
            m_coverPoint->FlagAsUnoccupied(m_coverType);
            m_coverPoint = 0;
            m_moveMode = 0;
            m_atCoverPoint = false;
        }
    }
    m_atCoverPoint = false;
    m_checkCover = flag;
    if (index >= 0) {
        m_coverPoint = g_aigAIFilterGlobalObject.GetCoverPointByIndex(index);
        if (m_coverPoint) {
            CAIFilterRealVector3 position;
            m_coverType = m_coverPoint->GetAvailablePeekPosition();
            m_coverPoint->GetModifiedPosition(position, m_coverType);
            if (!m_coverPoint->FlagAsOccupied(m_coverType))
                BSObjectTriggerEvent(m_filter->m_script, 62, 0, 0, false);
            const CAIFilterRealVector3& peek = m_coverPoint->GetPeekLocation();
            float pz = peek.z;
            float py = peek.y;
            float px = peek.x;
            m_peekLocation.x = px;
            m_peekLocation.y = py;
            m_peekLocation.z = pz;
            if (m_position.GetDistanceXYZReal(position) < 0.3f)
                m_atCoverPoint = true;
        }
    }
}

bool CAIObject::StartCoverPointWalk(bool commit) {
    CAIFilterRealVector3 zero;
    m_atCoverPoint = false;
    m_moveMode = 0;
    m_startNode = 0;
    m_endNode = 0;
    m_nextNode = 0;
    float z = m_arbitraryPoint.z;
    float y = m_arbitraryPoint.y;
    float x = m_arbitraryPoint.x;
    m_moveTarget.x = x;
    m_moveTarget.y = y;
    m_moveTarget.z = z;
    m_distanceSquared = m_position.GetDistanceSquaredXYZReal(m_moveTarget);
    m_closing = false;
    m_retried = false;
    m_moveMode = 0;
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
    m_coverPoint->GetModifiedPosition(m_arbitraryPoint, m_coverType);
    float pz = m_arbitraryPoint.z;
    float py = m_arbitraryPoint.y;
    float px = m_arbitraryPoint.x;
    m_coverPosition.x = px;
    m_coverPosition.y = py;
    m_coverPosition.z = pz;
    zero.x = zero.y = zero.z = 0.0f;
    CoverPointObjectView* object = m_coverPoint->m_object;
    CMatrix matrix;
    object->orientation.GetMatrix(matrix);
    CAIFilterRealVector3 direction;
    matrix.GetFront(direction);
    direction.x *= -1.0f;
    direction.y *= -1.0f;
    direction.z *= -1.0f;
    m_arbitraryPointType = 0;
    bool result = true;
    if (!SetupAStarPathWalk(zero, direction, false)) {
        result = false;
        float tz = m_arbitraryPoint.z;
        float ty = m_arbitraryPoint.y;
        float tx = m_arbitraryPoint.x;
        m_moveTarget.x = tx;
        m_moveTarget.y = ty;
        m_moveTarget.z = tz;
    }
    if (commit) {
        CommitCoverPointWalkToStored();
    } else {
        m_distanceSquared = m_position.GetDistanceSquaredXYReal(m_moveTarget);
        m_closing = false;
        m_retried = false;
        m_nextRepath = 1.0f + g_aigAIFilterGlobalObject.m_time;
        m_nextNodeCheck = 0.3f + g_aigAIFilterGlobalObject.m_time;
        m_unknown2e4 = false;
        if (m_unknown2e4) {
            m_obstacleAvoidance.PrepareForAvoidance(m_moveTarget);
            m_unknown67c = 0.0f;
        }
    }
    if (!m_coverPoint->FlagAsOccupied(m_coverType))
        BSObjectTriggerEvent(m_filter->m_script, 62, 0, 0, false);
    return result;
}

void CAIObject::CommitCoverPointWalkToStored() {
    m_distanceSquared = m_position.GetDistanceSquaredXYReal(m_moveTarget);
    m_closing = false;
    m_retried = false;
    m_nextRepath = 1.0f + g_aigAIFilterGlobalObject.m_time;
    m_nextNodeCheck = 0.3f + g_aigAIFilterGlobalObject.m_time;
    m_unknown2e4 = false;
    if (m_unknown2e4) {
        m_obstacleAvoidance.PrepareForAvoidance(m_moveTarget);
        m_unknown67c = 0.0f;
    }
    m_moveMode = 9;
    m_filter->SetMoveTarget(m_moveTarget);
    m_state &= ~7;
}

void CAIObject::StartCoverPointWalkToStored(bool walk) {
    StartCoverPointWalk(walk);
}

void CAIObject::CheckCoverpointUsefulness() {
    if (m_checkCover) {
        if (!m_coverPoint) {
            BSObjectTriggerEvent(m_filter->m_script, 62, 0, 0, true);
        } else if (!(g_aigAIFilterGlobalObject.m_time < m_nextCoverCheck)) {
            m_nextCoverCheck = 0.2f + g_aigAIFilterGlobalObject.m_time;
            if (m_threat) {
                CAIFilterRealVector3 position;
                m_coverPoint->GetModifiedPosition(position, m_coverType);
                if (!m_coverPoint->ProvidesCover(position, &m_threat->m_position, this))
                    BSObjectTriggerEvent(m_filter->m_script, 62, 0, 0, true);
            }
        }
    }
}

void CAIObject::WalkToCoverPoint() {
    if (!(g_aigAIFilterGlobalObject.m_time < m_nextNodeCheck)) {
        m_nextNodeCheck = 0.1f + g_aigAIFilterGlobalObject.m_time;
        CheckCoverpointUsefulness();
        if (!m_coverPoint)
            return;
    }
    UpdateAStarPathWalk();
    if (!m_coverPoint)
        return;
    float distance = m_position.GetDistanceXYZReal(m_coverPosition);
    CoverPointObjectView* object = m_coverPoint->m_object;
    CMatrix matrix;
    object->orientation.GetMatrix(matrix);
    CAIFilterRealVector3 front;
    matrix.GetFront(front);
    CAIFilterRealVector3 facing = m_facing;
    float dx = m_coverPosition.x - m_position.x;
    float dy = m_coverPosition.y - m_position.y;
    float dz = m_coverPosition.z - m_position.z;
    float length = sqrtf(dx * dx + dy * dy + dz * dz);
    if (length != 0.0f) {
        float inverse = 1.0f / length;
        dx *= inverse;
        dy *= inverse;
        dz *= inverse;
    }
    if (!(m_state & 2) && MathFunCloseToZero(distance - 4.9f, 0.3f)) {
        if (MathFunCloseToZero(distance - 4.9f - 0.6f, 0.3f)) {
            m_state |= 2;
        } else if (dx * front.x + dy * front.y + dz * front.z > 0.01f) {
            if (facing.x * dx + facing.y * dy + facing.z * dz > 0.7071f) {
                if (PathCost(((CSearchNode*)m_searchNodes.m_head)) < 1.25f * distance) {
                    CAIFilterRealVector3 to = m_coverPosition;
                    CAIFilterRealVector3 from = m_position;
                    to.z += 0.5f;
                    from.z += 0.5f;
                    if (!CAIFilterGlobal::CheckCollisionLineToEnvironment(from, to, (CAIFilterObject*)m_filter)) {
                        if (m_coverType & 0xc0)
                            BSObjectTriggerEvent(m_filter->m_script, 58, (void*)m_coverType, 0, false);
                        else
                            BSObjectTriggerEvent(m_filter->m_script, 57, (void*)m_coverType, 0, false);
                        m_walkArrived = true;
                        m_atCoverPoint = true;
                        m_state |= 2;
                        m_coverPoint->GetModifiedPosition(m_coverPosition, m_coverType);
                        float z = m_coverPosition.z;
                        float y = m_coverPosition.y;
                        float x = m_coverPosition.x;
                        m_moveTarget.x = x;
                        m_moveTarget.y = y;
                        m_moveTarget.z = z;
                        m_filter->SetMoveTarget(m_moveTarget);
                    }
                }
            }
        }
    }
    if (!(m_state & 4) && MathFunCloseToZero(distance - 0.8f, 0.2f)) {
        if (MathFunCloseToZero(distance - 0.8f - 0.4f, 0.2f)) {
            m_state |= 4;
        } else if (front.x * dx + front.y * dy + front.z * dz > 0.01f) {
            if (m_coverType & 0xc0)
                BSObjectTriggerEvent(m_filter->m_script, 60, (void*)m_coverType, 0, false);
            else
                BSObjectTriggerEvent(m_filter->m_script, 59, (void*)m_coverType, 0, false);
            m_walkArrived = true;
            m_atCoverPoint = true;
            m_state |= 4;
            m_coverPoint->GetModifiedPosition(m_coverPosition, m_coverType);
            float z = m_coverPosition.z;
            float y = m_coverPosition.y;
            float x = m_coverPosition.x;
            m_moveTarget.x = x;
            m_moveTarget.y = y;
            m_moveTarget.z = z;
            m_filter->SetMoveTarget(m_moveTarget);
        }
    }
    if (!(m_state & 1) && distance < 0.3f) {
        if (dx * front.x + dy * front.y + dz * front.z > 0.01f) {
            m_atCoverPoint = true;
            BSObjectTriggerEvent(m_filter->m_script, 56, (void*)m_coverType, 0, true);
            m_state |= 1;
        }
    }
}

void CAIObject::WalkToCoverPointDirect() {
    if (!(g_aigAIFilterGlobalObject.m_time < m_nextNodeCheck)) {
        m_nextNodeCheck = 0.1f + g_aigAIFilterGlobalObject.m_time;
        CheckCoverpointUsefulness();
    }
    float distance = m_position.GetDistanceSquaredXYZReal(m_moveTarget);
    if (!(m_state & 2) && MathFunCloseToZero(distance - 24.01f, 0.09f)) {
        if (m_coverType & 0xc0)
            BSObjectTriggerEvent(m_filter->m_script, 58, (void*)m_coverType, 0, false);
        else
            BSObjectTriggerEvent(m_filter->m_script, 57, (void*)m_coverType, 0, false);
        m_state |= 2;
    }
    if (!(m_state & 1) && distance < 0.09f) {
        m_atCoverPoint = true;
        BSObjectTriggerEvent(m_filter->m_script, 56, (void*)m_coverType, 0, true);
        m_state |= 1;
    }
}

void CAIObject::WalkToCoverPointIndirect() {
    if (!(g_aigAIFilterGlobalObject.m_time < m_nextNodeCheck)) {
        m_nextNodeCheck = 0.1f + g_aigAIFilterGlobalObject.m_time;
        CheckCoverpointUsefulness();
    }
    float distance = m_position.GetDistanceSquaredXYZReal(m_moveTarget);
    if (!(m_state & 4) && distance < 0.040000003f) {
        if (m_coverType & 0xc0)
            BSObjectTriggerEvent(m_filter->m_script, 60, (void*)m_coverType, 0, false);
        else
            BSObjectTriggerEvent(m_filter->m_script, 59, (void*)m_coverType, 0, false);
        m_state |= 4;
        m_moveMode = 3;
        m_coverPoint->GetModifiedPosition(m_moveTarget, m_coverType);
        m_filter->SetMoveTarget(m_moveTarget);
    }
}

void CAIObject::SetupArbitraryPointUpdate(EMovePointType type, BS_STRUCT_Vector_struct* vector, float value) {
    m_arbitraryPointType = type;
    m_walkStopDistance = value;
    m_unknown2e0 = (CAIFilterRealPosition*)vector;
}

void CAIObject::StartWalkToArbitraryPoint(CAIFilterRealPosition& point) {
    m_moveMode = 0;
    m_startNode = 0;
    m_endNode = 0;
    m_nextNode = 0;
    float z = m_arbitraryPoint.z;
    float y = m_arbitraryPoint.y;
    float x = m_arbitraryPoint.x;
    m_moveTarget.x = x;
    m_moveTarget.y = y;
    m_moveTarget.z = z;
    m_distanceSquared = m_position.GetDistanceSquaredXYZReal(m_moveTarget);
    m_closing = false;
    m_retried = false;
    m_moveMode = 0;
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
    m_moveMode = 8;
    float pz = point.z;
    float py = point.y;
    float px = point.x;
    m_arbitraryPoint.x = px;
    m_arbitraryPoint.y = py;
    m_arbitraryPoint.z = pz;
    float tz = m_arbitraryPoint.z;
    float ty = m_arbitraryPoint.y;
    float tx = m_arbitraryPoint.x;
    m_moveTarget.x = tx;
    m_moveTarget.y = ty;
    m_moveTarget.z = tz;
    m_filter->SetMoveTarget(m_moveTarget);
}

void CAIObject::WalkToArbitraryPoint() {
    if (g_aigAIFilterGlobalObject.m_time < m_nextNodeCheck) {
        switch (m_arbitraryPointType) {
        case 0:
        case 1:
        case 2:
        default:
            if (m_threat && m_unknown6d8) {
                float z = m_threat->m_position.z;
                float y = m_threat->m_position.y;
                float x = m_threat->m_position.x;
                m_arbitraryPoint.x = x;
                m_arbitraryPoint.y = y;
                m_arbitraryPoint.z = z;
            } else {
                float z = m_unknown150.z;
                float y = m_unknown150.y;
                float x = m_unknown150.x;
                m_arbitraryPoint.x = x;
                m_arbitraryPoint.y = y;
                m_arbitraryPoint.z = z;
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
        m_moveTarget.x = x;
        m_moveTarget.y = y;
        m_moveTarget.z = z;
    } else {
        m_nextNodeCheck = g_aigAIFilterGlobalObject.m_time + 0.1f;
    }
    float distance;
    if (m_arbitraryPointType == 4)
        distance = m_position.GetDistanceXYZReal(m_moveTarget);
    else
        distance = m_position.GetDistanceXYReal(m_moveTarget);
    if (distance - m_walkStopDistance < 0.5f)
        BSObjectTriggerEvent(m_filter->m_script, 152, 0, 0, false);
}

void CAIObject::EngageObstacleAvoidance(bool engage, float, float, float distance) {
    m_unknown2e4 = engage;
    if (m_unknown2e4) {
        m_obstacleAvoidance.PrepareForAvoidance(m_moveTarget);
        m_unknown67c = distance * distance;
    }
}

void CAIObject::ProcessObstacleAvoidance() {
    float z = m_moveTarget.z;
    float y = m_moveTarget.y;
    float x = m_moveTarget.x;
    m_obstacleAvoidance.m_unknown368.x = x;
    m_obstacleAvoidance.m_unknown368.y = y;
    m_obstacleAvoidance.m_unknown368.z = z;
    m_obstacleAvoidance.RunEnvironmentCollisionChecks();
    m_obstacleAvoidance.DetermineNewObjectivePosition(m_unknown688);
}

void CAIObject::SetupFieldOfViewModeObject(CAIObject* object, float distance, float angle) {
    m_seekObject = object;
    if (distance <= 0.001f) {
        printf("Warning: SFOVMO - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_visionDistance = distance;
    if (angle <= 0.001f)
        angle = 90.0f;
    m_fovAngle = 0.017453292f * angle;
}

void CAIObject::SetupFieldOfViewModeList(aistatus_match* list, int count, float distance, float angle) {
    memcpy(m_list, list, count * sizeof(aistatus_match));
    m_listCount = count;
    m_listObject = 0;
    if (distance <= 0.001f) {
        printf("Warning: SFOVML - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_listDistance = distance;
    if (angle <= 0.001f)
        angle = 90.0f;
    m_listAngle = 0.017453292f * angle;
}

void CAIObject::SetupFieldOfViewCorpse(float distance, float angle) {
    if (distance <= 0.001f) {
        printf("Warning: SFOVC - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_corpseDistance = distance;
    if (angle <= 0.001f) {
        printf("Warning: FOV is zero, changing to default\n");
        angle = 90.0f;
    }
    m_corpseAngle = 0.017453292f * angle;
}

CAIObject* CAIObject::CheckFieldOfViewModeObject() {
    float distance;
    float angle;
    CAIObject* object = m_seekObject;
    angle = m_fovAngle;
    distance = m_visionDistance;
    if (object->m_unknown730)
        distance *= 0.5f;
    if (object->IsCrouching())
        distance *= 0.75f;
    if (IsObjectInFieldOfView(object->m_position, distance, angle, object->IsCrouching())) {
        if (m_seekObject == m_threat) {
            float z = m_seekObject->m_position.z;
            float y = m_seekObject->m_position.y;
            float x = m_seekObject->m_position.x;
            m_unknown150.x = x;
            m_unknown150.y = y;
            m_unknown150.z = z;
            m_unknown6d8 = 1;
            m_unknown731 = 1;
        }
        return m_seekObject;
    }
    return 0;
}

CAIObject* CAIObject::CheckFieldOfViewModeList() {
    CAIObject* start = m_listObject;
    CAIObject* object = Search2(this, GetListStart(), start ? start : GetListStart());
    if (!object)
        return 0;
    float distance;
    float angle;
    angle = m_listAngle;
    distance = m_listDistance;
    if (object->m_unknown730)
        distance *= 0.5f;
    if (object->IsCrouching())
        distance *= 0.75f;
    CAIObject* result;
    if (IsObjectInFieldOfView(object->m_position, distance, angle, object->IsCrouching()))
        result = object;
    else if (object == start)
        result = 0;
    else
        result = 0;
    return result;
}

inline int CAIStatus::GetVarValue(aistatus_var var) const {
    int value = 0;
    switch (var) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:
    case 13:
        value = m_values[var];
        break;
    case 6:
        value = s_paioAttacker->ComputeStatusVisibility(s_paioTarget);
        break;
    case 7:
        value = s_paioAttacker->ComputeStatusDistance(s_paioTarget);
        break;
    case 9:
        value = s_paioAttacker->ComputeStatusThreatValue(s_paioTarget);
        break;
    case 10:
        value = s_paioAttacker->ComputeStatusLOSValue(s_paioTarget);
        break;
    case 11:
        value = s_paioAttacker->ComputeStatusExponentialDistance(s_paioTarget);
        break;
    case 12:
        value = s_paioAttacker->ComputeStatusIsEnemy(s_paioTarget);
        break;
    }
    return value;
}

int CAIObject::ComputeStatusIsEnemy(CAIObject* other) {
    return AreEnemyTeams(m_team, other->m_team) ? 15 : 0;
}

int CAIObject::ComputeStatusLOSValue(CAIObject* other) {
    float distance = m_sightDistance;
    if (other->m_unknown730)
        distance *= 0.5f;
    if (other->IsCrouching())
        distance *= 0.75f;
    return CanSee(other, distance) ? 15 : 0;
}

int CAIObject::ComputeStatusThreatValue(CAIObject* other) {
    int value = other->GetAIStatusVar((aistatus_var)3);
    int distance = ComputeStatusExponentialDistance(other);
    int count = other->GetAIStatusVar((aistatus_var)8);
    int threat = value + (15 - distance) * 2;
    threat /= count + 3;
    if (threat < 0)
        threat = 0;
    else if (threat > 15)
        threat = 15;
    return threat;
}

int CAIObject::ComputeStatusDistance(CAIObject* other) {
    float distance = 0.1f * m_position.GetDistanceXYReal(other->m_position);
    if (distance >= 15.0f)
        return 15;
    return distance;
}

int CAIObject::ComputeStatusVisibility(CAIObject*) {
    return 0;
}

inline int CAIObject::GetAIStatusVar(aistatus_var var) const {
    return m_status.GetVarValue(var);
}

CAIObject* CAIObject::CheckFieldOfViewCorpse() {
    for (CAIObject* object = g_paioAIObjectListStart; object; object = object->m_next) {
        if (object->GetAIStatusVar((aistatus_var)13)) {
            float distance;
            float angle;
            angle = m_corpseAngle;
            distance = m_corpseDistance;
            if (object->m_unknown730)
                distance *= 0.5f;
            if (object->IsCrouching())
                distance *= 0.75f;
            if (IsObjectInFieldOfView(object->m_position, distance, angle, object->IsCrouching()))
                return object;
        }
    }
    return 0;
}

bool CAIObject::IsObjectInFieldOfView(CAIFilterRealPosition& position, float distance, float angle, bool low) {
    if (angle <= 0.001f)
        angle = 1.5707964f;
    if (distance <= 0.001f) {
        printf("Warning: IOIFOV - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    if ((m_flags & 8) == 8)
        return false;
    float range = distance;
    if (range <= 0.001f) {
        printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
        range = 50.0f;
    }
    if (!(m_position.GetDistanceXYZReal(position) <= range))
        return false;
    CAIFilterRealVector3 look;
    CAIFilterRealVector3 eye;
    m_filter->GetLookDirection(look, eye);
    CAIFilterRealVector3 target;
    float z = position.z;
    float y = position.y;
    float x = position.x;
    target.x = x;
    target.y = y;
    target.z = z;
    target.z += 0.8f;
    CAIFilterRealVector3 direction(target);
    direction.x -= eye.x;
    direction.y -= eye.y;
    direction.z -= eye.z;
    direction.Normalize();
    look.Normalize();
    float dot = direction.x * look.x + direction.y * look.y + direction.z * look.z;
    if (dot < (float)cos(angle))
        return false;
    float standing[2] = {0.8f, 1.2f};
    float crouching[2] = {0.8f, 0.1f};
    int count = 2;
    float* heights = standing;
    if (low) {
        count = 2;
        heights = crouching;
    }
    for (int i = 0; i < count; i++) {
        float tz = position.z;
        float ty = position.y;
        float tx = position.x;
        target.x = tx;
        target.y = ty;
        target.z = tz;
        target.z += heights[i];
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(eye, target, m_filter))
            return true;
    }
    return false;
}

void CAIObject::SetupLineOfSightModeObject(CAIObject* object, float distance) {
    m_seekObject = object;
    if (distance <= 0.001f) {
        printf("Warning: SLOSMO - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_visionDistance = distance;
}

void CAIObject::SetupLineOfSightModeList(aistatus_match* matches, int count, float distance) {
    memcpy(m_list, matches, count * sizeof(aistatus_match));
    m_listCount = count;
    m_listObject = 0;
    if (distance <= 0.001f) {
        printf("Warning: SLOSML - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_listDistance = distance;
}

inline bool CAIObject::IsTargetInLineOfSight(float distance) {
    if (m_coverPoint && m_atCoverPoint) {
        bool visible = IsObjectInLineOfSightFromEye(m_threat->m_position, distance, m_threat->IsCrouching());
        return visible || IsThreatInLineOfSight(distance);
    }
    return IsThreatInLineOfSight(distance);
}

CAIObject* CAIObject::CheckLineOfSightModeObject() {
    CAIObject* object = m_seekObject;
    if (!object)
        return 0;
    bool visible;
    if (object == m_threat) {
        visible = IsTargetInLineOfSight(m_visionDistance);
        if (visible) {
            float z = m_seekObject->m_position.z;
            float y = m_seekObject->m_position.y;
            float x = m_seekObject->m_position.x;
            m_unknown150.x = x;
            m_unknown150.y = y;
            m_unknown150.z = z;
            m_unknown6d8 = 1;
            m_unknown731 = 1;
        } else {
            m_unknown6d8 = 0;
        }
    } else {
        float distance = m_visionDistance;
        if (object->m_unknown730)
            distance *= 0.5f;
        if (object->IsCrouching())
            distance *= 0.75f;
        visible = IsObjectInLineOfSight(object, distance, object->IsCrouching());
    }
    if (visible)
        return m_seekObject;
    return 0;
}

CAIObject* CAIObject::CheckLineOfSightModeList() {
    CAIObject* start = m_listObject;
    CAIObject* object = FindListObject();
    if (!object)
        return 0;
    float distance = m_listDistance;
    if (object->m_unknown730)
        distance *= 0.5f;
    if (object->IsCrouching())
        distance *= 0.75f;
    CAIObject* result;
    if (IsObjectInLineOfSight(object, distance, object->IsCrouching()))
        result = object;
    else if (object == start)
        result = 0;
    else
        result = 0;
    return result;
}

__declspec(weak) void CAIFilterObject::GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&) {
}

bool CAIObject::IsObjectInLineOfSight(CAIFilterRealPosition& position, float distance, bool low) {
    if ((m_flags & 8) == 8)
        return false;
    float range = distance;
    if (range <= 0.001f) {
        printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
        range = 50.0f;
    }
    if (!(m_position.GetDistanceXYZReal(position) <= range))
        return false;
    CAIFilterRealVector3 unused;
    CAIFilterRealVector3 eye;
    m_filter->GetLookDirection(unused, eye);
    float standing[2] = {0.8f, 1.2f};
    float crouching[2] = {0.8f, 0.1f};
    int count = 2;
    float* heights = standing;
    if (low) {
        count = 2;
        heights = crouching;
    }
    for (int i = 0; i < count; i++) {
        CAIFilterRealPosition target;
        target = position;
        target.z += heights[i];
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(eye, target, m_filter))
            return true;
    }
    return false;
}

void CAIObject::SetupVisionSeekingMode(CAIObject* object, CAIFilterRealPosition& position, float distance) {
    m_seekPosition = position;
    m_seekObject = object;
    if (distance <= 0.001f) {
        printf("Warning: SVSM - Vision distance is zero, changing to default\n");
        distance = 50.0f;
    }
    m_visionDistance = distance;
    CAIFilterRealVector3 eye;
    float z = m_position.z;
    float y = m_position.y;
    float x = m_position.x;
    eye.x = x;
    eye.y = y;
    eye.z = z;
    eye.z += 1.5f;
    const CAIFilterRealPosition* e = &eye;
    m_seekDirection.x = position.x - e->x;
    m_seekDirection.y = position.y - e->y;
    m_seekDirection.z = position.z - e->z;
    m_seekDirection.z = 0.0f;
    float sideways = m_seekDirection.x;
    m_seekDirection.x = m_seekDirection.y;
    m_seekDirection.y = -sideways;
    float length = sqrtf(m_seekDirection.x * m_seekDirection.x + m_seekDirection.y * m_seekDirection.y +
                         m_seekDirection.z * m_seekDirection.z);
    if (length != 0.0f) {
        m_seekDirection.x *= 1.0f / length;
        m_seekDirection.y *= 1.0f / length;
        m_seekDirection.z *= 1.0f / length;
    }
    m_seekSideways = -0.25f;
    m_seekUp = -0.75f;
}

CAIObject* CAIObject::CheckVisionSeekingMode() {
    for (int i = 0; i < 2; i++) {
        CAIFilterRealVector3 eye;
        CAIFilterRealPosition target;
        m_seekSideways = MathFunRandomRealSigned(-0.25f, 0.25f);
        m_seekUp = MathFunRandomRealSigned(-0.75f, 0.75f);
        target.x = m_seekSideways * m_seekDirection.x + m_seekPosition.x;
        target.y = m_seekSideways * m_seekDirection.y + m_seekPosition.y;
        target.z = m_seekUp + m_seekPosition.z;
        float z = m_position.z;
        float y = m_position.y;
        float x = m_position.x;
        eye.x = x;
        eye.y = y;
        eye.z = z;
        eye.z += 1.5f;
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(eye, target, m_filter)) {
            float distance = m_visionDistance;
            if (distance <= 0.001f) {
                printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
                distance = 50.0f;
            }
            if (m_position.GetDistanceXYZReal(m_seekPosition) <= distance)
                return m_seekObject;
        }
    }
    return 0;
}
