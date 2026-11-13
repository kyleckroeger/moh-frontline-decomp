// A fragment of animmorph.cpp (0x800653c0): CAnimMorph::Draw, then the CAnimMorph destructor (its virtual table
// pointer then each base's through the
// inline destructors of CAnimated, and the object freed when asked). The file
// name is this project's; the original record is animmorph.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CAnimMorph declares another of its virtual functions (defined
// elsewhere; the result type is not known) before its destructor, so its own
// global virtual table is not emitted here.
// CAnimated has a global table (keyed on another of its virtual functions,
// declared first here) and an inline destructor (weak in the original), which
// is only inlined here (its table is not emitted, so no copy is).
#include "game/CVector3.h"



/* inferred: a single-use assigning inline (name and body inferred). The original's last
   stack slot holds a dead copy of the view position below the argument copies, so
   'left = view' ran inside an inline that copied the vector through a local (the
   second-level Assign form accepted 2026-11-12, with the shared header's reference
   operator=). */
inline void Assign(CVector3& d, const CVector3& s) { CVector3 t = s; d = t; }
class CVector4 {
public:
    void DivY() { y /= w; }
    void DivX() { x /= w; }
    void Div() { float iw = w; y /= iw; x /= iw; }
    void Div2() { y /= w; x /= w; }
    float x;
    float y;
    float z;
    float w;
};

class CMatrix {
public:
    void TransformPoint(CVector3&, CVector3) const;
    void Transform(CVector4&, CVector3) const;

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
};

class CDrawContext {
public:
    unsigned char unknown000[192];
    CMatrix* m_localToWorld;
    CMatrix* m_worldToView;
    CMatrix* m_viewToScreen;
};

struct DMRenderObject_T {
    unsigned char unknown00[20];
};
struct DMGeometryState_T {
    unsigned char unknown00[588];
};
struct AnimCacheMatrixView;

/* inferred: the cluster's matrix count and matrices */
struct DMClusterObject_T {
    int m_count;
    AnimCacheMatrixView* m_matrices;
    unsigned char unknown08[8];
};

void DMRenderSetFlags(DMRenderObject_T*, unsigned long);
void DMMorphBlendDisplayLists(DMRenderObject_T*);

struct AnimCacheVectorView;
struct AnimCacheSlotView {
    AnimCacheSlotView(AnimCacheVectorView* v, AnimCacheMatrixView* m) : vectors(v), matrices(m) {}

    AnimCacheVectorView* vectors;
    AnimCacheMatrixView* matrices;
};

class CAnimMatrixCache {
public:
    AnimCacheSlotView GetEntry(void*, bool&);
    void Update();

    unsigned char unknown00[36];
};

extern CAnimMatrixCache g_animMorphCache;

class CAnimated {
public:
    virtual void UpdateAnimation(float); /* result type not known */
    virtual ~CAnimated() {}
    virtual void Draw(CDrawContext&);

    unsigned char unknown0004[2620];
    DMRenderObject_T m_render;
    DMGeometryState_T m_geometry;
    DMClusterObject_T m_cluster;
    void* m_mesh;
};

class CAnimMorph : public CAnimated {
public:
    virtual void UpdateAnimation(float); /* result type not known */
    virtual ~CAnimMorph();
    virtual void Draw(CDrawContext&);
};

void CAnimMorph::Draw(CDrawContext& context) {
    if (m_mesh) {
        g_animMorphCache.Update();
        bool fresh;
        AnimCacheSlotView slot = g_animMorphCache.GetEntry(this, fresh);
        m_cluster.m_matrices = slot.matrices;
        m_cluster.m_count = 8;
        if (fresh)
            DMRenderSetFlags(&m_render, 1);
        CMatrix* world = context.m_localToWorld;
        CVector3 position;
        position.x = world->pos.x;
        position.y = world->pos.y;
        position.z = world->pos.z;
        CVector3 view;
        context.m_worldToView->TransformPoint(view, position);
        CVector3 left;
        Assign(left, view);
        left.x -= 1.0f;
        CVector3 right = view;
        right.x += 1.0f;
        CVector4 a;
        context.m_viewToScreen->Transform(a, left);
        CVector4 b;
        context.m_viewToScreen->Transform(b, right);
        a.x /= a.w;
        a.y /= a.w;
        b.x /= b.w;
        b.y /= b.w;
        float dy = b.y - a.y;
        float dx = b.x - a.x;
        if (dx * dx + dy * dy > 1.5f)
            DMMorphBlendDisplayLists(&m_render);
        CAnimated::Draw(context);
    }
}

CAnimMorph::~CAnimMorph() {
}
