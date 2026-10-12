// CCompartment's visibility and draw-enabled queries and the shared shadow
// texture setter, and DrawShadows (unless the configuration turns shadows
// off, each shadow of the draw context's list is made current and drawn on
// the compartment's tree from its root). CCompartment, CTexture,
// g_CptShadowBin, g_config and the shadow globals are named by the symbols
// (g_config's 20-byte size too; its view must keep that size); the members
// and the shadow-bin, draw-context, shadow-list, configuration and tree views
// are inferred from offsets, and the class is a non-virtual view. The rest of the file is not part of this
// unit.
class CTexture;
class CPTNode;
class CCharacterShadow;

/* Inferred: the draw context's shadow list (a vector of shadow pointers). */
struct ShadowListView {
    CCharacterShadow** data;
    int unknown04;
    int size;
};

class CDrawContext {
public:
    unsigned char unknown000[244];
    ShadowListView* m_shadows;
};

/* Inferred: the configuration flag that turns character shadows off. */
struct ConfigView {
    unsigned char unknown00[2];
    bool noShadows;
    unsigned char unknown03[17];
};

extern ConfigView g_config;
extern bool g_bShadowSet;
extern CCharacterShadow* g_pCurrentShadow;

struct CompartmentTreeView {
    unsigned char unknown00[32];
    CPTNode* root;
};

struct CptShadowBinView {
    unsigned char unknown00[32];
    CTexture* texture;
};

extern CptShadowBinView g_CptShadowBin;

class CCompartment {
public:
    bool IsVisible(CDrawContext&) const;
    bool IsDrawEnabled() const;
    static void SetShadowTexture(CTexture&);
    void DrawShadows(CDrawContext&);
    void DrawNodeShadows(CPTNode&, CCharacterShadow&);

    unsigned char unknown00[88];
    CompartmentTreeView* m_tree;
    unsigned char unknown5c[4];
    bool m_drawEnabled;
};

bool CCompartment::IsVisible(CDrawContext&) const {
    return true;
}

bool CCompartment::IsDrawEnabled() const {
    return m_drawEnabled;
}

void CCompartment::SetShadowTexture(CTexture& texture) {
    g_CptShadowBin.texture = &texture;
}

void CCompartment::DrawShadows(CDrawContext& context) {
    if (!g_config.noShadows) {
        ShadowListView* shadows = context.m_shadows;
        for (CCharacterShadow** it = shadows->data; it != shadows->data + shadows->size; it++) {
            g_bShadowSet = false;
            g_pCurrentShadow = *it;
            DrawNodeShadows(*m_tree->root, **it);
        }
    }
}
