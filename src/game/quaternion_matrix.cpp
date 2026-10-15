// The end of quaternion.cpp (0x800917f0): CQuaternion::Slerp interpolates
// between two quaternions (taking the shorter arc, falling back to a linear
// blend when they are nearly equal), SetFromMatrix converts a rotation matrix
// (the usual trace / largest-diagonal method with a next-index table and
// component pointers) and normalises, GetMatrix builds the rotation matrix
// (products written as 2*a*b, which the compiler shares as doubled factors),
// Normalize divides the four components by the length (the MSL-style inline
// sqrtf, as in bsbifunc_targetsearch.cpp) and Set stores the vector part then
// the scalar. CQuaternion and CMatrix are named by the mangled symbols; the
// member layout (scalar first) follows quaternion.cpp and the matrix element
// layout is inferred. SetFromMatrix inlines Normalize, which is emitted after
// it, so this file is compiled with deferred inlining and the functions are
// defined in reverse image order. Multiply, before this range, is not part of
// this unit.
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

extern "C" double acos(double);
extern "C" double sin(double);

class CQuaternion {
public:
    void Slerp(CQuaternion, CQuaternion, float);
    void SetFromMatrix(const CMatrix&);
    void GetMatrix(CMatrix&) const;
    void Normalize();
    void Set(float, float, float, float);

    float w;
    float x;
    float y;
    float z;
};

void CQuaternion::Set(float nx, float ny, float nz, float nw) {
    x = nx;
    y = ny;
    z = nz;
    w = nw;
}

void CQuaternion::Normalize() {
    float len = sqrtf(x * x + y * y + z * z + w * w);
    float inv = 1.0f / len;
    x *= inv;
    y *= inv;
    z *= inv;
    w *= inv;
}

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

void CQuaternion::SetFromMatrix(const CMatrix& mtx) {
    float tr = mtx.m[0][0] + mtx.m[1][1] + mtx.m[2][2];
    if (tr > 0.0f) {
        float s = sqrtf(tr + 1.0f);
        w = s * 0.5f;
        s = 0.5f / s;
        x = s * (mtx.m[2][1] - mtx.m[1][2]);
        y = s * (mtx.m[0][2] - mtx.m[2][0]);
        z = s * (mtx.m[1][0] - mtx.m[0][1]);
    } else {
        static const int next[3] = {1, 2, 0};
        int i = 0;
        if (mtx.m[1][1] > mtx.m[0][0]) {
            i = 1;
        }
        if (mtx.m[2][2] > mtx.m[i][i]) {
            i = 2;
        }
        int j = next[i];
        int k = next[j];
        float s = sqrtf(mtx.m[i][i] - mtx.m[j][j] - mtx.m[k][k] + 1.0f);
        float* q[3] = {&x, &y, &z};
        *q[i] = s * 0.5f;
        s = 0.5f / s;
        *q[j] = s * (mtx.m[j][i] + mtx.m[i][j]);
        *q[k] = s * (mtx.m[k][i] + mtx.m[i][k]);
        w = s * (mtx.m[k][j] - mtx.m[j][k]);
    }
    Normalize();
}

void CQuaternion::Slerp(CQuaternion from, CQuaternion to, float t) {
    float cosom, omega, sinom, scale0, scale1;
    cosom = to.x * from.x + to.y * from.y + to.z * from.z + to.w * from.w;
    if (cosom < 0.0f) {
        cosom = -cosom;
        x = -from.x;
        y = -from.y;
        z = -from.z;
        w = -from.w;
    } else {
        x = from.x;
        y = from.y;
        z = from.z;
        w = from.w;
    }
    if (1.0f - cosom > 0.0005f) {
        omega = acos(cosom);
        sinom = sin(omega);
        scale0 = (float)sin((1.0f - t) * omega) / sinom;
        scale1 = (float)sin(t * omega) / sinom;
    } else {
        scale0 = 1.0f - t;
        scale1 = t;
    }
    x = scale0 * to.x + scale1 * x;
    y = scale0 * to.y + scale1 * y;
    z = scale0 * to.z + scale1 * z;
    w = scale0 * to.w + scale1 * w;
}

