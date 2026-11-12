// CCDBVolBox's weak destructor (0x8006a620: its table pointer, then CVolBox's
// destructor, called; the object freed when asked) and collision tests
// emitted in cdbgeom.cpp: no collision with world volumes, and the point, CDB-object, box and generic-volume tests made as its
// CVolBox base. The names come from the mangled symbols; CVolBox's tests are a
// non-virtual view of the base and the result type (bool) is inferred. They
// are inline in the original (weak symbols), so they are defined
// __declspec(weak). CVolBox and CCDBVolBox are views here: CCDBVolBox
// declares Create (defined elsewhere) first so its global table stays
// elsewhere. The rest of the file is not part of this unit.
#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }

class CCollision;
class CWorldVolume;
class CCDBObject;
class IVolume;

union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    CVector3 operator=(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
        return *this;
    }
    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }
    float LengthSq() const { return d.c.x * d.c.x + d.c.y * d.c.y + d.c.z * d.c.z; }
    float LengthSq2() const {
        float xx = d.c.x * d.c.x;
        float yy = d.c.y * d.c.y;
        float zz = d.c.z * d.c.z;
        return xx + yy + zz;
    }
    void Cross(const CVector3& a, const CVector3& b) {
        d.c.x = a.d.c.y * b.d.c.z - a.d.c.z * b.d.c.y;
        d.c.y = a.d.c.z * b.d.c.x - a.d.c.x * b.d.c.z;
        d.c.z = a.d.c.x * b.d.c.y - a.d.c.y * b.d.c.x;
    }
    void Scale(const CVector3& v, float k) {
        d.c.x = k * v.d.c.x;
        d.c.y = k * v.d.c.y;
        d.c.z = k * v.d.c.z;
    }
    CVector3 operator*(float k) const {
        CVector3 r;
        r.d.c.x = k * d.c.x;
        r.d.c.y = k * d.c.y;
        r.d.c.z = k * d.c.z;
        return r;
    }
    CVector3 operator+(const CVector3& o) const {
        CVector3 r;
        r.d.c.x = d.c.x + o.d.c.x;
        r.d.c.y = d.c.y + o.d.c.y;
        r.d.c.z = d.c.z + o.d.c.z;
        return r;
    }
    CVector3 operator-(const CVector3& o) const {
        CVector3 r;
        r.d.c.x = d.c.x - o.d.c.x;
        r.d.c.y = d.c.y - o.d.c.y;
        r.d.c.z = d.c.z - o.d.c.z;
        return r;
    }
    CVector3& operator+=(const CVector3& o) {
        d.c.x += o.d.c.x;
        d.c.y += o.d.c.y;
        d.c.z += o.d.c.z;
        return *this;
    }
    float Normalize();
    float Length() const;
    void DotDotDot(const CVector3&, const CVector3&, const CVector3&, const CVector3&);

    CVector3Data d;
} __attribute__((aligned(8)));
class CVolBox {
public:
    virtual ~CVolBox();
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
    bool TestCollision(const IVolume&, CCollision&, bool) const;
};

class CCDBVolBox : public CVolBox {
public:
    virtual IVolume* Create() const; /* defined elsewhere; keeps the table out */
    virtual ~CCDBVolBox();
    bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
    bool TestCollision(const IVolume&, CCollision&, bool) const;
};

__declspec(weak) CCDBVolBox::~CCDBVolBox() {
}

__declspec(weak) bool CCDBVolBox::TestCollision(const CWorldVolume&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool CCDBVolBox::TestCollision(CVector3 point, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(point, collision, flag);
}

__declspec(weak) bool CCDBVolBox::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(other, collision, flag);
}

__declspec(weak) bool CCDBVolBox::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(other, collision, flag);
}

__declspec(weak) bool CCDBVolBox::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(other, collision, flag);
}

inline void SetDifference(CVector3& r, const CVector3& a, const CVector3& b) {
    r.d.c.x = a.d.c.x - b.d.c.x;
    r.d.c.y = a.d.c.y - b.d.c.y;
    r.d.c.z = a.d.c.z - b.d.c.z;
}

inline void SetSum(CVector3& r, const CVector3& a, const CVector3& b) {
    r.d.c.x = a.d.c.x + b.d.c.x;
    r.d.c.y = a.d.c.y + b.d.c.y;
    r.d.c.z = a.d.c.z + b.d.c.z;
}

inline float Separation2(float sq, float r) { float d = sqrtf(sq); return d - r; }

class CLine3 {
public:
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 GetDirection() const { return m_dir; }
    void SetSD(CVector3, CVector3);
    void GetSquaredDistanceBetweenSegments(const CLine3&, float&, float&, float*) const;

    ~CLine3() {}
    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    mutable float m_length;
    mutable float m_lengthSq;
    mutable unsigned char m_lengthValid;
    mutable unsigned char m_lengthSqValid;
};

class CDBTri;

struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    const CDBTri* m_tri;
    const IVolume* m_object;
} __attribute__((aligned(16)));

class CCollision {
public:
    void AddContact(const SClsnContact&);

    unsigned char unknown00[20];
    int m_result;
};

extern const float CONTACT_EPSILON;

inline int Compare(float separation) {
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

inline int Classify(float sq, float r, float& sep) { sep = sqrtf(sq) - r; return Compare(sep); }
inline float Separation(float sq, float r);
inline int Classify2(float& sep, float sq, float r) { sep = Separation(sq, r); return Compare(sep); }
inline float Separation(float sq, float r) { return sqrtf(sq) - r; }
class CVolCapsule {
public:
    unsigned char unknown00[12];
    float m_radius;
    CLine3 m_line;
};

class CCDBTriangle {
public:
    int TestCollision(const CVolCapsule&, CCollision&, const CDBTri*, bool) const;
    CVector3 GetNearestPoint(CVector3, float*, float*) const;

    CVector3 m_vertices[3];
};

#define EDGE0(a, b, dist) \
    seg.SetSD(m_vertices[0], e0); \
    line.GetSquaredDistanceBetweenSegments(seg, a, b, &dist)
#define EDGE1(a, b, dist) \
    seg.SetSD(m_vertices[0], e1); \
    line.GetSquaredDistanceBetweenSegments(seg, a, b, &dist)
#define EDGE2(a, b, dist) \
    seg.SetSD(m_vertices[1], e2); \
    line.GetSquaredDistanceBetweenSegments(seg, a, b, &dist)
#define POINT(p) \
    pt = GetNearestPoint(p, &fS0, &fT0); \
    SetDifference(delta, p, pt); \
    fSqrDist0 = delta.LengthSq()
#define TAKE() \
    if (fSqrDist0 < fSqrDist) { \
        fSqrDist = fSqrDist0; \
        fR = fR0; \
        fS = fS0; \
        fT = fT0; \
    }

int CCDBTriangle::TestCollision(const CVolCapsule& capsule, CCollision& collision, const CDBTri* tri, bool flag) const {
    float fSqrDist;
    float fSqrDist0;
    float fR;
    float fS;
    float fT;
    float fR0;
    float fS0;
    float fT0;
    CLine3 seg;
    SClsnContact contact;
    CVector3 pt;
    CVector3 delta;
    const CLine3& line = capsule.m_line;
    CVector3 start = line.GetStart();
    CVector3 end = line.GetEnd();
    CVector3 dir = line.GetDirection();
    CVector3 diff;
    CVector3 e0;
    CVector3 e1;
    CVector3 e2;
    CVector3 normal;
    SetDifference(diff, m_vertices[0], start);
    SetDifference(e0, m_vertices[1], m_vertices[0]);
    SetDifference(e1, m_vertices[2], m_vertices[0]);
    SetDifference(e2, m_vertices[2], m_vertices[1]);
    normal.Cross(e0, e1);
    float lengthN = normal.Length();
    float lengthD = dir.Length();
    if ((float)fabs(normal.Dot(dir)) > 0.001745328f * (lengthN * lengthD)) {
        CVector3 a;
        a.DotDotDot(dir, e0, e1, dir);
        float fA00 = a.d.c.x;
        float fA01 = -a.d.c.y;
        float fA02 = -a.d.c.z;
        float fA11 = e0.LengthSq2();
        float fA12 = e0.Dot(e1);
        float fA22 = e1.LengthSq();
        float fB0 = -diff.Dot(dir);
        float fB1 = diff.Dot(e0);
        float fB2 = diff.Dot(e1);
        float fCof00 = fA11 * fA22 - fA12 * fA12;
        float fCof01 = fA12 * fA02 - fA01 * fA22;
        float fCof02 = fA01 * fA12 - fA11 * fA02;
        float fCof11 = fA00 * fA22 - fA02 * fA02;
        float fCof12 = fA02 * fA01 - fA00 * fA12;
        float fCof22 = fA00 * fA11 - fA01 * fA01;
        float fInvDet = 1.0f / (fA00 * fCof00 + fA01 * fCof01 + fA02 * fCof02);
        float fRhs0 = -fB0 * fInvDet;
        float fRhs1 = -fB1 * fInvDet;
        float fRhs2 = -fB2 * fInvDet;
        fR = fCof00 * fRhs0 + fCof01 * fRhs1 + fCof02 * fRhs2;
        fS = fCof01 * fRhs0 + fCof11 * fRhs1 + fCof12 * fRhs2;
        fT = fCof02 * fRhs0 + fCof12 * fRhs1 + fCof22 * fRhs2;
        if (fR < 0.0f) {
            if (fS + fT <= 1.0f) {
                if (fS < 0.0f) {
                    if (fT < 0.0f) {
                        EDGE1(fR, fT, fSqrDist);
                        fS = 0.0f;
                        EDGE0(fR0, fS0, fSqrDist0);
                        fT0 = 0.0f;
                        TAKE();
                        POINT(start);
                        fR0 = 0.0f;
                        TAKE();
                    } else {
                        EDGE1(fR, fT, fSqrDist);
                        fS = 0.0f;
                        POINT(start);
                        fR0 = 0.0f;
                        TAKE();
                    }
                } else if (fT < 0.0f) {
                    EDGE0(fR, fS, fSqrDist);
                    fT = 0.0f;
                    POINT(start);
                    fR0 = 0.0f;
                    TAKE();
                } else {
                    pt = GetNearestPoint(start, &fS, &fT);
                    SetDifference(delta, start, pt);
                    fSqrDist = delta.LengthSq();
                    fR = 0.0f;
                }
            } else {
                if (fS < 0.0f) {
                    EDGE1(fR, fT, fSqrDist);
                    fS = 0.0f;
                    EDGE2(fR0, fT0, fSqrDist0);
                    fS0 = 1.0f - fT0;
                    TAKE();
                    pt = GetNearestPoint(start, &fS0, &fT0);
                    SetDifference(diff, start, pt);
                    fSqrDist0 = diff.LengthSq2();
                    fR0 = 0.0f;
                    TAKE();
                } else if (fT < 0.0f) {
                    EDGE0(fR, fS, fSqrDist);
                    fT = 0.0f;
                    EDGE2(fR0, fT0, fSqrDist0);
                    fS0 = 1.0f - fT0;
                    TAKE();
                    POINT(start);
                    fR0 = 0.0f;
                    TAKE();
                } else {
                    EDGE2(fR, fT, fSqrDist);
                    fS = 1.0f - fT;
                    POINT(start);
                    fR0 = 0.0f;
                    TAKE();
                }
            }
        } else if (fR <= 1.0f) {
            if (fS + fT <= 1.0f) {
                if (fS < 0.0f) {
                    if (fT < 0.0f) {
                        EDGE1(fR, fT, fSqrDist);
                        fS = 0.0f;
                        EDGE0(fR0, fS0, fSqrDist0);
                        fT0 = 0.0f;
                        TAKE();
                    } else {
                        EDGE1(fR, fT, fSqrDist);
                        fS = 0.0f;
                    }
                } else if (fT < 0.0f) {
                    EDGE0(fR, fS, fSqrDist);
                    fT = 0.0f;
                } else {
                    fSqrDist = fR * (fA00 * fR + fA01 * fS + fA02 * fT + 2.0f * fB0) +
                               fS * (fA01 * fR + fA11 * fS + fA12 * fT + 2.0f * fB1) +
                               fT * (fA02 * fR + fA12 * fS + fA22 * fT + 2.0f * fB2) + diff.LengthSq();
                }
            } else {
                if (fS < 0.0f) {
                    EDGE1(fR, fT, fSqrDist);
                    fS = 0.0f;
                    EDGE2(fR0, fT0, fSqrDist0);
                    fS0 = 1.0f - fT0;
                    TAKE();
                } else if (fT < 0.0f) {
                    EDGE0(fR, fS, fSqrDist);
                    fT = 0.0f;
                    EDGE2(fR0, fT0, fSqrDist0);
                    fS0 = 1.0f - fT0;
                    TAKE();
                } else {
                    EDGE2(fR, fT, fSqrDist);
                    fS = 1.0f - fT;
                }
            }
        } else {
            if (fS + fT <= 1.0f) {
                if (fS < 0.0f) {
                    if (fT < 0.0f) {
                        EDGE1(fR, fT, fSqrDist);
                        fS = 0.0f;
                        EDGE0(fR0, fS0, fSqrDist0);
                        fT0 = 0.0f;
                        TAKE();
                        POINT(end);
                        fR0 = 1.0f;
                        TAKE();
                    } else {
                        EDGE1(fR, fT, fSqrDist);
                        fS = 0.0f;
                        POINT(end);
                        fR0 = 1.0f;
                        TAKE();
                    }
                } else if (fT < 0.0f) {
                    EDGE0(fR, fS, fSqrDist);
                    fT = 0.0f;
                    POINT(end);
                    fR0 = 1.0f;
                    TAKE();
                } else {
                    pt = GetNearestPoint(end, &fS, &fT);
                    SetDifference(delta, end, pt);
                    fSqrDist = delta.LengthSq();
                    fR = 1.0f;
                }
            } else {
                if (fS < 0.0f) {
                    EDGE1(fR, fT, fSqrDist);
                    fS = 0.0f;
                    EDGE2(fR0, fT0, fSqrDist0);
                    fS0 = 1.0f - fT0;
                    TAKE();
                    POINT(end);
                    fR0 = 1.0f;
                    TAKE();
                } else if (fT < 0.0f) {
                    EDGE0(fR, fS, fSqrDist);
                    fT = 0.0f;
                    EDGE2(fR0, fT0, fSqrDist0);
                    fS0 = 1.0f - fT0;
                    TAKE();
                    POINT(end);
                    fR0 = 1.0f;
                    TAKE();
                } else {
                    EDGE2(fR, fT, fSqrDist);
                    fS = 1.0f - fT;
                    POINT(end);
                    fR0 = 1.0f;
                    TAKE();
                }
            }
        }
    } else {
        EDGE0(fR, fS, fSqrDist);
        fT = 0.0f;
        EDGE1(fR0, fT0, fSqrDist0);
        fS0 = 0.0f;
        TAKE();
        EDGE2(fR0, fT0, fSqrDist0);
        fS0 = 1.0f - fT0;
        TAKE();
        POINT(start);
        fR0 = 0.0f;
        TAKE();
        POINT(end);
        fR0 = 1.0f;
        TAKE();
    }
    float separation;
    int code = Classify(fSqrDist, capsule.m_radius, separation);
    if (flag) {
        if (code > collision.m_result)
            collision.m_result = code;
        if (code == 2) {
            // The edge products are named locals (the original keeps them apart from the
            // edge loads); no value wrapper.
            CVector3 point;
            float svx = fS * e0.d.c.x;
            float svy = fS * e0.d.c.y;
            float svz = fS * e0.d.c.z;
            CVector3 sv;
            sv.d.c.x = svx;
            sv.d.c.y = svy;
            sv.d.c.z = svz;
            SetSum(point, m_vertices[0], sv);
            float tvx = fT * e1.d.c.x;
            float tvy = fT * e1.d.c.y;
            float tvz = fT * e1.d.c.z;
            CVector3 tv;
            tv.d.c.x = tvx;
            tv.d.c.y = tvy;
            tv.d.c.z = tvz;
            point += tv;
            CVector3 rv;
            rv.Scale(dir, fR);
            diff.d.c.x = start.d.c.x + rv.d.c.x - point.d.c.x;
            diff.d.c.y = start.d.c.y + rv.d.c.y - point.d.c.y;
            diff.d.c.z = start.d.c.z + rv.d.c.z - point.d.c.z;
            diff.Normalize();
            contact.m_separation = separation;
            contact.m_vector1 = point;
            contact.m_vector0 = diff;
            contact.m_tri = tri;
            collision.AddContact(contact);
        }
    }
    return code;
}
