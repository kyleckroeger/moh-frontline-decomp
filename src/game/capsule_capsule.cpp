// A fragment of capsule.cpp (0x800c69b0): CVolCapsule::TestCollision against
// another capsule. The other axis (a copy of its line) is projected on this
// axis at both ends and the segments' squared distance taken; beyond the
// summed radii (plus CONTACT_EPSILON) they miss (1). When the other axis lies
// wholly past this axis's end (or before its start) the separation is the
// nearer end's distance along the axis (this axis's cached length), otherwise
// the segment distance less the radii; 1/3/2 against CONTACT_EPSILON. When
// asked, a touch raises the result and a contact (2) is added: along the axis
// the (reversed past the end) direction at the start or end, otherwise the
// unit direction from the projected point to the other start and the point
// on this capsule's surface. The file name is this project's; the original
// record is capsule.cpp. The classes and functions are named by the mangled
// symbols (the line has an inline destructor whose weak copy, used by the
// exception table, is linked from elsewhere); the members, the cached-length
// line layout, the vector helpers and
// the result codes are inferred. Constants are the file's .sdata2 pool
// entries. The rest of the file is in other units.
#include <math.h>

class IVolume;

// Inferred: CVector3 as four floats overlaid with two doubles.
union CVector3Data {
    double pair[2];
    float v[4];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3 operator=(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
        return *this;
    }
    void Cross(const CVector3& a, const CVector3& b) {
        d.v[0] = a.d.v[1] * b.d.v[2] - a.d.v[2] * b.d.v[1];
        d.v[1] = a.d.v[2] * b.d.v[0] - a.d.v[0] * b.d.v[2];
        d.v[2] = a.d.v[0] * b.d.v[1] - a.d.v[1] * b.d.v[0];
    }
    void Scale(const CVector3& v, float k) {
        d.c.x = k * v.d.c.x;
        d.c.y = k * v.d.c.y;
        d.c.z = k * v.d.c.z;
    }
    CVector3& operator+=(const CVector3& o) {
        d.c.x += o.d.c.x;
        d.c.y += o.d.c.y;
        d.c.z += o.d.c.z;
        return *this;
    }
    void Normalize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            d.c.x *= 1.0f / length;
            d.c.y *= 1.0f / length;
            d.c.z *= 1.0f / length;
        }
    }
    CVector3& operator*=(float k) {
        d.c.x *= k;
        d.c.y *= k;
        d.c.z *= k;
        return *this;
    }
    float Dot(const CVector3& o) const { return d.v[0] * o.d.v[0] + d.v[1] * o.d.v[1] + d.v[2] * o.d.v[2]; }
    float LengthSq() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }

    CVector3Data d;
} __attribute__((aligned(8)));

inline void SetDifference(CVector3& r, const CVector3& a, const CVector3& b) {
    r.d.v[0] = a.d.v[0] - b.d.v[0];
    r.d.v[1] = a.d.v[1] - b.d.v[1];
    r.d.v[2] = a.d.v[2] - b.d.v[2];
}

// Inferred: the line with its lazily cached length and squared length.
class CLine3 {
public:
    CVector3 GetStart() const { return m_start; }
    CVector3 GetDirection() const { return m_dir; }
    CVector3 GetEnd() const { return m_end; }
    void GetProjection(CVector3, CVector3&, float&) const;
    float GetSquaredDistanceBetweenSegments(const CLine3&, float&, float&, float*) const;
    float GetLengthSq() const {
        if (!m_lengthSqValid) {
            m_lengthSq = m_dir.LengthSq();
            m_lengthSqValid = 1;
        }
        return m_lengthSq;
    }
    float GetLength() const {
        if (!m_lengthValid) {
            m_length = sqrtf(GetLengthSq());
            m_lengthValid = 1;
        }
        return m_length;
    }

    ~CLine3() {}
    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    mutable float m_length;
    mutable float m_lengthSq;
    mutable unsigned char m_lengthValid;
    mutable unsigned char m_lengthSqValid;
};

// Inferred contact record view: two vectors, the separation, an index and
// an object.
struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
    const IVolume* m_object;
} __attribute__((aligned(16)));

// Inferred collision view: the time, the line flag and the result.
class CCollision {
public:
    void AddContact(const SClsnContact&);

    unsigned char unknown00[12];
    float m_time;
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
};

extern const float CONTACT_EPSILON;

inline int Compare(float separation) {
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

class IVolume {
public:
    virtual ~IVolume();
};

class CVolCapsule : public IVolume {
public:
    int TestCollision(const CVolCapsule&, CCollision&, bool) const;

    unsigned char unknown04[8];
    float m_radius;
    CLine3 m_axis;
};

int CVolCapsule::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    float radii = m_radius + other.m_radius;
    float radiiSq = radii * radii;
    CLine3 otherAxis = other.m_axis;
    bool alongAxis = true;
    bool pastEnd = false;
    CVector3 p0;
    CVector3 p1;
    float t0;
    float t1;
    m_axis.GetProjection(otherAxis.m_start, p0, t0);
    m_axis.GetProjection(otherAxis.m_end, p1, t1);
    float s;
    float t;
    float distSq;
    m_axis.GetSquaredDistanceBetweenSegments(other.m_axis, s, t, &distSq);
    float length = m_axis.GetLength();
    if (distSq <= radiiSq + CONTACT_EPSILON) {
        float separation;
        if (t0 > 1.0f + CONTACT_EPSILON && t1 > 1.0f + CONTACT_EPSILON) {
            float a = (t0 - 1.0f) * length;
            float b = (t1 - 1.0f) * length;
            separation = a > b ? b : a;
            pastEnd = true;
        } else if (t0 < -CONTACT_EPSILON && t1 < -CONTACT_EPSILON) {
            float a = -t0 * length;
            float b = -t1 * length;
            separation = a > b ? b : a;
        } else {
            float otherRadius = other.m_radius;
            separation = sqrtf(distSq) - (m_radius + otherRadius);
            alongAxis = false;
        }
        int result = Compare(separation);
        if (flag && result != 1) {
            CVector3 normal;
            CVector3 point;
            if (alongAxis) {
                normal = m_axis.GetDirection();
                if (pastEnd) {
                    normal *= -1.0f;
                    point = m_axis.GetStart();
                } else {
                    point = m_axis.GetEnd();
                }
            } else {
                normal = otherAxis.GetStart();
                normal.d.c.x -= p0.d.c.x;
                normal.d.c.y -= p0.d.c.y;
                normal.d.c.z -= p0.d.c.z;
                normal.Normalize();
                point.Scale(normal, m_radius);
                point += p0;
            }
            if (result > collision.m_result)
                collision.m_result = result;
            if (result == 2) {
                SClsnContact contact;
                contact.m_separation = separation;
                contact.m_vector1 = point;
                contact.m_vector0 = normal;
                contact.m_index = 0;
                contact.m_object = 0;
                collision.AddContact(contact);
            }
        }
        return result;
    }
    return 1;
}
