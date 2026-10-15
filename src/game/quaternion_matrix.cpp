// A fragment of quaternion.cpp (0x80091c9c): CQuaternion::GetMatrix builds
// the rotation matrix of a unit quaternion (products written as 2*a*b, which
// the compiler shares as doubled factors), Normalize divides the four
// components by the length (the MSL-style inline sqrtf, as in
// bsbifunc_targetsearch.cpp) and Set stores the vector part then the scalar.
// CQuaternion and CMatrix are named by the mangled symbols; the member layout
// (scalar first) follows quaternion.cpp and the matrix element layout is
// inferred. The functions before this range are not part of this unit.
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
        return y ;
    }
    return x;
}

class CMatrix {
public:
    float m[4][4];
};

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;
    void Normalize();
    void Set(float, float, float, float);

    float w;
    float x;
    float y;
    float z;
};

void CQuaternion::GetMatrix(CMatrix& mtx) const {
    float xx = 2.0f * x * x;
    float yy = 2.0f * y * y;
    float zz = 2.0f * z * z;
    float xy = 2.0f * x * y;
    float xz = 2.0f * x * z;
    float yz = 2.0f * y * z;
    float wx = 2.0f * w * x;
    float wy = 2.0f * w * y;
    float wz = 2.0f * w * z;
    mtx.m[0][0] = 1.0f - (yy + zz);
    mtx.m[0][1] = xy - wz;
    mtx.m[0][2] = xz + wy;
    mtx.m[0][3] = 0.0f;
    mtx.m[1][0] = xy + wz;
    mtx.m[1][1] = 1.0f - (xx + zz);
    mtx.m[1][2] = yz - wx;
    mtx.m[1][3] = 0.0f;
    mtx.m[2][0] = xz - wy;
    mtx.m[2][1] = yz + wx;
    mtx.m[2][2] = 1.0f - (xx + yy);
    mtx.m[2][3] = 0.0f;
    mtx.m[3][0] = 0.0f;
    mtx.m[3][1] = 0.0f;
    mtx.m[3][2] = 0.0f;
    mtx.m[3][3] = 1.0f;
}

void CQuaternion::Normalize() {
    float len = sqrtf(x * x + y * y + z * z + w * w);
    float inv = 1.0f / len;
    x *= inv;
    y *= inv;
    z *= inv;
    w *= inv;
}

void CQuaternion::Set(float nx, float ny, float nz, float nw) {
    x = nx;
    y = ny;
    z = nz;
    w = nw;
}
