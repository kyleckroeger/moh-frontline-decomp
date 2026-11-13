// animated.cpp (0x800609a0): CAnimated, the animated mesh behind the game's
// animated objects, and its matrix cache. The weak destructor and defaults (no
// rotation or translation to extract, an empty rotation setter and a null
// morph cast); CAnimMatrixCache (GetEntry returns the keyed entry's slice of
// the vector and matrix buffers, reusing the oldest or a free entry and
// flagging it as created when the key is new; Update ages the used entries
// and resets the buffer index; the constructor allocates the entries and
// matrix buffer 0); the binding helpers (BindSkeleton and BindMesh bind
// through the cluster and render objects and keep the pointer; SetAnimSpeed
// sets a channel's speed; AdjustRootTrans adds a vector to the root
// translation; SetGeomNodeState forwards to the geometry state;
// BindSleeveTexture sets the texture of every mesh material flagged 0x200 and
// BindTexture one material's texture, or all of them for a negative index);
// Draw (with a mesh: render parts, cluster matrices under the local-to-camera
// matrix, the render object's projection, reject, scale and matrices, then a
// lighting block from the owner's light volume - up to three directional
// lights and the ambient, colours scaled by 255 and the intensity - plus the
// strongest of the context's point lights in range in the next directional
// slot, the alpha scaled when flag 0x100 is set, and the mesh drawn);
// UpdateAnimation (state machine and channels stepped, the motion weight list
// rebuilt), FinishAnimationState, SetAnimationState (refuses a lip-synch
// state while +3264 is set and the object's +9392 flag is clear, reports a
// pending transition, otherwise starts the state and records the animation
// in the state's user memory), ValidState, Init and InitClass. The file owns
// __vt__9CAnimated (slots in table order; AsAnimSkinned's weak copy is linked
// from another file) and its RTTI record. Classes and functions are named by
// the symbols; the members (names not original), the cache, mesh, material,
// lighting, light and draw-context views, the owner's light-volume getter
// (table slot +288) and the squared-length helper are inferred. The hoisted
// float declarations in Draw follow the original's register use. Compiled with
// deferred inlining (the RTTI name then leads .rodata), so the functions are
// written in reverse image order.
#include <math.h>
extern "C" void* memset(void*, int, unsigned long);



#include "game/CVector3.h"

#include "game/CMatrix.h"


class CTexture;
struct CharSkel_t;
struct CharOrient3_t;
class CDrawContext;

struct DMClusterObject_T {
    unsigned char unknown00[4];
    float (*m_matrices)[3][4];
    unsigned char unknown08[8];
};
struct DMRenderObject_T {
    unsigned char unknown00[20];
};
struct DMGeometryState_T {
    unsigned char unknown000[316];
    long unknown13c;
    unsigned char unknown140[268];
};
struct AnimChannelView {
    unsigned char unknown00[136];
};

struct CDMeshMaterialView {
    unsigned char unknown00[36];
    unsigned int flags;
    CTexture* texture;
    unsigned char unknown2c[12];
};

class CDMesh {
public:
    void Draw(CDrawContext&, DMRenderObject_T*);

    unsigned char unknown00[40];
    unsigned int materialCount;
    CDMeshMaterialView* materials;
};

class CMatrixStack {
public:
    void Push();
    void Pop();
    void Load(const CMatrix*);
};
extern CMatrixStack* g_matStack;

struct CharOrient3_t;
class CDrawContext;


/* inferred: the lighting block passed to DMLightingUpdate */
struct DMLighting_T {
    float colors[3][4];
    float ambient[4];
    float dirs[3][3];
};

void DMRenderUpdateParts(DMRenderObject_T*);
void DMClusterSynthesizeMatrices(unsigned long, DMClusterObject_T*, CDMesh*, CharOrient3_t*);
void DMRenderResetState(DMRenderObject_T*);
void DMRenderSetViewProj(DMRenderObject_T*, CMatrix*);
void DMRenderSet2dReject(DMRenderObject_T*, unsigned long);
void DMRenderSetLocalToCamera(DMRenderObject_T*, CMatrix*, CMatrix*);
void DMRenderSetScale(DMRenderObject_T*, float, float, float);
void DMRenderSetClusterMatrices(DMRenderObject_T*, float (*)[3][4]);
void DMLightingUpdate(DMRenderObject_T*, DMLighting_T*);
void DMLightingScaleAlpha(DMRenderObject_T*, float);

/* inferred: a light record of a light volume */
struct LightRecordView {
    int type;
    unsigned char unknown04[12];
    float dir[3];
    float color[3];
    float intensity;
    unsigned char unknown2c[4];
};

struct CLightVolume {
    unsigned char unknown00[32];
    int m_count;
    LightRecordView* m_lights;
};

class CLight {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual void unknown34();
    virtual void unknown38();
    virtual void unknown3c();
    virtual void GetPosition(CVector3&) const;
    static CLightVolume* GetDefaultLightVolume();

    unsigned char unknown004[172];
    float m_color[3];
    unsigned char unknown0bc[8];
    float m_radius;
};

struct LightListView {
    CLight** m_data;
    int m_unknown4;
    int m_size;
};

class CDrawContext {
public:
    unsigned char unknown000[192];
    CMatrix* m_localToWorld;
    CMatrix* m_worldToView;
    CMatrix* m_viewToScreen;
    unsigned char unknown0cc[20];
    CVector3 m_scale;
    LightListView* m_lights;
    unsigned char unknown0f4[4];
    CLightVolume* m_lightVolume;
    CLight* m_light;
    unsigned long m_reject;
};

struct AnimObjectView {
    unsigned char unknown00[56];
    CharOrient3_t* m_orient;
};

class CTexture;
struct CharSkel_t;
struct CharOrient3_t;

class CDrawContext;
class CAnimMorph;
class CAnimSkinned;

/* inferred: the animation object's root (translation at +8, orientation at +56) */
struct AnimRootView {
    unsigned char unknown00[8];
    float x;
    float y;
    float z;
    unsigned char unknown14[36];
    CharOrient3_t* m_orient;
};

void DMClusterBindSkeleton(DMClusterObject_T*, CharSkel_t*);
void DMRenderBindModel(DMRenderObject_T*, CDMesh*);
void DMGeomSetNodeState(DMGeometryState_T*, long, long);
extern "C" void AnimChanSetSpeed(AnimChannelView*, float);

class CAnimDatabase {
public:
    int GetStateCallbackID(unsigned long, unsigned short);

    unsigned char unknown0000[176];
};

extern CAnimDatabase g_AnimDB;

class CAnimSkinned {
public:
    static void InitClass();
};

/* the owner: its light-volume getter (table slot +288) and the flag at +9392 */
class CAnimObject {
public:
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual void v3c();
    virtual void v40();
    virtual void v44();
    virtual void v48();
    virtual void v4c();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5c();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6c();
    virtual void v70();
    virtual void v74();
    virtual void v78();
    virtual void v7c();
    virtual void v80();
    virtual void v84();
    virtual void v88();
    virtual void v8c();
    virtual void v90();
    virtual void v94();
    virtual void v98();
    virtual void v9c();
    virtual void va0();
    virtual void va4();
    virtual void va8();
    virtual void vac();
    virtual void vb0();
    virtual void vb4();
    virtual void vb8();
    virtual void vbc();
    virtual void vc0();
    virtual void vc4();
    virtual void vc8();
    virtual void vcc();
    virtual void vd0();
    virtual void vd4();
    virtual void vd8();
    virtual void vdc();
    virtual void ve0();
    virtual void ve4();
    virtual void ve8();
    virtual void vec();
    virtual void vf0();
    virtual void vf4();
    virtual void vf8();
    virtual void vfc();
    virtual void v100();
    virtual void v104();
    virtual void v108();
    virtual void v10c();
    virtual void v110();
    virtual void v114();
    virtual void v118();
    virtual void v11c();
    virtual CLightVolume* GetLightVolume();

    void FinishAnimationState(long, bool);

    unsigned char unknown0004[9388];
    bool m_flag24b0;
};

class CAnimated {
public:
    virtual ~CAnimated();
    virtual void UpdateAnimation(float);
    virtual void Draw(CDrawContext&);
    virtual int ExtractRotation(long*);
    virtual int ExtractTranslation(CVector3*);
    virtual void SetRotation(long);
    virtual void BindSkeleton(CharSkel_t*);
    virtual CAnimMorph* AsAnimMorph();
    virtual CAnimSkinned* AsAnimSkinned() { return 0; }

    void SetAnimSpeed(unsigned char, float);
    void AdjustRootTrans(CVector3);
    void SetGeomNodeState(long, long);
    void BindSleeveTexture(CTexture*);
    void BindTexture(CTexture*, char);
    void BindMesh(CDMesh*);
    int FinishAnimationState(unsigned short);
    long SetAnimationState(unsigned short, float, unsigned short&, long, void*);
    bool ValidState(unsigned short);
    void Init(CAnimObject*, unsigned short);
    static void InitClass();

    unsigned short unknown0004;      /* start of the channel data at +4 */
    unsigned short m_animSet;        /* passed to GetStateCallbackID */
    unsigned char unknown0008[4];
    long unknown000c;
    AnimChannelView m_channels[16];  /* +16 */
    unsigned char m_stateMachine[420];
    void* m_weightList;
    unsigned int m_flags;
    unsigned char m_alpha;
    unsigned char unknownA3D[3];
    DMRenderObject_T m_render;
    DMGeometryState_T m_geometry;
    DMClusterObject_T m_cluster;
    CDMesh* m_mesh;
    CharSkel_t* m_skeleton;
    CAnimObject* m_animObject;
    AnimRootView* m_root;
    bool unknown0cc0;
    bool unknown0cc1;
};

extern "C" {
void* AnimStGetStateUserMemory(void*, unsigned short);
int AnimStTransPreCheck(void*, void*, unsigned short);
long AnimStStartState(void*, void*, void*, unsigned short, void*, void*, float);
void AnimChanInitChannels(void*, void*);
void AnimStInitStructure(void*);
void AnimStProcess(void*, void*, void*, void*);
void AnimChanProcess(float, void*, void*, void*);
void* AnimMotCreateWeightList(void*, void*, void*);
}

void DebugMsg(const char*, ...);

void* DWI_alloc(const char*, int, int);

// Inferred views of the cache's buffers: 16-byte and 48-byte elements.
struct AnimCacheVectorView {
    unsigned char unknown00[16];
};
struct AnimCacheMatrixView {
    unsigned char unknown00[48];
};
struct AnimCacheEntryView {
    void* key;
    int age;
};
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

    AnimCacheSlotView Slot(int index) {
        return AnimCacheSlotView(m_vectors[m_buffer] + index * m_vectorCount,
                                 m_matrices[m_buffer] + index * m_matrixCount);
    }

    int m_count;
    int m_vectorCount;
    int m_matrixCount;
    int m_buffer;
    AnimCacheEntryView* m_entries;
    AnimCacheVectorView* m_vectors[2];
    AnimCacheMatrixView* m_matrices[2];
};

static inline float LengthSq(const CVector3& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }

void CAnimated::InitClass() {
    CAnimSkinned::InitClass();
}

void CAnimated::Init(CAnimObject* object, unsigned short) {
    m_mesh = 0;
    m_geometry.unknown13c = 0;
    AnimChanInitChannels(&unknown0004, m_channels);
    AnimStInitStructure(m_stateMachine);
    m_animObject = object;
    unknown000c = 0;
    m_alpha = 128;
    m_flags = 0;
    unknown0cc1 = false;
}

bool CAnimated::ValidState(unsigned short state) {
    if (g_AnimDB.GetStateCallbackID(state, m_animSet) == -1) {
        DebugMsg("WARNING: No callback fn found for lip synch 0x%X (Search in anim_ids.h for %d to find missing anim\n)", state, state);
        return false;
    }
    return true;
}

long CAnimated::SetAnimationState(unsigned short state, float blend, unsigned short& transition, long animation, void* data) {
    if (unknown0cc0 && !m_animObject->m_flag24b0 && g_AnimDB.GetStateCallbackID(state, m_animSet) == 3)
        return 0xFFFF0000;
    int pending = AnimStTransPreCheck(m_stateMachine, &unknown0004, state);
    if (pending >= 0) {
        transition = pending;
        return 0xF0F0F0F0;
    }
    unknown0cc1 = false;
    long result = AnimStStartState(m_stateMachine, &unknown0004, m_channels, state, this, data, blend);
    *(long*)AnimStGetStateUserMemory(m_stateMachine, state) = animation;
    return result;
}

int CAnimated::FinishAnimationState(unsigned short state) {
    long* animation = (long*)AnimStGetStateUserMemory(m_stateMachine, state);

    if (*animation != -1)
        m_animObject->FinishAnimationState(*animation, (state & 0x8000) != 0);
    return 0;
}

void CAnimated::UpdateAnimation(float time) {
    AnimStProcess(m_stateMachine, &unknown0004, m_channels, this);
    AnimChanProcess(time, &unknown0004, m_channels, this);
    m_weightList = AnimMotCreateWeightList(&unknown0004, m_channels, m_stateMachine);
}

void CAnimated::Draw(CDrawContext& context) {
    if (!m_mesh)
        return;
    DMRenderUpdateParts(&m_render);
    CMatrix localToCamera;
    localToCamera.Multiply(*context.m_localToWorld, *context.m_worldToView);
    g_matStack->Push();
    g_matStack->Load(&localToCamera);
    DMClusterSynthesizeMatrices(0, &m_cluster, m_mesh, m_root->m_orient);
    g_matStack->Pop();
    DMRenderResetState(&m_render);
    CMatrix viewProj(*context.m_viewToScreen);
    DMRenderSetViewProj(&m_render, &viewProj);
    DMRenderSet2dReject(&m_render, context.m_reject);
    DMRenderSetLocalToCamera(&m_render, &localToCamera, 0);
    DMRenderSetScale(&m_render, context.m_scale.x, context.m_scale.y, context.m_scale.z);
    DMRenderSetClusterMatrices(&m_render, m_cluster.m_matrices);
    g_matStack->Push();
    g_matStack->Load(context.m_worldToView);
    CLightVolume* volume = m_animObject->GetLightVolume();
    if (!volume)
        volume = CLight::GetDefaultLightVolume();
    context.m_lightVolume = volume;
    LightRecordView* light = volume->m_lights;
    DMLighting_T lighting;
    memset(&lighting, 0, sizeof(lighting));
    lighting.colors[0][3] = 255.0f;
    lighting.colors[1][3] = 255.0f;
    lighting.colors[2][3] = 255.0f;
    lighting.ambient[3] = 255.0f;
    int count = 0;
    for (int i = 0; i < volume->m_count; i++, light++) {
        float intensity = light->intensity;
        if (light->type == 2) {
            lighting.dirs[count][0] = light->dir[0];
            lighting.dirs[count][1] = light->dir[1];
            lighting.dirs[count][2] = light->dir[2];
            lighting.colors[count][0] = 255.0f * light->color[0] * intensity;
            lighting.colors[count][1] = 255.0f * light->color[1] * intensity;
            lighting.colors[count][2] = 255.0f * light->color[2] * intensity;
            count++;
        } else {
            lighting.ambient[0] = 255.0f * light->color[0] * intensity;
            lighting.ambient[1] = 255.0f * light->color[1] * intensity;
            lighting.ambient[2] = 255.0f * light->color[2] * intensity;
        }
    }
    LightListView* lights = context.m_lights;
    if (lights && lights->m_size) {
    float radius;
    float py;
    float weight;
    float dist;
    float pz;
    float bestWeight;
        CLight* best = 0;
        bestWeight = 0.0f;
        CVector3 pos = context.m_localToWorld->pos;
        py = pos.y;
        pz = pos.z;
        for (CLight** it = lights->m_data; it != context.m_lights->m_data + context.m_lights->m_size; it++) {
            CVector3 d;
            (*it)->GetPosition(d);
            d.x -= pos.x;
            d.y -= py;
            d.z -= pz;
            dist = LengthSq(d);
            radius = (*it)->m_radius * (*it)->m_radius;
            if (dist < radius) {
                weight = 1.0f - dist / radius;
                if (weight > bestWeight) {
                    bestWeight = weight;
                    best = *it;
                }
            }
        }
        if (best) {
            if (count >= 3)
                count = 2;
            CVector3 d;
            best->GetPosition(d);
            d.x -= pos.x;
            d.y -= py;
            d.z -= pz;
            float length = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);
            if (length != 0.0f) {
                float scale = 1.0f / length;
                d.x *= scale;
                d.y *= scale;
                d.z *= scale;
            }
            float r = best->m_color[0];
            float g = best->m_color[1];
            float b = best->m_color[2];
            lighting.dirs[count][0] = d.x;
            lighting.dirs[count][1] = d.y;
            lighting.dirs[count][2] = d.z;
            lighting.colors[count][0] = bestWeight * r;
            lighting.colors[count][1] = bestWeight * g;
            lighting.colors[count][2] = bestWeight * b;
        }
        context.m_light = best;
    }
    DMLightingUpdate(&m_render, &lighting);
    if (m_flags & 0x100)
        DMLightingScaleAlpha(&m_render, m_alpha / 127.0f);
    g_matStack->Pop();
    m_mesh->Draw(context, &m_render);
}

void CAnimated::BindMesh(CDMesh* mesh) {
    DMRenderBindModel(&m_render, mesh);
    m_mesh = mesh;
}

void CAnimated::BindTexture(CTexture* texture, char index) {
    if (!m_mesh)
        return;
    if (index >= 0) {
        m_mesh->materials[index].texture = texture;
        return;
    }
    for (unsigned int i = 0; i < m_mesh->materialCount; i++)
        m_mesh->materials[i].texture = texture;
}

void CAnimated::BindSleeveTexture(CTexture* texture) {
    if (!m_mesh)
        return;
    for (unsigned int i = 0; i < m_mesh->materialCount; i++) {
        if (m_mesh->materials[i].flags & 0x200)
            m_mesh->materials[i].texture = texture;
    }
}

void CAnimated::SetGeomNodeState(long node, long state) {
    DMGeomSetNodeState(&m_geometry, node, state);
}

void CAnimated::AdjustRootTrans(CVector3 delta) {
    AnimRootView* root = m_root;
    root->x += delta.x;
    root->y += delta.y;
    root->z += delta.z;
}

void CAnimated::SetAnimSpeed(unsigned char channel, float speed) {
    AnimChanSetSpeed(&m_channels[channel], speed);
}

void CAnimated::BindSkeleton(CharSkel_t* skeleton) {
    DMClusterBindSkeleton(&m_cluster, skeleton);
    m_skeleton = skeleton;
}

CAnimMatrixCache::CAnimMatrixCache(int count, int vectorCount, int matrixCount) {
    m_count = count;
    m_vectorCount = vectorCount;
    m_matrixCount = matrixCount;
    m_buffer = 0;
    m_entries = (AnimCacheEntryView*)DWI_alloc("Animation Matrix Cache", m_count * sizeof(AnimCacheEntryView), 1024);
    for (int i = 0; i < m_count; i++) {
        m_entries[i].key = 0;
        m_entries[i].age = 0;
    }
    m_vectors[0] = 0;
    m_vectors[1] = 0;
    m_matrices[0] = (AnimCacheMatrixView*)DWI_alloc("Animation Matrix buffer 0", m_matrixCount * (m_count * sizeof(AnimCacheMatrixView)), 1024);
    m_matrices[1] = 0;
}

void CAnimMatrixCache::Update() {
    m_buffer = 0;
    for (int i = 0; i < m_count; i++) {
        if (m_entries[i].key)
            m_entries[i].age++;
    }
}

AnimCacheSlotView CAnimMatrixCache::GetEntry(void* key, bool& created) {
    int oldest = 0;
    int oldestAge = 0;

    created = false;
    for (int i = 0; i < m_count; i++) {
        if (m_entries[i].key == key) {
            m_entries[i].age = 0;
            return Slot(i);
        }
        if (m_entries[i].key == 0) {
            oldest = i;
            oldestAge = 0x7FFFFFF;
        } else if (m_entries[i].age > oldestAge) {
            oldest = i;
            oldestAge = m_entries[i].age;
        }
    }
    created = true;
    m_entries[oldest].key = key;
    m_entries[oldest].age = 0;
    return Slot(oldest);
}

__declspec(weak) CAnimMorph* CAnimated::AsAnimMorph() {
    return 0;
}

__declspec(weak) void CAnimated::SetRotation(long) {
}

__declspec(weak) int CAnimated::ExtractTranslation(CVector3*) {
    return 0;
}

__declspec(weak) int CAnimated::ExtractRotation(long*) {
    return 0;
}

__declspec(weak) CAnimated::~CAnimated() {
}
