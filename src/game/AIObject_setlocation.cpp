// A fragment of AIObject.cpp (0x8004d75c): CAIObject::SetLocation from a
// position and from three coordinates (the position at +24; the copy loads z,
// y, x as the class's inline assignment does, inferred), then
// PerformCollisionTest, which casts a line from the object's position (raised
// 1.2) towards a point in the tested direction (along the facing axes, a
// diagonal scaled by 0.707) and returns the first collision distance found:
// at chest height, then 0.8 lower, then straight down from the far point
// (the copy of that point loads y first, an inferred helper); -1 when every
// line is clear, 0 when nothing is below the far point. CalculateTargetPosition
// restarts the three tracking filters on the threat (+196): its velocity
// (+168) and a random offset (three signed random values around the spread
// +508, scaled by 0.05) with the coefficients of a period +504 and a decay
// ln 10 / (3 * spread * period), its position with the decay ln 10 / period
// (each from rest); then the aim point (+240) is the filtered position raised
// by aim mode (+1832; modes 3 and 5 pick a random sub-mode once, +1836, and
// a crouching threat, virtual slot 0x14, lowers it), plus the offset scaled by
// the distance, and the time is recorded (+16). The classes and functions are
// named by the symbols; the members, the filter view (value, rate and two
// coefficients) and its setup helper, the virtual slot names and the
// forwarding inline around MathFunGetRandomPercent are inferred. The rest of
// the file is in other units.
class CAIFilterRealPosition {
public:
    CAIFilterRealPosition& operator=(const CAIFilterRealPosition& o) {
        float z = o.z;
        float y = o.y;
        float x = o.x;
        this->x = x;
        this->y = y;
        this->z = z;
        return *this;
    }

    void Assign(const CAIFilterRealPosition& o) {
        float y;
        float z = o.z;
        y = o.y;
        float x = o.x;
        this->x = x;
        this->y = y;
        this->z = z;
    }

    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

/* Inferred: vectors are copied as two doubles. */
struct CAIFilterRealVector3Pair {
    double d[2];
};

/* Inferred: a vector of four floats copied as two doublewords. */
class CAIFilterRealVector3 {
public:
    CAIFilterRealVector3& operator=(const CAIFilterRealVector3& o) {
        *(CAIFilterRealVector3Pair*)this = *(const CAIFilterRealVector3Pair*)&o;
        return *this;
    }
    void operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

/* inferred: a second-order tracking filter (value, rate, two coefficients) */
struct CAIFilterTrackView {
    void Setup(float a, float b, const CAIFilterRealVector3& value, const CAIFilterRealVector3& rate) {
        m_a = a;
        m_b = b;
        m_value = value;
        m_rate = rate;
    }

    CAIFilterRealVector3 m_value;
    CAIFilterRealVector3 m_rate;
    float m_a;
    float m_b;
};

class CAIFilterObject;

class CAIFilterGlobal {
public:
    static float GetCollisionDistLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&, CAIFilterObject*, float);

    unsigned char unknown00[4];
    float m_time;
    unsigned char unknown08[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

float MathFunRandomRealSigned(float, float);
long long MathFunGetRandomPercent();
bool MathFunTestPercent(long long, long long);

/* inferred: a forwarding inline (it keeps the result's registers as in the original) */
inline long long RandomPercent() { return MathFunGetRandomPercent(); }

enum ECollisionTestDirection {};

class CAIObject {
public:
    void SetLocation(CAIFilterRealPosition&);
    void SetLocation(float, float, float);
    float PerformCollisionTest(ECollisionTestDirection, float);
    void CalculateTargetPosition();
    virtual void vf08();
    virtual void vf0c();
    virtual void vf10();
    virtual bool vf14();

    unsigned char unknown04[4];
    CAIFilterObject* m_filterObject;
    unsigned char unknown0c[4];
    float m_targetTime;
    unsigned char unknown14[4];
    CAIFilterRealPosition m_position;
    unsigned char unknown24[76];
    CAIFilterRealVector3 m_axis70;
    CAIFilterRealVector3 m_axis80;
    unsigned char unknown90[24];
    CAIFilterRealVector3 m_velocity;
    unsigned char unknownb8[12];
    CAIObject* m_threat;
    CAIFilterTrackView m_positionFilter;
    CAIFilterRealPosition m_aimPoint;
    unsigned char unknownfc[4];
    CAIFilterTrackView m_offsetFilter;
    CAIFilterTrackView m_velocityFilter;
    unsigned char unknown150[168];
    float m_trackPeriod;
    float m_trackSpread;
    unsigned char unknown200[1320];
    int m_aimMode;
    int m_aimChoice;
};

void CAIObject::SetLocation(CAIFilterRealPosition& position) {
    m_position = position;
}

void CAIObject::SetLocation(float x, float y, float z) {
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}

float CAIObject::PerformCollisionTest(ECollisionTestDirection direction, float distance) {
    CAIFilterRealVector3 end;
    switch (direction) {
    case 0:
        end = m_axis80;
        end *= distance;
        break;
    case 1:
        end.x = m_axis80.x + m_axis70.x;
        end.y = m_axis80.y + m_axis70.y;
        end.z = m_axis80.z + m_axis70.z;
        end *= 0.707f * distance;
        break;
    case 2:
        end = m_axis70;
        end *= distance;
        break;
    case 3:
        end.x = m_axis70.x - m_axis80.x;
        end.y = m_axis70.y - m_axis80.y;
        end.z = m_axis70.z - m_axis80.z;
        end *= 0.707f * distance;
        break;
    case 4:
        end = m_axis80;
        end *= -distance;
        break;
    case 5:
        end.x = m_axis80.x + m_axis70.x;
        end.y = m_axis80.y + m_axis70.y;
        end.z = m_axis80.z + m_axis70.z;
        end *= 0.707f * -distance;
        break;
    case 6:
        end = m_axis70;
        end *= -distance;
        break;
    case 7:
        end.x = m_axis70.x - m_axis80.x;
        end.y = m_axis70.y - m_axis80.y;
        end.z = m_axis70.z - m_axis80.z;
        end *= 0.707f * -distance;
        break;
    }
    end.x += m_position.x;
    end.y += m_position.y;
    end.z += m_position.z;
    CAIFilterRealPosition start;
    start.x = m_position.x;
    start.y = m_position.y;
    start.z = m_position.z;
    end.z += 1.2f;
    start.z += 1.2f;
    distance = CAIFilterGlobal::GetCollisionDistLineToEnvironment(start, (CAIFilterRealPosition&)end, m_filterObject, distance);
    if (distance != -1.0f)
        return distance;
    end.z += -0.80000007f;
    start.z += -0.80000007f;
    distance = CAIFilterGlobal::GetCollisionDistLineToEnvironment(start, (CAIFilterRealPosition&)end, m_filterObject, distance);
    if (distance != -1.0f)
        return distance;
    start.Assign((CAIFilterRealPosition&)end);
    distance = -0.8f;
    end.z += distance;
    if (CAIFilterGlobal::GetCollisionDistLineToEnvironment(start, (CAIFilterRealPosition&)end, m_filterObject, distance) == -1.0f)
        return 0.0f;
    return -1.0f;
}

void CAIObject::CalculateTargetPosition() {
    CAIFilterRealVector3 zero;
    CAIFilterRealVector3 v;
    m_aimChoice = 0;
    zero.x = zero.y = zero.z = 0.0f;
    CAIObject* target = m_threat;
    float omega = 6.2831855f / m_trackPeriod;
    float omega2 = omega * omega;
    float decay = 2.30258489f / (3.0f * m_trackSpread * m_trackPeriod);
    float a = decay * decay + omega2;
    float b = 2.0f * decay;
    m_velocityFilter.Setup(a, b, target->m_velocity, zero);
    v.x = MathFunRandomRealSigned(m_trackSpread, 2.0f * m_trackSpread);
    v.y = MathFunRandomRealSigned(m_trackSpread, 2.0f * m_trackSpread);
    v.z = MathFunRandomRealSigned(m_trackSpread, 2.0f * m_trackSpread);
    v *= 0.05f;
    m_offsetFilter.Setup(a, b, v, zero);
    decay = 2.30258489f / m_trackPeriod;
    m_positionFilter.Setup(decay * decay + omega2, 2.0f * decay, *(CAIFilterRealVector3*)&target->m_position, zero);
    float distance = m_position.GetDistanceXYZReal((CAIFilterRealPosition&)m_positionFilter.m_value);
    m_aimPoint = (CAIFilterRealPosition&)m_positionFilter.m_value;
    switch (m_aimMode) {
    case 0:
    case 1:
    case 2:
    case 9:
    default:
        m_aimPoint.z += 1.0f;
        if (m_threat && m_threat->vf14())
            m_aimPoint.z += -0.8f;
        break;
    case 3:
        if (m_aimChoice == 0) {
            if (MathFunTestPercent(MathFunGetRandomPercent(), 10))
                m_aimChoice = 1;
            else
                m_aimChoice = 2;
        }
        if (m_aimChoice != 2)
            m_aimPoint.z += 1.2f;
        else
            m_aimPoint.z += 1.0f;
        if (m_threat && m_threat->vf14())
            m_aimPoint.z += -0.8f;
        break;
    case 4:
        m_aimPoint.z += 1.0f;
        if (m_threat && m_threat->vf14())
            m_aimPoint.z += -0.8f;
        break;
    case 5:
        if (m_aimChoice == 0) {
            long long percent = RandomPercent();
            if (MathFunTestPercent(percent, 10))
                m_aimChoice = 6;
            else if (MathFunTestPercent(percent, 80))
                m_aimChoice = 6;
            else
                m_aimChoice = 8;
        }
        switch (m_aimChoice) {
        case 6:
        default:
            m_aimPoint.z += 0.5f;
            if (m_threat && m_threat->vf14())
                m_aimPoint.z += -0.8f;
            break;
        case 7:
            m_aimPoint.z += -0.5f;
            break;
        case 8:
            m_aimPoint.z += 0.5f;
            m_aimPoint.x += 1.25f;
            m_aimPoint.y += 1.25f;
            break;
        }
        break;
    case 6:
        break;
    case 7:
        m_aimPoint.z += 1.2f;
        if (m_threat && m_threat->vf14())
            m_aimPoint.z += 1.2f;
        break;
    case 8:
        m_aimPoint.z += 1.0f;
        if (m_threat && m_threat->vf14())
            m_aimPoint.z += -0.8f;
        break;
    }
    v.x = distance * m_offsetFilter.m_value.x;
    v.y = distance * m_offsetFilter.m_value.y;
    v.z = distance * m_offsetFilter.m_value.z;
    m_aimPoint.x += v.x;
    m_aimPoint.y += v.y;
    m_aimPoint.z += v.z;
    m_targetTime = g_aigAIFilterGlobalObject.m_time;
}
