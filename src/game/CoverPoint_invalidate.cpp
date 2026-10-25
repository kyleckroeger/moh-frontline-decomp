// A fragment of CoverPoint.cpp (0x800526b0): CCoverPoint::Invalidate clears
// the eight flag bits of the cover point data's byte at +112 and fires script
// event 62 on every AI object (from the global list, next at +4) whose cover
// point (+656) is this one. CCoverPoint::ProvidesCover: a position gives no
// cover from a threat closer than 6 (XY), or when the cover point's facing
// (row 1 of its rotation) is not within cosCoverPointEffectiveAngle of the
// direction to the threat; with an AI object, every other object of another
// (or no) team must also be at least 6 from the AI, not closer than 6 or than
// the AI to the position, and not behind the facing within 18. CCoverPoint,
// CAIObject, BSObject, cosCoverPointEffectiveAngle and the functions are
// named by the symbols; the data view, the flag order (the bits are cleared in
// the order 0x08, 0x40, 0x80, 0x01, 0x02, 0x04, 0x10, 0x20; their names are
// not known), the object views, the team rule's meaning and the vector helpers
// are inferred.
#include <math.h>

struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);


union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    CVector3 GetFront() const { return front; }

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
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

/* inferred: the cover point data's flag byte at +112 */
struct CoverPointDataView {
    unsigned char unknown00[28];
    CQuaternion rotation;
    unsigned char unknown2c[68];
    unsigned char m_flag80 : 1;
    unsigned char m_flag40 : 1;
    unsigned char m_flag20 : 1;
    unsigned char m_flag10 : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_flag04 : 1;
    unsigned char m_flag02 : 1;
    unsigned char m_flag01 : 1;
};

class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

// Inferred: a direction between two filter positions, normalised in place.
class CAIFilterRealVector3 {
public:
    CAIFilterRealVector3(const CAIFilterRealPosition& from, const CAIFilterRealPosition& to)
        : x(to.x - from.x), y(to.y - from.y), z(to.z - from.z) {}
    void Normalize() {
        float length = sqrtf(x * x + y * y + z * z);
        if (length != 0.0f) {
            x *= 1.0f / length;
            y *= 1.0f / length;
            z *= 1.0f / length;
        }
    }
    float Dot(const CVector3& v) const { return x * v.d.v[0] + y * v.d.v[1] + z * v.d.v[2]; }

    float x;
    float y;
    float z;
};

class CAIObject;

class CCoverPoint {
public:
    void Invalidate();
    bool ProvidesCover(const CAIFilterRealPosition&, const CAIFilterRealPosition*, const CAIObject*) const;

    unsigned char unknown00[4];
    CoverPointDataView* m_data;
};

struct AIFilterScriptView {
    unsigned char unknown00[12];
    BSObject* script;
};

class CAIObject {
public:
    static CAIObject* GetGlobalListStart();

    unsigned char unknown000[4];
    CAIObject* m_next;
    AIFilterScriptView* m_filter;
    unsigned char unknown00c[8];
    unsigned char m_team;
    unsigned char unknown015[3];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[620];
    CCoverPoint* m_coverPoint;
};

extern const float cosCoverPointEffectiveAngle; // file-local in the original

void CCoverPoint::Invalidate() {
    CoverPointDataView* data = m_data;
    data->m_flag08 = 0;
    data->m_flag40 = 0;
    data->m_flag80 = 0;
    data->m_flag01 = 0;
    data->m_flag02 = 0;
    data->m_flag04 = 0;
    data->m_flag10 = 0;
    data->m_flag20 = 0;
    for (CAIObject* object = CAIObject::GetGlobalListStart(); object; object = object->m_next) {
        if (object->m_coverPoint == this)
            BSObjectTriggerEvent(object->m_filter->script, 62, 0, 0, true);
    }
}

bool CCoverPoint::ProvidesCover(const CAIFilterRealPosition& position, const CAIFilterRealPosition* threat,
                                const CAIObject* ai) const {
    if (position.GetDistanceSquaredXYReal(*threat) < 36.0f)
        return false;
    CoverPointDataView* data = m_data;
    CMatrix matrix;
    data->rotation.GetMatrix(matrix);
    CVector3 front;
    front = matrix.GetFront();
    CAIFilterRealVector3 direction(position, *threat);
    direction.Normalize();
    if (direction.Dot(front) < cosCoverPointEffectiveAngle)
        return false;
    if (!ai)
        return true;
    float distance = position.GetDistanceSquaredXYReal(ai->m_position);
    for (CAIObject* object = CAIObject::GetGlobalListStart(); object; object = object->m_next) {
        if (object == ai)
            continue;
        if (object->m_team && object->m_team == ai->m_team)
            continue;
        if (ai->m_position.GetDistanceSquaredXYZReal(object->m_position) < 36.0f)
            return false;
        float other = position.GetDistanceSquaredXYReal(object->m_position);
        if (other < 36.0f || other < distance)
            return false;
        direction = CAIFilterRealVector3(position, object->m_position);
        direction.Normalize();
        if (direction.Dot(front) < 0.0f && other < 324.0f)
            return false;
    }
    return true;
}
