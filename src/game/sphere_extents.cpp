// The start of sphere.cpp (0x800ca6c8): CVolSphere::GetExtents gives the
// sphere's bounding box, the centre less and plus the radius on each axis.
// The classes and functions are named by the symbols; the member layout
// (radius at +12, centre at +16) follows sphere.cpp. The rest of the file is
// in other units.
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
};

class CVolSphere {
public:
    void GetExtents(CVector3&, CVector3&) const;

    unsigned char unknown00[12];
    float m_radius;
    CVector3 m_center;
};

void CVolSphere::GetExtents(CVector3& min, CVector3& max) const {
    float radius = m_radius;
    min.x = m_center.x - radius;
    min.y = m_center.y - radius;
    min.z = m_center.z - radius;
    max.x = m_center.x + radius;
    max.y = m_center.y + radius;
    max.z = m_center.z + radius;
}
