// csg_volume.cpp: CCSGVolume, a collision volume made of a hierarchy
// object's sub-volumes. The whole file, with the class's virtual table and
// RTTI record (slot order from __vt__10CCSGVolume; the destructor is the key
// function). The weak empty SClsnContact constructor comes first, then an
// empty TransformedCopy, the extents as the union of the hierarchy object's
// sub-volume extents (starting from FLT_MAX and -FLT_MAX), Create, and the
// collision tests against each volume type: every sub-volume of the
// hierarchy object is tested against the other volume into one of twenty
// collision records, a result of 2 records the sub-volume and its depth and a
// result of 3 ends the test, and CheckContacts picks the nearest (the
// smallest depth, FLT_MAX when none: a miss, raising the result to 1 when
// asked), copies its depth, line flag and result into the collision and,
// when asked, raises the result and adds a contact (separation, both vectors
// and the sub-volume index) for every record at that depth (each copied over
// the nearest record first); the test against IVolume swaps the order and
// lets the other volume test this one. Init keeps the sub-volume count and
// the hierarchy object; the destructor (its table pointer, then IVolume's
// through the inline base destructor, and the object freed when asked) and
// the default constructor (IVolume's and CCSGVolume's table pointers, two
// cleared words) end the file. Create allocates a new CSG volume (12 bytes;
// the constructor is inlined into it).
// The results are compared with 2 and 3, so the TestCollision results are
// declared int here (inferred). The names come from the mangled symbols; the
// members are inferred. The collision record view (32 bytes with an
// out-of-line constructor and destructor; depth at +12, line flag, result at
// +20, contact at +24) and the contact record (two vectors, the separation,
// an index and an object) are inferred. IVolume's destructor is inline (weak
// in the original), so the compiler's copies of it and of IVolume's weak
// virtual table are weak duplicates, linked to the original copies.
// The file is compiled with deferred inlining, so its functions are written
// here in reverse of their order in the original.

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


#include "game/CVector3.h"


class IVolume;

struct SClsnContact {
    SClsnContact();

    /* the vectors as their 16 bytes (copied as two doubles, like the
       implicit copies of the original) */
    CVector3Data m_vector0;
    CVector3Data m_vector1;
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
    IVolume() {}
    virtual ~IVolume() {}
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
    virtual int TestCollision(const CCSGVolume&, CCollision&, bool) const;
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
    CCSGVolume();

    CHierObject* m_object;
    int m_count;
};

CCSGVolume::CCSGVolume() : m_object(0), m_count(0) {
}

CCSGVolume::~CCSGVolume() {
}

void CCSGVolume::Init(int count, CHierObject* object) {
    m_count = count;
    m_object = object;
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

int CCSGVolume::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CCSGVolume::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
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

int CCSGVolume::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
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

int CCSGVolume::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
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

int CCSGVolume::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
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

int CCSGVolume::TestCollision(CVector3 other, CCollision& collision, bool flag) const {
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

int CCSGVolume::TestCollision(const CPlane& other, CCollision& collision, bool flag) const {
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

int CCSGVolume::TestCollision(const CTriangle& other, CCollision& collision, bool flag) const {
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

int CCSGVolume::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
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

int CCSGVolume::TestCollision(const CWorldVolume& other, CCollision& collision, bool flag) const {
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

int CCSGVolume::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
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

int CCSGVolume::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
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

IVolume* CCSGVolume::Create() const {
    return new CCSGVolume;
}

void CCSGVolume::GetExtents(CVector3& minimum, CVector3& maximum) const {
    minimum.x = 3.4028235e38f;
    minimum.y = 3.4028235e38f;
    minimum.z = 3.4028235e38f;
    maximum.x = -3.4028235e38f;
    maximum.y = -3.4028235e38f;
    maximum.z = -3.4028235e38f;
    for (int i = 0; i < m_count; i++) {
        IVolume* volume = m_object->GetSubVolume(i);
        if (volume) {
            CVector3 low;
            CVector3 high;
            volume->GetExtents(low, high);
            if (low.x < minimum.x)
                minimum.x = low.x;
            if (low.y < minimum.y)
                minimum.y = low.y;
            if (low.z < minimum.z)
                minimum.z = low.z;
            if (high.x > maximum.x)
                maximum.x = high.x;
            if (high.y > maximum.y)
                maximum.y = high.y;
            if (high.z > maximum.z)
                maximum.z = high.z;
        }
    }
}

void CCSGVolume::TransformedCopy(const IVolume&, const CMatrix&) {
}

__declspec(weak) SClsnContact::SClsnContact() {
}
