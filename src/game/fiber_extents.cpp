// A fragment of fiber.cpp (0x800c9290): CVolFiber::GetExtents, the axis-
// aligned bounds of the fiber's line: per axis the smaller of the start and
// end coordinates is the minimum and the larger the maximum (each read
// through the line's by-value getters). The file name is this project's; the
// original record is fiber.cpp. The classes and functions are named by the
// mangled symbols; the members and the vector view (x, y, z over two doubles,
// implicit copies) are inferred. The rest of the file is in other units.
class CMatrix;
class CTriangle;
class CPlane;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

// Inferred: CVector3 as x, y, z (and padding) over two doubles.


#include "game/CVector3.h"


// Inferred: the line's start, end and direction with by-value getters.
class CLine3 {
public:
    CVector3 GetStart() const { return m_start; }
    CVector3 GetEnd() const { return m_end; }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
};

class IVolume {
public:
    virtual ~IVolume();
};

class CVolFiber : public IVolume {
public:
    void GetExtents(CVector3&, CVector3&) const;

    unsigned char unknown04[12];
    CLine3 m_line;
};

void CVolFiber::GetExtents(CVector3& min, CVector3& max) const {
    if (m_line.GetStart().x < m_line.GetEnd().x) {
        min.x = m_line.GetStart().x;
        max.x = m_line.GetEnd().x;
    } else {
        max.x = m_line.GetStart().x;
        min.x = m_line.GetEnd().x;
    }
    if (m_line.GetStart().y < m_line.GetEnd().y) {
        min.y = m_line.GetStart().y;
        max.y = m_line.GetEnd().y;
    } else {
        max.y = m_line.GetStart().y;
        min.y = m_line.GetEnd().y;
    }
    if (m_line.GetStart().z < m_line.GetEnd().z) {
        min.z = m_line.GetStart().z;
        max.z = m_line.GetEnd().z;
    } else {
        max.z = m_line.GetStart().z;
        min.z = m_line.GetEnd().z;
    }
}
