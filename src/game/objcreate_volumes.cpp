// A fragment of objcreate.cpp (0x8003c1b0): the collision-volume pools.
// Sphere and box volumes share one instance allocator and animated volumes
// have their own; each Allocate function constructs the volume in place in
// the next free element (when there is one) and each Free function returns
// the element. BSGO_Basic::Destroy (weak, empty) follows them, then
// DelayDestroyObject (marks the object's scene node for destruction, mode 2,
// and clears the script object's queue identity to -1), then
// BSGO_Player::Destroy (cleans up the player, destroys the script object and
// clears both pointers; the key function of __vt__11BSGO_Player). The file name is
// this project's; the original record is objcreate.cpp. BSGO_Basic's three
// words before its table pointer, its table (five slots, the first three
// named after the bsbifunc views, the last two unknown), the script object's
// queue identity at +16 and ISceneNode's first slot are inferred; its
// compiled table is a weak duplicate of the original __vt__10BSGO_Basic. The classes and
// globals are named by the mangled symbols; the volume classes are declared
// only as far as their constructors (inline for the sphere and box, which
// store the IVolume and then the derived vtable), and their virtual tables
// are defined elsewhere.
inline void* operator new(unsigned long, void* place) {
    return place;
}

class BSUtilObjectInstanceMemoryAllocator {
public:
    void FreeElement(void*);
    void* GetFreeElement(bool);

    unsigned char unknown00[40];
};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
};

class CVolSphere : public IVolume {
public:
    CVolSphere() {}
    virtual ~CVolSphere();
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
};

class CAnimatedVolume {
public:
    CAnimatedVolume();
};

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
};

struct BSObjectView {
    unsigned char unknown00[16];
    int queueIdentity;
};

class BSGO_Basic {
public:
    BSObjectView* m_object;
    unsigned char unknown04[8];

    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData(); /* result type not known */
    virtual void* GetEmbeddedHitReactionInfo(); /* result type not known */

};

class CPlayerObject {
public:
    void CleanUp();
};

class BSObject;
void DestroyBSObject(BSObject*);

/* inferred: the player script object (see player_ctor.cpp), its player at +44 */
class BSGO_Player : public BSGO_Basic {
public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
    virtual void* GetProximityData();

    unsigned char unknown10[28];
    CPlayerObject* m_player;
};

extern BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;
extern BSUtilObjectInstanceMemoryAllocator g_pAnimatedVolumeAllocator;

void FreeSphereVolume(void* volume) {
    g_pVolumeAllocator.FreeElement(volume);
}

void* AllocateSphereVolume() {
    return new (g_pVolumeAllocator.GetFreeElement(true)) CVolSphere;
}

void FreeBoxVolume(void* volume) {
    g_pVolumeAllocator.FreeElement(volume);
}

void* AllocateBoxVolume() {
    return new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;
}

void FreeAnimatedVolume(void* volume) {
    g_pAnimatedVolumeAllocator.FreeElement(volume);
}

void* AllocateAnimatedVolume() {
    return new (g_pAnimatedVolumeAllocator.GetFreeElement(true)) CAnimatedVolume;
}

__declspec(weak) void BSGO_Basic::Destroy() {
}

void DelayDestroyObject(BSGO_Basic* object) {
    if (object->GetSceneNode())
        object->GetSceneNode()->MarkForDestruction(2);
    if (object->m_object)
        object->m_object->queueIdentity = -1;
}

void BSGO_Player::Destroy() {
    m_player->CleanUp();
    DestroyBSObject((BSObject*)m_object);
    m_object = 0;
    m_player = 0;
}
