// A fragment of cdbgeom.cpp (0x8006f738): the CCDBVolBox constructor from a
// database box record. The three basis vectors and the centre are built from
// the record's floats, the basis and centre are set, then the width, depth
// and height. The file name is this project's; the original record is
// cdbgeom.cpp. The classes and functions are named by the mangled symbols;
// the record layout (three extents, a 3x3 basis, the centre) and the vector
// view (built from three floats, passed by value as two doubles) are inferred. CCDBVolBox declares its
// destructor (defined elsewhere) first so its table stays elsewhere.
//
// Then the two file-local separate overloads of the box-triangle separating
// axis test: the distance of the projected interval beyond the box's radius
// on an axis (from one projection and one more, or the nearer of two more),
// true when it is beyond CONTACT_EPSILON (a separating axis), otherwise
// recorded with its axis when it is the smallest penetration so far. The
// names and parameter types come from the symbols; CONTACT_EPSILON is
// defined in another file; the parameter meanings, the side classification
// and min/max helpers and the axis enumeration's members are inferred.
#include <math.h>

struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CVector3 {
public:
    CVector3() {}
    CVector3(const float* v) : x(v[0]), y(v[1]), z(v[2]) {}
    CVector3(const CVector3& o) {
        ((double*)this)[0] = ((const double*)&o)[0];
        ((double*)this)[1] = ((const double*)&o)[1];
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

/* Inferred: the database box record. */
struct CDBBox {
    float width;
    float depth;
    float height;
    float basis[3][3];
    float center[3];
};

class IVolume {
public:
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    void SetBasis(CVector3, CVector3, CVector3);
    void SetCenter(CVector3);
    void SetWidth(float);
    void SetDepth(float);
    void SetHeight(float);

    float m_halfWidth;
    float m_halfDepth;
    float m_halfHeight;
    VECTOR3VIEW m_basis[3];
    VECTOR3VIEW m_center;
};

class CCDBVolBox : public CVolBox {
public:
    virtual ~CCDBVolBox();
    CCDBVolBox(const CDBBox&);
};

CCDBVolBox::CCDBVolBox(const CDBBox& box) {
    CVector3 right(box.basis[0]);
    CVector3 forward(box.basis[1]);
    CVector3 up(box.basis[2]);
    CVector3 center(box.center);
    SetBasis(right, forward, up);
    SetCenter(center);
    SetWidth(box.width);
    SetDepth(box.depth);
    SetHeight(box.height);
}

extern const float CONTACT_EPSILON;

/* Inferred: only the name is known. */
enum EBoxTriSeperatingAxis { kAxisNone };

/* Inferred: which side of the contact band a distance lies on. */
static inline int Classify(float d, float eps) {
    if (d > eps)
        return 1;
    if (d < -eps)
        return 3;
    return 2;
}

/* Inferred: the smaller and larger of two values, by reference. */
template <class T> inline const T& Min(const T& a, const T& b) { return a < b ? a : b; }
template <class T> inline const T& Max(const T& a, const T& b) { return a < b ? b : a; }

static bool separate(float r, float p0, float p1, EBoxTriSeperatingAxis axis, float& best, EBoxTriSeperatingAxis& bestAxis) {
    float d;
    if (p0 >= 0.0f) {
        if (p1 > 0.0f)
            p1 = 0.0f;
        d = p0 + p1 - r;
    } else {
        if (p1 < 0.0f)
            p1 = 0.0f;
        d = -(p0 + p1) - r;
    }
    if (Classify(d, CONTACT_EPSILON) == 1)
        return true;
    if ((float)fabs(d) < (float)fabs(best)) {
        best = d;
        bestAxis = axis;
    }
    return false;
}

static bool separate(float r, float p0, float p1, float p2, EBoxTriSeperatingAxis axis, float& best, EBoxTriSeperatingAxis& bestAxis) {
    float d;
    if (p0 >= 0.0f) {
        float m = Min(p1, p2);
        if (m > 0.0f)
            d = p0 - r;
        else
            d = p0 + m - r;
    } else {
        float m = Max(p1, p2);
        if (m < 0.0f)
            d = -p0 - r;
        else
            d = -(p0 + m) - r;
    }
    if (Classify(d, CONTACT_EPSILON) == 1)
        return true;
    if ((float)fabs(d) < (float)fabs(best)) {
        best = d;
        bestAxis = axis;
    }
    return false;
}
