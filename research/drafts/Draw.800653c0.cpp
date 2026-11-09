union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) {
        x = ax;
        y = ay;
        z = az;
    }
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3 operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CVector4 {
public:
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
    void Draw(CDrawContext&);

    unsigned char unknown0000[2624];
    DMRenderObject_T m_render;
    DMGeometryState_T m_geometry;
    DMClusterObject_T m_cluster;
    void* m_mesh;
};

class CAnimMorph : public CAnimated {
public:
    void Draw(CDrawContext&);
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
        left = view;
        left.x -= 1.0f;
        CVector3 right = view;
        right.x += 1.0f;
        CVector4 a;
        context.m_viewToScreen->Transform(a, left);
        CVector4 b;
        context.m_viewToScreen->Transform(b, right);
        a.y /= a.w;
        b.y /= b.w;
        a.x /= a.w;
        b.x /= b.w;
        float dy = b.y - a.y;
        float dx = b.x - a.x;
        if (dx * dx + dy * dy > 1.5f)
            DMMorphBlendDisplayLists(&m_render);
    }
    CAnimated::Draw(context);
}
