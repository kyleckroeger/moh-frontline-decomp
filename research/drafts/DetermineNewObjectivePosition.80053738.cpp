struct BSObject;
void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealEulerDirection {
public:
    void CalculateAngleDiff(const CAIFilterRealPosition&, const CAIFilterRealPosition&, float*, float*) const;

    float m_angles[3];
};

class CAIFilterObject {
public:
    unsigned char unknown00[12];
    BSObject* m_script;
};

class CAIObject {
public:
    unsigned char unknown00[8];
    CAIFilterObject* m_filter;
    unsigned char unknown0c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown24[60];
    CAIFilterRealEulerDirection m_direction;
};

class CObstacleGridEntry {
public:
    CObstacleGridEntry();
    ~CObstacleGridEntry();
    int GetMaskB() const {
        int mask = m_blocked[2] ? 1 : 0;
        if (m_blocked[1])
            mask += 2;
        if (m_blocked[0])
            mask += 4;
        return mask;
    }
    int GetMaskC() const {
        int mask = 0;
        if (m_blocked[2] != false)
            mask = 1;
        if (m_blocked[1])
            mask += 2;
        if (m_blocked[0])
            mask += 4;
        return mask;
    }
    int GetMaskD() const {
        int mask = 0;
        bool b = m_blocked[2];
        if (b)
            mask = 1;
        if (m_blocked[1])
            mask += 2;
        if (m_blocked[0])
            mask += 4;
        return mask;
    }
    int GetMaskT() const {
        int mask = 0;
        if ((int)m_blocked[2] != 0)
            mask = 1;
        if (m_blocked[1])
            mask += 2;
        if (m_blocked[0])
            mask += 4;
        return mask;
    }
    int GetBlockedMask() const {
        int mask = 0;
        if (m_blocked[2])
            mask = 1;
        if (m_blocked[1])
            mask += 2;
        if (m_blocked[0])
            mask += 4;
        return mask;
    }

    float m_distance[3];
    bool m_blocked[3];
    unsigned char unknown0f[1];
    CAIFilterRealPosition m_position;
    unsigned char unknown1c[4];
};

extern float f_rrObstacleAvoidanceLineAngleMin[11];
extern float f_rrObstacleAvoidanceLineAngleMax[11];

inline int FindLine(float angle) {
    for (int i = 0; i < 11; i++) {
        if (angle >= f_rrObstacleAvoidanceLineAngleMin[i] && angle <= f_rrObstacleAvoidanceLineAngleMax[i])
            return i;
    }
    return 5;
}

inline void CopyPosition(CAIFilterRealPosition& out, const CAIFilterRealPosition& in) {
    float y;
    float z = in.z;
    y = in.y;
    float x = in.x;
    out.x = x;
    out.y = y;
    out.z = z;
}

class CObstacleAvoidance {
public:
    void DetermineNewObjectivePosition(CAIFilterRealPosition&);
    bool HasClearance() const {
        if (0.0f == m_unknown38c[2])
            return false;
        return true;
    }
    bool IsClear(float distance, int line) const {
        if (distance < m_grid[line].m_distance[1] && distance < m_grid[line].m_distance[2])
            return true;
        return false;
    }
    bool IsFree(int mask) const {
        if (mask == 0)
            return true;
        return false;
    }
    bool ShouldAvoid(int line, int mask) const {
        bool has = HasClearance();
        if (!has)
            return false;
        if (line != 5)
            return false;
        switch (mask) {
        case 0:
        case 1:
        case 5:
        case 7:
            return false;
        case 2:
        case 3:
        case 6:
            return false;
        }
        if (m_grid[line].m_distance[0] < 0.25f + m_unknown38c[2])
            return false;
        return true;
    }

    short m_unknown000;
    short m_unknown002;
    short m_unknown004;
    short m_unknown006;
    short m_unknown008;
    unsigned char unknown00a[2];
    float m_range;
    float m_unknown010;
    unsigned char unknown014[4];
    CObstacleGridEntry m_grid[13];
    CObstacleGridEntry m_previousGrid[13];
    float m_unknown358;
    float m_unknown35c;
    float m_unknown360;
    int m_unknown364;
    CAIFilterRealPosition m_position;
    unsigned char unknown374[4];
    CAIFilterRealPosition m_avoidPosition;
    unsigned char unknown384[4];
    CAIObject* m_owner;
    float m_unknown38c[3];
    int m_state;
    int m_side;
};

void CObstacleAvoidance::DetermineNewObjectivePosition(CAIFilterRealPosition& out) {
    switch (m_state) {
    case 3:
    default:
        CopyPosition(out, m_position);
        break;
    case 4:
        CopyPosition(out, m_position);
        if (m_owner->m_position.GetDistanceSquaredXYReal(m_avoidPosition) < 0.25f + m_unknown38c[2]) {
            m_state = 3;
            BSObjectTriggerEvent(m_owner->m_filter->m_script, 144, 0, 0, false);
        }
        break;
    case 0: {
        float distance = m_owner->m_position.GetDistanceSquaredXYReal(m_position);
        float other;
        float angle;
        m_owner->m_direction.CalculateAngleDiff(m_owner->m_position, m_position, &angle, &other);
        int line = FindLine(angle);
        bool clear;
        if (IsClear(distance, line)) {
            clear = true;
        } else {
            int mask = m_grid[line].GetBlockedMask();
            if (ShouldAvoid(line, mask)) {
                m_state = 4;
                clear = true;
                CopyPosition(m_avoidPosition, m_grid[line].m_position);
            } else {
                if (mask == 0)
                    clear = true;
                else
                    clear = false;
            }
        }
        if (clear) {
            CopyPosition(out, m_position);
            m_side = 0;
            return;
        }
        int low = line;
        int next = -1;
        if (m_side == 1) {
            if (line > 0) {
                low = next = line - 1;
            }
        } else if (m_side == 2) {
            if (line < 10) {
                line = next = line + 1;
            }
        }
        if (next != -1 && m_grid[next].GetMaskT() == 0) {
            CopyPosition(out, m_grid[next].m_position);
            return;
        }
        do {
            if (low > 0) {
                low--;
                if (m_grid[low].GetMaskT() == 0) {
                    CopyPosition(out, m_grid[low].m_position);
                    return;
                }
            }
            if (line < 10) {
                line++;
                if (m_grid[line].GetMaskT() == 0) {
                    CopyPosition(out, m_grid[line].m_position);
                    return;
                }
            }
        } while (low != 0 || line != 10);
        CopyPosition(out, m_position);
        break;
    }
    }
}
