// The start of quaternion.cpp (0x80091420): CQuaternion::EndianSwap
// byte-swaps the vector part and then the scalar part; Rotate(out, v) rotates
// a vector by the quaternion's rotation matrix (elements built from doubled
// products, then applied as v times the matrix) and Rotate(v) rotates in
// place through a copy. CQuaternion and CVector3 are named by the mangled
// symbols; the member layout (scalar first) is inferred from the order of the
// swaps, and CVector3 is the 16-byte doubleword-copied view of
// matrix_translate.cpp. The conversion helpers are the inlined ones described
// in propdat.cpp (inferred). Rotate(v) inlines Rotate(out, v), which is
// emitted after it, so this file is compiled with deferred inlining and the
// functions are defined in reverse image order. The interpolation and matrix
// functions after them are not part of this unit.
inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

inline void EndianSwap(float& value, bool) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CQuaternion {
public:
    void EndianSwap();
    void Rotate(CVector3&);
    void Rotate(CVector3&, CVector3);

    float w;
    float x;
    float y;
    float z;
};

void CQuaternion::Rotate(CVector3& out, CVector3 v) {
    float xx = 2.0f * x * x;
    float yy = 2.0f * y * y;
    float zz = 2.0f * z * z;
    float xy = 2.0f * x * y;
    float xz = 2.0f * x * z;
    float yz = 2.0f * y * z;
    float wx = 2.0f * w * x;
    float wy = 2.0f * w * y;
    float wz = 2.0f * w * z;
    float m00 = 1.0f - (yy + zz);
    float m01 = xy - wz;
    float m02 = xz + wy;
    float m10 = xy + wz;
    float m11 = 1.0f - (xx + zz);
    float m12 = yz - wx;
    float m20 = xz - wy;
    float m21 = yz + wx;
    float m22 = 1.0f - (xx + yy);
    out.x = m00 * v.x + m10 * v.y + m20 * v.z;
    out.y = m01 * v.x + m11 * v.y + m21 * v.z;
    out.z = m02 * v.x + m12 * v.y + m22 * v.z;
}

void CQuaternion::Rotate(CVector3& v) {
    Rotate(v, v);
}

void CQuaternion::EndianSwap() {
    ::EndianSwap(x, true);
    ::EndianSwap(y, true);
    ::EndianSwap(z, true);
    ::EndianSwap(w, true);
}
