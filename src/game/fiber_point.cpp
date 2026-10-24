// A fragment of fiber.cpp (0x800c9ba4): CVolFiber::TestCollision against a
// point. The collision's line flag (top bit of +16) is set. The point is on
// the fiber when its distance from the line start matches its projection on
// the line direction within 1e-12 and the projection lies in [0, length]
// (the line's cached length); otherwise the result is raised to 1 when asked
// and 1 is returned. A hit raises the result to 2, stores the time (the
// projection over the length, 0 for a line not longer than 1e-12) and adds a contact:
// the projection as separation, a normal perpendicular to the line (its
// direction crossed with the axis of its smallest leading component, picked
// as in the code: x if |x| < |y|, else y if |y| < |z|, else z), the point
// and index 0. The file name is this project's; the original record is
// fiber.cpp. The classes and functions are named by the mangled symbols; the
// members, the cached-length layout (+48 length, +52 squared length, +56/+57
// valid flags inside the line), the vector helpers, the by-value assignment
// and the result codes are inferred. Constants are the file's .sdata2 pool
// entries. The rest of the file is in other units.
#include <math.h>

class IVolume;

// Inferred: CVector3 as four floats overlaid with two doubles.
union CVector3Data {
    double pair[2];
    float v[4];
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

class IVolume {
public:
    virtual ~IVolume();
};

class CVolFiber : public IVolume {
public:
    int TestCollision(CVector3, CCollision&, bool) const;

    unsigned char unknown04[12];
    CLine3 m_line;
};

int CVolFiber::TestCollision(CVector3 point, CCollision& collision, bool flag) const {
    CVector3 dir;
    CVector3 diff;
    collision.m_line = 1;
    dir = m_line.GetDirection();
    SetDifference(diff, point, m_line.GetStart());
    float t = dir.Dot(diff);
    float distSq = diff.LengthSq();
    float dist = sqrtf(distSq);
    if (t < dist - 1e-12f || t > dist + 1e-12f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float length = m_line.GetLength();
    if (t < 0.0f || t > length) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    if (flag) {
        if (collision.m_result < 2)
            collision.m_result = 2;
        if (length > 1e-12f)
            collision.m_time = t / length;
        else
            collision.m_time = 0.0f;
        SClsnContact contact;
        contact.m_separation = t;
        double ay = fabs(dir.d.v[1]);
        double ax = fabs(dir.d.v[0]);
        if (ax < ay)
            contact.m_vector0.Cross(dir, CVector3(1.0f, 0.0f, 0.0f));
        else {
            CVector3& normal = contact.m_vector0;
            if (ay < fabs(dir.d.v[2]))
                normal.Cross(dir, CVector3(0.0f, 1.0f, 0.0f));
            else
                normal.Cross(dir, CVector3(0.0f, 0.0f, 1.0f));
        }
        contact.m_vector1 = point;
        contact.m_index = 0;
        collision.AddContact(contact);
    }
    return 2;
}
