// A fragment of box.cpp (0x800c37d0): CVolBox::TestCollision against another
// box, followed by the weak CVector3::Cross (the cross product of the
// arguments into this vector), box.cpp's local Sign (+1, -1 or 0 with the
// argument's sign, built from its bits; the pointer casts keep the original's
// store of the result back to the argument's stack slot), the weak
// CVector3::Abs (each coordinate of the argument through fabs into this
// vector) and the double-dispatch test against IVolume (swapped order, the
// other volume tests this box).
// The box test is the separating-axis test of two oriented boxes: the centre
// offset and the other box's axes expressed in this box's frame (rotation
// terms and their absolute values through CVector3::DotDotDot and Abs), the
// three face axes of each box and the nine edge-pair axes (skipped when the
// projection is zero) through box.cpp's static BoxBoxTest, which keeps the
// shallowest overlap, its sign and its axis. A separated axis marks the
// collision as missed. Otherwise the depth is classified against
// CONTACT_EPSILON and a touching contact gets the axis as its normal and a
// point: the other box's corner for this box's faces, this box's corner for
// the other box's faces, and for an edge pair the closest point of this box's
// edge to the other's (the thirteenth axis divides by 1 - R10 * R10, as the
// original does). The class layouts, the axis numbering, the local names and
// the helper macros are inferred; the statement order inside the tests and the
// per-case scope of the edge-case temporaries follow the original's register
// allocation. The other functions' result types and the vector view are
// inferred; the virtual functions are declared in the order of __vt__7IVolume.
#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }

union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    void Cross(const CVector3&, const CVector3&);
    void Abs(const CVector3&);
    void Sub(const CVector3&, const CVector3&);
    void Scale(const CVector3&, float);
    float Normalize();
    void DotDotDot(const CVector3&, const CVector3&, const CVector3&, const CVector3&);
    CVector3& operator*=(float k) {
        d.c.x *= k;
        d.c.y *= k;
        d.c.z *= k;
        return *this;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

struct VECTOR3VIEW {
    double pair[2];
};

class CMatrix {
public:
    void TransformPoint(CVector3&, CVector3) const;

    CVector3 GetRow(int i) const { return *(const CVector3*)&m_rows[i]; }

    VECTOR3VIEW m_rows[4];
} __attribute__((aligned(16)));

class CTriangle;
class CPlane;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;
class IVolume;

struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    const IVolume* m_object;
} __attribute__((aligned(16)));

class CCollision {
public:
    void SwapOrder();
    void AddContact(const SClsnContact&);

    unsigned char unknown00[16];
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
};

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
};

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;

    CVector3 GetCenter() const { return *(const CVector3*)&m_matrix.m_rows[3]; }
    CVector3 GetAxis(int i) const { return *(const CVector3*)&m_matrix.m_rows[i]; }

    float m_half[3];
    CMatrix m_matrix;
};

enum EAxis {};

extern const float CONTACT_EPSILON;

inline int Compare(float separation) {
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

static float Sign(float x);
static bool BoxBoxTest(float extent, float projection, EAxis axis, float& depth, float& sign, EAxis& bestAxis);

#define FAIL() \
    { \
        if (flag && collision.m_result < 1) \
            collision.m_result = 1; \
        return 1; \
    }

#define TEST(a, b, p, n) \
    ra = a; \
    rb = b; \
    if (!BoxBoxTest(ra + rb, p, (EAxis)n, depth, sign, axis)) \
        FAIL();

#define ETEST(a, b, p, n) \
    ra = a; \
    rb = b; \
    proj = p; \
    if (proj != 0.0f && !BoxBoxTest(ra + rb, proj, (EAxis)n, depth, sign, axis)) \
        FAIL();

int CVolBox::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    const CMatrix& ma = m_matrix;
    const CMatrix& mb = other.m_matrix;
    SClsnContact contact;
    CVector3 d;
    CVector3 r;
    CVector3 a0;
    CVector3 a1;
    CVector3 a2;
    CVector3 b0;
    CVector3 b1;
    CVector3 b2;
    CVector3 p;
    float sign;
    EAxis axis;
    float depth;
    float proj;
    float ra;
    float rb;
    int code;
    d.Sub(mb.GetRow(3), ma.GetRow(3));
    axis = (EAxis)0;
    depth = 3.4028235e38f;
    a0 = GetAxis(0);
    a1 = GetAxis(1);
    a2 = GetAxis(2);
    b0 = other.GetAxis(0);
    b1 = other.GetAxis(1);
    b2 = other.GetAxis(2);
    r.DotDotDot(a0, a1, a2, d);
    float tx = r.d.c.x;
    float ty = r.d.c.y;
    float tz = r.d.c.z;
    r.DotDotDot(b0, b1, b2, d);
    float ux = r.d.c.x;
    float uy = r.d.c.y;
    float uz = r.d.c.z;
    r.DotDotDot(b0, b1, b2, a0);
    float R00 = r.d.c.x;
    float R01 = r.d.c.y;
    float R02 = r.d.c.z;
    r.Abs(r);
    float A00 = r.d.c.x;
    float A01 = r.d.c.y;
    float A02 = r.d.c.z;
    r.DotDotDot(b0, b1, b2, a1);
    float R10 = r.d.c.x;
    float R11 = r.d.c.y;
    float R12 = r.d.c.z;
    r.Abs(r);
    float A10 = r.d.c.x;
    float A11 = r.d.c.y;
    float A12 = r.d.c.z;
    r.DotDotDot(b0, b1, b2, a2);
    float R20 = r.d.c.x;
    float R21 = r.d.c.y;
    float R22 = r.d.c.z;
    r.Abs(r);
    float A20 = r.d.c.x;
    float A21 = r.d.c.y;
    float A22 = r.d.c.z;

    TEST(m_half[0], other.m_half[0] * A00 + other.m_half[1] * A01 + other.m_half[2] * A02, tx, 1);
    TEST(m_half[1], other.m_half[0] * A10 + other.m_half[1] * A11 + other.m_half[2] * A12, ty, 2);
    TEST(m_half[2], other.m_half[0] * A20 + other.m_half[1] * A21 + other.m_half[2] * A22, tz, 3);
    TEST(m_half[0] * A00 + m_half[1] * A10 + m_half[2] * A20, other.m_half[0], ux, 4);
    TEST(m_half[0] * A01 + m_half[1] * A11 + m_half[2] * A21, other.m_half[1], uy, 5);
    TEST(m_half[0] * A02 + m_half[1] * A12 + m_half[2] * A22, other.m_half[2], uz, 6);

    ETEST(m_half[1] * A20 + m_half[2] * A10, other.m_half[1] * A02 + other.m_half[2] * A01, R10 * tz - R20 * ty, 7);
    ETEST(m_half[1] * A21 + m_half[2] * A11, other.m_half[0] * A02 + other.m_half[2] * A00, R11 * tz - R21 * ty, 8);
    ETEST(m_half[1] * A22 + m_half[2] * A12, other.m_half[0] * A01 + other.m_half[1] * A00, R12 * tz - R22 * ty, 9);
    ETEST(m_half[0] * A20 + m_half[2] * A00, other.m_half[1] * A12 + other.m_half[2] * A11, R20 * tx - R00 * tz, 10);
    ETEST(m_half[0] * A21 + m_half[2] * A01, other.m_half[0] * A12 + other.m_half[2] * A10, R21 * tx - R01 * tz, 11);
    ETEST(m_half[0] * A22 + m_half[2] * A02, other.m_half[0] * A11 + other.m_half[1] * A10, R22 * tx - R02 * tz, 12);
    ETEST(m_half[0] * A10 + m_half[1] * A00, other.m_half[1] * A22 + other.m_half[2] * A21, R00 * ty - R10 * tx, 13);
    ETEST(m_half[0] * A11 + m_half[1] * A01, other.m_half[0] * A22 + other.m_half[2] * A20, R01 * ty - R11 * tx, 14);
    ETEST(m_half[0] * A12 + m_half[1] * A02, other.m_half[0] * A21 + other.m_half[1] * A20, R02 * ty - R12 * tx, 15);

    code = Compare(depth);
    if (flag) {
        if (code > collision.m_result)
            collision.m_result = code;
        if (code == 2) {
            contact.m_separation = depth;
            switch (axis) {
            case 1:
                {
                contact.m_vector0.Scale(a0, sign);
                p.d.c.x = other.m_half[0] * (-sign * Sign(R00));
                p.d.c.y = other.m_half[1] * (-sign * Sign(R01));
                p.d.c.z = other.m_half[2] * (-sign * Sign(R02));
                mb.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 2:
                {
                contact.m_vector0.Scale(a1, sign);
                p.d.c.x = other.m_half[0] * (-sign * Sign(R10));
                p.d.c.y = other.m_half[1] * (-sign * Sign(R11));
                p.d.c.z = other.m_half[2] * (-sign * Sign(R12));
                mb.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 3:
                {
                contact.m_vector0.Scale(a2, sign);
                p.d.c.x = other.m_half[0] * (-sign * Sign(R20));
                p.d.c.y = other.m_half[1] * (-sign * Sign(R21));
                p.d.c.z = other.m_half[2] * (-sign * Sign(R22));
                mb.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 4:
                {
                contact.m_vector0.Scale(b0, sign);
                p.d.c.x = m_half[0] * (sign * Sign(R00));
                p.d.c.y = m_half[1] * (sign * Sign(R10));
                p.d.c.z = m_half[2] * (sign * Sign(R20));
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 5:
                {
                contact.m_vector0.Scale(b1, sign);
                p.d.c.x = m_half[0] * (sign * Sign(R01));
                p.d.c.y = m_half[1] * (sign * Sign(R11));
                p.d.c.z = m_half[2] * (sign * Sign(R21));
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 6:
                {
                contact.m_vector0.Scale(b2, sign);
                p.d.c.x = m_half[0] * (sign * Sign(R02));
                p.d.c.y = m_half[1] * (sign * Sign(R12));
                p.d.c.z = m_half[2] * (sign * Sign(R22));
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 7:
                {
                contact.m_vector0.Cross(a0, b0);
                contact.m_vector0.Normalize();
                contact.m_vector0 *= sign;
                p.d.c.y = m_half[1] * (-sign * Sign(R20));
                p.d.c.z = m_half[2] * (sign * Sign(R10));
                float qy = other.m_half[1] * (-sign * Sign(R02));
                float qz = other.m_half[2] * (sign * Sign(R01));
                float den = 1.0f - R00 * R00;
                if ((float)fabs(den) > 1e-5f)
                    p.d.c.x = (tx + R00 * (R10 * p.d.c.y + R20 * p.d.c.z - ux) + R01 * qy + R02 * qz) / den;
                else
                    p.d.c.x = 0.0f;
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 8:
                {
                contact.m_vector0.Cross(a0, b1);
                contact.m_vector0.Normalize();
                contact.m_vector0 *= sign;
                p.d.c.y = m_half[1] * (-sign * Sign(R21));
                p.d.c.z = m_half[2] * (sign * Sign(R11));
                float qx = other.m_half[0] * (sign * Sign(R02));
                float qz = other.m_half[2] * (-sign * Sign(R00));
                float den = 1.0f - R01 * R01;
                if ((float)fabs(den) > 1e-5f)
                    p.d.c.x = (tx + R01 * (R11 * p.d.c.y + R21 * p.d.c.z - uy) + R00 * qx + R02 * qz) / den;
                else
                    p.d.c.x = 0.0f;
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 9:
                {
                contact.m_vector0.Cross(a0, b2);
                contact.m_vector0.Normalize();
                contact.m_vector0 *= sign;
                p.d.c.y = m_half[1] * (-sign * Sign(R22));
                p.d.c.z = m_half[2] * (sign * Sign(R12));
                float qx = other.m_half[0] * (-sign * Sign(R01));
                float qy = other.m_half[1] * (sign * Sign(R00));
                float den = 1.0f - R02 * R02;
                if ((float)fabs(den) > 1e-5f)
                    p.d.c.x = (tx + R02 * (R12 * p.d.c.y + R22 * p.d.c.z - uz) + R00 * qx + R01 * qy) / den;
                else
                    p.d.c.x = 0.0f;
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 10:
                {
                contact.m_vector0.Cross(a1, b0);
                contact.m_vector0.Normalize();
                contact.m_vector0 *= sign;
                p.d.c.x = m_half[0] * (sign * Sign(R20));
                p.d.c.z = m_half[2] * (-sign * Sign(R00));
                float qy = other.m_half[1] * (-sign * Sign(R12));
                float qz = other.m_half[2] * (sign * Sign(R11));
                float den = 1.0f - R10 * R10;
                if ((float)fabs(den) > 1e-5f)
                    p.d.c.y = (ty + R10 * (R00 * p.d.c.x + R20 * p.d.c.z - ux) + R11 * qy + R12 * qz) / den;
                else
                    p.d.c.y = 0.0f;
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 11:
                {
                contact.m_vector0.Cross(a1, b1);
                contact.m_vector0.Normalize();
                contact.m_vector0 *= sign;
                p.d.c.x = m_half[0] * (sign * Sign(R21));
                p.d.c.z = m_half[2] * (-sign * Sign(R01));
                float qx = other.m_half[0] * (sign * Sign(R12));
                float qz = other.m_half[2] * (-sign * Sign(R10));
                float den = 1.0f - R11 * R11;
                if ((float)fabs(den) > 1e-5f)
                    p.d.c.y = (ty + R11 * (R01 * p.d.c.x + R21 * p.d.c.z - uy) + R10 * qx + R12 * qz) / den;
                else
                    p.d.c.y = 0.0f;
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 12:
                {
                contact.m_vector0.Cross(a1, b2);
                contact.m_vector0.Normalize();
                contact.m_vector0 *= sign;
                p.d.c.x = m_half[0] * (sign * Sign(R22));
                p.d.c.z = m_half[2] * (-sign * Sign(R02));
                float qx = other.m_half[0] * (-sign * Sign(R11));
                float qy = other.m_half[1] * (sign * Sign(R10));
                float den = 1.0f - R12 * R12;
                if ((float)fabs(den) > 1e-5f)
                    p.d.c.y = (ty + R12 * (R02 * p.d.c.x + R22 * p.d.c.z - uz) + R10 * qx + R11 * qy) / den;
                else
                    p.d.c.y = 0.0f;
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 13:
                {
                contact.m_vector0.Cross(a2, b0);
                contact.m_vector0.Normalize();
                contact.m_vector0 *= sign;
                p.d.c.x = m_half[0] * (-sign * Sign(R10));
                p.d.c.y = m_half[1] * (sign * Sign(R00));
                float qy = other.m_half[1] * (-sign * Sign(R22));
                float qz = other.m_half[2] * (sign * Sign(R21));
                float den = 1.0f - R10 * R10;
                if ((float)fabs(den) > 1e-5f)
                    p.d.c.z = (tz + R20 * (R00 * p.d.c.x + R10 * p.d.c.y - ux) + R21 * qy + R22 * qz) / den;
                else
                    p.d.c.z = 0.0f;
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 14:
                {
                contact.m_vector0.Cross(a2, b1);
                contact.m_vector0.Normalize();
                contact.m_vector0 *= sign;
                p.d.c.x = m_half[0] * (-sign * Sign(R11));
                p.d.c.y = m_half[1] * (sign * Sign(R01));
                float qx = other.m_half[0] * (sign * Sign(R22));
                float qz = other.m_half[2] * (-sign * Sign(R20));
                float den = 1.0f - R21 * R21;
                if ((float)fabs(den) > 1e-5f)
                    p.d.c.z = (tz + R21 * (R01 * p.d.c.x + R11 * p.d.c.y - uy) + R20 * qx + R22 * qz) / den;
                else
                    p.d.c.z = 0.0f;
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            case 15:
                {
                contact.m_vector0.Cross(a2, b2);
                contact.m_vector0.Normalize();
                contact.m_vector0 *= sign;
                p.d.c.x = m_half[0] * (-sign * Sign(R12));
                p.d.c.y = m_half[1] * (sign * Sign(R02));
                float qx = other.m_half[0] * (-sign * Sign(R21));
                float qy = other.m_half[1] * (sign * Sign(R20));
                float den = 1.0f - R22 * R22;
                if ((float)fabs(den) > 1e-5f)
                    p.d.c.z = (tz + R22 * (R02 * p.d.c.x + R12 * p.d.c.y - uz) + R20 * qx + R21 * qy) / den;
                else
                    p.d.c.z = 0.0f;
                ma.TransformPoint(contact.m_vector1, p);
                }
                break;
            }
            contact.m_object = 0;
            collision.AddContact(contact);
        }
    }
    return code;
}

__declspec(weak) void CVector3::Cross(const CVector3& a, const CVector3& b) {
    d.c.x = a.d.c.y * b.d.c.z - a.d.c.z * b.d.c.y;
    d.c.y = a.d.c.z * b.d.c.x - a.d.c.x * b.d.c.z;
    d.c.z = a.d.c.x * b.d.c.y - a.d.c.y * b.d.c.x;
}

static float Sign(float x) {
    unsigned int r = *(unsigned int*)&x;
    if (r & 0x7f800000) {
        r &= 0x80000000;
        r |= 0x3f800000;
    } else {
        r = 0;
    }
    x = *(float*)&r;
    return x;
}

__declspec(weak) void CVector3::Abs(const CVector3& v) {
    d.c.x = __fabs(v.d.c.x);
    d.c.y = __fabs(v.d.c.y);
    d.c.z = __fabs(v.d.c.z);
}

int CVolBox::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

// box.cpp's BoxBoxTest, compiled here only so this unit's calls bind to it
// (its original copy follows CVolBox's destructor).
static bool BoxBoxTest(float extent, float projection, EAxis axis, float& depth, float& sign, EAxis& bestAxis) {
    float d = (float)__fabs(projection) - extent;
    if (Compare(d) != 1) {
        if (d < depth) {
            depth = d;
            sign = Sign(projection);
            bestAxis = axis;
        }
        return true;
    }
    return false;
}
