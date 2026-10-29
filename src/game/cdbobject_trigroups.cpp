// A fragment of cdbobject.cpp (0x800716b8): the line test of one tri group
// (the line's end points straddle the group's plane, then each triangle is
// tested on a copy of the collision, kept when nearer; timed in profiler bins
// 26 and 27), and the weak template instances after it: the tri-group tests
// of CAnimatedVolume, CVolCapsule, CVolSphere and CVolBox (each group's plane
// first, then its triangles unless the pass-through flag stops them) and the
// branch-node tests of CAnimatedVolume, CVolCapsule and CVolSphere (a stack of
// branch nodes over 20 entries of stack storage; each node's plane classifies
// the volume, and the children on its side are pushed or leaf-tested). The
// file name is this project's; the original record is cdbobject.cpp. The
// classes, functions and templates are named by the mangled symbols; the
// layouts, member names and the inline helpers are inferred views, and the
// tests return int-sized results (inferred). The compiler's copies of the
// weak CPlane and dwi::fast_vec<const CDBBranchNode*> destructors are weak
// duplicates.
class CMatrix;
/* Inferred: CVector3 as four floats overlaid with two doubles (its copies
   move doubleword pairs). */
#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const struct CDBVector& v);
    CVector3(float x, float y, float z) { d.v[0] = x; d.v[1] = y; d.v[2] = z; }
    CVector3Data d;
} __attribute__((aligned(8)));

/* CPlane: the normal and the distance (inferred layout; 16-aligned, as the
   frames holding planes are). Its destructor is
   inline (weak in the original, which keeps a local plane in memory); Dist,
   the signed distance of a point, is an inferred inline helper (the name is
   this project's). The constructor from the database's node plane is inline
   (inferred). */
class CPlane {
public:
    CPlane(CVector3, float);
    CPlane(const struct CDBPlane& p);
    ~CPlane() {}
    float Dist(const CVector3& v) const {
        return m_normal.d.v[0] * v.d.v[0] + m_normal.d.v[1] * v.d.v[1] + m_normal.d.v[2] * v.d.v[2] - m_d;
    }
    CVector3 m_normal;
    float m_d;
} __attribute__((aligned(16)));

class CTriangle;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;
class IVolume;
class CVolFiber;

/* Only the members used here (the distance at +12 inferred); the copy
   constructor, destructor and assignment are out of line. */
class CCollision {
public:
    CCollision(const CCollision&);
    ~CCollision();
    CCollision& operator=(const CCollision&);

    unsigned char unknown00[12];
    float m_distance;
    unsigned char unknown10[16];
};

/* The line's two stored points (16-aligned); the by-value accessors are
   inferred inline helpers (names are this project's). */
class CLine3 {
public:
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 m_start;
    CVector3 m_end;
} __attribute__((aligned(16)));

extern "C" unsigned int OSGetTick();
extern "C" void MEM_free(void*);

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

/* IVolume's virtual functions in the order of __vt__7IVolume. */
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

/* Inferred: the box's half sizes and its frame (three axis rows and the
   centre row, 16-aligned), as GetDistanceFromPlane reads them. */
struct CVolBoxFrame {
    CVector3 m_row[4];
};

class CVolBox : public IVolume {
public:
    float m_halfSize[3];
    CVolBoxFrame m_frame;
};
class CVolSphere : public IVolume {
public:
    float GetRadius() const;
    CVector3 GetCenter() const;
};
class CVolCapsule : public IVolume {};
class CAnimatedVolume : public CVolSphere {};

namespace dwi {
// Inferred: the vector over caller storage (inline constructor; the
// destructor frees the storage only when the vector owns it).
template <class T>
class fast_vec {
public:
    fast_vec(T* storage, int capacity, int growth)
        : m_data(storage), m_unknown4(0), m_size(0), m_capacity(capacity), m_owned(false), m_growth(growth) {}
    ~fast_vec() {
        if (m_owned && m_data)
            MEM_free(m_data);
    }
    int size() const { return m_size; }
    T& back() { return m_data[m_size - 1]; }
    void pop_back() { m_size--; }
    void push_back(const T& x) { m_data[m_size++] = x; }
    T* begin() const { return m_data; }
    T* end() const { return m_data + m_size; }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

// Inferred: a triangle's pass-through flag (bit 1 of +14, a signed field).
struct CDBTri {
    unsigned char unknown00[14];
    signed char unknown0e_0 : 6;
    signed char m_passThru : 1;
    signed char unknown0e_7 : 1;
};

struct CDBVector {
    float x, y, z;
};

inline CVector3::CVector3(const CDBVector& v) {
    d.v[0] = v.x;
    d.v[1] = v.y;
    d.v[2] = v.z;
}

// Inferred: a tri group's plane, its mark at +16 (negated once gathered)
// and its triangles from +20.
struct CDBTriGroup {
    CDBVector m_normal;
    float m_d;
    int m_mark;
    CDBTri* m_tris[1];
};

class CCDBTriangle {
public:
    CCDBTriangle(const CDBTri&);
    int TestCollision(const CVolCapsule&, CCollision&, const CDBTri*, bool) const;
    int TestCollision(const CAnimatedVolume&, CCollision&, const CDBTri*, bool) const;
    int TestCollision(const CVolSphere&, CCollision&, const CDBTri*, bool) const;
    int TestCollision(const CVolBox&, CCollision&, const CDBTri*, bool) const;
    int TestCollision(const CLine3&, CCollision&, const CDBTri*, bool) const;

    unsigned char unknown00[48];
};

// File-local in the original; declared extern to link to it.
extern bool g_bDisablePassThru;

int TestCollisionWithTriGroup(const CDBTriGroup& group, const CLine3& line, CCollision& collision, bool flag) {
    g_ProfileBins[26].m_time -= OSGetTick();
    int result = 1;
    CVector3 normal(group.m_normal);
    CPlane plane(normal, group.m_d);
    float a = plane.Dist(line.GetStart());
    float b = plane.Dist(line.GetEnd());
    if (a * b < 0.0f) {
        CDBTri* const* tris = group.m_tris;
        for (int i = 0; i < -group.m_mark; i++, tris++) {
            CDBTri* tri = *tris;
            CCDBTriangle triangle(*tri);
            CCollision local(collision);
            g_ProfileBins[27].m_time -= OSGetTick();
            int r = triangle.TestCollision(line, local, tri, flag);
            g_ProfileBins[27].m_time += OSGetTick();
            g_ProfileBins[27].m_count++;
            if (r != 1) {
                if (local.m_distance <= collision.m_distance) {
                    collision = local;
                    result = r;
                }
                if (result == 3) {
                    g_ProfileBins[26].m_time += OSGetTick();
                    g_ProfileBins[26].m_count++;
                    return result;
                }
            }
        }
    }
    g_ProfileBins[26].m_time += OSGetTick();
    g_ProfileBins[26].m_count++;
    return result;
}

template <class T>
__declspec(weak) int TestCollisionWithTriGroups(const dwi::fast_vec<CDBTriGroup*>& groups, const T& volume, CCollision& collision, bool flag) {
    int result = 1;
    for (CDBTriGroup** it = groups.begin(); it != groups.end(); it++) {
        CDBTriGroup* group = *it;
        CVector3 normal(group->m_normal);
        CPlane plane(normal, group->m_d);
        if (volume.TestCollision(plane, collision, false) != 1) {
            CDBTri** tris = group->m_tris;
            for (int i = 0; i < -group->m_mark; i++, tris++) {
                CDBTri* tri = *tris;
                if (g_bDisablePassThru & tri->m_passThru)
                    break;
                CCDBTriangle triangle(*tri);
                int r = triangle.TestCollision(volume, collision, tri, flag);
                if (r > result) {
                    if ((result = r) == 3)
                        return result;
                }
            }
        }
    }
    return result;
}

/* The instances the collision tests use; explicit instantiations of the
   weak templates stand in for the implicit ones (the compiler emits them in
   the reverse order). */
template int TestCollisionWithTriGroups<CVolBox>(const dwi::fast_vec<CDBTriGroup*>&, const CVolBox&, CCollision&, bool);
template int TestCollisionWithTriGroups<CVolSphere>(const dwi::fast_vec<CDBTriGroup*>&, const CVolSphere&, CCollision&, bool);
template int TestCollisionWithTriGroups<CVolCapsule>(const dwi::fast_vec<CDBTriGroup*>&, const CVolCapsule&, CCollision&, bool);
template int TestCollisionWithTriGroups<CAnimatedVolume>(const dwi::fast_vec<CDBTriGroup*>&, const CAnimatedVolume&, CCollision&, bool);

// Inferred: a branch node's plane (the struct name is this project's).
struct CDBPlane {
    CDBVector m_normal;
    float m_d;
};

inline CPlane::CPlane(const CDBPlane& p) : m_normal(p.m_normal), m_d(p.m_d) {}

// Inferred: a node's type (1 is a branch) and a branch node's children.
struct CDBNode {
    int m_type;
};

struct CDBLeafNode : CDBNode {
};

struct CDBBranchNode : CDBNode {
    CDBNode* m_front;
    CDBNode* m_back;
    CDBPlane m_plane;
};

/* The branch test's classification of a volume against a node's plane: the
   distance beyond the volume's extent, or 0 when the plane crosses it. The
   sphere's (also used for CAnimatedVolume) is inline; the capsule's is the
   file-local out-of-line copy after this unit. */
inline float GetDistanceFromPlane(const CPlane& plane, const CVolSphere& sphere) {
    float distance = plane.Dist(sphere.GetCenter());
    float radius = sphere.GetRadius();
    float side = distance - radius;
    if (side > 0.0f)
        return side;
    side = distance + radius;
    if (side < 0.0f)
        return side;
    return 0.0f;
}

float GetDistanceFromPlane(const CPlane&, const CVolCapsule&);

/* Inferred inline (as the sphere's): the box's projected radius is the sum of
   its half sizes scaled by how much each axis faces the plane. The axis rows
   are copied in reverse into named locals declared in order (the stack slots
   and the copy schedule), the projection uses the three-float constructor
   (arguments evaluated right to left), and the centre row is copied last. */
inline float Dot(const CVector3& a, const CVector3& b) {
    return a.d.v[0] * b.d.v[0] + a.d.v[1] * b.d.v[1] + a.d.v[2] * b.d.v[2];
}

inline float GetDistanceFromPlane(const CPlane& plane, const CVolBox& box) {
    CVector3 r0, r1, r2;
    r2 = box.m_frame.m_row[2];
    r1 = box.m_frame.m_row[1];
    r0 = box.m_frame.m_row[0];
    CVector3 proj(Dot(r0, plane.m_normal), Dot(r1, plane.m_normal), Dot(r2, plane.m_normal));
    float width = (float)fabs(proj.d.v[0]);
    float depth = (float)fabs(proj.d.v[1]);
    float height = (float)fabs(proj.d.v[2]);
    float radius = width * box.m_halfSize[0] + depth * box.m_halfSize[1] + height * box.m_halfSize[2];
    CVector3 r3 = box.m_frame.m_row[3];
    float distance = plane.Dist(r3);
    float side = distance - radius;
    if (side > 0.0f)
        return side;
    side = distance + radius;
    if (side < 0.0f)
        return side;
    return 0.0f;
}

/* CCDBObject's destructor (defined elsewhere) is declared first so its
   table is not emitted here; the leaf-node test template is declared only. */
class CCDBObject : public IVolume {
public:
    virtual ~CCDBObject();
    template <class T>
    int TestCDBBranchNodeCollision(const CDBBranchNode&, const T&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
    template <class T>
    int TestCDBLeafNodeCollision(const CDBLeafNode&, const T&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
};

template <class T>
__declspec(weak) int CCDBObject::TestCDBBranchNodeCollision(const CDBBranchNode& root, const T& volume, CCollision& collision, bool flag,
                                                  dwi::fast_vec<CDBTriGroup*>& groups) const {
    OSGetTick();
    int result = 1;
    const CDBBranchNode* storage[20];
    dwi::fast_vec<const CDBBranchNode*> stack(storage, 20, 11);
    stack.push_back(&root);
    while (stack.size() != 0) {
        const CDBBranchNode* node = stack.back();
        stack.pop_back();
        CPlane plane(node->m_plane);
        float side = GetDistanceFromPlane(plane, volume);
        if (node->m_front && side >= 0.0f) {
            if (node->m_front->m_type == 1) {
                stack.push_back((const CDBBranchNode*)node->m_front);
            } else {
                int r = TestCDBLeafNodeCollision(*(const CDBLeafNode*)node->m_front, volume, collision, flag, groups);
                if (r > result)
                    result = r;
            }
        }
        if (node->m_back && side <= 0.0f) {
            if (node->m_back->m_type == 1) {
                stack.push_back((const CDBBranchNode*)node->m_back);
            } else {
                int r = TestCDBLeafNodeCollision(*(const CDBLeafNode*)node->m_back, volume, collision, flag, groups);
                if (r > result)
                    result = r;
            }
        }
    }
    OSGetTick();
    return result;
}

template int CCDBObject::TestCDBBranchNodeCollision<CVolBox>(const CDBBranchNode&, const CVolBox&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
template int CCDBObject::TestCDBBranchNodeCollision<CVolSphere>(const CDBBranchNode&, const CVolSphere&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
template int CCDBObject::TestCDBBranchNodeCollision<CVolCapsule>(const CDBBranchNode&, const CVolCapsule&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
template int CCDBObject::TestCDBBranchNodeCollision<CAnimatedVolume>(const CDBBranchNode&, const CAnimatedVolume&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
