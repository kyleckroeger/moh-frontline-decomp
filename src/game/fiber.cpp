// fiber.cpp: CVolFiber, the whole file, with its vtable and the RTTI records of IVolume and
// CVolFiber: the line's extents and the visibility through a box around them; the collision
// tests against a triangle (none), a plane, a line (the closest points of the two lines), a
// fiber, a point, and the tests that hand the pair to the other volume (the collision order
// swapped, the line flag set); the line accessors, TransformedCopy, Create, the destructor and
// the three constructors.
//
// The names come from the mangled symbols; IVolume's virtual functions are declared in the
// order of __vt__7IVolume and CVolFiber's new one (the CSG-volume test) follows
// __vt__9CVolFiber. IVolume's destructor is inline (weak in the original): its copy is a weak
// duplicate.
//
// Inferred, not in any mangled name: the int results of the tests, the members (the line at
// +16), the line's cached length fields, the collision, plane and contact views, the box
// view (plain 16-byte-aligned vector records: the box is a 16-aligned stack object without
// array construction), and the file-private views of CVector3 (copied through an 8-byte-aligned
// base as game/CVector3.h does, with the implicit assignment, as sphere.cpp) and CMatrix
// (only the functions used). The unit is compiled -inline auto,deferred, which emits the
// functions in reverse source order (and the RTTI name before the constants), so they are
// written last to first.
#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }

class CDrawContext;
class CTriangle;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;
class CCSGVolume;

/* file-private views (inferred) */
union CVector3Data {
    double pair[2];
    float v[4];
};

struct CVector3Base {
    CVector3Data d;
} __attribute__((aligned(8)));

class CVector3 : public CVector3Base {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    CVector3(const CVector3& o) : CVector3Base(o) {}
    void Sub(const CVector3& a, const CVector3& b) { d.v[0] = a.d.v[0] - b.d.v[0]; d.v[1] = a.d.v[1] - b.d.v[1]; d.v[2] = a.d.v[2] - b.d.v[2]; }
    void Scale(const CVector3& v, float k) {
        d.v[0] = k * v.d.v[0];
        d.v[1] = k * v.d.v[1];
        d.v[2] = k * v.d.v[2];
    }
    CVector3& operator+=(const CVector3& o) {
        d.v[0] += o.d.v[0];
        d.v[1] += o.d.v[1];
        d.v[2] += o.d.v[2];
        return *this;
    }
    CVector3& operator-=(const CVector3& o) {
        d.v[0] -= o.d.v[0];
        d.v[1] -= o.d.v[1];
        d.v[2] -= o.d.v[2];
        return *this;
    }
    void Cross(const CVector3& a, const CVector3& b) {
        d.v[0] = a.d.v[1] * b.d.v[2] - a.d.v[2] * b.d.v[1];
        d.v[1] = a.d.v[2] * b.d.v[0] - a.d.v[0] * b.d.v[2];
        d.v[2] = a.d.v[0] * b.d.v[1] - a.d.v[1] * b.d.v[0];
    }
    float Dot(const CVector3& o) const { return d.v[0] * o.d.v[0] + d.v[1] * o.d.v[1] + d.v[2] * o.d.v[2]; }
    float LengthSq() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }
    float Length() const { return sqrtf(LengthSq()); }
} __attribute__((aligned(8)));

class CMatrix {
public:
    void TransformPoint(CVector3&, CVector3) const;

    float m[4][4];
} __attribute__((aligned(16)));

class CLine3 {
public:
    CLine3() {}
    CLine3(const CLine3& other)
        : m_start(other.m_start), m_end(other.m_end), m_dir(other.m_dir), m_length(other.m_length),
          m_lengthSq(other.m_lengthSq), m_lengthValid(other.m_lengthValid), m_lengthSqValid(other.m_lengthSqValid) {}
    CLine3(CVector3 start, CVector3 end) { SetSE(start, end); }
    CLine3& operator=(const CLine3& other) {
        if (&other != this) {
            m_start = other.m_start;
            m_end = other.m_end;
            m_dir = other.m_dir;
            m_length = other.m_length;
            m_lengthSq = other.m_lengthSq;
            m_lengthValid = other.m_lengthValid;
            m_lengthSqValid = other.m_lengthSqValid;
        }
        return *this;
    }
    void SetSE(CVector3 start, CVector3 end) {
        m_start = start;
        m_end = end;
        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];
        m_lengthValid = 0;
        m_lengthSqValid = 0;
    }
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 GetDirection() const { return m_dir; }
    float GetLengthSq() const {
        if (!m_lengthSqValid) {
            m_lengthSq = m_dir.LengthSq();
            m_lengthSqValid = 1;
        }
        return m_lengthSq;
    }
    float GetLength() const {
        if (!m_lengthValid) {
            m_length = sqrtf(GetLengthSq());
            m_lengthValid = 1;
        }
        return m_length;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    mutable float m_length;
    mutable float m_lengthSq;
    mutable unsigned char m_lengthValid;
    mutable unsigned char m_lengthSqValid;
};

class IVolume;

struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
    const IVolume* m_object;
} __attribute__((aligned(16)));

class CCollision {
public:
    void SwapOrder();
    void AddContact(const SClsnContact&);

    unsigned char unknown00[12];
    float m_time;
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
};

class CPlane {
public:
    bool GetIntersection(const CLine3&, float&, CVector3*) const;

    CVector3 m_normal;
    float m_d;
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

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    virtual bool TestVisibility(CDrawContext&) const;
    void SetCorners(CVector3, CVector3);

    float m_halfWidth;
    float m_halfDepth;
    float m_halfHeight;
    /* plain vector records, 16-byte aligned (the box is a 16-aligned stack object) */
    CVector3Base m_basis[3] __attribute__((aligned(16)));
    CVector3Base m_center;
};

class CCDBObject : public IVolume {};
class CVolCapsule : public IVolume {};
class CWorldVolume : public IVolume {};
class CCSGVolume : public IVolume {};
class CVolSphere : public IVolume {};
class CAnimatedVolume : public CVolSphere {};

class CVolFiber : public IVolume {
public:
    CVolFiber() {}
    CVolFiber(const CLine3&);
    CVolFiber(CVector3, CVector3);
    CVolFiber(const CVolFiber&);
    virtual ~CVolFiber();
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
    virtual int TestCollision(const CCSGVolume&, CCollision&, bool) const;

    const CLine3& GetLine() const;
    void Set(CVector3, CVector3);

    unsigned char unknown04[12];
    CLine3 m_line;
};

CVolFiber::CVolFiber(const CVolFiber& other) : m_line(other.m_line) {
}

CVolFiber::CVolFiber(CVector3 start, CVector3 end) : m_line(start, end) {
}

CVolFiber::CVolFiber(const CLine3& line) : m_line(line) {
}

CVolFiber::~CVolFiber() {
}

IVolume* CVolFiber::Create() const {
    return new CVolFiber;
}

void CVolFiber::TransformedCopy(const IVolume& volume, const CMatrix& matrix) {
    const CVolFiber& other = (const CVolFiber&)volume;
    CVector3 start;
    CVector3 end;

    m_line = other.m_line;
    matrix.TransformPoint(start, other.m_line.m_start);
    matrix.TransformPoint(end, other.m_line.m_end);
    m_line.SetSE(start, end);
}

void CVolFiber::Set(CVector3 start, CVector3 end) {
    m_line.SetSE(start, end);
}

const CLine3& CVolFiber::GetLine() const {
    return m_line;
}

int CVolFiber::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolFiber::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

int CVolFiber::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

int CVolFiber::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

int CVolFiber::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

int CVolFiber::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

int CVolFiber::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

int CVolFiber::TestCollision(const CWorldVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    collision.m_line = 1;
    return other.TestCollision(*this, collision, flag);
}

int CVolFiber::TestCollision(CVector3 point, CCollision& collision, bool flag) const {
    CVector3 dir;
    CVector3 diff;
    collision.m_line = 1;
    dir = m_line.GetDirection();
    diff.Sub(point, m_line.GetStart());
    float t = dir.Dot(diff);
    float distSq = diff.LengthSq();
    float dist = sqrtf(distSq);
    if (t < dist - 1e-12f || t > dist + 1e-12f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float length = m_line.GetLength();
    if (t < 0.0f || t > length) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    if (flag) {
        if (collision.m_result < 2)
            collision.m_result = 2;
        if (length > 1e-12f)
            collision.m_time = t / length;
        else
            collision.m_time = 0.0f;
        SClsnContact contact;
        contact.m_separation = t;
        double ay = fabs(dir.d.v[1]);
        double ax = fabs(dir.d.v[0]);
        if (ax < ay)
            contact.m_vector0.Cross(dir, CVector3(1.0f, 0.0f, 0.0f));
        else {
            CVector3& normal = contact.m_vector0;
            if (ay < fabs(dir.d.v[2]))
                normal.Cross(dir, CVector3(0.0f, 1.0f, 0.0f));
            else
                normal.Cross(dir, CVector3(0.0f, 0.0f, 1.0f));
        }
        contact.m_vector1 = point;
        contact.m_index = 0;
        collision.AddContact(contact);
    }
    return 2;
}

int CVolFiber::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    collision.m_line = 1;
    return TestCollision(other.m_line, collision, flag);
}

int CVolFiber::TestCollision(const CLine3& line, CCollision& collision, bool flag) const {
    CVector3 pa;
    CVector3 n;
    CVector3 l;
    collision.m_line = 1;
    CVector3 p21;
    CVector3 p43;
    CVector3 p13;
    p21.Sub(m_line.GetEnd(), m_line.GetStart());
    p43.Sub(line.GetEnd(), line.GetStart());
    p13.Sub(m_line.GetStart(), line.GetStart());
    if (fabs(p21.d.v[0]) < 1e-12f && fabs(p21.d.v[1]) < 1e-12f && fabs(p21.d.v[2]) < 1e-12f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    if (fabs(p43.d.v[0]) < 1e-12f && fabs(p43.d.v[1]) < 1e-12f && fabs(p43.d.v[2]) < 1e-12f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float d4321 = p43.Dot(p21);
    float d4343 = p43.LengthSq();
    float d2121 = p21.LengthSq();
    float denom = d4343 * d2121 - d4321 * d4321;
    if (fabs(denom) < 1e-12f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float inv = 1.0f / denom;
    float d1321 = p13.Dot(p21);
    float d1343 = p13.Dot(p43);
    float mua = inv * (d1343 * d4321 - d4343 * d1321);
    if (mua < 0.0f || mua > 1.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    float mub = inv * (d4321 * mua + d1343);
    if (mub < 0.0f || mub > 1.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    pa.Scale(p21, mua);
    pa += m_line.GetStart();
    CVector3 pb;
    pb.Scale(p43, mub);
    pb += line.GetStart();
    pa -= pb;
    if (fabs(pa.d.v[0]) > 0.001f || fabs(pa.d.v[1]) > 0.001f || fabs(pa.d.v[2]) > 0.001f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    if (flag) {
        if (collision.m_result < 2)
            collision.m_result = 2;
        SClsnContact contact;
        n.Sub(pa, m_line.GetStart());
        contact.m_separation = n.Length();
        collision.m_time = mua;
        n = m_line.GetDirection();
        l = line.GetDirection();
        contact.m_vector0.d.v[0] = n.d.v[1] * l.d.v[2] - n.d.v[2] * l.d.v[1];
        contact.m_vector0.d.v[1] = n.d.v[2] * l.d.v[0] - n.d.v[0] * l.d.v[2];
        contact.m_vector0.d.v[2] = n.d.v[0] * l.d.v[1] - n.d.v[1] * l.d.v[0];
        contact.m_vector1 = pa;
        contact.m_index = 0;
        collision.AddContact(contact);
    }
    return 2;
}

int CVolFiber::TestCollision(const CPlane& plane, CCollision& collision, bool record) const {
    collision.m_line = 1;
    float t;
    CVector3 point;
    bool hit = plane.GetIntersection(m_line, t, &point);
    bool inRange = false;
    if (t >= 0.0f && t <= 1.0f)
        inRange = true;
    hit &= inRange;
    if (!hit) {
        if (record && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    collision.m_time = t;
    long result = 2;
    if (record) {
        if (result > collision.m_result)
            collision.m_result = result;
        SClsnContact contact;
        contact.m_separation = 0.0f;
        contact.m_vector0.d.v[0] = -plane.m_normal.d.v[0];
        contact.m_vector0.d.v[1] = -plane.m_normal.d.v[1];
        contact.m_vector0.d.v[2] = -plane.m_normal.d.v[2];
        contact.m_vector1 = point;
        contact.m_index = 0;
        collision.AddContact(contact);
    }
    return result;
}

int CVolFiber::TestCollision(const CTriangle&, CCollision& collision, bool) const {
    collision.m_line = 1;
    return false;
}

bool CVolFiber::TestVisibility(CDrawContext& context) const {
    CVolBox box;
    CVector3 min;
    CVector3 max;
    GetExtents(min, max);
    box.SetCorners(m_line.m_start, m_line.m_end);
    return box.TestVisibility(context);
}

void CVolFiber::GetExtents(CVector3& min, CVector3& max) const {
    if (m_line.GetStart().d.v[0] < m_line.GetEnd().d.v[0]) {
        min.d.v[0] = m_line.GetStart().d.v[0];
        max.d.v[0] = m_line.GetEnd().d.v[0];
    } else {
        max.d.v[0] = m_line.GetStart().d.v[0];
        min.d.v[0] = m_line.GetEnd().d.v[0];
    }
    if (m_line.GetStart().d.v[1] < m_line.GetEnd().d.v[1]) {
        min.d.v[1] = m_line.GetStart().d.v[1];
        max.d.v[1] = m_line.GetEnd().d.v[1];
    } else {
        max.d.v[1] = m_line.GetStart().d.v[1];
        min.d.v[1] = m_line.GetEnd().d.v[1];
    }
    if (m_line.GetStart().d.v[2] < m_line.GetEnd().d.v[2]) {
        min.d.v[2] = m_line.GetStart().d.v[2];
        max.d.v[2] = m_line.GetEnd().d.v[2];
    } else {
        max.d.v[2] = m_line.GetStart().d.v[2];
        min.d.v[2] = m_line.GetEnd().d.v[2];
    }
}
