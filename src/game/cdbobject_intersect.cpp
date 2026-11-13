// A fragment of cdbobject.cpp (0x8006fa54): CCDBObject::GetIntersectingTris
// for a sphere: the branch nodes are walked on a stack over 20 entries of
// stack storage (each node's plane classifies the sphere, as in the
// branch-node test, and the children on its side are pushed), and at each
// leaf the leaf's box and then each triangle of its tri groups are tested
// against the sphere; touching triangles not yet in the vector are appended
// until it is full. The two OSGetTick calls are unused, as in the image. The
// file name is this project's; the original record is cdbobject.cpp. The
// classes and functions are named by the mangled symbols; the layouts,
// member names and the inline helpers are inferred views (shared with
// cdbobject_trigroups.cpp). The compiler's copies of the weak CCDBVolBox,
// CPlane and dwi::fast_vec<const CDBBranchNode*> destructors are weak
// duplicates.
class CMatrix;
/* Inferred: CVector3 as four floats overlaid with two doubles (its copies
   move doubleword pairs). */
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const struct CDBVector& v);
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
    CCollision();
    CCollision(const CCollision&);
    ~CCollision();
    CCollision& operator=(const CCollision&);

    unsigned char unknown00[12];
    float m_distance;
    unsigned char unknown10[16];
};

extern "C" unsigned int OSGetTick();
extern "C" void MEM_free(void*);

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

/* Only the 16-aligned layout of the box's matrix is part of this view. */
class CMatrixView {
public:
    float m[4][4];
} __attribute__((aligned(16)));

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();

    float m_values[3];
    CMatrixView m_transform;
};

struct CDBBox;

/* CCDBVolBox declares an override defined elsewhere first so its table is not
   emitted here. */
class CCDBVolBox : public CVolBox {
public:
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
    CCDBVolBox(const CDBBox&);
    virtual ~CCDBVolBox() {}
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
};
class CVolSphere : public IVolume {
public:
    float GetRadius() const;
    CVector3 GetCenter() const;
};
class CVolCapsule : public IVolume {};
class CAnimatedVolume : public CVolSphere {};

#include "game/DwiContainers.h"
namespace dwi {
// Inferred: the vector over caller storage (inline constructor; the
// destructor frees the storage only when the vector owns it).

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
    int TestCollision(const CVolSphere&, CCollision&, const CDBTri*, bool) const;

    unsigned char unknown00[48];
};

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
    unsigned char m_box[60];
    int m_groupCount;
    CDBTriGroup* m_groups[1];
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

/* CCDBObject's destructor (defined elsewhere) is declared first so its table
   is not emitted here. */
class CCDBObject : public IVolume {
public:
    virtual ~CCDBObject();
    void GetIntersectingTris(dwi::fast_vec<const CDBTri*>&, const CVolSphere&) const;

    // Inferred: the database's root branch node at +48.
    struct CDBView {
        unsigned char unknown00[48];
        const CDBBranchNode* m_root;
    }* m_data;
};

namespace dwi {
// Inferred: dwi's linear search (the library has find_if and copy).
template <class I, class T>
inline I find(I first, I last, const T& value) {
    for (; first != last; first++)
        if (*first == value)
            break;
    return first;
}
}

/* Inferred inline helper (the name is this project's): the leaf's box is
   tested against the sphere, then each triangle of its tri groups; touching
   triangles not yet listed are appended, until the vector is full. */
inline void GetLeafTris(const CDBLeafNode& leaf, dwi::fast_vec<const CDBTri*>& tris, const CVolSphere& sphere) {
    CCDBVolBox box(*(const CDBBox*)leaf.m_box);
    CCollision collision;
    if (box.TestCollision(sphere, collision, false) != 1) {
        for (int i = 0; i < leaf.m_groupCount; i++) {
            CDBTriGroup* group = leaf.m_groups[i];
            for (int j = 0; j < group->m_mark; j++) {
                if (tris.size() == tris.capacity())
                    return;
                const CDBTri* tri = group->m_tris[j];
                CCDBTriangle triangle(*tri);
                if (triangle.TestCollision(sphere, collision, tri, false) != 1) {
                    if (dwi::find(tris.begin(), tris.end(), tri) == tris.end())
                        tris.push_back(tri);
                }
            }
        }
    }
}

void CCDBObject::GetIntersectingTris(dwi::fast_vec<const CDBTri*>& tris, const CVolSphere& sphere) const {
    OSGetTick();
    const CDBBranchNode* storage[20];
    dwi::fast_vec<const CDBBranchNode*> stack(storage, 20, 11);
    stack.push_back(m_data->m_root);
    while (stack.size() != 0) {
        const CDBBranchNode* node = stack.back();
        stack.pop_back();
        CPlane plane(node->m_plane);
        float side = GetDistanceFromPlane(plane, sphere);
        if (node->m_front && side >= 0.0f) {
            if (node->m_front->m_type == 1) {
                stack.push_back((const CDBBranchNode*)node->m_front);
            } else {
                const CDBLeafNode* leaf = (const CDBLeafNode*)node->m_front;
                GetLeafTris(*leaf, tris, sphere);
            }
        }
        if (node->m_back && side <= 0.0f) {
            if (node->m_back->m_type == 1) {
                stack.push_back((const CDBBranchNode*)node->m_back);
            } else {
                const CDBLeafNode* leaf = (const CDBLeafNode*)node->m_back;
                GetLeafTris(*leaf, tris, sphere);
            }
        }
    }
    OSGetTick();
}
