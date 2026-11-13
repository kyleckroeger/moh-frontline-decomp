// A fragment of cdbobject.cpp (0x8006ff68): CCDBObject's line test (the root
// branch node's test of the line, gathering tri groups into a vector over
// 2048 entries of stack storage, whose marks are negated again; timed in
// profiler bin 24 with the second OSGetTick call of each pair unused, as in
// the image), the weak dwi::fast_vec<CDBTriGroup*> destructor (emitted after
// its first use, which inlines it), the fiber test (sets the collision's line
// flag, tests the fiber's line through the line-test virtual and records
// this object in the contact on a result of 2; profiler bin 28) and
// GetExtents (the stored minimum and maximum). The file name is this
// project's; the original record is cdbobject.cpp and the functions around
// these are not part of this unit. The classes come from the mangled
// symbols; IVolume's virtual functions are declared in the order of
// __vt__7IVolume and CCDBObject redeclares them with its destructor first
// (defined elsewhere, so its virtual table is not emitted here). The
// collision flag, the contact's object at +40, the vector's storage
// constructor, the tri group's mark, the data's root node, the extents at +8
// and +24 and the profiler bin fields are inferred views; the templates are
// declared only, and the tests return int-sized results (inferred).
class CMatrix;
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
class CTriangle;
class CPlane;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

class IVolume;
class CVolFiber;

// Inferred: the contact's colliding object at +40.
struct SClsnContactView {
    unsigned char unknown00[40];
    const IVolume* m_object;
};

class CCollision {
public:
    void SwapOrder();

    unsigned char unknown00[16];
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[7];
    SClsnContactView* m_contact;
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

class CCSGVolume : public IVolume {};

class CVolFiber : public IVolume {
public:
    const CLine3& GetLine() const;
};

extern "C" void MEM_free(void*);
extern "C" unsigned int OSGetTick();

#include "game/DwiContainers.h"
namespace dwi {
// Inferred: the vector over caller storage (inline constructor; the
// destructor frees the storage only when the vector owns it).

}

// Inferred: a tri group's mark at +16, negated after each test.
struct CDBTriGroup {
    unsigned char unknown00[16];
    int m_mark;
};

struct CDBBranchNode;

// Inferred: the collision data's root branch node at +48.
struct CDBDataView {
    unsigned char unknown00[48];
    CDBBranchNode* m_root;
};

// The profiler bins (24 bytes each; time at +8 and count at +20 inferred).
struct SProfileBin {
    SProfileBin();

    int unknown00;
    int unknown04;
    int m_time;
    int unknown0c;
    int unknown10;
    int m_count;
};

extern SProfileBin g_ProfileBins[31];

template <class T>
int TestCollisionWithTriGroups(const dwi::fast_vec<CDBTriGroup*>&, const T&, CCollision&, bool);

class CCDBObject : public IVolume {
public:
    virtual ~CCDBObject();
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
    template <class T>
    int TestCDBBranchNodeCollision(const CDBBranchNode&, const T&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;

    void GetExtents(CVector3&, CVector3&) const;

    CDBDataView* m_data;
    CVector3 m_min;
    CVector3 m_max;
};

int CCDBObject::TestCollision(const CLine3& other, CCollision& collision, bool flag) const {
    g_ProfileBins[24].m_time -= OSGetTick();
    OSGetTick();
    CDBTriGroup* storage[2048];
    dwi::fast_vec<CDBTriGroup*> groups(storage, 2048);
    int result = TestCDBBranchNodeCollision(*m_data->m_root, other, collision, flag, groups);
    for (CDBTriGroup** group = groups.m_data; group != groups.m_data + groups.m_size; group++)
        (*group)->m_mark = -(*group)->m_mark;
    OSGetTick();
    g_ProfileBins[24].m_time += OSGetTick();
    g_ProfileBins[24].m_count++;
    return result;
}

int CCDBObject::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    g_ProfileBins[28].m_time -= OSGetTick();
    collision.m_line = 1;
    int result = TestCollision(other.GetLine(), collision, flag);
    if (result == 2 && collision.m_contact)
        collision.m_contact->m_object = this;
    g_ProfileBins[28].m_time += OSGetTick();
    g_ProfileBins[28].m_count++;
    return result;
}

void CCDBObject::GetExtents(CVector3& minimum, CVector3& maximum) const {
    minimum = m_min;
    maximum = m_max;
}
