// The end of ObstacleAvoidance.cpp (0x80053c34): CObstacleAvoidance::
// RunEnvironmentCollisionChecks (one obstacle-avoidance line per call: the
// line from the owner's position, raised by the current height, along its
// forward/right/up axes scaled by the line's end point and the range scale,
// is tested against the environment; the result updates the previous grid's
// entry for the line and the current height level, the hit counters and the
// balance; states 1/2 and 3 then probe the ceiling and the floor; after the
// 13th line the balance widens or narrows the range and the height level
// cycles, copying the previous grid to the current one after the last level),
// CObstacleAvoidance::PrepareForAvoidance (resets the counters and the state,
// keeps the objective position, sets the range to 112.5, rebuilds the
// previous grid's 13 entries - each direction at the range distance,
// unblocked, at the objective - and copies it to the current grid, then two
// 4.0 values and a cleared vector), the CObstacleAvoidance destructor (both
// grids destroyed), the constructor (its two 13-entry grids of
// CObstacleGridEntry, then, once for all instances, the file's 13
// obstacle-avoidance line end points (x, y, 0) and the angle range of each
// line sector: the minimum angles fall from 9pi/16 to -pi in steps of pi/8
// and each maximum is the previous minimum, starting at pi), the empty
// CObstacleGridEntry destructor and constructor, then the static
// initialisation: the 13 line vectors are built through CVector3's
// constructor (__construct_array). The vector's inline constructor, emitted
// for the array construction, follows the static initialisation (a weak copy
// of this file). CObstacleAvoidance, CObstacleGridEntry, CVector3 and the
// statics are named by the symbols; the array sizes come from the symbols,
// the grid entry size and grid offsets from the construction calls, and the
// vector layout and the grid entry's Reset inline are inferred (the position
// copies declare their temporaries in the order that gives the original
// registers). The constants are .sdata2 pool entries. The rest of the file is
// in other units.
extern "C" void* memcpy(void*, const void*, unsigned long);

class CVector3 {
public:
    CVector3() {}

    float x;
    float y;
    float z;
    float w;
};

class CAIFilterRealVector3 {
public:
    CAIFilterRealVector3() {}
    CAIFilterRealVector3(const CAIFilterRealVector3& o) { *(Pair*)this = *(const Pair*)&o; }
    CAIFilterRealVector3& operator=(const CAIFilterRealVector3& o) {
        *(Pair*)this = *(const Pair*)&o;
        return *this;
    }
    CAIFilterRealVector3& operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    CAIFilterRealVector3 operator*(float f) const {
        CAIFilterRealVector3 r(*this);
        r *= f;
        return r;
    }
    struct Pair {
        double a, b;
    };

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition& operator+=(const CAIFilterRealVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;
    CAIFilterRealPosition() {}
    CAIFilterRealPosition(float ax, float ay, float az) {
        x = ax;
        y = ay;
        z = az;
    }
    CAIFilterRealPosition operator+(const CAIFilterRealVector3& v) const { return CAIFilterRealPosition(x + v.x, y + v.y, z + v.z); }

    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CAIFilterObject;

class CAIFilterGlobal {
public:
    static float GetCollisionDistLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&, CAIFilterObject*, float);
};

/* Only the members used here are part of this view. */
class CAIObject {
public:
    void GetCoordinateAxisDirection(CAIFilterRealVector3*, CAIFilterRealVector3*, CAIFilterRealVector3*);
    unsigned long GetFlags() const { return m_flags; }

    unsigned char unknown000[8];
    CAIFilterObject* m_filter;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown028[392];
    unsigned long m_flags;
};

inline void CopyPosition(CAIFilterRealPosition& out, const CAIFilterRealPosition& in) {
    float y;
    float z = in.z;
    y = in.y;
    float x = in.x;
    out.x = x;
    out.y = y;
    out.z = z;
}

class CObstacleGridEntry {
public:
    CObstacleGridEntry();
    ~CObstacleGridEntry();
    /* Inferred: each direction's distance, blocked when below the range. */
    void Set(int i, float distance, float range) {
        m_distance[i] = distance;
        if (distance < range)
            m_blocked[i] = true;
        else
            m_blocked[i] = false;
    }
    void Reset(float distance, float range, const CAIFilterRealPosition& position) {
        for (int i = 0; i < 3; i++) {
            m_distance[i] = distance;
            if (distance < range)
                m_blocked[i] = true;
            else
                m_blocked[i] = false;
        }
        float y;
        float z = position.z;
        y = position.y;
        float x = position.x;
        m_position.x = x;
        m_position.y = y;
        m_position.z = z;
    }

    float m_distance[3];
    bool m_blocked[3];
    unsigned char unknown0f[1];
    CAIFilterRealPosition m_position;
};

class CObstacleAvoidance {
public:
    void RunEnvironmentCollisionChecks();
    void PrepareForAvoidance(CAIFilterRealPosition&);
    ~CObstacleAvoidance();
    CObstacleAvoidance();

    unsigned short m_line;
    short m_counters[3];
    short m_balance;
    unsigned char unknown00a[2];
    float m_range;
    float m_scale;
    unsigned char unknown014[4];
    CObstacleGridEntry m_grid[13];
    CObstacleGridEntry m_previousGrid[13];
    float m_unknown358;
    float m_unknown35c;
    float m_unknown360;
    int m_unknown364;
    CAIFilterRealPosition m_position;
    unsigned char unknown378[16];
    CAIObject* m_owner;
    float m_unknown38c[3];
    int m_unknown398;
    int m_unknown39c;
};

CVector3 f_rv3ObstacleAvoidanceLines[13];
float f_rrObstacleAvoidanceLineAngleMin[11];
float f_rrObstacleAvoidanceLineAngleMax[11];
extern bool f_bObstacleAvoidanceLinesInitialized;

void CObstacleAvoidance::RunEnvironmentCollisionChecks() {
    float range = m_range;
    float scale = m_scale;
    CAIFilterRealPosition start;
    CAIFilterRealPosition end;
    CAIFilterRealVector3 forward;
    CAIFilterRealVector3 right;
    CAIFilterRealVector3 up;
    CAIFilterRealVector3 offset;
    CopyPosition(start, m_owner->m_position);
    start.z += m_unknown360;
    m_owner->GetCoordinateAxisDirection(&forward, &right, &up);
    if (m_line == 12)
        start += (offset = forward) *= -0.5f;
    else if (m_line == 11)
        start += (offset = forward) *= 0.5f;
    forward *= scale * f_rv3ObstacleAvoidanceLines[m_line].x;
    right *= scale * f_rv3ObstacleAvoidanceLines[m_line].y;
    up *= scale * f_rv3ObstacleAvoidanceLines[m_line].z;
    end.x = start.x + forward.x;
    end.y = start.y + forward.y;
    end.z = start.z + forward.z;
    end += right;
    end += up;
    CopyPosition(m_previousGrid[m_line].m_position, end);
    float length;
    float distance;
    if (m_line == 11 || m_line == 12) {
        length = 7.25f;
        distance = CAIFilterGlobal::GetCollisionDistLineToEnvironment(start, end, m_owner->m_filter, length);
    } else {
        length = start.GetDistanceSquaredXYZReal(end);
        distance = CAIFilterGlobal::GetCollisionDistLineToEnvironment(start, end, m_owner->m_filter, length);
    }
    if (distance == -1.0f) {
        if (m_line == 11 || m_line == 5 || m_line == 12) {
            m_counters[m_unknown364]--;
            if (m_counters[m_unknown364] <= 0)
                m_previousGrid[5].Set(m_unknown364, range, range);
        } else {
            m_previousGrid[m_line].Set(m_unknown364, length, length);
        }
        m_balance--;
    } else {
        if (m_line == 5) {
            m_counters[m_unknown364] = 3;
            m_previousGrid[5].Set(m_unknown364, distance, range);
        } else if (m_line == 11 || m_line == 12) {
            m_counters[m_unknown364] = 3;
            m_previousGrid[5].Set(m_unknown364, 3.0f * distance, range);
        } else {
            m_previousGrid[m_line].Set(m_unknown364, distance, length);
        }
        m_balance++;
    }
    switch (m_unknown398) {
    case 0:
        // gives the original's dead branch after the dispatch
        if (distance == -1.0f)
            break;
        break;
    case 1:
    case 2:
        CopyPosition(start, m_owner->m_position);
        start.z -= 0.1f;
        end = start;
        end.z += 2.0f;
        distance = CAIFilterGlobal::GetCollisionDistLineToEnvironment(start, end, m_owner->m_filter, 4.0f);
        if (distance == -1.0f)
            m_unknown358 = 4.0f;
        else
            m_unknown358 = distance;
        break;
    case 3:
        if ((m_owner->GetFlags() & 4) == 4) {
            m_owner->m_flags &= ~4;
            m_unknown398 = 0;
        } else {
            CopyPosition(start, m_owner->m_position);
            start.z -= 2.1f;
            end = start;
            end.z -= 2.0f;
            distance = CAIFilterGlobal::GetCollisionDistLineToEnvironment(start, end, m_owner->m_filter, 4.0f);
            if (distance == -1.0f)
                m_unknown35c = 4.0f;
            else
                m_unknown35c = distance;
        }
        break;
    }
    m_line++;
    if (m_line >= 13) {
        m_line = 0;
        if (m_balance <= -26) {
            m_balance = 0;
            if (m_range < 149.99625f) {
                m_range += 18.750937f;
                m_scale = m_range / 112.5f;
            }
        }
        if (m_balance >= 13) {
            m_balance = 0;
            if (m_range > 74.9925f) {
                m_range -= 18.750937f;
                m_scale = m_range / 112.5f;
            }
        }
        switch (m_unknown364) {
        case 0:
        default:
            m_unknown360 = 0.8f;
            m_unknown364 = 1;
            break;
        case 1:
            m_unknown360 = 1.2f;
            m_unknown364 = 2;
            break;
        case 2:
            m_unknown360 = 0.4f;
            m_unknown364 = 0;
            m_balance = 0;
            memcpy(m_grid, m_previousGrid, sizeof(m_grid));
            break;
        }
    }
}

void CObstacleAvoidance::PrepareForAvoidance(CAIFilterRealPosition& position) {
    m_line = 0;
    m_unknown360 = 0.4f;
    m_unknown364 = 0;
    float x;
    float z = position.z;
    float y = position.y;
    x = position.x;
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    m_unknown398 = 0;
    m_unknown39c = 0;
    m_balance = 0;
    m_range = 112.5f;
    m_scale = 1.0f;
    m_counters[0] = 0;
    m_counters[1] = 0;
    m_counters[2] = 0;
    CObstacleGridEntry* entry = m_previousGrid;
    CObstacleGridEntry* end = entry + 13;
    for (; entry < end; entry++)
        entry->Reset(m_range, m_range, m_position);
    memcpy(m_grid, m_previousGrid, sizeof(m_grid));
    m_unknown358 = 4.0f;
    m_unknown35c = 4.0f;
    m_unknown38c[0] = 0.0f;
    m_unknown38c[1] = 0.0f;
    m_unknown38c[2] = 0.0f;
}

CObstacleAvoidance::~CObstacleAvoidance() {
}

CObstacleAvoidance::CObstacleAvoidance() {
    if (!f_bObstacleAvoidanceLinesInitialized) {
        f_rv3ObstacleAvoidanceLines[0].x = -15.0f;
        f_rv3ObstacleAvoidanceLines[0].y = -5.625f;
        f_rv3ObstacleAvoidanceLines[0].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[1].x = -11.25f;
        f_rv3ObstacleAvoidanceLines[1].y = 0.0f;
        f_rv3ObstacleAvoidanceLines[1].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[2].x = -9.375f;
        f_rv3ObstacleAvoidanceLines[2].y = 4.5f;
        f_rv3ObstacleAvoidanceLines[2].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[3].x = -7.5f;
        f_rv3ObstacleAvoidanceLines[3].y = 7.5f;
        f_rv3ObstacleAvoidanceLines[3].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[4].x = -3.75f;
        f_rv3ObstacleAvoidanceLines[4].y = 7.5f;
        f_rv3ObstacleAvoidanceLines[4].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[5].x = 0.0f;
        f_rv3ObstacleAvoidanceLines[5].y = 7.5f;
        f_rv3ObstacleAvoidanceLines[5].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[6].x = 3.75f;
        f_rv3ObstacleAvoidanceLines[6].y = 7.5f;
        f_rv3ObstacleAvoidanceLines[6].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[7].x = 7.5f;
        f_rv3ObstacleAvoidanceLines[7].y = 7.5f;
        f_rv3ObstacleAvoidanceLines[7].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[8].x = 9.375f;
        f_rv3ObstacleAvoidanceLines[8].y = 4.5f;
        f_rv3ObstacleAvoidanceLines[8].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[9].x = 11.25f;
        f_rv3ObstacleAvoidanceLines[9].y = 0.0f;
        f_rv3ObstacleAvoidanceLines[9].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[10].x = 15.0f;
        f_rv3ObstacleAvoidanceLines[10].y = -5.625f;
        f_rv3ObstacleAvoidanceLines[10].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[11].x = -1.0f;
        f_rv3ObstacleAvoidanceLines[11].y = 2.5f;
        f_rv3ObstacleAvoidanceLines[11].z = 0.0f;
        f_rv3ObstacleAvoidanceLines[12].x = 1.0f;
        f_rv3ObstacleAvoidanceLines[12].y = 2.5f;
        f_rv3ObstacleAvoidanceLines[12].z = 0.0f;
        f_rrObstacleAvoidanceLineAngleMin[0] = 1.7671459f;
        f_rrObstacleAvoidanceLineAngleMax[0] = 3.1415927f;
        f_rrObstacleAvoidanceLineAngleMin[1] = 1.3744467f;
        f_rrObstacleAvoidanceLineAngleMax[1] = 1.7671459f;
        f_rrObstacleAvoidanceLineAngleMin[2] = 0.9817477f;
        f_rrObstacleAvoidanceLineAngleMax[2] = 1.3744467f;
        f_rrObstacleAvoidanceLineAngleMin[3] = 0.5890486f;
        f_rrObstacleAvoidanceLineAngleMax[3] = 0.9817477f;
        f_rrObstacleAvoidanceLineAngleMin[4] = 0.19634955f;
        f_rrObstacleAvoidanceLineAngleMax[4] = 0.5890486f;
        f_rrObstacleAvoidanceLineAngleMin[5] = -0.19634955f;
        f_rrObstacleAvoidanceLineAngleMax[5] = 0.19634955f;
        f_rrObstacleAvoidanceLineAngleMin[6] = -0.5890486f;
        f_rrObstacleAvoidanceLineAngleMax[6] = -0.19634955f;
        f_rrObstacleAvoidanceLineAngleMin[7] = -0.9817477f;
        f_rrObstacleAvoidanceLineAngleMax[7] = -0.5890486f;
        f_rrObstacleAvoidanceLineAngleMin[8] = -1.3744467f;
        f_rrObstacleAvoidanceLineAngleMax[8] = -0.9817477f;
        f_rrObstacleAvoidanceLineAngleMin[9] = -1.7671459f;
        f_rrObstacleAvoidanceLineAngleMax[9] = -1.3744467f;
        f_rrObstacleAvoidanceLineAngleMin[10] = -3.1415927f;
        f_rrObstacleAvoidanceLineAngleMax[10] = -1.7671459f;
        f_bObstacleAvoidanceLinesInitialized = true;
    }
}

CObstacleGridEntry::~CObstacleGridEntry() {
}

CObstacleGridEntry::CObstacleGridEntry() {
}
