// The end of ObstacleAvoidance.cpp (0x800544b0): the CObstacleAvoidance
// constructor (its two 13-entry grids of CObstacleGridEntry, then, once for
// all instances, the file's 13 obstacle-avoidance line end points (x, y, 0)
// and the angle range of each line sector: the minimum angles fall from 9pi/16
// to -pi in steps of pi/8 and each maximum is the previous minimum, starting
// at pi), the empty CObstacleGridEntry destructor and constructor, then the
// static initialisation: the 13 line vectors are built through CVector3's
// constructor (__construct_array). The vector's inline constructor, emitted
// for the array construction, follows the static initialisation (a weak copy
// of this file). CObstacleAvoidance, CObstacleGridEntry, CVector3 and the
// statics are named by the symbols; the array sizes come from the symbols,
// the grid entry size and grid offsets from the construction calls, and the
// vector layout is inferred. The constants are .sdata2 pool entries. The
// rest of the file is in other units.
class CVector3 {
public:
    CVector3() {}

    float x;
    float y;
    float z;
    float w;
};

class CObstacleGridEntry {
public:
    CObstacleGridEntry();
    ~CObstacleGridEntry();

    unsigned char unknown00[32];
};

class CObstacleAvoidance {
public:
    CObstacleAvoidance();

    unsigned char unknown000[24];
    CObstacleGridEntry m_grid[13];
    CObstacleGridEntry m_previousGrid[13];
};

CVector3 f_rv3ObstacleAvoidanceLines[13];
float f_rrObstacleAvoidanceLineAngleMin[11];
float f_rrObstacleAvoidanceLineAngleMax[11];
extern bool f_bObstacleAvoidanceLinesInitialized;

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
