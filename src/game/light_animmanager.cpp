// A fragment of light.cpp (0x800777c0): CLight's weak AsLight casts (both
// return the light itself); CAnimLightManager::Create from light data (a
// light taken from the animated-light pool and constructed in place as a
// CPropertyAnimLight, then added to the scene) and Create from a CRC (the
// light data with that CRC, a light from the instanced pool constructed as a
// CInstancedAnimLight and added to the scene); CInstancedAnimLight::Destroy
// (its light returned to the global manager's instanced pool) and the
// CInstancedAnimLight constructor. The file name is this project's; the
// original record is light.cpp, after light_instanced_dtor.cpp. The classes,
// structs and functions are named by the mangled symbols; the pools (used and
// free lists linked through the light at +224, a count) are an inferred view
// whose name is not known, and the other members are inferred. ISceneNode is
// reduced to the casts (no virtual call is made here); each class declares
// its own first virtual (defined elsewhere) first so the global tables are not
// emitted; for CInstancedAnimLight that is its destructor, weak in the
// original (light_instanced_dtor.cpp), declared here without a body because
// Destroy is defined here. The manager is a file-local object in the original; this fragment
// declares it non-static so it links to the original object.
inline void* operator new(unsigned long, void* p) { return p; }
class ISceneNode;
class CLight;

#include "game/CScene.h"


struct MOH_animatedLight_Struct {
    unsigned char unknown00[72];
    unsigned long crc;
    unsigned char unknown4c[48];
};

/* ISceneNode reduced to what these functions use: the light casts. The
   slots of the full table are not needed here (no virtual call is made). */
class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual const CLight* AsLight() const;
    virtual CLight* AsLight();
};

class CLight : public ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual const CLight* AsLight() const;
    virtual CLight* AsLight();

    unsigned char unknown004[220];
    CLight* m_poolNext;
};

class CPropertyAnimLight : public CLight {
public:
    CPropertyAnimLight(MOH_animatedLight_Struct*);
    virtual void Destroy();

    unsigned char unknown0e4[24];
};

class CInstancedAnimLight : public CPropertyAnimLight {
public:
    virtual ~CInstancedAnimLight();
    CInstancedAnimLight(MOH_animatedLight_Struct*);
    virtual void Destroy();
};

/* inferred: a pool of lights linked through the light's pool link */
template <class T>
struct LightPoolView {
    T* Alloc() {
        T* light = m_free;
        if (!light)
            return 0;
        m_free = (T*)light->m_poolNext;
        light->m_poolNext = m_used;
        m_used = light;
        m_count++;
        return light;
    }
    void Free(T* light) {
        if (light == m_used) {
            m_used = (T*)light->m_poolNext;
        } else {
            for (T* prev = m_used; prev; prev = (T*)prev->m_poolNext) {
                if (prev->m_poolNext == light) {
                    prev->m_poolNext = light->m_poolNext;
                    break;
                }
            }
        }
        m_count--;
        light->m_poolNext = m_free;
        m_free = light;
    }

    T* m_used;
    T* m_free;
    int data08;
    int m_count;
    int data10;
};

class CAnimLightManager {
public:
    CPropertyAnimLight* Create(MOH_animatedLight_Struct*);
    CInstancedAnimLight* Create(unsigned long);

    unsigned char unknown00[36];
    MOH_animatedLight_Struct* m_lights;
    int m_lightCount;
    CScene* m_scene;
    unsigned char unknown30[4];
    LightPoolView<CInstancedAnimLight> m_instanced;
    LightPoolView<CPropertyAnimLight> m_animated;
};

extern CAnimLightManager g_AnimLightManager;

__declspec(weak) const CLight* CLight::AsLight() const {
    return this;
}

__declspec(weak) CLight* CLight::AsLight() {
    return this;
}

CPropertyAnimLight* CAnimLightManager::Create(MOH_animatedLight_Struct* data) {
    void* memory = m_animated.Alloc();
    CPropertyAnimLight* light = (CPropertyAnimLight*)memory;
    if (memory) {
        new (light) CPropertyAnimLight(data);
        m_scene->Add(*light);
        return light;
    }
    return 0;
}

CInstancedAnimLight* CAnimLightManager::Create(unsigned long crc) {
    for (int i = 0; i < m_lightCount; i++) {
        if (crc == m_lights[i].crc) {
            CInstancedAnimLight* light = m_instanced.Alloc();
            if (light) {
                light = new (light) CInstancedAnimLight(&m_lights[i]);
                m_scene->Add(*light);
                return light;
            }
            return 0;
        }
    }
    return 0;
}

void CInstancedAnimLight::Destroy() {
    g_AnimLightManager.m_instanced.Free(this);
}

CInstancedAnimLight::CInstancedAnimLight(MOH_animatedLight_Struct* data) : CPropertyAnimLight(data) {
}
