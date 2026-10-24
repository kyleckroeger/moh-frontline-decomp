// A fragment of box.cpp (0x800c14cc): CVolBox::TestVisibility, the box
// against the camera's six frustum planes (32 bytes each, at +0x150 of the
// draw context's camera): the box's half extents projected on each plane
// normal give a radius; a centre farther than the radius in front of a plane
// is invisible (false), and a box straddling a plane sets that plane's bit
// (the function-local remapCodes table) in the context's clip flags (+0x100).
// The file name is this project's; the original record is box.cpp. The
// classes and functions are named by the mangled symbols; the members, the
// camera and draw-context views, the radius helper and the plane distance
// (centre by value) are inferred; fabs is the inline double function of the
// MSL math header. The rest of the file is in other units.
#include <math.h>
#undef fabs
inline double fabs(double x) { return __fabs(x); }

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
    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }

    CVector3Data d;
} __attribute__((aligned(8)));

struct FrustumPlane {
    CVector3 m_normal;
    float m_distance;
    unsigned char unknown14[12];

    float GetDistance(CVector3 p) const { return m_normal.Dot(p) - m_distance; }
};

class CCamera {
public:
    unsigned char unknown00[336];
    FrustumPlane m_planes[6];
};

class CDrawContext {
public:
    unsigned char unknown00[212];
    CCamera* m_camera;
    unsigned char unknownD8[40];
    unsigned int m_clipFlags;
};

inline float Radius(float a0, float a1, float a2, float h0, float h1, float h2) { return a0 * h0 + a1 * h1 + a2 * h2; }

class IVolume {
public:
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    bool TestVisibility(CDrawContext&) const;

    float m_half[3];
    CVector3 m_axes[3];
    CVector3 m_center;
};

bool CVolBox::TestVisibility(CDrawContext& context) const {
    static unsigned int remapCodes[6] = {0x20, 0x10, 0x2, 0x1, 0x4, 0x8};
    const unsigned int* code = remapCodes;
    const FrustumPlane* plane = context.m_camera->m_planes;
    float h0;
    float h1;
    float h2;
    h2 = m_half[2];
    h1 = m_half[1];
    h0 = m_half[0];
    for (int i = 0; i < 6; i++) {
        unsigned int mask = *code;
        float nx = plane->m_normal.d.c.x;
        float ny = plane->m_normal.d.c.y;
        float nz = plane->m_normal.d.c.z;
        CVector3 proj(m_axes[0].d.c.x * nx + m_axes[0].d.c.y * ny + m_axes[0].d.c.z * nz, m_axes[1].d.c.x * nx + m_axes[1].d.c.y * ny + m_axes[1].d.c.z * nz, m_axes[2].d.c.x * nx + m_axes[2].d.c.y * ny + m_axes[2].d.c.z * nz);
        float a0 = (float)fabs(proj.d.c.x);
        float a1 = (float)fabs(proj.d.c.y);
        float a2 = (float)fabs(proj.d.c.z);
        float radius = Radius(a0, a1, a2, h0, h1, h2);
        float distance = plane->GetDistance(m_center);
        if (distance > radius)
            return false;
        if (distance >= -radius)
            context.m_clipFlags |= mask;
        code++;
        plane++;
    }
    return true;
}
