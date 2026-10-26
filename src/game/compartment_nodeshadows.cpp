// A fragment of compartment.cpp (0x80074438): CCompartment::DrawNodeShadows.
// For a visible portal-tree node whose box the character shadow's box
// reaches (IntersectNode), it renders the node's geometry into the shadow
// bin and recurses into both children; the compiler inlines the recursion
// two levels deep (the second level calls the weak CVector3 copy
// constructor, GetBox and CVolBox constructor, the third calls
// IntersectNode). IntersectNode and those weak functions are compiled here
// as well; their original copies are placed by compartment_intersect and
// compartment_volbox. The file name is this project's; the original record
// is compartment.cpp. The functions and classes are named by the mangled
// symbols; the node layout (basis, centre, children, item list, visible bit),
// the render-list call and the views are inferred (as in
// compartment_intersect.cpp). DrawNodeShadows inlines IntersectNode, which
// the image places after it, so this unit is compiled with deferred
// inlining. The rest of the file is in other units.
class CCollision {
public:
    CCollision();
    ~CCollision();

    unsigned char unknown00[32];
};

/* Inferred: CVector3 copied as two doubles. */
struct CVector3Pair {
    double d[2];
};

class CVector3 {
public:
    CVector3(const CVector3& o) { *(CVector3Pair*)this = *(const CVector3Pair*)&o; }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(16)));

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    virtual IVolume* Create() const;
    virtual ~CVolBox();
    CVolBox() {}
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
    CPTNode* m_children[2];
    void* m_items20;
    unsigned char unknown4c[14];
    signed char m_visible : 1;
    signed char m_bits : 7;

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
    box.SetWidth(2.0f * node.right.w);
    box.SetDepth(2.0f * node.forward.w);
    box.SetHeight(2.0f * node.up.w);
    const CVolBox& other = shadow->GetBox(); /* read before the collision is built */
    CCollision collision;
    if (box.TestCollision(other, collision, false) == 1)
        return false;
    return true;
}


class CRenderBin;
class CRenderList {
public:
    void Render(CRenderBin&, void*);
};
extern CRenderList* g_pRenderList;
extern CRenderBin g_CptShadowBin;

class CCompartment {
public:
    void DrawNodeShadows(CPTNode&, CCharacterShadow&);
};

void CCompartment::DrawNodeShadows(CPTNode& node, CCharacterShadow& shadow) {
    if (node.m_visible && IntersectNode(node, &shadow)) {
        if (node.m_items20)
            g_pRenderList->Render(g_CptShadowBin, &node);
        if (node.m_children[0])
            DrawNodeShadows(*node.m_children[0], shadow);
        if (node.m_children[1])
            DrawNodeShadows(*node.m_children[1], shadow);
    }
}
