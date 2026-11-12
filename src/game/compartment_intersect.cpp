// A fragment of compartment.cpp (0x80074a78): the file-local IntersectNode,
// which builds a box volume from a portal-tree node (three basis vectors, the
// centre, and twice the half extents kept in the basis vectors' fourth words)
// and reports whether the character shadow's box misses it. The file name is
// this project's; the original record is compartment.cpp. The function and
// classes are named by the mangled symbols; the node layout and its extent
// accessor (this project's), the 16-byte vector view copied as two doubles
// (the frame is realigned), the shadow's box at +16, the collision test's
// int result and IntersectNode's result type are inferred. The weak GetBox, CVector3 copy constructor and
// CVolBox constructor emitted after it are not part of this unit.
class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[32];
};

/* Inferred: a 16-byte vector copied as two doubles. */
struct CVector3View {
    double pair[2];
} __attribute__((aligned(16)));

#include "game/CVector3.h"


struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(16)));

class IVolume {
public:
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    int TestCollision(const CVolBox&, CCollision&, bool) const;
    void SetBasis(CVector3, CVector3, CVector3);
    void SetCenter(CVector3);
    void SetWidth(float);
    void SetDepth(float);
    void SetHeight(float);

    float m_halfWidth;
    float m_halfDepth;
    float m_halfHeight;
    VECTOR3VIEW m_basis[3];
    VECTOR3VIEW m_center;
};

/* Inferred: a node's basis (half extents in the fourth words) and centre. */
struct CPTNode {
    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 center;

    float Extent(int axis) const { return ((const float*)this)[axis * 4 + 3]; }
};

class CCharacterShadow {
public:
    virtual bool IsDrawEnabled() const;
    const CVolBox& GetBox() { return m_box; }

    unsigned char unknown04[12];
    CVolBox m_box;
};

static bool IntersectNode(CPTNode& node, CCharacterShadow* shadow) {
    CVolBox box;
    box.SetBasis(node.right, node.forward, node.up);
    box.SetCenter(node.center);
    box.SetWidth(2.0f * node.Extent(0));
    box.SetDepth(2.0f * node.Extent(1));
    box.SetHeight(2.0f * node.Extent(2));
    CCollision collision;
    if (box.TestCollision(shadow->GetBox(), collision, false) == 1)
        return false;
    return true;
}

