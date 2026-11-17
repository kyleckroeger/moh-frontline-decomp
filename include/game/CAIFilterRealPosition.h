// CAIFilterRealPosition: an AI filter position (x, y, z), shared by every file that uses it
// (PortReadiness step 3.3).
//
// From the original: the four distance members are out of line (AIFilter.cpp); the default
// constructor is a weak empty copy (__ct__21CAIFilterRealPositionFv, 4 bytes), so it is inline here.
// 12 bytes in 35 matched views. 12 views make it 16 by deriving from CVector3, which no symbol shows.
//
// Inferred, not in any mangled name: the member names (x, y, z, as 33 matched views spell them).
#ifndef GAME_CAIFILTERREALPOSITION_H
#define GAME_CAIFILTERREALPOSITION_H

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition() {}
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

#endif
