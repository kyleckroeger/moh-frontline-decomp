#include <math.h>

union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& v) : d(v.d) {}
    CVector3Data d;
} __attribute__((aligned(8)));

class CCDBTriangle {
public:
    CVector3 GetNearestPoint(CVector3, float*, float*) const;

    CVector3 m_vertices[3];
};

CVector3 CCDBTriangle::GetNearestPoint(CVector3 point, float* outS, float* outT) const {
    CVector3 e0;
    CVector3 e1;
    CVector3 diff;
    float a01, a00, b1, b0, a11;
    e0.d.v[0] = m_vertices[1].d.v[0] - m_vertices[0].d.v[0];
    e0.d.v[1] = m_vertices[1].d.v[1] - m_vertices[0].d.v[1];
    e0.d.v[2] = m_vertices[1].d.v[2] - m_vertices[0].d.v[2];
    e1.d.v[0] = m_vertices[2].d.v[0] - m_vertices[0].d.v[0];
    e1.d.v[1] = m_vertices[2].d.v[1] - m_vertices[0].d.v[1];
    e1.d.v[2] = m_vertices[2].d.v[2] - m_vertices[0].d.v[2];
    diff.d.v[1] = m_vertices[0].d.v[1] - point.d.v[1];
    diff.d.v[0] = m_vertices[0].d.v[0] - point.d.v[0];
    diff.d.v[2] = m_vertices[0].d.v[2] - point.d.v[2];
    a01 = e1.d.v[0] * e0.d.v[0] + e1.d.v[1] * e0.d.v[1] + e1.d.v[2] * e0.d.v[2];
    b1 = diff.d.v[0] * e1.d.v[0] + diff.d.v[1] * e1.d.v[1] + diff.d.v[2] * e1.d.v[2];
    a00 = e0.d.v[0] * e0.d.v[0] + e0.d.v[1] * e0.d.v[1] + e0.d.v[2] * e0.d.v[2];
    b0 = diff.d.v[0] * e0.d.v[0] + diff.d.v[1] * e0.d.v[1] + diff.d.v[2] * e0.d.v[2];
    a11 = e1.d.v[0] * e1.d.v[0] + e1.d.v[1] * e1.d.v[1] + e1.d.v[2] * e1.d.v[2];
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
    float sx = s * e0.d.v[0];
    float sy = s * e0.d.v[1];
    float sz = s * e0.d.v[2];
    result.d.v[0] = m_vertices[0].d.v[0] + sx;
    result.d.v[1] = m_vertices[0].d.v[1] + sy;
    result.d.v[2] = m_vertices[0].d.v[2] + sz;
    float tx = t * e1.d.v[0];
    float ty = t * e1.d.v[1];
    float tz = t * e1.d.v[2];
    result.d.v[0] = result.d.v[0] + tx;
    result.d.v[1] = result.d.v[1] + ty;
    result.d.v[2] = result.d.v[2] + tz;
    if (outS)
        *outS = s;
    if (outT)
        *outT = t;
    return result;
}
