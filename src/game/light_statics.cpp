// A fragment of light.cpp (0x800785a4): CLight's default light volume, marking
// a light for destruction (removed from the scene first when it is in it, then
// the subject's marking), the empty Destroy, the two Create overloads and
// Register, which forward to the animated-light manager, then ResetClass
// (every light in the manager's two in-use lists removed from its scene, then
// both node pools reset to all-free) and InitClass (the scene kept, both pools
// allocated with 10 nodes each and reset, and the file's ready flag set).
// CLight, CScene,
// ISceneNode, ISubject, CAnimLightManager, BPDLightVolume and
// MOH_animatedLight_Struct are named by the mangled symbols; CLight is a
// non-virtual view used as its scene node and subject (both at its start),
// the manager's registered members are inferred, and the result types are
// inferred. The pool (data, in-use list, free list, capacity, count) and its
// 240-byte nodes (a light, the next link at +224) are an inferred view; its
// Init and Reset are inferred inlines, and g_AnimLightManager and g_bReady are
// the file's statics. The rest of the file is not part of this unit.
struct BPDLightVolume;
struct MOH_animatedLight_Struct;
class ISceneNode;
class CLight;

class ISubject {
public:
    void MarkForDestruction(int);
};

#include "game/CScene.h"


void* DWI_alloc(const char*, int, int);

/* Inferred: a pool node, a light with the next link after it. */
struct AnimLightNodeView {
    unsigned char light[224];
    AnimLightNodeView* next;
    unsigned char unknownE4[12];
};

/* Inferred: a fixed pool of light nodes with in-use and free lists. */
struct AnimLightPoolView {
    void Reset() {
        m_used = 0;
        m_free = m_data;
        m_count = 0;
        m_data[m_capacity - 1].next = 0;
        for (int i = 0; i < m_capacity - 1; i++)
            m_free[i].next = &m_free[i + 1];
    }
    void Init(int capacity) {
        m_capacity = capacity;
        m_data = (AnimLightNodeView*)DWI_alloc(0, m_capacity * sizeof(AnimLightNodeView), 1024);
        Reset();
    }

    AnimLightNodeView* m_data;
    AnimLightNodeView* m_used;
    AnimLightNodeView* m_free;
    int m_capacity;
    int m_count;
};

class CAnimLightManager {
public:
    CLight* Create(MOH_animatedLight_Struct*);
    CLight* Create(unsigned long);

    unsigned char unknown00[36];
    void* m_registered;
    int m_registeredCount;
    CScene* m_scene;
    AnimLightPoolView m_pool30;
    AnimLightPoolView m_pool44;
};

extern CScene g_scene;
extern CAnimLightManager g_AnimLightManager;
extern BPDLightVolume g_DefaultLightVolume;
extern bool g_bReady;

class CLight {
public:
    static BPDLightVolume* GetDefaultLightVolume();
    void MarkForDestruction(int);
    void Destroy();
    static CLight* Create(void*);
    static CLight* Create(unsigned long);
    static void Register(void*, int);
    static void ResetClass();
    static void InitClass(CScene*);
};

BPDLightVolume* CLight::GetDefaultLightVolume() {
    return &g_DefaultLightVolume;
}

void CLight::MarkForDestruction(int flag) {
    if (g_scene.IsNodeInScene((ISceneNode*)this))
        g_scene.Remove(*(ISceneNode*)this);
    ((ISubject*)this)->MarkForDestruction(flag);
}

void CLight::Destroy() {
}

CLight* CLight::Create(void* data) {
    return g_AnimLightManager.Create((MOH_animatedLight_Struct*)data);
}

CLight* CLight::Create(unsigned long id) {
    return g_AnimLightManager.Create(id);
}

void CLight::Register(void* lights, int count) {
    g_AnimLightManager.m_registered = lights;
    g_AnimLightManager.m_registeredCount = count;
}

void CLight::ResetClass() {
    AnimLightNodeView* node;
    for (node = g_AnimLightManager.m_pool30.m_used; node; node = node->next)
        g_AnimLightManager.m_scene->Remove(*(ISceneNode*)node);
    for (node = g_AnimLightManager.m_pool44.m_used; node; node = node->next)
        g_AnimLightManager.m_scene->Remove(*(ISceneNode*)node);
    g_AnimLightManager.m_pool44.Reset();
    g_AnimLightManager.m_pool30.Reset();
}

void CLight::InitClass(CScene* scene) {
    g_AnimLightManager.m_scene = scene;
    g_AnimLightManager.m_pool44.Init(10);
    g_AnimLightManager.m_pool30.Init(10);
    g_bReady = true;
}
