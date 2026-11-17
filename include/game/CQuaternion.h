// CQuaternion: a rotation (w, x, y, z), shared by every file that uses it (PortReadiness step 3.3).
//
// From the original: out-of-line members Set, Normalize, Multiply, Slerp, Rotate (two overloads),
// SetFromMatrix, GetMatrix and EndianSwap (quaternion.cpp); no weak copies, so nothing here is
// inline but the constructors. 16 bytes in every matched view (research/layout/classes.tsv).
//
// Inferred, not in any mangled name: the member names and order (w first, as 29 matched views
// declare them) and the four-float constructor's argument order (x, y, z, w), as the matched views
// that construct one have it.
#ifndef GAME_CQUATERNION_H
#define GAME_CQUATERNION_H

class CVector3;
class CMatrix;

class CQuaternion {
public:
    CQuaternion() {}
    CQuaternion(float ax, float ay, float az, float aw) : w(aw), x(ax), y(ay), z(az) {}
    void Set(float, float, float, float);
    void Normalize();
    void Multiply(CQuaternion);
    void Slerp(CQuaternion, CQuaternion, float);
    void Rotate(CVector3&);
    void Rotate(CVector3&, CVector3);
    void SetFromMatrix(const CMatrix&);
    void GetMatrix(CMatrix&) const;
    void EndianSwap();

    float w;
    float x;
    float y;
    float z;
};

#endif
