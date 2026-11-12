// A fragment of cdbgeom.cpp (0x8006c428): CCDBTriangle::TestCollision
// against an animated volume (tested as its sphere through the same code,
// inlined) and against a sphere (the triangle's nearest point to the centre;
// the distance beyond the radius gives 1 (apart) above CONTACT_EPSILON, 3
// below its negative and 2 within it; on request the result is raised into
// the collision and a touching contact recorded: the unit direction from the
// point to the centre, the point, the separation and the triangle). The file
// name is this project's; the original record is cdbgeom.cpp. The classes and
// functions are named by the mangled symbols; CVector3 is the double-pair
// view with a copy constructor (the call temporaries are built in place, as
// in the image), the contact view follows capsule_dispatch.cpp with the
// triangle at +36 (as decal_reset.cpp reads it), and the collision's result
// at +20 is inferred. The animated-volume test's inline helper is inferred
// from its stack layout (the temporaries sit as an inline's do). The square
// root is the libc inline sqrtf. The constants link to their pool entries.
// GetNearestPoint follows: the point of the triangle nearest a point, by the
// usual edge-parameter regions (Eberly's point-triangle distance), with the
// two parameters returned when asked. The vertex member is named as in
// cdbgeom_triangle_line.cpp; the local names are inferred. The dot products
// are CVector3::Dot in the order b0, a01, a00, b1, a11 (that order gives the
// original's schedule).
#include <math.h>

/* Inferred: CVector3 as four floats overlaid with two doubles. */


#include "game/CVector3.h"

class CDBTri;
class IVolume;

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

class CVolSphere {
public:
    float GetRadius() const;
    CVector3 GetCenter() const;
};

extern const float CONTACT_EPSILON;

class CAnimatedVolume : public CVolSphere {};

class CCDBTriangle {
public:
    CVector3 GetNearestPoint(CVector3, float*, float*) const;
    int TestCollision(const CAnimatedVolume&, CCollision&, const CDBTri*, bool) const;
    int TestCollision(const CVolSphere&, CCollision&, const CDBTri*, bool) const;

    CVector3 m_vertices[3];
};

inline int TestSphere(const CCDBTriangle* t, const CVolSphere& sphere, CCollision& collision, const CDBTri* tri, bool flag) {
    CVector3 nearest = t->GetNearestPoint(sphere.GetCenter(), 0, 0);
    const CVector3& center = sphere.GetCenter();
    CVector3 diff;
    diff.x = center.x - nearest.x;
    diff.y = center.y - nearest.y;
    diff.z = center.z - nearest.z;
    float distance = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
    float separation = distance - sphere.GetRadius();
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
            float scale = 1.0f / distance;
            SClsnContact contact;
            contact.m_vector0.x = scale * diff.x;
            contact.m_vector0.y = scale * diff.y;
            contact.m_vector0.z = scale * diff.z;
            contact.m_vector1 = nearest;
            contact.m_separation = separation;
            contact.m_tri = tri;
            collision.AddContact(contact);
        }
    }
    return result;
}

int CCDBTriangle::TestCollision(const CAnimatedVolume& volume, CCollision& collision, const CDBTri* tri, bool flag) const {
    return TestSphere(this, volume, collision, tri, flag);
}

int CCDBTriangle::TestCollision(const CVolSphere& sphere, CCollision& collision, const CDBTri* tri, bool flag) const {
    CVector3 nearest = GetNearestPoint(sphere.GetCenter(), 0, 0);
    const CVector3& center = sphere.GetCenter();
    CVector3 diff;
    diff.x = center.x - nearest.x;
    diff.y = center.y - nearest.y;
    diff.z = center.z - nearest.z;
    float distance = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
    float separation = distance - sphere.GetRadius();
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
            float scale = 1.0f / distance;
            SClsnContact contact;
            contact.m_vector0.x = scale * diff.x;
            contact.m_vector0.y = scale * diff.y;
            contact.m_vector0.z = scale * diff.z;
            contact.m_vector1 = nearest;
            contact.m_separation = separation;
            contact.m_tri = tri;
            collision.AddContact(contact);
        }
    }
    return result;
}

CVector3 CCDBTriangle::GetNearestPoint(CVector3 point, float* outS, float* outT) const {
    CVector3 e0;
    CVector3 e1;
    CVector3 diff;
    float a01, a00, b1, b0, a11;
    e0.x = m_vertices[1].x - m_vertices[0].x;
    e0.y = m_vertices[1].y - m_vertices[0].y;
    e0.z = m_vertices[1].z - m_vertices[0].z;
    e1.x = m_vertices[2].x - m_vertices[0].x;
    e1.y = m_vertices[2].y - m_vertices[0].y;
    e1.z = m_vertices[2].z - m_vertices[0].z;
    diff.y = m_vertices[0].y - point.y;
    diff.x = m_vertices[0].x - point.x;
    diff.z = m_vertices[0].z - point.z;
    b0 = diff.Dot(e0);
    a01 = e1.Dot(e0);
    a00 = e0.Dot(e0);
    b1 = diff.Dot(e1);
    a11 = e1.Dot(e1);
    float det = fabs(a00 * a11 - a01 * a01);
    float s = a01 * b1 - a11 * b0;
    float t = a01 * b0 - a00 * b1;

    if (s + t <= det) {
        if (s < 0.0f) {
            if (t < 0.0f) {
                if (b0 < 0.0f) {
                    t = 0.0f;
                    if (-b0 >= a00)
                        s = 1.0f;
                    else
                        s = -b0 / a00;
                } else {
                    s = 0.0f;
                    if (b1 >= 0.0f)
                        t = 0.0f;
                    else if (-b1 >= a11)
                        t = 1.0f;
                    else
                        t = -b1 / a11;
                }
            } else {
                s = 0.0f;
                if (b1 >= 0.0f)
                    t = 0.0f;
                else if (-b1 >= a11)
                    t = 1.0f;
                else
                    t = -b1 / a11;
            }
        } else if (t < 0.0f) {
            t = 0.0f;
            if (b0 >= 0.0f)
                s = 0.0f;
            else if (-b0 >= a00)
                s = 1.0f;
            else
                s = -b0 / a00;
        } else {
            float inverse = 1.0f / det;
            s *= inverse;
            t *= inverse;
        }
    } else {
        if (s < 0.0f) {
            float tmp0 = a01 + b0;
            float tmp1 = a11 + b1;
            if (tmp1 > tmp0) {
                float numer = tmp1 - tmp0;
                float denom = a00 - 2.0f * a01 + a11;
                if (numer >= denom) {
                    s = 1.0f;
                    t = 0.0f;
                } else {
                    s = numer / denom;
                    t = 1.0f - s;
                }
            } else {
                s = 0.0f;
                if (tmp1 <= 0.0f)
                    t = 1.0f;
                else if (b1 >= 0.0f)
                    t = 0.0f;
                else
                    t = -b1 / a11;
            }
        } else if (t < 0.0f) {
            float tmp0 = a01 + b1;
            float tmp1 = a00 + b0;
            if (tmp1 > tmp0) {
                float numer = tmp1 - tmp0;
                float denom = a00 - 2.0f * a01 + a11;
                if (numer >= denom) {
                    t = 1.0f;
                    s = 0.0f;
                } else {
                    t = numer / denom;
                    s = 1.0f - t;
                }
            } else {
                t = 0.0f;
                if (tmp1 <= 0.0f)
                    s = 1.0f;
                else if (b0 >= 0.0f)
                    s = 0.0f;
                else
                    s = -b0 / a00;
            }
        } else {
            float numer = a11 + b1 - a01 - b0;
            if (numer <= 0.0f) {
                s = 0.0f;
                t = 1.0f;
            } else {
                float denom = a00 - 2.0f * a01 + a11;
                if (numer >= denom) {
                    s = 1.0f;
                    t = 0.0f;
                } else {
                    s = numer / denom;
                    t = 1.0f - s;
                }
            }
        }
    }

    CVector3 result;
    float sx = s * e0.x;
    float sy = s * e0.y;
    float sz = s * e0.z;
    result.x = m_vertices[0].x + sx;
    result.y = m_vertices[0].y + sy;
    result.z = m_vertices[0].z + sz;
    float tx = t * e1.x;
    float ty = t * e1.y;
    float tz = t * e1.z;
    result.x = result.x + tx;
    result.y = result.y + ty;
    result.z = result.z + tz;
    if (outS)
        *outS = s;
    if (outT)
        *outT = t;
    return result;
}
