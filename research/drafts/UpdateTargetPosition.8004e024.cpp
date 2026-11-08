// A fragment of AIObject.cpp (0x8004d75c): CAIObject::SetLocation from a
// position and from three coordinates (the position at +24; the copy loads z,
// y, x as the class's inline assignment does, inferred), then
// PerformCollisionTest, which casts a line from the object's position (raised
// 1.2) towards a point in the tested direction (along the facing axes, a
// diagonal scaled by 0.707) and returns the first collision distance found:
// at chest height, then 0.8 lower, then straight down from the far point
// (the copy of that point loads y first, an inferred helper); -1 when every
// line is clear, 0 when nothing is below the far point. The classes and
// functions are named by the symbols; the members and vector views are
// inferred. The rest of the file is in other units.
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

extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

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
    void operator+=(const CAIFilterRealVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
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
    void Update(const CAIFilterRealVector3& target, const CAIFilterRealVector3& targetRate, float dt) {
        float t = dt;
        float step = 0.008333334f;
        if (t > 0.10000001f)
            t = 0.10000001f;
        for (; t; t -= step) {
            if (step > t)
                step = t;
            float bx, ax, bz, cz, cx, by, az, ay, cy, kb, ka;
            const CAIFilterRealVector3& rate = m_rate;
            ka = m_a;
            kb = m_b;
            ax = target.x - m_value.x;
            ax *= ka;
            bx = targetRate.x - rate.x;
            bx *= kb;
            ay = target.y - m_value.y;
            ay *= ka;
            az = target.z - m_value.z;
            by = targetRate.y - rate.y;
            by *= kb;
            az *= ka;
            bz = targetRate.z - rate.z;
            bz *= kb;
            CAIFilterRealVector3 old;
            old = m_rate;
            cx = ax + bx;
            cy = ay + by;
            cz = az + bz;
            cx *= step;
            cy *= step;
            cz *= step;
            m_rate.x += cx;
            m_rate.y += cy;
            m_rate.z += cz;
            old += m_rate;
            old *= 0.5f * step;
            m_value += old;
        }
    }
    void Update2(const CAIFilterRealVector3& target, const CAIFilterRealVector3& targetRate, float dt) {
        float t = dt;
        float step = 0.008333334f;
        if (t > 0.10000001f)
            t = 0.10000001f;
        for (; t; t -= step) {
            if (step > t)
                step = t;
            float bx, ax, bz, kb, cx, by, az, cz, cy, ay, ka;
            const CAIFilterRealVector3& rate = m_rate;
            ka = m_a;
            kb = m_b;
            ax = target.x - m_value.x;
            ax *= ka;
            bx = targetRate.x - rate.x;
            bx *= kb;
            ay = target.y - m_value.y;
            ay *= ka;
            az = target.z - m_value.z;
            by = targetRate.y - rate.y;
            by *= kb;
            az *= ka;
            bz = targetRate.z - rate.z;
            bz *= kb;
            CAIFilterRealVector3 old;
            old = m_rate;
            cx = ax + bx;
            cy = ay + by;
            cz = az + bz;
            cx *= step;
            cy *= step;
            cz *= step;
            m_rate.x += cx;
            m_rate.y += cy;
            m_rate.z += cz;
            old += m_rate;
            old *= 0.5f * step;
            m_value += old;
        }
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

enum ECollisionTestDirection {};

class CAIObject {
public:
    void SetLocation(CAIFilterRealPosition&);
    void SetLocation(float, float, float);
    float PerformCollisionTest(ECollisionTestDirection, float);
    void CalculateTargetPosition();
    void UpdateTargetPosition();
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
    CAIFilterRealPosition m_unknown150;
    unsigned char unknown15c[84];
    volatile unsigned long m_trackFlags;
    unsigned char unknown1b4[68];
    float m_trackPeriod;
    float m_trackSpread;
    unsigned char unknown200[1240];
    bool m_unknown6d8;
    unsigned char unknown6d9[79];
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

#define X_POSF ((CAIFilterRealPosition&)m_positionFilter.m_value)

void CAIObject::CalculateTargetPosition() {
    CAIFilterRealVector3 zero;
    CAIFilterRealVector3 v;
    m_aimChoice = 0;
    zero.x = zero.y = zero.z = 0.0f;
    CAIObject* target = m_threat;
    float omega = 6.2831855f / m_trackPeriod;
    float omega2 = omega * omega;
    float decay = 2.3025851f / (3.0f * m_trackSpread * m_trackPeriod);
    float a = decay * decay + omega2;
    float b = 2.0f * decay;
    m_velocityFilter.Setup(a, b, target->m_velocity, zero);
    v.x = MathFunRandomRealSigned(m_trackSpread, 2.0f * m_trackSpread);
    v.y = MathFunRandomRealSigned(m_trackSpread, 2.0f * m_trackSpread);
    v.z = MathFunRandomRealSigned(m_trackSpread, 2.0f * m_trackSpread);
    v *= 0.05f;
    m_offsetFilter.Setup(a, b, v, zero);
    decay = 2.3025851f / m_trackPeriod;
    m_positionFilter.Setup(decay * decay + omega2, 2.0f * decay, *(CAIFilterRealVector3*)&target->m_position, zero);
    float distance = m_position.GetDistanceXYZReal(X_POSF);
    m_aimPoint = X_POSF;
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
            long long percent = MathFunGetRandomPercent();
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

void CAIObject::UpdateTargetPosition() {
    float length, d, dx, dy, dz, k, dt, oz, ox, oy, distance;
    CAIFilterRealVector3 zero;
    CAIFilterRealPosition position;
    CAIObject* target = m_threat;
    if (m_unknown6d8)
        position.Assign(target->m_position);
    else
        position.Assign(m_unknown150);
    CAIFilterRealVector3 relative = target->m_velocity;
    dt = g_aigAIFilterGlobalObject.m_time - m_targetTime;
    relative.x -= m_velocity.x;
    relative.y -= m_velocity.y;
    relative.z -= m_velocity.z;
    zero.x = zero.y = zero.z = 0.0f;
    m_velocityFilter.Update(relative, zero, dt);
    m_positionFilter.Update(*(CAIFilterRealVector3*)&position, relative, dt);
    distance = m_position.GetDistanceXYZReal(position);
    if ((m_trackFlags & 0x10) == 0x10 && m_unknown6d8) {
        dx = position.x - m_position.x;
        dy = position.y - m_position.y;
        dz = position.z - m_position.z;
        k = 0.1f * m_trackSpread;
        CAIFilterRealVector3 offset;
        offset.x = k * m_velocityFilter.m_value.x;
        offset.y = k * m_velocityFilter.m_value.y;
        offset.z = k * m_velocityFilter.m_value.z;
        length = sqrtf(dx * dx + dy * dy + dz * dz);
        if (length != 0.0f) {
            dx *= 1.0f / length;
            dy *= 1.0f / length;
            dz *= 1.0f / length;
        }
        d = dx * offset.x + dy * offset.y + dz * offset.z;
        dx *= d;
        dy *= d;
        dz *= d;
        offset.x -= dx;
        offset.y -= dy;
        offset.z -= dz;
        m_offsetFilter.Update2(offset, zero, dt);
    }
    if ((m_trackFlags & 1) == 1) {
        m_trackFlags &= ~1;
        m_trackFlags &= ~0x10;
    }
    m_aimPoint.Assign(X_POSF);
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
            long long percent = MathFunGetRandomPercent();
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
    ox = distance * m_offsetFilter.m_value.x;
    oy = distance * m_offsetFilter.m_value.y;
    oz = distance * m_offsetFilter.m_value.z;
    m_aimPoint.x += ox;
    m_aimPoint.y += oy;
    m_aimPoint.z += oz;
    m_targetTime = g_aigAIFilterGlobalObject.m_time;
}
