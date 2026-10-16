// A fragment of plane.cpp (0x80091344): CPlane::Transform (the normal scaled
// by the distance is transformed as a point, the normal as a vector, and the
// distance is recomputed as their dot product) and the CPlane constructor
// from a normal (copied as two doubles) and a distance. The file name is this
// project's; the original record is plane.cpp and GetIntersection before
// these is not reconstructed. CPlane, CVector3 and CMatrix are named by the
// mangled symbols; the vector view (four floats, copied through a double
// pair) is inferred.
struct CVector3Pair {
    double pair[2];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    void TransformPoint(CVector3&, CVector3) const;
    void TransformVector(CVector3&, CVector3) const;
};

class CPlane {
public:
    CPlane(CVector3, float);
    void Transform(const CMatrix&);

    CVector3 m_normal;
    float m_d;
};

void CPlane::Transform(const CMatrix& matrix) {
    CVector3 scaled;
    CVector3 point;
    scaled.x = m_d * m_normal.x;
    scaled.y = m_d * m_normal.y;
    scaled.z = m_d * m_normal.z;
    matrix.TransformPoint(point, scaled);
    matrix.TransformVector(m_normal, m_normal);
    m_d = m_normal.x * point.x + m_normal.y * point.y + m_normal.z * point.z;
}

CPlane::CPlane(CVector3 normal, float d) : m_normal(normal), m_d(d) {
}
