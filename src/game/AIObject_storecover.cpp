// A fragment of AIObject.cpp (0x8004b000): CAIObject::ChooseCoverPoint
// chooses a cover point of the given selection type from the filter: away
// from the given node's position (or the threat's, +196) within half the
// radius squared and the range squared, or without either the closest usable
// cover point, around the object's position raised by 0.5; the choice is
// stored at +680. The 'self' and 'type' locals, the split float temporaries
// and the selecting static inline are inferred.
// CAIObject::StoreClosestCoverPoint
// asks the global filter for the closest usable cover point to the position
// (raised by 0.5) within half the cover radius (+520), away from the threat
// (+196) when there is one, in the object's proximity leaf; it stores the
// point (+656), its type (+660) and a position (+664), flags it occupied
// (firing script event 62 when that fails) and stores its peek location
// (+688), returning the point. A random number is drawn and ignored, as the
// code does. The classes and functions are named by the symbols; the game
// object, proximity data and BSGO_Basic views follow
// bsbifunc_soundbank.cpp, and the members are inferred from offsets;
// CAIFilterRealVector3 follows bsbifunc_coverpoint.cpp, its 8-byte alignment
// inferred from the stack layout. The
// rest of the file is in other units.
struct BSObject;
struct _PropBSPLeaf;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);
long long MathFunRandomI64(long long, long long);

class CAIFilterRealPosition {
public:
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    float w;
} __attribute__((aligned(8)));

enum COVERPOINT_TYPE {};
enum ECoverPointSelectionType {};
enum COVERPOINT_WALK_MODE {};

class CAIObject;
class CMatrix;

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

class CCoverPoint {
public:
    void FlagAsUnoccupied(COVERPOINT_TYPE);
    bool FlagAsOccupied(COVERPOINT_TYPE);
    COVERPOINT_TYPE GetAvailablePeekPosition() const;
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;
    CAIFilterRealVector3 GetPeekLocation();
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

struct AIFilterScriptView {
    unsigned char unknown00[12];
    BSObject* script;
    BSGO_Basic* user;
};

class CAIFilterGlobal {
public:
    CCoverPoint* GetCoverPointByIndex(int) const;
    CCoverPoint* ChooseCoverPointByType(const _PropBSPLeaf*, const CAIFilterRealPosition&,
                                        const CAIFilterRealPosition*, ECoverPointSelectionType, float, float,
                                        CAIObject*) const;
    CCoverPoint* GetClosestUsableCoverPoint(const _PropBSPLeaf*, const CAIFilterRealPosition&,
                                            const CAIFilterRealPosition*, COVERPOINT_WALK_MODE*, COVERPOINT_TYPE*,
                                            CAIFilterRealPosition*, float, CAIObject*) const;

    unsigned char unknown00[24];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

class CAIObject {
public:
    void SetCoverPoint(int, bool);
    void ChooseCoverPoint(ECoverPointSelectionType, const ISceneNode*, float, float);
    CCoverPoint* StoreClosestCoverPoint();

    unsigned char unknown000[8];
    AIFilterScriptView* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[160];
    CAIObject* m_threat;
    unsigned char unknown0c8[224];
    unsigned long m_flags1a8;
    unsigned char unknown1ac[92];
    float m_coverRadius;
    unsigned char unknown20c[12];
    int m_moveMode;
    unsigned char unknown21c[116];
    CCoverPoint* m_coverPoint;
    COVERPOINT_TYPE m_coverType;
    CAIFilterRealPosition m_coverPosition;
    unsigned char unknown2a4[4];
    CCoverPoint* m_chosenCoverPoint;
    bool m_flag2ac;
    unsigned char unknown2ad[3];
    CAIFilterRealPosition m_peekLocation;
    unsigned char unknown2bc[4];
    bool m_atCoverPoint;
};

static inline CCoverPoint* Select(CAIObject* self, const CAIFilterRealVector3& position,
                                  const CAIFilterRealPosition* from, int type, float distance, float range) {
    if (from)
        return g_aigAIFilterGlobalObject.ChooseCoverPointByType(
            self->m_filter->user->GetProximityData()->leaf, position, from, (ECoverPointSelectionType)type, distance,
            range, self);
    return g_aigAIFilterGlobalObject.GetClosestUsableCoverPoint(
        self->m_filter->user->GetProximityData()->leaf, position, 0, 0, 0, 0, distance, self);
}

static inline CAIObject* Me(CAIObject* p) { return p; }
static inline int TypeOf(ECoverPointSelectionType p) { return p; }

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
    float range = 0.5f * m_coverRadius;
    range *= range;
    MathFunRandomI64(0, 1);
    COVERPOINT_WALK_MODE mode;
    m_coverPoint = g_aigAIFilterGlobalObject.GetClosestUsableCoverPoint(
        m_filter->user->GetProximityData()->leaf, position, threat, &mode, &m_coverType, &m_coverPosition, range,
        this);
    if (m_coverPoint) {
        if (!m_coverPoint->FlagAsOccupied(m_coverType))
            BSObjectTriggerEvent(m_filter->script, 62, 0, 0, false);
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
