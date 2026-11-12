// A fragment of anim_obj.cpp (0x80095734): the CAnimObject destructor (deletes
// the animated volume at +9112 through its virtual destructor and clears the
// pointer; then the members: the light-volume manager at +9400, the collision
// list at +9272 (dwi::fast_vec<CAnimObject::CClsnInfo>, freed when it owns
// its storage) and the skinned animation at +48; then the inline
// IMovingSceneNode and ISceneNode destructors and IObserver's; the object
// freed when asked) and BindLocationCollision (forwarded to the animated
// volume when there is one). The file name is this project's; the original
// record is anim_obj.cpp. The classes and functions are named by the mangled
// symbols; the member positions come from the code, while their names and
// the space between them are inferred, and only the virtuals these functions
// need are declared. CAnimObject declares MarkForDestruction (defined
// elsewhere) before its destructor so its global virtual table is not emitted
// here. IMovingSceneNode's and ISceneNode's destructors are inline and their
// tables weak in the original, so the compiler's copies are weak duplicates,
// linked to the original copies.
extern "C" void MEM_free(void*);







#include "game/ISceneNode.h"


class IMovingSceneNode : public ISceneNode {
public:
    IMovingSceneNode() {}
    virtual ~IMovingSceneNode() {}
};

namespace dwi {
template <class T>
class fast_vec {
public:
    ~fast_vec() {
        if (m_owned && m_data)
            MEM_free(m_data);
    }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

class CLightVolumeManager {
public:
    ~CLightVolumeManager();

    unsigned char unknown00[4];
};

class CAnimSkinned {
public:
    virtual ~CAnimSkinned();

    unsigned char unknown04[44];
};

class CObjLocationCollisionData;
class CObjLocationCollisionMap;

class CAnimatedVolume {
public:
    virtual ~CAnimatedVolume();
    void Set(CObjLocationCollisionData*, CObjLocationCollisionMap*);
};

class CAnimObject : public IMovingSceneNode {
public:
    struct CClsnInfo;
    virtual void MarkForDestruction(int);
    virtual ~CAnimObject();
    void BindLocationCollision(CObjLocationCollisionData*, CObjLocationCollisionMap*);

    unsigned char unknown24[12];
    CAnimSkinned m_skinned;
    unsigned char unknown0060[9016];
    CAnimatedVolume* m_volume;
    unsigned char unknown239c[156];
    dwi::fast_vec<CClsnInfo> m_collisions;
    unsigned char unknown2450[104];
    CLightVolumeManager m_lightVolumes;
};

CAnimObject::~CAnimObject() {
    if (m_volume) {
        delete m_volume;
        m_volume = 0;
    }
}

void CAnimObject::BindLocationCollision(CObjLocationCollisionData* data, CObjLocationCollisionMap* map) {
    if (m_volume)
        m_volume->Set(data, map);
}
