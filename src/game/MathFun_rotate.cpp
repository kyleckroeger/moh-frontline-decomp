// The end of MathFun.cpp (0x80092ee4): MathFunGetTiltAngleDiffNoRoll and
// MathFunGetPanAngleDiffNoRoll (the target direction relative to the facing,
// the facing turned out of the pair first; the vector difference is taken in
// place), MathFunNormalizeAngleNegativePiToPi
// (removes whole turns, then brings the angle into -pi..pi),
// MathFunRotateAboutZ and MathFunRotateAboutY (rotate a coordinate pair in
// place by an angle, through the library's double sin and cos) and
// MathFunAtan2F (the quadrant-corrected angle of a y/x pair from atan; note
// the right half-plane uses -y). The constants are entries of the file's
// .sdata2 pool. The functions are named by their symbols; the parameter
// meanings and the CVector3 view are inferred. The angle helpers are inlined
// by the two angle differences, which the image places first, so the unit is
// compiled with deferred inlining and the functions are written in reverse
// image order.
extern "C" {
double sin(double);
double cos(double);
double atan(double);
}

#include "game/CVector3.h"

float MathFunNormalizeAngleNegativePiToPi(float);
void MathFunRotateAboutZ(float*, float*, float);
float MathFunAtan2F(float, float);

float MathFunAtan2F(float y, float x) {
    if (x > 0.0f)
        return atan(-y / x);
    if (x < 0.0f) {
        if (y < 0.0f)
            return 3.1415927f - (float)atan(y / x);
        return -3.1415927f - (float)atan(y / x);
    }
    if (y <= 0.0f)
        return 1.5707964f;
    return -1.5707964f;
}

void MathFunRotateAboutY(float* z, float* x, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    float ox = *x;
    float oz = *z;
    *x = ox * c - oz * s;
    *z = ox * s + oz * c;
}

void MathFunRotateAboutZ(float* x, float* y, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    float ox = *x;
    float oy = *y;
    *x = ox * c - oy * s;
    *y = ox * s + oy * c;
}

float MathFunNormalizeAngleNegativePiToPi(float angle) {
    angle -= 3.1415927f * (float)((int)(angle / 6.2831855f) * 2);
    if (angle > 3.1415927f)
        angle -= 6.2831855f;
    else if (angle < -3.1415927f)
        angle += 6.2831855f;
    return angle;
}

float MathFunGetPanAngleDiffNoRoll(CVector3* from, CVector3* to, CVector3* direction) {
    *to -= *from;
    float facing = MathFunAtan2F(-direction->y, direction->x);
    float target = MathFunAtan2F(to->x, to->y);
    return MathFunNormalizeAngleNegativePiToPi(target - facing);
}

float MathFunGetTiltAngleDiffNoRoll(CVector3* from, CVector3* to, CVector3* forward, CVector3* up) {
    float pan = MathFunAtan2F(-forward->y, forward->x);
    MathFunRotateAboutZ(&up->x, &up->y, -pan);
    *to -= *from;
    pan = MathFunAtan2F(to->x, to->y);
    MathFunRotateAboutZ(&to->x, &to->y, -pan);
    float facing = MathFunAtan2F(-up->z, up->y);
    float target = MathFunAtan2F(-to->z, to->y);
    return MathFunNormalizeAngleNegativePiToPi(target - facing);
}
