// A fragment of AIObject.cpp (0x8004cd34): CAIObject's spline path walking.
// WalkNextSplinePathPoint sets the time of the next check to a tenth of a
// second after the AI clock and asks the traversal at +560 for the next point
// forward, which becomes the move target; StartSplinePathWalkForward resets
// the walk state as StartWalkToArbitraryPoint does (deleting the A* search
// nodes), prepares the traversal of the path and walks to its first point
// (the earlier WalkNextSplinePathPoint inlined). The members are inferred from
// offsets in a non-virtual view (see AIObject_visionseek.cpp); the 0.1f is an
// entry of the file's .sdata2 pool.
enum aifilter_target_mode {};
enum EMovePointType {};
struct aistatus_match;
struct BS_STRUCT_Vector_struct;

class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3;

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual void ChooseTarget(float);
    virtual void SetTargetMode(aifilter_target_mode);
    virtual void ResetTargetMatchList(aifilter_target_mode);
    virtual void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    virtual void SetMoveTarget(const CAIFilterRealPosition&) {}
    virtual void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);
};

class CLinkedListElement;

class CLinkedList {
public:
    void RemoveElement(CLinkedListElement*);

    CLinkedListElement* m_head;
};

class CLinkedListElement {
public:
    unsigned char unknown00[4];
};

class CSearchNode : public CLinkedListElement {
public:
    ~CSearchNode();
};

class CAISplinePath;

class CAISplinePathTraversal {
public:
    void PrepareForTraversalForward(CAISplinePath*, bool, CAIFilterObject*);
    void GetNextPointForward(const CAIFilterRealPosition&, CAIFilterRealPosition*, CAIFilterObject*);

    unsigned char unknown00[40];
};

class CAIFilterGlobal {
public:
    unsigned char unknown00[4];
    float m_time;
    unsigned char unknown08[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

class CAIObject {
public:
    void WalkNextSplinePathPoint();
    void StartSplinePathWalkForward(CAISplinePath*);

    unsigned char unknown000[8];
    CAIFilterObject* m_filterObject;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[44];
    CAIFilterRealPosition m_moveTarget;
    unsigned char unknown05c[444];
    int m_unknown218;
    float m_nextCheck;
    float m_unknown220;
    bool m_unknown224;
    bool m_unknown225;
    unsigned char unknown226[10];
    CAISplinePathTraversal m_traversal;
    CLinkedList m_searchNodes;
    unsigned char unknown25c[8];
    int m_unknown264;
    int m_unknown268;
    int m_unknown26c;
    unsigned char unknown270[88];
    CAIFilterRealPosition m_arbitraryPoint;
};

void CAIObject::WalkNextSplinePathPoint() {
    m_nextCheck = 0.1f + g_aigAIFilterGlobalObject.m_time;
    m_traversal.GetNextPointForward(m_position, &m_moveTarget, m_filterObject);
}

void CAIObject::StartSplinePathWalkForward(CAISplinePath* path) {
    m_unknown218 = 0;
    m_unknown264 = 0;
    m_unknown268 = 0;
    m_unknown26c = 0;
    float z = m_arbitraryPoint.z;
    float y = m_arbitraryPoint.y;
    float x = m_arbitraryPoint.x;
    m_moveTarget.x = x;
    m_moveTarget.y = y;
    m_moveTarget.z = z;
    m_unknown220 = m_position.GetDistanceSquaredXYZReal(m_moveTarget);
    m_unknown224 = false;
    m_unknown225 = false;
    m_unknown218 = 0;
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
    m_traversal.PrepareForTraversalForward(path, true, m_filterObject);
    m_unknown218 = 1;
    WalkNextSplinePathPoint();
}
