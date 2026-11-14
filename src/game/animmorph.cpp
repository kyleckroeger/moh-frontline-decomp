// The end of animmorph.cpp (0x80065198), everything after the open
// LLCharObjCompositeMorph, as one unit (merged 2026-11-13 by agent undef from
// animmorph_weight, animmorph_dtor and animmorph): CAnimMorph::UpdateAnimation runs
// the base update, then composites the morph weights (the morph object at
// +3280 starts with them) from the animation's frame list (+2612) when it
// has frames, flagging the render object (+2624) and marking the weights
// set (+3552); otherwise it clears the 64 weights once when they are marked.
// SetBlendTargetWeight stores a blend target's weight and flags the render
// object for an update when the weight changed. Init initialises the header
// (+4), the base, the frame source (+3260), the render, cluster (+3232) and
// morph objects (64 targets, 12 active), the geometry state (+2644) and marks
// the weights. Draw updates the morph matrix cache, binds the cluster's
// matrices and blends the display lists when the object's screen size is large
// enough. Then the destructor (the inline CAnimated destructor), the
// constructor (the CAnimated part: 16 animation channels, two matrices whose
// inline constructors initialise the matrix class once, two vector arrays), the
// weak empty CVector4 and AnimChannel_t constructors and the static
// initialisation of the morph matrix cache (16, 384, 8, as the skinned one).
// The unit is built with RTTI on and declares the destructor first, so it emits
// and owns the file's table (__vt__10CAnimMorph, CAnimated's slots in table
// order) and RTTI records. The classes, functions and globals are named by the
// mangled symbols; the members, the structures' fields and the C functions'
// parameter types are inferred from offsets and calls (the render object as in
// animated_bind.cpp, the header and its unextended count as in
// animskinned_skeleton.cpp, the CAnimated part as in animskinned.cpp).
#include "game/CMatrix.h"

class CDrawContext;
class CAnimObject;
struct CharFrame_t;
struct CharSkel_t;
struct TUInt128;
struct AnimMotFrameList_t;
class CAnimSkinned;

struct AnimHeaderView {
    short field00;
    short field02;
    short field04;
};

struct DMRenderObject_T {
    unsigned char unknown00[20];
};

struct DMGeometryState_T {
    unsigned char unknown00[4];
};

struct AnimCacheMatrixView;

/* inferred: the cluster's matrix count and matrices */
struct DMClusterObject_T {
    int m_count;
    AnimCacheMatrixView* m_matrices;
    unsigned char unknown08[8];
};

struct DMMorphObject_T {
    float weights[64];
};

void DMRenderSetFlags(DMRenderObject_T*, unsigned long);
void DMRenderInitObject(DMRenderObject_T*, TUInt128*, TUInt128*, unsigned long);
void DMClusterInitObject(DMClusterObject_T*, float (*)[3][4], float (*)[3][4], unsigned long);
void DMRenderBindClusterObj(DMRenderObject_T*, DMClusterObject_T*);
void DMMorphInitObject(DMMorphObject_T*, unsigned long);
void DMRenderBindMorphObj(DMRenderObject_T*, DMMorphObject_T*);
void DMMorphSetMaxActiveTargets(DMMorphObject_T*, unsigned long);
void DMGeomSetNodeState(DMGeometryState_T*, long, long);
void DMMorphBlendDisplayLists(DMRenderObject_T*);
void LLCharObjCompositeMorph(float*, AnimMotFrameList_t*);

extern "C" void AnimInitHeader(AnimHeaderView*, short, unsigned short, short);

/* inferred: the frame list starts with its frame count */
struct AnimMotFrameList_t {
    unsigned short count;
};

/* inferred: a single-use assigning inline (name and body inferred). The original's last
   stack slot holds a dead copy of the view position below the argument copies, so
   'left = view' ran inside an inline that copied the vector through a local (the
   second-level Assign form accepted 2026-11-12, with the shared header's reference
   operator=). */
inline void Assign(CVector3& d, const CVector3& s) { CVector3 t = s; d = t; }

class CVector4 {
public:
    CVector4() {}

    float x;
    float y;
    float z;
    float w;
};

struct AnimChannel_t {
    AnimChannel_t() {}

    unsigned char unknown00[136];
};

class CDrawContext {
public:
    unsigned char unknown000[192];
    CMatrix* m_localToWorld;
    CMatrix* m_worldToView;
    CMatrix* m_viewToScreen;
};

struct AnimCacheVectorView;
struct AnimCacheSlotView {
    AnimCacheSlotView(AnimCacheVectorView* v, AnimCacheMatrixView* m) : vectors(v), matrices(m) {}

    AnimCacheVectorView* vectors;
    AnimCacheMatrixView* matrices;
};

class CAnimMatrixCache {
public:
    CAnimMatrixCache(int, int, int);
    AnimCacheSlotView GetEntry(void*, bool&);
    void Update();

    unsigned char unknown00[36];
};

extern CAnimMatrixCache g_animMorphCache;

/* CAnimated's virtuals in table order (__vt__10CAnimMorph); its destructor is inline
   (weak in the original) and its table is keyed elsewhere (UpdateAnimation). */
class CAnimated {
public:
    virtual ~CAnimated() {}
    virtual void UpdateAnimation(float);
    virtual void Draw(CDrawContext&);
    virtual int ExtractRotation(long*);
    virtual int ExtractTranslation(CVector3*);
    virtual void SetRotation(long);
    virtual void BindSkeleton(CharSkel_t*);
    virtual class CAnimMorph* AsAnimMorph();
    virtual CAnimSkinned* AsAnimSkinned() { return 0; }
    void Init(CAnimObject*, unsigned short);

    AnimHeaderView m_header;
    unsigned char unknown000a[6];
    AnimChannel_t m_channels[16];
    unsigned char unknown890[16];
    CMatrix m_matrix0;
    CMatrix m_matrix1;
    unsigned char unknown920[276];
    AnimMotFrameList_t* m_frames;
    unsigned char unknown0a38[8];
    DMRenderObject_T m_render;
    DMGeometryState_T m_geometry;
    unsigned char unknown0a58[192];
    CVector4 m_vectors0[3];
    CVector4 m_vectors1[4];
    unsigned char unknown0b88[280];
    DMClusterObject_T m_cluster;
    void* m_mesh;
    unsigned char unknown0cb4[8];
    CharFrame_t* m_frameSource;
    unsigned char unknown0cc0[16];
    DMMorphObject_T m_morph;
    unsigned char unknown0dd0[16];
    bool m_weightsSet;
};

class CAnimMorph : public CAnimated {
public:
    virtual ~CAnimMorph();
    virtual void UpdateAnimation(float);
    virtual void Draw(CDrawContext&);
    virtual CAnimMorph* AsAnimMorph() { return this; }
    CAnimMorph();
    void SetBlendTargetWeight(unsigned long, float);
    void Init(CAnimObject*, unsigned short, CharFrame_t*);
};

void CAnimMorph::UpdateAnimation(float time) {
    CAnimated::UpdateAnimation(time);
    if (m_frames->count) {
        LLCharObjCompositeMorph(m_morph.weights, m_frames);
        DMRenderSetFlags(&m_render, 1);
        m_weightsSet = true;
    } else if (m_weightsSet) {
        for (int i = 0; i < 64; i++)
            m_morph.weights[i] = 0.0f;
        m_weightsSet = false;
    }
}

void CAnimMorph::SetBlendTargetWeight(unsigned long target, float weight) {
    float old = m_morph.weights[target];
    m_morph.weights[target] = weight;
    if (weight != old)
        DMRenderSetFlags(&m_render, 1);
}

void CAnimMorph::Init(CAnimObject* object, unsigned short count, CharFrame_t* frame) {
    AnimInitHeader(&m_header, 2, count, 4);
    CAnimated::Init(object, count);
    m_frameSource = frame;
    DMRenderInitObject(&m_render, 0, 0, 0);
    DMClusterInitObject(&m_cluster, 0, 0, 0);
    DMRenderBindClusterObj(&m_render, &m_cluster);
    DMMorphInitObject(&m_morph, 64);
    DMRenderBindMorphObj(&m_render, &m_morph);
    DMMorphSetMaxActiveTargets(&m_morph, 12);
    DMGeomSetNodeState(&m_geometry, 0, 0);
    m_weightsSet = true;
}

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

CAnimMorph::CAnimMorph() {
}

CAnimMatrixCache g_animMorphCache(16, 384, 8);
