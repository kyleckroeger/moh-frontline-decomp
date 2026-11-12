#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }

class CMatrix;
class CTriangle;
class CPlane;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.c.x = x;
        d.c.y = y;
        d.c.z = z;
    }
    void Normalize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            d.c.x *= 1.0f / length;
            d.c.y *= 1.0f / length;
            d.c.z *= 1.0f / length;
        }
    }
    void Scale(const CVector3& v, float k) {
        d.c.x = k * v.d.c.x;
        d.c.y = k * v.d.c.y;
        d.c.z = k * v.d.c.z;
    }
    CVector3& operator+=(const CVector3& o) {
        d.c.x += o.d.c.x;
        d.c.y += o.d.c.y;
        d.c.z += o.d.c.z;
        return *this;
    }
    void Negate(const CVector3& v) {
        d.c.x = -v.d.c.x;
        d.c.y = -v.d.c.y;
        d.c.z = -v.d.c.z;
    }
    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }
    float LengthSq() const { return d.c.x * d.c.x + d.c.y * d.c.y + d.c.z * d.c.z; }

    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 GetDirection() const { return m_dir; }
    void GetProjection(CVector3, CVector3&, float&) const;

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
    void AddContact(const SClsnContact&);

    unsigned char unknown00[12];
    float m_time;
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
};

extern "C" unsigned long OSGetTick();
extern const float CONTACT_EPSILON;

inline float Sign(float x) {
    float r;
    *(int*)&r = *(int*)&x;
    if (*(int*)&x & 0x7f800000) {
        *(int*)&r = *(int*)&x & 0x80000000;
        *(int*)&r |= 0x3f800000;
    } else {
        *(int*)&r = 0;
    }
    x = r;
    return x;
}

class IVolume {
public:
    virtual ~IVolume();
};

class CVolSphere : public IVolume {
public:
    CVector3 GetCenter() const;
    float GetRadius() const;
};

class CVolBox : public IVolume {
public:
    int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    CVector3 GetCenter() const { return m_center; }
    void BoxPoint(float b0, float b1, float b2, float& x, float& y, float& z) const {
        float px = b0 * m_axes[0].d.c.x;
        float py = b0 * m_axes[0].d.c.y;
        float pz = b0 * m_axes[0].d.c.z;
        x = m_center.d.c.x + px;
        y = m_center.d.c.y + py;
        z = m_center.d.c.z + pz;
        float qx = b1 * m_axes[1].d.c.x;
        float qy = b1 * m_axes[1].d.c.y;
        float qz = b1 * m_axes[1].d.c.z;
        x += qx;
        y += qy;
        z += qz;
        float rx = b2 * m_axes[2].d.c.x;
        float ry = b2 * m_axes[2].d.c.y;
        float rz = b2 * m_axes[2].d.c.z;
        x += rx;
        y += ry;
        z += rz;
    }
    void Rotate(CVector3& r, const CVector3& v) const {
        r.d.c.x = m_axes[0].d.c.x * v.d.c.x + m_axes[1].d.c.x * v.d.c.y + m_axes[2].d.c.x * v.d.c.z;
        r.d.c.y = m_axes[0].d.c.y * v.d.c.x + m_axes[1].d.c.y * v.d.c.y + m_axes[2].d.c.y * v.d.c.z;
        r.d.c.z = m_axes[0].d.c.z * v.d.c.x + m_axes[1].d.c.z * v.d.c.y + m_axes[2].d.c.z * v.d.c.z;
    }

    float m_half[3];
    CVector3 m_axes[3];
    CVector3 m_center;
};

class CVolCapsule : public IVolume {
public:
    unsigned char unknown04[8];
    float m_radius;
    CLine3 m_axis;
};

float BoxLineSqrDistance(const CLine3&, const CVolBox&, float*, float*, float*, float*);
float BoxPointSqrDistance(const CVector3&, const CVolBox&, float*, float*, float*);

inline bool AxisSeparates(float dd, float ds, float half, float r) {
    bool sep;
    if ((float)fabs(dd) > 1e-5f) {
        if (dd > 0.0f) {
            if (ds > 0.0f)
                sep = ds - r > half;
            else
                sep = r + (ds + dd) < -half;
        } else if (ds > 0.0f) {
            sep = (ds + dd) - r > half;
        } else {
            sep = ds + r < -half;
        }
    } else {
        sep = (float)fabs(ds) - r > half;
    }
    return sep;
}

inline float SegmentBoxSqrDistance(const CLine3& line, const CVolBox& box, float* t, float* b0, float* b1, float* b2) {
    float lt, lb0, lb1, lb2;
    float sqDist = BoxLineSqrDistance(line, box, &lt, &lb0, &lb1, &lb2);
    if (lt >= 0.0f) {
        if (lt <= 1.0f) {
            *t = lt;
            *b0 = lb0;
            *b1 = lb1;
            *b2 = lb2;
            return sqDist;
        } else {
            sqDist = BoxPointSqrDistance(line.GetEnd(), box, b0, b1, b2);
            if (t)
                *t = 1.0f;
            return sqDist;
        }
    } else {
        sqDist = BoxPointSqrDistance(line.GetStart(), box, b0, b1, b2);
        if (t)
            *t = 0.0f;
        return sqDist;
    }
}

inline void Evaluate(const CLine3& line, float t, float& x, float& y, float& z) {
    CVector3 s;
    CVector3 d;
    d = line.GetDirection();
    s = line.GetStart();
    float sx = t * d.d.c.x;
    float sy = t * d.d.c.y;
    float sz = t * d.d.c.z;
    x = s.d.c.x + sx;
    y = s.d.c.y + sy;
    z = s.d.c.z + sz;
}

inline float SegmentBoxDistance(const CLine3& line, const CVolBox& box, float* t, float* b0, float* b1, float* b2, const float& radius) {
    float sq = SegmentBoxSqrDistance(line, box, t, b0, b1, b2);
    float rr = radius;
    return sqrtf(sq) - rr;
}

inline int Compare(float separation) {
    if (separation > CONTACT_EPSILON)
        return 1;
    if (separation < -CONTACT_EPSILON)
        return 3;
    return 2;
}

int CVolBox::TestCollision(const CVolCapsule& capsule, CCollision& collision, bool flag) const {
    OSGetTick();
    CVector3 start = capsule.m_axis.GetStart();
    CVector3 dir = capsule.m_axis.GetDirection();
    const CLine3& line = capsule.m_axis;
    CVector3 diff;
    diff.d.c.x = start.d.c.x - m_center.d.c.x;
    diff.d.c.y = start.d.c.y - m_center.d.c.y;
    diff.d.c.z = start.d.c.z - m_center.d.c.z;
    float r = CONTACT_EPSILON + capsule.m_radius;
    int result;
    if (AxisSeparates(dir.Dot(m_axes[0]), diff.Dot(m_axes[0]), m_half[0], r)) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        result = 1;
    } else if (AxisSeparates(dir.Dot(m_axes[1]), diff.Dot(m_axes[1]), m_half[1], r)) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        result = 1;
    } else if (AxisSeparates(dir.Dot(m_axes[2]), diff.Dot(m_axes[2]), m_half[2], r)) {
        if (flag && collision.m_result < 1)
            collision.m_result = 1;
        result = 1;
    } else {
        CVector3 p;
        CVector3 tmp;
        CVector3 normal;
        float t, b0, b1, b2;
        line.GetProjection(m_center, p, b2);
        float nx = p.d.c.x - m_center.d.c.x;
        float ny = p.d.c.y - m_center.d.c.y;
        float nz = p.d.c.z - m_center.d.c.z;
        if (nx * nx + ny * ny + nz * nz > r * r) {
    float h0;
    float h1;
    float h2;
    float lenSq;
    float inv;
    float a2;
    float len;
    float a1;
    float a0;
            lenSq = nx * nx + ny * ny + nz * nz;
            len = sqrtf(lenSq);
            inv = 1.0f / len;
            nx *= inv;
            ny *= inv;
            nz *= inv;
        h0 = m_half[0];
            h2 = m_half[2];
            a2 = (float)fabs(h2 * m_axes[2].d.c.x * nx + h2 * m_axes[2].d.c.y * ny + h2 * m_axes[2].d.c.z * nz);
            h1 = m_half[1];
            a1 = (float)fabs(h1 * m_axes[1].d.c.x * nx + h1 * m_axes[1].d.c.y * ny + h1 * m_axes[1].d.c.z * nz);
            a0 = (float)fabs(h0 * m_axes[0].d.c.x * nx + h0 * m_axes[0].d.c.y * ny + h0 * m_axes[0].d.c.z * nz);
            float proj = a0 + a1 + a2;
            if (len - proj > r) {
                if (flag && collision.m_result < 1)
                    collision.m_result = 1;
                result = 1;
                goto done;
            }
        }
        float separation = SegmentBoxDistance(line, *this, &t, &b0, &b1, &b2, capsule.m_radius);
        int code = Compare(separation);
        result = code;
        if (flag) {
            if (code > collision.m_result)
                collision.m_result = code;
            if (code == 2) {
                float cx, cy, cz;
                Evaluate(line, t, cx, cy, cz);
                float bx, by, bz;
                BoxPoint(b0, b1, b2, bx, by, bz);
                normal.d.c.x = cx - bx;
                normal.d.c.y = cy - by;
                normal.d.c.z = cz - bz;
                normal.Normalize();
                SClsnContact contact;
                contact.m_separation = separation;
                float k = -capsule.m_radius;
                float ox = k * normal.d.c.x;
                float oy = k * normal.d.c.y;
                float oz = k * normal.d.c.z;
                tmp.d.c.x = cx + ox;
                tmp.d.c.y = cy + oy;
                tmp.d.c.z = cz + oz;
                contact.m_vector1 = tmp;
                contact.m_vector0 = normal;
                contact.m_index = 0;
                collision.AddContact(contact);
            }
        }
    }
done:
    return result;
}

