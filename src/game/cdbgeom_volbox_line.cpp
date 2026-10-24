// A fragment of cdbgeom.cpp (0x8006efc0): CCDBVolBox::TestCollision against
// a line, a slab test over the box's three axes: each axis clips the line
// parameter range by the projections of the centre offset and the direction
// (a direction nearly perpendicular to the axis misses when the offset is
// outside the half extent). An empty range, or one outside [0, 1], returns 1;
// otherwise 3 (the collision is not written). The file name is this
// project's; the original record is cdbgeom.cpp. The classes and functions
// are named by the mangled symbols; the members, the inline clip helper with
// by-value vectors and the result codes are inferred. // The vector helpers use the x, y, z view of CVector3 (its copies move
// doubleword pairs) and fabs is the inline double function of the MSL math
// header (the SDK macro form gives other register numbers). 
// Constants are the file's .sdata2 pool entries. The rest of the file is in
// other units.
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
    CVector3(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
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
    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }
    float LengthSq() const { return d.c.x * d.c.x + d.c.y * d.c.y + d.c.z * d.c.z; }

    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }
    CVector3 GetDirection() const { return m_dir; }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
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

inline bool Clip(CVector3 axis, CVector3 offset, CVector3 dir, float half, float& tNear, float& tFar) {
    float e = axis.Dot(offset);
    float d = axis.Dot(dir);
    if ((float)fabs(d) > 1e-05f) {
        float inv = 1.0f / d;
        float t1 = inv * (e + half);
        float t2 = inv * (e - half);
        if (t1 > t2) {
            if (t2 > tNear)
                tNear = t2;
            if (t1 < tFar)
                tFar = t1;
        } else {
            if (t1 > tNear)
                tNear = t1;
            if (t2 < tFar)
                tFar = t2;
        }
        if (tNear > tFar)
            return false;
        if (tFar < 0.0f)
            return false;
    } else if (e > half || e < -half) {
        return false;
    }
    return true;
}

class IVolume {
public:
    virtual ~IVolume();
};

class CCDBVolBox : public IVolume {
public:
    int TestCollision(const CLine3&, CCollision&, bool) const;
    CVector3 GetAxis(int i) const { return m_axes[i]; }

    float m_half[3];
    CVector3 m_axes[3];
    CVector3 m_center;
};

int CCDBVolBox::TestCollision(const CLine3& line, CCollision&, bool) const {
    CVector3 dir;
    CVector3 offset;
    float tNear = -3.4028235e+38f;
    float tFar = 3.4028235e+38f;
    dir = line.GetDirection();
    const CVector3& start = line.GetStart();
    offset.d.c.x = m_center.d.c.x - start.d.c.x;
    offset.d.c.y = m_center.d.c.y - start.d.c.y;
    offset.d.c.z = m_center.d.c.z - start.d.c.z;
    const CVector3* rdir = &dir;
    const CVector3* roff = &offset;
    if (!Clip(GetAxis(0), *roff, *rdir, m_half[0], tNear, tFar))
        return 1;
    if (!Clip(GetAxis(1), *roff, *rdir, m_half[1], tNear, tFar))
        return 1;
    if (!Clip(GetAxis(2), *roff, *rdir, m_half[2], tNear, tFar))
        return 1;
    if (tNear > 1.0f || tFar < 0.0f)
        return 1;
    return 3;
}
