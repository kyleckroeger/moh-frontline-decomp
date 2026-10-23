// A fragment of sphere.cpp (0x800cab00): CVolSphere::TestCollision against a
// triangle, which reports no collision, and against a plane: the signed
// distance of the centre from the plane less the radius classifies the sphere
// as in front of the plane (1), touching it within CONTACT_EPSILON (2) or
// behind it (3); when asked, the collision's result is raised to it, and a
// touching sphere adds a contact (the reversed plane normal and the centre
// moved onto the plane). Then the line test: the line start relative to the
// centre and the line direction give the quadratic a t^2 + b t + c (c = the
// squared distance less the squared radius). A line shorter than 1e-5 is
// tested as its start point at time 1 (a fiber line never reports 3; the
// contact is the normalised offset, the line end and the separation).
// Otherwise no real root (or both roots outside [0, 1]) is a miss (1); a
// start inside the sphere is 3, or time 0 for a fiber line; a hit stores the
// time and the point on the line and is a contact (2) when it lies within the
// squared contact epsilon of the line end or the line is a fiber, else 3. A
// contact adds the normalised direction from the centre, the hit point and the
// negated distance to the end. The classes and functions are named by the
// symbols; the members, the vector and line views (copies one double at a
// time), the compare helpers, the contact record view and the result codes
// are inferred (as in capsule_plane.cpp). The rest of the file is in other
// units.
#include <math.h>

class CTriangle;

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
    }
    void Scale(const CVector3& v, float k) {
        d.v[0] = k * v.d.v[0];
        d.v[1] = k * v.d.v[1];
        d.v[2] = k * v.d.v[2];
    }
    CVector3& operator+=(const CVector3& o) {
        d.v[0] += o.d.v[0];
        d.v[1] += o.d.v[1];
        d.v[2] += o.d.v[2];
        return *this;
    }
    float Dot(const CVector3& o) const { return d.v[0] * o.d.v[0] + d.v[1] * o.d.v[1] + d.v[2] * o.d.v[2]; }
    float Length() const { return sqrtf(LengthSq()); }
    float LengthSq() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }
    void Normalize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 GetDirection() const { return m_dir; }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
};

struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
    const void* m_object;
} __attribute__((aligned(16)));

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

class CPlane {
public:
    float Dist(const CVector3& v) const {
        return m_normal.d.v[0] * v.d.v[0] + m_normal.d.v[1] * v.d.v[1] + m_normal.d.v[2] * v.d.v[2] - m_d;
    }

    CVector3 m_normal;
    float m_d;
};

extern const float CONTACT_EPSILON;
extern const float CONTACT_EPSILON_SQRD;

inline int Compare(float separation) {
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

inline int CompareSqrd(float distSq) {
    if (-distSq > CONTACT_EPSILON_SQRD)
        return 1;
    if (distSq > CONTACT_EPSILON_SQRD)
        return 3;
    return 2;
}

class CVolSphere {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
    int TestCollision(const CPlane&, CCollision&, bool) const;
    int TestCollision(const CLine3&, CCollision&, bool) const;

    unsigned char unknown00[12];
    float m_radius;
    CVector3 m_center;
};

bool CVolSphere::TestCollision(const CTriangle&, CCollision&, bool) const {
    return false;
}

int CVolSphere::TestCollision(const CPlane& plane, CCollision& collision, bool flag) const {
    CVector3 center = m_center;
    float distance = plane.Dist(center) - m_radius;
    int result;
    if (distance > CONTACT_EPSILON)
        result = 1;
    else if (distance < -CONTACT_EPSILON)
        result = 3;
    else
        result = 2;
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            SClsnContact contact;
            distance = -distance;
            contact.m_vector1.d.v[0] = distance * plane.m_normal.d.v[0];
            contact.m_vector1.d.v[1] = distance * plane.m_normal.d.v[1];
            contact.m_vector1.d.v[2] = distance * plane.m_normal.d.v[2];
            contact.m_vector1.d.v[0] += m_center.d.v[0];
            contact.m_vector1.d.v[1] += m_center.d.v[1];
            contact.m_vector1.d.v[2] += m_center.d.v[2];
            contact.m_vector0.d.v[0] = -plane.m_normal.d.v[0];
            contact.m_vector0.d.v[1] = -plane.m_normal.d.v[1];
            contact.m_vector0.d.v[2] = -plane.m_normal.d.v[2];
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}

int CVolSphere::TestCollision(const CLine3& line, CCollision& collision, bool flag) const {
    CVector3 dir;
    CVector3 diff;
    CVector3 point;
    CVector3 normal;
    float t;
    int result;
    dir = line.GetDirection();
    const CVector3& start = line.GetStart();
    diff.d.v[0] = start.d.v[0] - m_center.d.v[0];
    diff.d.v[1] = start.d.v[1] - m_center.d.v[1];
    diff.d.v[2] = start.d.v[2] - m_center.d.v[2];
    float a = dir.LengthSq();
    float b = 2.0f * dir.Dot(diff);
    float distSq = diff.LengthSq();
    float c = distSq - m_radius * m_radius;
    if (a > 1e-05f) {
        float discr = b * b - 4.0f * a * c;
        if (discr < 0.0f) {
            if (flag && collision.m_result < 1)
                collision.m_result = 1;
            return 1;
        }
        if (discr > 1e-05f) {
            float root = sqrtf(discr);
            float inv = 0.5f / a;
            float t0 = inv * (-b - root);
            float t1 = inv * (-b + root);
            if (t0 > 1.0f || t1 < 0.0f) {
                if (flag && collision.m_result < 1)
                    collision.m_result = 1;
                return 1;
            }
            if (t0 >= 0.0f) {
                t = t0;
            } else if (collision.m_line) {
                t = 0.0f;
            } else {
                if (flag && collision.m_result < 3)
                    collision.m_result = 3;
                return 3;
            }
        } else {
            t = -b / (2.0f * a);
        }
    } else {
        collision.m_time = 1.0f;
        float separation = diff.Length() - m_radius;
        int code = Compare(separation);
        if (collision.m_line && code == 3)
            code = 2;
        if (flag) {
            if (code > collision.m_result)
                collision.m_result = code;
            if (code == 2) {
                SClsnContact contact;
                contact.m_vector0 = diff;
                contact.m_separation = separation;
                contact.m_vector1 = line.GetEnd();
                contact.m_vector0.Normalize();
                contact.m_index = 0;
                collision.AddContact(contact);
            }
        }
        return code;
    }
    if (t < 0.0f || t > 1.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    collision.m_time = t;
    point.Scale(dir, t);
    point += line.GetStart();
    if (collision.m_line) {
        distSq = 0.0f;
        result = 2;
    } else {
        const CVector3& end = line.GetEnd();
        diff.d.v[0] = end.d.v[0] - point.d.v[0];
        diff.d.v[1] = end.d.v[1] - point.d.v[1];
        diff.d.v[2] = end.d.v[2] - point.d.v[2];
        distSq = diff.LengthSq();
        result = CompareSqrd(distSq);
        if (result != 2)
            result = 3;
    }
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            normal.d.v[0] = point.d.v[0] - m_center.d.v[0];
            normal.d.v[1] = point.d.v[1] - m_center.d.v[1];
            normal.d.v[2] = point.d.v[2] - m_center.d.v[2];
            normal.Normalize();
            SClsnContact contact;
            contact.m_separation = -sqrtf(distSq);
            contact.m_vector1 = point;
            contact.m_vector0 = normal;
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}
