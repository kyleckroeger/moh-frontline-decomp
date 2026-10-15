// A fragment of AIObject.cpp (0x8004b170): CAIObject::StoreClosestCoverPoint
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
    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    float w;
} __attribute__((aligned(8)));

enum COVERPOINT_TYPE {};
enum COVERPOINT_WALK_MODE {};

class CAIObject;

class CCoverPoint {
public:
    bool FlagAsOccupied(COVERPOINT_TYPE);
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
    CCoverPoint* GetClosestUsableCoverPoint(const _PropBSPLeaf*, const CAIFilterRealPosition&,
                                            const CAIFilterRealPosition*, COVERPOINT_WALK_MODE*, COVERPOINT_TYPE*,
                                            CAIFilterRealPosition*, float, CAIObject*) const;

    unsigned char unknown00[24];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

class CAIObject {
public:
    CCoverPoint* StoreClosestCoverPoint();

    unsigned char unknown000[8];
    AIFilterScriptView* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[160];
    CAIObject* m_threat;
    unsigned char unknown0c8[320];
    float m_coverRadius;
    unsigned char unknown20c[132];
    CCoverPoint* m_coverPoint;
    COVERPOINT_TYPE m_coverType;
    CAIFilterRealPosition m_coverPosition;
    unsigned char unknown2a4[12];
    CAIFilterRealPosition m_peekLocation;
};

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
