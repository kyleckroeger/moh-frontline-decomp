// A fragment of csg_volume.cpp (0x800c8f3c): CCSGVolume::CheckContacts,
// which picks the nearest of the recorded sub-volume collisions (the
// smallest depth, FLT_MAX when none: a miss, raising the result to 1 when
// asked), copies its depth, line flag and result into the collision and,
// when asked, raises the result and adds a contact (separation, both
// vectors and the sub-volume index) for every record at that depth (each
// copied over the nearest record first); then CCSGVolume's collision test
// against a line (every sub-volume of the hierarchy object is tested
// against the line into one of twenty collision records; a result of 2
// records the sub-volume and its depth, a result of 3 ends the test, and
// CheckContacts picks the nearest) and Init (the sub-volume count and the
// hierarchy object). The file name is this project's; the original record
// is csg_volume.cpp. The views are those of csg_volume.cpp (inferred as
// there); the result is read through a small inline (inferred).
class CMatrix;
class CDrawContext;
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

// Inferred: CVector3 as four floats overlaid with two doubles (its copies
// move doubleword pairs).
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));


class IVolume;

/* Inferred contact record view (as in capsule_dispatch.cpp): two vectors,
   the separation, an index and an object. */
struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
    const IVolume* m_object;
} __attribute__((aligned(16)));

class CCollision {
public:
    CCollision();
    void SwapOrder();
    ~CCollision();
    CCollision& operator=(const CCollision&);
    void AddContact(const SClsnContact&);

    unsigned char unknown00[12];
    float m_depth;
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
    SClsnContact* m_contact;
    unsigned char unknown1c[4];
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
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CHierObject {
public:
    IVolume* GetSubVolume(int) const;
};

class CCSGVolume : public IVolume {
public:
    virtual ~CCSGVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    void GetExtents(CVector3&, CVector3&) const;
    int TestCollision(const CCSGVolume&, CCollision&, bool) const;
    int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    int TestCollision(const CVolFiber&, CCollision&, bool) const;
    int TestCollision(const CTriangle&, CCollision&, bool) const;
    int TestCollision(const CPlane&, CCollision&, bool) const;
    int TestCollision(CVector3, CCollision&, bool) const;
    int TestCollision(const CCDBObject&, CCollision&, bool) const;
    int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    int TestCollision(const CVolSphere&, CCollision&, bool) const;
    int TestCollision(const CVolBox&, CCollision&, bool) const;
    int TestCollision(const IVolume&, CCollision&, bool) const;
    int TestCollision(const CLine3&, CCollision&, bool) const;
    int CheckContacts(int*, float*, CCollision*, int, CCollision&, bool) const;

    void Init(int, CHierObject*);

    CCSGVolume() : m_object(0), m_count(0) {}

    CHierObject* m_object;
    int m_count;
};

static inline int Result(const CCollision& c) { return c.m_result; }
int CCSGVolume::CheckContacts(int* indices, float* depths, CCollision* collisions, int count, CCollision& collision,
                              bool all) const {
    float nearest = 3.4028235e38f;
    int index = -1;
    for (int i = 0; i < count; i++) {
        if (depths[i] < nearest) {
            nearest = depths[i];
            index = indices[i];
        }
    }
    int result;
    if (nearest == 3.4028235e38f) {
        if (all && collision.m_result < 1)
            collision.m_result = 1;
        result = 1;
    } else {
        CCollision& best = collisions[index];
        collision.m_depth = best.m_depth;
        collision.m_line = best.m_line;
        result = Result(best);
        if (all) {
            for (int i = 0; i < count; i++) {
                if (nearest == depths[i]) {
                    int other = indices[i];
                    best = collisions[other];
                    if (result > collision.m_result)
                        collision.m_result = result;
                    if (result == 2) {
                        SClsnContact contact;
                        contact.m_separation = best.m_contact->m_separation;
                        contact.m_vector0 = best.m_contact->m_vector0;
                        contact.m_vector1 = best.m_contact->m_vector1;
                        contact.m_index = other;
                        collision.AddContact(contact);
                    }
                }
            }
        }
    }
    return result;
}

int CCSGVolume::TestCollision(const CLine3& other, CCollision& collision, bool flag) const {
    CCollision collisions[20];
    int indices[20];
    float depths[20];
    int count = 0;
    for (int i = 0; i < m_count; i++) {
        IVolume* volume = m_object->GetSubVolume(i);
        if (volume) {
            int result = volume->TestCollision(other, collisions[i], true);
            if (result == 2) {
                indices[count] = i;
                depths[count] = collisions[i].m_depth;
                count++;
            } else if (result == 3) {
                return 3;
            }
        }
    }
    return CheckContacts(indices, depths, collisions, count, collision, flag);
}

void CCSGVolume::Init(int count, CHierObject* object) {
    m_count = count;
    m_object = object;
}
