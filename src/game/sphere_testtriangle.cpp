// A fragment of sphere.cpp (0x800cab00): CVolSphere::TestCollision against a triangle, which reports no collision. The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CTriangle;
class CCollision;

class CVolSphere {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
};

bool CVolSphere::TestCollision(const CTriangle&, CCollision&, bool) const {
    return false;
}
