// The start of sphere.cpp (0x800ca6c8): CVolSphere::GetExtents gives the
// sphere's bounding box, the centre less and plus the radius on each axis,
// and CVolSphere::TestVisibility takes the centre into view space, projects
// the corners of its radius square to the screen rectangle the draw context
// keeps, and tests the centre against the six frustum planes: a plane's flag
// in the context is set when the sphere reaches its positive side, and the
// sphere is visible when it lies wholly on the positive side of none. The
// classes and functions are named by the symbols; the member layout (radius
// at +12, centre at +16) follows sphere.cpp, and the vector view (a
// float/double-pair union), the 3-float constructor, the frustum view
// (matrices at +128 and +192, planes at +336 with their distances at +16),
// the draw context's flags and rectangle (+256..+272) and the flag bits (bool
// bitfields; the plane order of the bits is read from the code) are inferred.
// The rest of the file is in other units.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }

    CVector3Data d;
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

    float m[16];
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

class CVolSphere {
public:
    void GetExtents(CVector3&, CVector3&) const;
    bool TestVisibility(CDrawContext&) const;

    unsigned char unknown00[12];
    float m_radius;
    CVector3 m_center;
};

void CVolSphere::GetExtents(CVector3& min, CVector3& max) const {
    float radius = m_radius;
    min.d.v[0] = m_center.d.v[0] - radius;
    min.d.v[1] = m_center.d.v[1] - radius;
    min.d.v[2] = m_center.d.v[2] - radius;
    max.d.v[0] = m_center.d.v[0] + radius;
    max.d.v[1] = m_center.d.v[1] + radius;
    max.d.v[2] = m_center.d.v[2] + radius;
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
