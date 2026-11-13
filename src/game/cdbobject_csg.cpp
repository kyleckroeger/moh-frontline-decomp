// A fragment of cdbobject.cpp: CCDBObject's collision tests against
// CCSGVolume and IVolume (swapped: the collision order is reversed and the
// other volume tests this one) and against CAnimatedVolume, CVolCapsule,
// CVolSphere and CVolBox: the branch-node test of the collision data's root
// gathers tri groups into a vector over 2048 entries of stack storage, the
// groups are tested unless the branch test returned 1, and each gathered
// group's mark is negated again. The capsule and sphere tests are timed in
// profiler bins 30 and 29 (the second OSGetTick call of each pair is unused,
// as in the image). The tests return int-sized results here (inferred). The file name is this project's; the
// original record is cdbobject.cpp and the functions around these are not part of
// this unit. The classes come from the mangled symbols; IVolume's virtual
// functions are declared in the order of __vt__7IVolume and CCDBObject redeclares
// them with its destructor first (defined elsewhere, so its virtual table is
// not emitted here); CAnimatedVolume derives from CVolSphere (its tests call
// the sphere's), and the collision flag is an inferred bit-field view. The
// vector's storage constructor, the tri group's mark, the data's root node
// and the profiler bin fields are inferred views; the templates are declared
// only.
class CMatrix;
class CVector3;
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

class CCollision {
public:
    void SwapOrder();

    unsigned char unknown00[16];
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
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
    int TestCollision(const CCSGVolume&, CCollision&, bool) const;
    template <class T>
    int TestCDBBranchNodeCollision(const CDBBranchNode&, const T&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;

    CDBDataView* m_data;
};

int CCDBObject::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CCDBObject::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    int result = 1;
    CDBTriGroup* storage[2048];
    dwi::fast_vec<CDBTriGroup*> groups(storage, 2048);
    if (TestCDBBranchNodeCollision(*m_data->m_root, other, collision, flag, groups) != 1)
        result = TestCollisionWithTriGroups(groups, other, collision, flag);
    for (CDBTriGroup** group = groups.m_data; group != groups.m_data + groups.m_size; group++)
        (*group)->m_mark = -(*group)->m_mark;
    return result;
}

int CCDBObject::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    g_ProfileBins[30].m_time -= OSGetTick();
    int result = 1;
    OSGetTick();
    CDBTriGroup* storage[2048];
    dwi::fast_vec<CDBTriGroup*> groups(storage, 2048);
    if (TestCDBBranchNodeCollision(*m_data->m_root, other, collision, flag, groups) != 1)
        result = TestCollisionWithTriGroups(groups, other, collision, flag);
    for (CDBTriGroup** group = groups.m_data; group != groups.m_data + groups.m_size; group++)
        (*group)->m_mark = -(*group)->m_mark;
    OSGetTick();
    g_ProfileBins[30].m_time += OSGetTick();
    g_ProfileBins[30].m_count++;
    return result;
}

int CCDBObject::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    g_ProfileBins[29].m_time -= OSGetTick();
    int result = 1;
    OSGetTick();
    CDBTriGroup* storage[2048];
    dwi::fast_vec<CDBTriGroup*> groups(storage, 2048);
    if (TestCDBBranchNodeCollision(*m_data->m_root, other, collision, flag, groups) != 1)
        result = TestCollisionWithTriGroups(groups, other, collision, flag);
    for (CDBTriGroup** group = groups.m_data; group != groups.m_data + groups.m_size; group++)
        (*group)->m_mark = -(*group)->m_mark;
    OSGetTick();
    g_ProfileBins[29].m_time += OSGetTick();
    g_ProfileBins[29].m_count++;
    return result;
}

int CCDBObject::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    int result = 1;
    CDBTriGroup* storage[2048];
    dwi::fast_vec<CDBTriGroup*> groups(storage, 2048);
    if (TestCDBBranchNodeCollision(*m_data->m_root, other, collision, flag, groups) != 1)
        result = TestCollisionWithTriGroups(groups, other, collision, flag);
    for (CDBTriGroup** group = groups.m_data; group != groups.m_data + groups.m_size; group++)
        (*group)->m_mark = -(*group)->m_mark;
    return result;
}

int CCDBObject::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}
