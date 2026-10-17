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

class CAIFilterObject;

class CAIFilterGlobal {
public:
    static float GetCollisionDistLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&, CAIFilterObject*, float);
};

enum ECollisionTestDirection {};

class CAIObject {
public:
    void SetLocation(CAIFilterRealPosition&);
    void SetLocation(float, float, float);
    float PerformCollisionTest(ECollisionTestDirection, float);

    unsigned char unknown00[8];
    CAIFilterObject* m_filterObject;
    unsigned char unknown0c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown24[76];
    CAIFilterRealVector3 m_axis70;
    CAIFilterRealVector3 m_axis80;
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
