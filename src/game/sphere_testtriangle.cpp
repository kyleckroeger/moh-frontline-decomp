// A fragment of sphere.cpp (0x800cab00): CVolSphere::TestCollision against a
// triangle, which reports no collision, and against a plane: the signed
// distance of the centre from the plane less the radius classifies the sphere
// as in front of the plane (1), touching it within CONTACT_EPSILON (2) or
// behind it (3); when asked, the collision's result is raised to it, and a
// touching sphere adds a contact (the reversed plane normal and the centre
// moved onto the plane). The classes and functions are named by the symbols;
// the members, the vector view, the plane distance helper, the contact record
// view and the result codes are inferred (as in capsule_plane.cpp). The rest
// of the file is in other units.
class CTriangle;

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(16)));

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

    unsigned char unknown00[20];
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

class CVolSphere {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
    int TestCollision(const CPlane&, CCollision&, bool) const;

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
