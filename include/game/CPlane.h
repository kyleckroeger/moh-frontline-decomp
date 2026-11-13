// CPlane: a plane (unit normal and distance), shared by every file that uses it
// (PortReadiness step 3.3).
//
// Evidence: out of line in the original (plane.cpp, 0x800910ac..0x80091420):
// GetIntersection(const CLine3&, float&, CVector3*) const, Transform(const CMatrix&) and
// CPlane(CVector3, float). Weak copies: the default constructor (0x8001ad2c, empty) and the
// destructor (0x8001acf0), so both are inline and empty. 0x20 bytes: CCamera holds ten at
// +0x150..+0x290, and the matched views align it to 16.
// Inferred: the member names (plane_ctor.cpp's) and GetIntersection's result type.
#ifndef GAME_CPLANE_H
#define GAME_CPLANE_H

#include "game/CVector3.h"

class CMatrix;
class CLine3;

class CPlane {
public:
    CPlane() {}
    CPlane(CVector3, float);
    ~CPlane() {}
    unsigned char GetIntersection(const CLine3&, float&, CVector3*) const;
    void Transform(const CMatrix&);

    CVector3 m_normal;
    float m_d;
} __attribute__((aligned(16)));

#endif
