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
union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

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
    if (m_line.GetStart().d.c.x < m_line.GetEnd().d.c.x) {
        min.d.c.x = m_line.GetStart().d.c.x;
        max.d.c.x = m_line.GetEnd().d.c.x;
    } else {
        max.d.c.x = m_line.GetStart().d.c.x;
        min.d.c.x = m_line.GetEnd().d.c.x;
    }
    if (m_line.GetStart().d.c.y < m_line.GetEnd().d.c.y) {
        min.d.c.y = m_line.GetStart().d.c.y;
        max.d.c.y = m_line.GetEnd().d.c.y;
    } else {
        max.d.c.y = m_line.GetStart().d.c.y;
        min.d.c.y = m_line.GetEnd().d.c.y;
    }
    if (m_line.GetStart().d.c.z < m_line.GetEnd().d.c.z) {
        min.d.c.z = m_line.GetStart().d.c.z;
        max.d.c.z = m_line.GetEnd().d.c.z;
    } else {
        max.d.c.z = m_line.GetStart().d.c.z;
        min.d.c.z = m_line.GetEnd().d.c.z;
    }
}
