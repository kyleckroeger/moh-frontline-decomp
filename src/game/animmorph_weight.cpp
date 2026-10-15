// A fragment of animmorph.cpp (0x80065198): CAnimMorph::UpdateAnimation runs
// the base update, then composites the morph weights (the morph object at
// +3280 starts with them) from the animation's frame list (+2612) when it
// has frames, flagging the render object (+2624) and marking the weights
// set (+3552); otherwise it clears the 64 weights once when they are marked.
// SetBlendTargetWeight stores a blend target's weight and flags the render
// object for an update when the weight changed. Init initialises the header
// (+4), the base, the frame source (+3260), the render, cluster (+3232) and
// morph objects (64 targets, 12 active), the geometry state (+2644) and marks
// the weights. The classes and functions are named by the mangled symbols;
// the virtuals are declared in table order (the destructor, defined in
// another unit, first, so the table is not emitted here); the members, the
// structures' fields and the C functions' parameter types are inferred from
// offsets and calls (the render object as in animated_bind.cpp, the header
// and its unextended count as in animskinned_skeleton.cpp). The rest of the
// file is in other units.
class CDrawContext;
class CAnimObject;
struct CharFrame_t;
struct TUInt128;
struct AnimMotFrameList_t;

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

struct DMClusterObject_T {
    unsigned char unknown00[20];
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
void LLCharObjCompositeMorph(float*, AnimMotFrameList_t*);

extern "C" void AnimInitHeader(AnimHeaderView*, short, unsigned short, short);

/* inferred: the frame list starts with its frame count */
struct AnimMotFrameList_t {
    unsigned short count;
};

class CAnimated {
public:
    virtual ~CAnimated();
    virtual void UpdateAnimation(float);
    void Init(CAnimObject*, unsigned short);
};

class CAnimMorph : public CAnimated {
public:
    virtual ~CAnimMorph();
    virtual void UpdateAnimation(float);
    virtual void Draw(CDrawContext&);
    void SetBlendTargetWeight(unsigned long, float);
    void Init(CAnimObject*, unsigned short, CharFrame_t*);

    AnimHeaderView m_header;
    unsigned char unknown000a[2602];
    AnimMotFrameList_t* m_frames;
    unsigned char unknown0a38[8];
    DMRenderObject_T m_render;
    DMGeometryState_T m_geometry;
    unsigned char unknown0a58[584];
    DMClusterObject_T m_cluster;
    unsigned char unknown0cb4[8];
    CharFrame_t* m_frameSource;
    unsigned char unknown0cc0[16];
    DMMorphObject_T m_morph;
    unsigned char unknown0dd0[16];
    bool m_weightsSet;
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
