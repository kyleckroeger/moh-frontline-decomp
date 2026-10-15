// A fragment of box.cpp (0x800c15e4): CVolBox::TestCollision against a triangle, which reports no collision. The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CTriangle;
class CCollision;

class CVolBox {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
};

bool CVolBox::TestCollision(const CTriangle&, CCollision&, bool) const {
    return false;
}
