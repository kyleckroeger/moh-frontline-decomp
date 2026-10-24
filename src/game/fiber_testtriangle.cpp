// A fragment of fiber.cpp (0x800c9504): CVolFiber::TestCollision against a
// triangle sets the collision's top flag bit (+16) and reports no collision.
// CVolFiber::TestCollision against a plane (0x800c951c) also sets the flag,
// intersects the plane with the fiber's line and, when the line does not reach
// the plane within its length (t outside 0..1), returns 1 (recorded as the
// least result when asked); otherwise it stores the time and returns 2,
// recording the result and one contact (the plane's reversed normal and the
// intersection point, zero separation) when asked. The classes and functions
// are named by the symbols; the collision, contact and vector views are
// inferred. The result is a long local (an int is folded into the compare).
// The constants are entries of the file's .sdata2 pool. The rest of the file
// is in other units.
class CTriangle;

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CVector3 m_start;
    CVector3 m_end;
};

class CPlane {
public:
    bool GetIntersection(const CLine3&, float&, CVector3*) const;

    CVector3 m_normal;
    float m_d;
};

class IVolume;

struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
    const IVolume* m_object;
} __attribute__((aligned(16)));

/* inferred: the collision's flag bit at the top of +16, the time at +12 and the result at +20 */
class CCollision {
public:
    void AddContact(const SClsnContact&);

    unsigned char unknown00[12];
    float m_time;
    unsigned char m_flag : 1;
    unsigned char m_flags : 7;
    unsigned char unknown11[3];
    int m_result;
};

class CVolFiber {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
    int TestCollision(const CPlane&, CCollision&, bool) const;

    unsigned char unknown00[16];
    CLine3 m_line;
};

bool CVolFiber::TestCollision(const CTriangle&, CCollision& collision, bool) const {
    collision.m_flag = 1;
    return false;
}

int CVolFiber::TestCollision(const CPlane& plane, CCollision& collision, bool record) const {
    collision.m_flag = 1;
    float t;
    CVector3 point;
    bool hit = plane.GetIntersection(m_line, t, &point);
    bool inRange = false;
    if (t >= 0.0f && t <= 1.0f)
        inRange = true;
    hit &= inRange;
    if (!hit) {
        if (record && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    collision.m_time = t;
    long result = 2;
    if (record) {
        if (result > collision.m_result)
            collision.m_result = result;
        SClsnContact contact;
        contact.m_separation = 0.0f;
        contact.m_vector0.d.v[0] = -plane.m_normal.d.v[0];
        contact.m_vector0.d.v[1] = -plane.m_normal.d.v[1];
        contact.m_vector0.d.v[2] = -plane.m_normal.d.v[2];
        contact.m_vector1 = point;
        contact.m_index = 0;
        collision.AddContact(contact);
    }
    return result;
}
