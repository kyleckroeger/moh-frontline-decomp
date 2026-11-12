// A fragment of box.cpp (0x800c15e4): CVolBox::TestCollision against a
// triangle, which reports no collision, and against a plane (the box's eight
// corners walked by adding and subtracting the scaled half axes, each
// projected onto the plane; the deepest gives the contact). The classes and
// functions are named by the symbols; Scale, Sub and Add are the original's
// CVector3 members (weak copies at 800a4cec, 800a4dac and 800a4d14), Negate
// and the class layouts are inferred. The rest of the file is in other units.
class CTriangle;

// Inferred view: CVector3 without constructors (its implicit copy moves the two
// doubles), as in matrix_ps.cpp.
union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    void Scale(const CVector3& v, float k) {
        d.c.x = k * v.d.c.x;
        d.c.y = k * v.d.c.y;
        d.c.z = k * v.d.c.z;
    }
    void Sub(const CVector3& a, const CVector3& b) { d.c.x = a.d.c.x - b.d.c.x; d.c.y = a.d.c.y - b.d.c.y; d.c.z = a.d.c.z - b.d.c.z; }
    void Add(const CVector3& o) { d.c.x += o.d.c.x; d.c.y += o.d.c.y; d.c.z += o.d.c.z; }
    void Negate(const CVector3& v) {
        d.c.x = -v.d.c.x;
        d.c.y = -v.d.c.y;
        d.c.z = -v.d.c.z;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class IVolume;

struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
    const IVolume* m_object;
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

extern const float CONTACT_EPSILON;

inline int Compare(float separation) {
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

class CPlane {
public:
    float GetDistance(CVector3 r) const {
        return m_normal.d.c.x * r.d.c.x + m_normal.d.c.y * r.d.c.y + m_normal.d.c.z * r.d.c.z - m_distance;
    }
    float Project2(CVector3 p, CVector3& out) const {
        float distance = GetDistance(p);
        out.Scale(m_normal, -distance);
        out.Add(p);
        return distance;
    }
    CVector3 m_normal;
    float m_distance;
};

class IVolume {
public:
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
    int TestCollision(const CPlane&, CCollision&, bool) const;

    float m_half[3];
    CVector3 m_axes[3];
    CVector3 m_center;
};

bool CVolBox::TestCollision(const CTriangle&, CCollision&, bool) const {
    return false;
}

int CVolBox::TestCollision(const CPlane& plane, CCollision& collision, bool flag) const {
    CVector3 a;
    CVector3 b;
    CVector3 c;
    SClsnContact contact;
    CVector3 point;
    a.Scale(m_axes[0], m_half[0]);
    b.Scale(m_axes[1], m_half[1]);
    c.Scale(m_axes[2], m_half[2]);
    contact.m_vector0.Negate(plane.m_normal);
    CVector3 corner = m_center;
    corner.Sub(corner, a);
    corner.Sub(corner, b);
    corner.Sub(corner, c);
    contact.m_separation = plane.Project2(corner, point);
    contact.m_vector1 = point;
    corner.Add(a);
    corner.Add(a);
    {
        float distance = plane.Project2(corner, point);
        if (distance < contact.m_separation) {
            contact.m_separation = distance;
            contact.m_vector1 = point;
        }
    }
    corner.Add(b);
    corner.Add(b);
    {
        float distance = plane.Project2(corner, point);
        if (distance < contact.m_separation) {
            contact.m_separation = distance;
            contact.m_vector1 = point;
        }
    }
    corner.Sub(corner, a);
    corner.Sub(corner, a);
    {
        float distance = plane.Project2(corner, point);
        if (distance < contact.m_separation) {
            contact.m_separation = distance;
            contact.m_vector1 = point;
        }
    }
    corner.Add(c);
    corner.Add(c);
    {
        float distance = plane.Project2(corner, point);
        if (distance < contact.m_separation) {
            contact.m_separation = distance;
            contact.m_vector1 = point;
        }
    }
    corner.Sub(corner, b);
    corner.Sub(corner, b);
    {
        float distance = plane.Project2(corner, point);
        if (distance < contact.m_separation) {
            contact.m_separation = distance;
            contact.m_vector1 = point;
        }
    }
    corner.Add(a);
    corner.Add(a);
    {
        float distance = plane.Project2(corner, point);
        if (distance < contact.m_separation) {
            contact.m_separation = distance;
            contact.m_vector1 = point;
        }
    }
    corner.Add(b);
    corner.Add(b);
    {
        float distance = plane.Project2(corner, point);
        if (distance < contact.m_separation) {
            contact.m_separation = distance;
            contact.m_vector1 = point;
        }
    }
    int result = Compare(contact.m_separation);
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}
