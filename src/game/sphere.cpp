// sphere.cpp: CVolSphere, the whole file, with its vtable and the RTTI records of IVolume and
// CVolSphere: the extents and the view-frustum test; the collision tests against a triangle
// (none), a plane, a line (also a fiber's), a point and another sphere, and the tests that
// hand the pair to the other volume (the collision order swapped) or to the sphere test (an
// animated volume); the radius and centre accessors, TransformedCopy, Create (a new
// default-constructed sphere through DWI_alloc), the assignment and the destructor.
//
// The names come from the mangled symbols; IVolume's virtual functions are declared in the
// order of __vt__7IVolume and CVolSphere's new one (the CSG-volume test) follows
// __vt__10CVolSphere. IVolume's destructor is inline (weak in the original): its copy and
// IVolume's weak vtable are weak duplicates.
//
// Inferred, not in any mangled name: the int results of the tests, the members (radius at
// +12, centre at +16), the collision view (time, line flag, result), the contact record, the
// frustum and draw-context views, the inline class operator new, and the file-private views
// of CVector3 and CMatrix (only the functions used). The CVector3 view copies through an
// 8-byte-aligned base as game/CVector3.h does, but keeps the implicit assignment: the shared
// header's pair-cast operator= schedules SetCenter's copy differently. The point helpers
// Compare/CompareSqrd are inferred inlines. The unit is compiled -inline auto,deferred, which
// emits the functions in reverse source order (and the RTTI name before the constants), so
// they are written last to first.
#include <math.h>

class CDrawContext;
class CTriangle;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;
class CCSGVolume;

void* DWI_alloc(const char*, int, int);

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
    float Dot(const CVector3& o) const { return d.v[0] * o.d.v[0] + d.v[1] * o.d.v[1] + d.v[2] * o.d.v[2]; }
    float Length() const { return sqrtf(LengthSq()); }
    float LengthSq() const { return d.v[0] * d.v[0] + d.v[1] * d.v[1] + d.v[2] * d.v[2]; }
    void Normalize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            d.v[0] *= 1.0f / length;
            d.v[1] *= 1.0f / length;
            d.v[2] *= 1.0f / length;
        }
    }
} __attribute__((aligned(8)));

class CVector4 {
public:
    float x;
    float y;
    float z;
    float w;
};

class CMatrix {
public:
    void TransformPoint(CVector3&, CVector3) const;
    void Transform(CVector4&, CVector3) const;

    float m[4][4];
} __attribute__((aligned(16)));

class CLine3 {
public:
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 GetDirection() const { return m_dir; }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
};

struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
    const void* m_object;
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
    float Dist(const CVector3& v) const {
        return m_normal.d.v[0] * v.d.v[0] + m_normal.d.v[1] * v.d.v[1] + m_normal.d.v[2] * v.d.v[2] - m_d;
    }

    CVector3 m_normal;
    float m_d;
};

struct CFrustumPlane {
    float GetDistance(CVector3 point) const {
        return normal.d.v[0] * point.d.v[0] + normal.d.v[1] * point.d.v[1] + normal.d.v[2] * point.d.v[2] - distance;
    }

    CVector3 normal;
    float distance;
    unsigned char unknown14[12];
};

struct CFrustumView {
    unsigned char unknown000[128];
    CMatrix m_worldToView;
    CMatrix m_projection;
    unsigned char unknown100[80];
    CFrustumPlane planes[6];
};

union ClipFlags {
    struct {
        bool plane3 : 1;
        bool plane2 : 1;
        bool plane4 : 1;
        bool plane5 : 1;
        bool plane1 : 1;
        bool plane0 : 1;
    } bits;
    unsigned int all;
};

class CDrawContext {
public:
    unsigned char unknown00[0xd4];
    CFrustumView* m_view;
    unsigned char unknownD8[0x28];
    ClipFlags m_clipFlags;
    float m_screenMin[2];
    float m_screenMax[2];
};

extern const float CONTACT_EPSILON;
extern const float CONTACT_EPSILON_SQRD;

inline int Compare(float separation) {
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

inline int CompareSqrd(float distSq) {
    if (-distSq > CONTACT_EPSILON_SQRD)
        return 1;
    if (distSq > CONTACT_EPSILON_SQRD)
        return 3;
    return 2;
}

class IVolume {
public:
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

class CVolBox : public IVolume {};
class CCDBObject : public IVolume {};
class CVolCapsule : public IVolume {};
class CWorldVolume : public IVolume {};
class CCSGVolume : public IVolume {};

class CVolFiber : public IVolume {
public:
    const CLine3& GetLine() const;
};

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();
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

    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }

    CVolSphere& operator=(const CVolSphere&);
    float GetRadius() const;
    CVector3 GetCenter() const;
    void SetRadius(float);
    void SetCenter(CVector3);

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
};

class CAnimatedVolume : public CVolSphere {};

CVolSphere::~CVolSphere() {
}

CVolSphere& CVolSphere::operator=(const CVolSphere& other) {
    m_radius = other.m_radius;
    m_center = other.m_center;
    return *this;
}

IVolume* CVolSphere::Create() const {
    return new CVolSphere;
}

void CVolSphere::TransformedCopy(const IVolume& other, const CMatrix& matrix) {
    const CVolSphere& sphere = (const CVolSphere&)other;
    matrix.TransformPoint(m_center, sphere.m_center);
    m_radius = sphere.m_radius;
}

void CVolSphere::SetCenter(CVector3 center) {
    m_center = center;
}

void CVolSphere::SetRadius(float radius) {
    m_radius = radius;
}

CVector3 CVolSphere::GetCenter() const {
    return m_center;
}

float CVolSphere::GetRadius() const {
    return m_radius;
}

int CVolSphere::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    CVector3 direction;
    direction.d.v[0] = other.m_center.d.v[0] - m_center.d.v[0];
    direction.d.v[1] = other.m_center.d.v[1] - m_center.d.v[1];
    direction.d.v[2] = other.m_center.d.v[2] - m_center.d.v[2];
    float distance = sqrtf(direction.d.v[0] * direction.d.v[0] + direction.d.v[1] * direction.d.v[1] +
                           direction.d.v[2] * direction.d.v[2]);
    if (distance != 0.0f) {
        direction.d.v[0] *= 1.0f / distance;
        direction.d.v[1] *= 1.0f / distance;
        direction.d.v[2] *= 1.0f / distance;
    }
    float separation = distance - (m_radius + other.m_radius);
    int result;
    if (separation > CONTACT_EPSILON)
        result = 1;
    else if (separation < -CONTACT_EPSILON)
        result = 3;
    else
        result = 2;
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            SClsnContact contact;
            contact.m_separation = separation;
            float radius = m_radius;
            contact.m_vector0 = direction;
            direction.d.v[0] *= radius;
            direction.d.v[1] *= radius;
            direction.d.v[2] *= radius;
            contact.m_vector1.d.v[0] = m_center.d.v[0] + direction.d.v[0];
            contact.m_vector1.d.v[1] = m_center.d.v[1] + direction.d.v[1];
            contact.m_vector1.d.v[2] = m_center.d.v[2] + direction.d.v[2];
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}

int CVolSphere::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    return TestCollision((const CVolSphere&)other, collision, flag);
}

int CVolSphere::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CWorldVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    collision.m_line = 1;
    return TestCollision(other.GetLine(), collision, flag);
}

int CVolSphere::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(CVector3 point, CCollision& collision, bool flag) const {
    CVector3 direction;
    direction.d.v[0] = point.d.v[0] - m_center.d.v[0];
    direction.d.v[1] = point.d.v[1] - m_center.d.v[1];
    direction.d.v[2] = point.d.v[2] - m_center.d.v[2];
    float distance = sqrtf(direction.d.v[0] * direction.d.v[0] + direction.d.v[1] * direction.d.v[1] +
                           direction.d.v[2] * direction.d.v[2]);
    if (distance != 0.0f) {
        direction.d.v[0] *= 1.0f / distance;
        direction.d.v[1] *= 1.0f / distance;
        direction.d.v[2] *= 1.0f / distance;
    }
    float separation = distance - m_radius;
    int result;
    if (separation > CONTACT_EPSILON)
        result = 1;
    else if (separation < -CONTACT_EPSILON)
        result = 3;
    else
        result = 2;
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            SClsnContact contact;
            contact.m_separation = separation;
            float radius = m_radius;
            contact.m_vector0 = direction;
            direction.d.v[0] *= radius;
            direction.d.v[1] *= radius;
            direction.d.v[2] *= radius;
            contact.m_vector1.d.v[0] = m_center.d.v[0] + direction.d.v[0];
            contact.m_vector1.d.v[1] = m_center.d.v[1] + direction.d.v[1];
            contact.m_vector1.d.v[2] = m_center.d.v[2] + direction.d.v[2];
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}

int CVolSphere::TestCollision(const CLine3& line, CCollision& collision, bool flag) const {
    CVector3 dir;
    CVector3 diff;
    CVector3 point;
    CVector3 normal;
    float t;
    int result;
    dir = line.GetDirection();
    const CVector3& start = line.GetStart();
    diff.d.v[0] = start.d.v[0] - m_center.d.v[0];
    diff.d.v[1] = start.d.v[1] - m_center.d.v[1];
    diff.d.v[2] = start.d.v[2] - m_center.d.v[2];
    float a = dir.LengthSq();
    float b = 2.0f * dir.Dot(diff);
    float distSq = diff.LengthSq();
    float c = distSq - m_radius * m_radius;
    if (a > 1e-05f) {
        float discr = b * b - 4.0f * a * c;
        if (discr < 0.0f) {
            if (flag && collision.m_result < 1)
                collision.m_result = 1;
            return 1;
        }
        if (discr > 1e-05f) {
            float root = sqrtf(discr);
            float inv = 0.5f / a;
            float t0 = inv * (-b - root);
            float t1 = inv * (-b + root);
            if (t0 > 1.0f || t1 < 0.0f) {
                if (flag && collision.m_result < 1)
                    collision.m_result = 1;
                return 1;
            }
            if (t0 >= 0.0f) {
                t = t0;
            } else if (collision.m_line) {
                t = 0.0f;
            } else {
                if (flag && collision.m_result < 3)
                    collision.m_result = 3;
                return 3;
            }
        } else {
            t = -b / (2.0f * a);
        }
    } else {
        collision.m_time = 1.0f;
        float separation = diff.Length() - m_radius;
        int code = Compare(separation);
        if (collision.m_line && code == 3)
            code = 2;
        if (flag) {
            if (code > collision.m_result)
                collision.m_result = code;
            if (code == 2) {
                SClsnContact contact;
                contact.m_vector0 = diff;
                contact.m_separation = separation;
                contact.m_vector1 = line.GetEnd();
                contact.m_vector0.Normalize();
                contact.m_index = 0;
                collision.AddContact(contact);
            }
        }
        return code;
    }
    if (t < 0.0f || t > 1.0f) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        return 1;
    }
    collision.m_time = t;
    point.Scale(dir, t);
    point += line.GetStart();
    if (collision.m_line) {
        distSq = 0.0f;
        result = 2;
    } else {
        const CVector3& end = line.GetEnd();
        diff.d.v[0] = end.d.v[0] - point.d.v[0];
        diff.d.v[1] = end.d.v[1] - point.d.v[1];
        diff.d.v[2] = end.d.v[2] - point.d.v[2];
        distSq = diff.LengthSq();
        result = CompareSqrd(distSq);
        if (result != 2)
            result = 3;
    }
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            normal.d.v[0] = point.d.v[0] - m_center.d.v[0];
            normal.d.v[1] = point.d.v[1] - m_center.d.v[1];
            normal.d.v[2] = point.d.v[2] - m_center.d.v[2];
            normal.Normalize();
            SClsnContact contact;
            contact.m_separation = -sqrtf(distSq);
            contact.m_vector1 = point;
            contact.m_vector0 = normal;
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}

int CVolSphere::TestCollision(const CPlane& plane, CCollision& collision, bool flag) const {
    CVector3 center = m_center;
    float distance = plane.Dist(center) - m_radius;
    int result;
    if (distance > CONTACT_EPSILON)
        result = 1;
    else if (distance < -CONTACT_EPSILON)
        result = 3;
    else
        result = 2;
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            SClsnContact contact;
            distance = -distance;
            contact.m_vector1.d.v[0] = distance * plane.m_normal.d.v[0];
            contact.m_vector1.d.v[1] = distance * plane.m_normal.d.v[1];
            contact.m_vector1.d.v[2] = distance * plane.m_normal.d.v[2];
            contact.m_vector1.d.v[0] += m_center.d.v[0];
            contact.m_vector1.d.v[1] += m_center.d.v[1];
            contact.m_vector1.d.v[2] += m_center.d.v[2];
            contact.m_vector0.d.v[0] = -plane.m_normal.d.v[0];
            contact.m_vector0.d.v[1] = -plane.m_normal.d.v[1];
            contact.m_vector0.d.v[2] = -plane.m_normal.d.v[2];
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}

int CVolSphere::TestCollision(const CTriangle&, CCollision&, bool) const {
    return false;
}

bool CVolSphere::TestVisibility(CDrawContext& context) const {
    CFrustumView* view = context.m_view;
    CMatrix* projection = &view->m_projection;
    CVector3 point;
    view->m_worldToView.TransformPoint(point, m_center);
    CVector3 low(point.d.v[0] - m_radius, point.d.v[1] - m_radius, point.d.v[2]);
    CVector3 high(m_radius + point.d.v[0], m_radius + point.d.v[1], point.d.v[2]);
    CVector4 lowScreen;
    CVector4 highScreen;
    projection->Transform(lowScreen, low);
    projection->Transform(highScreen, high);
    float scale = 1.0f / lowScreen.w;
    context.m_screenMin[0] = scale * lowScreen.x;
    context.m_screenMin[1] = scale * lowScreen.y;
    scale = 1.0f / highScreen.w;
    context.m_screenMax[0] = scale * highScreen.x;
    context.m_screenMax[1] = scale * highScreen.y;
    ClipFlags outside;
    outside.all = 0;
    context.m_clipFlags.all = 0;
    float distance = view->planes[0].GetDistance(m_center);
    outside.bits.plane0 = distance - m_radius > 0.0f;
    context.m_clipFlags.bits.plane0 = distance + m_radius > 0.0f;
    distance = view->planes[1].GetDistance(m_center);
    outside.bits.plane1 = distance - m_radius > 0.0f;
    context.m_clipFlags.bits.plane1 = distance + m_radius > 0.0f;
    distance = view->planes[2].GetDistance(m_center);
    outside.bits.plane2 = distance - m_radius > 0.0f;
    context.m_clipFlags.bits.plane2 = distance + m_radius > 0.0f;
    distance = view->planes[3].GetDistance(m_center);
    outside.bits.plane3 = distance - m_radius > 0.0f;
    context.m_clipFlags.bits.plane3 = distance + m_radius > 0.0f;
    distance = view->planes[4].GetDistance(m_center);
    outside.bits.plane4 = distance - m_radius > 0.0f;
    context.m_clipFlags.bits.plane4 = distance + m_radius > 0.0f;
    distance = view->planes[5].GetDistance(m_center);
    outside.bits.plane5 = distance - m_radius > 0.0f;
    context.m_clipFlags.bits.plane5 = distance + m_radius > 0.0f;
    return outside.all == 0;
}

void CVolSphere::GetExtents(CVector3& min, CVector3& max) const {
    float radius = m_radius;
    min.d.v[0] = m_center.d.v[0] - radius;
    min.d.v[1] = m_center.d.v[1] - radius;
    min.d.v[2] = m_center.d.v[2] - radius;
    max.d.v[0] = m_center.d.v[0] + radius;
    max.d.v[1] = m_center.d.v[1] + radius;
    max.d.v[2] = m_center.d.v[2] + radius;
}
