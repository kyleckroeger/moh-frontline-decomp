// A fragment of fiber.cpp (0x800c9504): CVolFiber::TestCollision against a triangle sets the collision's top flag
// bit (+16) and reports no collision (the collision view is inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CTriangle;

/* inferred: the collision's flag bit at the top of +16 */
class CCollision {
public:
    unsigned char unknown00[16];
    unsigned char m_flag : 1;
    unsigned char m_flags : 7;
};

class CVolFiber {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
};

bool CVolFiber::TestCollision(const CTriangle&, CCollision& collision, bool) const {
    collision.m_flag = 1;
    return false;
}
