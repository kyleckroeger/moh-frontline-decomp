// CCSGVolume, the functions at the start of the file (0x800c7f50): the weak
// empty SClsnContact constructor (a view declaring only it), then an empty
// TransformedCopy, the extents as the union of the hierarchy object's
// sub-volume extents (starting from FLT_MAX and -FLT_MAX), Create, and the
// collision tests against each volume type: every sub-volume of the
// hierarchy object is tested against the other volume into one of twenty
// collision records, a result of 2 records the sub-volume and its depth and a
// result of 3 ends the test, and CheckContacts (not reconstructed) picks the
// nearest; the test against IVolume swaps the order and lets the other
// volume test this one. The results are compared with 2 and 3, so the
// TestCollision results are declared int here (inferred). The names come from
// the mangled symbols; IVolume's virtual functions are declared in the order
// of __vt__7IVolume, CCSGVolume declares its destructor first (defined
// elsewhere, so its virtual table is not emitted here) and its members are
// inferred. Create allocates a new CSG volume (12 bytes; the inline
// constructors set the virtual tables and clear the members). The collision
// record view (32 bytes with an out-of-line constructor and destructor;
// depth at +12, line flag, result at +20) is inferred. The rest of the file (CheckContacts, the line test, Init,
// the destructor and the constructor) is not part of this unit.
struct SClsnContact {
    SClsnContact();
};

__declspec(weak) SClsnContact::SClsnContact() {
}

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


class CCollision {
public:
    CCollision();
    void SwapOrder();
    ~CCollision();

    unsigned char unknown00[12];
    float m_depth;
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
    unsigned char unknown18[8];
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

    CCSGVolume() : m_object(0), m_count(0) {}

    CHierObject* m_object;
    int m_count;
};

void CCSGVolume::TransformedCopy(const IVolume&, const CMatrix&) {
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

IVolume* CCSGVolume::Create() const {
    return new CCSGVolume;
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


int CCSGVolume::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}
