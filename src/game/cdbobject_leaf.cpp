// A fragment of cdbobject.cpp (0x8007287c): the file-local
// GetDistanceFromPlane(const CPlane&, const CVolCapsule&), then the weak
// instances of CCDBObject::TestCDBLeafNodeCollision<T> for CVolBox,
// CVolSphere, CVolCapsule and CAnimatedVolume, the last code of the file.
// The capsule classification takes the signed plane distances of both end
// points: when they lie on one side by more than the radius (their product
// against the squared radius, through a small inline square helper, which
// fixes the register order) the nearer end's sphere is classified, otherwise
// 0. Each leaf test builds a CCDBVolBox from the leaf's box, tests the volume
// against it and, unless the result is 1, gathers each of the leaf's tri
// groups not yet gathered (a positive mark, then negated) into the caller's
// vector; the two OSGetTick calls are unused, as in the image. The file name
// is this project's; the original record is cdbobject.cpp. The classes,
// functions and templates are named by the mangled symbols; the layouts and
// member names are inferred views, and the tests return int-sized results
// (inferred). CCDBVolBox's override for CVolBox is inline (weak in the
// original: it calls CVolBox's test), and it declares an override defined
// elsewhere first so its table is not emitted here; the compiler's copy of
// its inline destructor is a weak duplicate. The explicit instantiations of
// the weak template stand in for the implicit ones made by the branch-node
// tests (the compiler emits them in the reverse order).
class CMatrix;
/* Inferred: CVector3 as four floats overlaid with two doubles (its copies
   move doubleword pairs); 16-aligned (the stack frames holding vectors,
   planes and lines are 16-aligned). */
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const struct CDBVector& v);
    CVector3Data d;
} __attribute__((aligned(16)));

/* CPlane: the normal and the distance (inferred layout). Its destructor is
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
inline float Dot(const CVector3& a, const CVector3& b) {
    return a.d.v[0] * b.d.v[0] + a.d.v[1] * b.d.v[1] + a.d.v[2] * b.d.v[2];
}
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

/* The line's two stored points; the by-value accessors are inferred inline
   helpers (names are this project's). */
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
};

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();
    int TestCollision(const CVolBox&, CCollision&, bool) const;

    float m_values[3];
    CVector3 m_vectors[4];
};

struct CDBBox;

class CCDBVolBox : public CVolBox {
public:
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
    CCDBVolBox(const CDBBox&);
    virtual ~CCDBVolBox() {}
    virtual int TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
        return CVolBox::TestCollision(other, collision, flag);
    }
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
};
class CVolSphere : public IVolume {
public:
    CVolSphere(const CVector3& center, float radius) : m_radius(radius), m_center(center) {}
    virtual ~CVolSphere();
    float GetRadius() const;
    CVector3 GetCenter() const;

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
};
class CVolCapsule : public IVolume {
public:
    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_start;
    CVector3 m_end;
};
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

inline float Sq(float x) { return x * x; }
static float GetDistanceFromPlane(const CPlane& plane, const CVolCapsule& capsule) {
    CVector3 start = capsule.m_start;
    CVector3 end = capsule.m_end;
    float a = Dot(start, plane.m_normal) - plane.m_d;
    float b = Dot(end, plane.m_normal) - plane.m_d;
    if (a * b > Sq(capsule.m_radius)) {
        float fb = __fabs(b);
        float fa = __fabs(a);
        if (fa <= fb) {
            CVolSphere sphere(start, capsule.m_radius);
            return GetDistanceFromPlane(plane, sphere);
        } else {
            CVolSphere sphere(end, capsule.m_radius);
            return GetDistanceFromPlane(plane, sphere);
        }
    }
    return 0.0f;
}

template <class T>
__declspec(weak) int CCDBObject::TestCDBLeafNodeCollision(const CDBLeafNode& leaf, const T& volume, CCollision& collision, bool flag,
                                                         dwi::fast_vec<CDBTriGroup*>& groups) const {
    OSGetTick();
    CCDBVolBox box(*(const CDBBox*)leaf.m_box);
    int result = box.TestCollision(volume, collision, false);
    if (result != 1) {
        for (int i = 0; i < leaf.m_groupCount; i++) {
            CDBTriGroup* group = leaf.m_groups[i];
            if (group->m_mark > 0) {
                group->m_mark = -group->m_mark;
                groups.push_back(group);
            }
        }
    }
    OSGetTick();
    return result;
}

template int CCDBObject::TestCDBLeafNodeCollision<CAnimatedVolume>(const CDBLeafNode&, const CAnimatedVolume&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
template int CCDBObject::TestCDBLeafNodeCollision<CVolCapsule>(const CDBLeafNode&, const CVolCapsule&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
template int CCDBObject::TestCDBLeafNodeCollision<CVolSphere>(const CDBLeafNode&, const CVolSphere&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
template int CCDBObject::TestCDBLeafNodeCollision<CVolBox>(const CDBLeafNode&, const CVolBox&, CCollision&, bool, dwi::fast_vec<CDBTriGroup*>&) const;
