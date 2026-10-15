// A fragment of animskinned.cpp (0x80066a08): CAnimSkinned's
// UpdateSkeletonMatrices traverses the skeleton (+3252) for the frame
// (+3280) under a unit matrix pushed on g_matStack, then widens the bounds
// it reports (when asked for) by 0.1 on each side. BindSkeleton clears +3320
// and the vector at +3288, copies each bone's three short offsets (+1312 of
// the skeleton, its bone count at +6) scaled by 16 into the array at +3336,
// binds the skeleton to the cluster object (+3232) and keeps it. SetRotation,
// ExtractRotation and ExtractTranslation keep or report the rotation (+12)
// and the extracted rotation (+3828) and translation (+3832, copied as a
// double pair) flagged by bits 0 and 1 of +3824. UpdateAnimation runs the
// base update, blends the skeleton frames, copies a flag of the animation
// data (+2612) unless +3265 is set, extracts the motion's rotation and
// translation for the current state and updates the floating bone. Init
// initialises the header (+4), the base, the render (+2624), cluster and
// geometry (+2644) objects and keeps the frame source (+3260) when given;
// Draw takes a matrix cache entry for the cluster and flags the render object
// when the entry is new before the base draw; InitClass is empty.
// The classes, functions and globals are named by the symbols; the virtuals
// are declared in table order (the destructor, defined in another unit,
// first, so the table is not emitted here); the members, the structures'
// fields and the C functions' parameter types are inferred (the frame is
// embedded at +3280; m_frame only marks its start, its size unknown; the
// cache slot follows animated_bind.cpp). The rest of the file is in other
// units.
class CMatrix;
class CDrawContext;
class CAnimObject;

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
};

struct CharFrame_t;

/* inferred: the skeleton's bone count (+6) and per-bone short triples (+1312) */
struct CharSkel_t {
    unsigned char unknown000[6];
    unsigned short numBones;
    unsigned char unknown008[1304];
    short offsets[1][3];
};

struct AnimHeaderView {
    short field00;
    short field02;
    short field04;
};

struct AnimStateMachineView {
    unsigned char unknown00[4];
};

/* inferred: the animation data; its first flag bit is copied to +3264 */
struct AnimDataView {
    unsigned char unknown00[16];
    unsigned char flag : 1;
};

struct DMRenderObject_T {
    unsigned char unknown00[20];
};

struct DMGeometryState_T {
    unsigned char unknown00[4];
};

struct DMClusterObject_T {
    unsigned long flags;
    void* matrices;
    unsigned char unknown08[12];
};

struct TUInt128;

void DMRenderInitObject(DMRenderObject_T*, TUInt128*, TUInt128*, unsigned long);
void DMClusterInitObject(DMClusterObject_T*, float (*)[3][4], float (*)[3][4], unsigned long);
void DMRenderBindClusterObj(DMRenderObject_T*, DMClusterObject_T*);
void DMGeomSetNodeState(DMGeometryState_T*, long, long);
void DMRenderSetFlags(DMRenderObject_T*, unsigned long);
void DMClusterBindSkeleton(DMClusterObject_T*, CharSkel_t*);
void DMClusterSkelTraverse(CharFrame_t*, CharSkel_t*, CMatrix*, CVector3*, CVector3*);

extern "C" {
void AnimInitHeader(AnimHeaderView*, short, unsigned short, short); /* the count is passed unextended */
unsigned short AnimStGetCurrentState(AnimStateMachineView*);
void BlendSkelFrames(void*, AnimDataView*);
bool AnimMotCalcAnimRot(AnimHeaderView*, void*, AnimDataView*, long*, unsigned short, int);
bool AnimMotCalcAnimPos(AnimHeaderView*, void*, AnimDataView*, CVector3*, unsigned short, int);
bool AnimMotCalcFloatingBone(AnimHeaderView*, void*, AnimDataView*, void*, void*, unsigned long, int);
}

class CMatrixStack {
public:
    void PushUnit();
    void Pop();
};

extern CMatrixStack* g_matStack;

struct AnimCacheVectorView;
struct AnimCacheMatrixView;

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

extern CAnimMatrixCache g_animSkinnedCache;

class CAnimated {
public:
    virtual ~CAnimated();
    virtual void UpdateAnimation(float);
    virtual void Draw(CDrawContext&);
    void Init(CAnimObject*, unsigned short);
};

class CAnimSkinned : public CAnimated {
public:
    virtual ~CAnimSkinned();
    virtual void UpdateAnimation(float);
    virtual void Draw(CDrawContext&);
    virtual bool ExtractRotation(long*);
    virtual bool ExtractTranslation(CVector3*);
    virtual void SetRotation(long);
    virtual void BindSkeleton(CharSkel_t*);
    void UpdateSkeletonMatrices(CMatrix*, CVector3*, CVector3*);
    void Init(CAnimObject*, unsigned short, CharFrame_t*);
    static void InitClass();
    /* inferred inline helper: the register numbering of the copy needs one */
    void CopyOffsets(CharSkel_t* skeleton) {
        for (unsigned int i = 0; i < skeleton->numBones; i++) {
            m_boneOffsets[i][0] = skeleton->offsets[i][0] << 4;
            m_boneOffsets[i][1] = skeleton->offsets[i][1] << 4;
            m_boneOffsets[i][2] = skeleton->offsets[i][2] << 4;
        }
    }

    AnimHeaderView m_header;
    unsigned char unknown00a[2];
    long m_rotation;
    unsigned char m_motion[2176];
    AnimStateMachineView m_machine;
    unsigned char unknown0894[416];
    AnimDataView* m_animData;
    unsigned char unknown0a38[8];
    DMRenderObject_T m_render;
    DMGeometryState_T m_geometry;
    unsigned char unknown0a58[584];
    DMClusterObject_T m_cluster;
    CharSkel_t* m_skeleton;
    unsigned char unknown0cb8[4];
    CharFrame_t* m_frameSource;
    unsigned char m_flag0cc0;
    unsigned char m_flag0cc1;
    unsigned char unknown0cc2[14];
    unsigned char m_frame[8];
    CVector3 m_offset;
    unsigned char unknown0ce4[16];
    int m_unknown0cf8;
    unsigned char unknown0cfc[12];
    short (*m_boneOffsets)[3];
    unsigned char unknown0d0c[484];
    unsigned long m_extractFlags;
    long m_extractedRotation;
    CVector3 m_extractedTranslation __attribute__((aligned(8)));
};

void CAnimSkinned::UpdateSkeletonMatrices(CMatrix* matrices, CVector3* min, CVector3* max) {
    g_matStack->PushUnit();
    DMClusterSkelTraverse((CharFrame_t*)m_frame, m_skeleton, matrices, min, max);
    g_matStack->Pop();
    if (min) {
        min->x -= 0.1f;
        min->y -= 0.1f;
        min->z -= 0.1f;
    }
    if (max) {
        max->x += 0.1f;
        max->y += 0.1f;
        max->z += 0.1f;
    }
}

void CAnimSkinned::BindSkeleton(CharSkel_t* skeleton) {
    m_unknown0cf8 = 0;
    m_offset.x = 0.0f;
    m_offset.y = 0.0f;
    m_offset.z = 0.0f;
    CopyOffsets(skeleton);
    DMClusterBindSkeleton(&m_cluster, skeleton);
    m_skeleton = skeleton;
}

void CAnimSkinned::SetRotation(long rotation) {
    m_rotation = rotation;
}

bool CAnimSkinned::ExtractTranslation(CVector3* translation) {
    if (m_extractFlags & 2) {
        *(CVector3Pair*)translation = *(CVector3Pair*)&m_extractedTranslation;
        return true;
    }
    return false;
}

bool CAnimSkinned::ExtractRotation(long* rotation) {
    if (m_extractFlags & 1) {
        *rotation = m_extractedRotation;
        return true;
    }
    return false;
}

void CAnimSkinned::UpdateAnimation(float time) {
    CAnimated::UpdateAnimation(time);
    BlendSkelFrames(m_frame, m_animData);
    if (!m_flag0cc1)
        m_flag0cc0 = m_animData->flag;
    unsigned short state = AnimStGetCurrentState(&m_machine);
    m_extractedRotation = 0;
    if (AnimMotCalcAnimRot(&m_header, m_motion, m_animData, &m_extractedRotation, state, 0))
        m_extractFlags = 1;
    m_extractedTranslation.x = m_extractedTranslation.y = m_extractedTranslation.z = 0.0f;
    if (AnimMotCalcAnimPos(&m_header, m_motion, m_animData, &m_extractedTranslation, state, 0))
        m_extractFlags |= 2;
    AnimMotCalcFloatingBone(&m_header, m_motion, m_animData, (char*)m_frameSource + 24, (char*)m_frameSource + 44,
                            0xffff, 1);
}

void CAnimSkinned::Init(CAnimObject* object, unsigned short count, CharFrame_t* frame) {
    AnimInitHeader(&m_header, 0, count, 16);
    CAnimated::Init(object, count);
    DMRenderInitObject(&m_render, 0, 0, 0);
    DMClusterInitObject(&m_cluster, 0, 0, 0);
    DMRenderBindClusterObj(&m_render, &m_cluster);
    DMGeomSetNodeState(&m_geometry, 0, 0);
    if (frame)
        m_frameSource = frame;
}

void CAnimSkinned::Draw(CDrawContext& context) {
    g_animSkinnedCache.Update();
    bool created;
    AnimCacheSlotView slot = g_animSkinnedCache.GetEntry(this, created);
    m_cluster.matrices = slot.matrices;
    m_cluster.flags = 192;
    if (created)
        DMRenderSetFlags(&m_render, 1);
    CAnimated::Draw(context);
}

void CAnimSkinned::InitClass() {
}
