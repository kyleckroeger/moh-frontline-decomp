// CCollisionVolume, the functions at the start of the file: collision id and
// script accessors, identity casts, destruction through the script object and
// marking for destruction. The class names come from the mangled symbols.
// This is an inferred, non-virtual view: only the members these functions
// touch are declared, at their offsets (their names are not original), and
// the virtual table is not reproduced. The accessors and casts are inline in
// the original (weak symbols), so they are defined __declspec(weak). The row
// accessors later in the file are the collisionvolume_rows.cpp fragment, and
// the bullet handler that follows these functions is not reconstructed.
enum EClsnId {};

// The object at BSObject+12; only its first virtual slot is called here, and
// its name is unknown (as in bsmachin.cpp).
class BSObjectUserView {
    unsigned char unknown00[12];

public:
    virtual void unknownVirtual0();
};

struct BSObject {
    unsigned char unknown00[12];
    BSObjectUserView* user;
};

class ISubject {
public:
    void MarkForDestruction(int);
};

class ISceneNode;

#include "game/CScene.h"


extern CScene g_scene;

class CCollisionVolume {
public:
    EClsnId GetCollisionId() const;
    const CCollisionVolume* AsCollisionVolume() const;
    CCollisionVolume* AsCollisionVolume();
    BSObject* GetScriptObject() const;
    void SetCollisionId(EClsnId);
    void Destroy();
    void MarkForDestruction(int);

    unsigned char unknown00[116];
    EClsnId m_collisionId;
    BSObject* m_script;
};

__declspec(weak) EClsnId CCollisionVolume::GetCollisionId() const {
    return m_collisionId;
}

__declspec(weak) const CCollisionVolume* CCollisionVolume::AsCollisionVolume() const {
    return this;
}

__declspec(weak) CCollisionVolume* CCollisionVolume::AsCollisionVolume() {
    return this;
}

__declspec(weak) BSObject* CCollisionVolume::GetScriptObject() const {
    return m_script;
}

__declspec(weak) void CCollisionVolume::SetCollisionId(EClsnId id) {
    m_collisionId = id;
}

void CCollisionVolume::Destroy() {
    if (m_script && m_script->user)
        m_script->user->unknownVirtual0();
}

void CCollisionVolume::MarkForDestruction(int flag) {
    // The ISubject and ISceneNode bases are at the start of the object.
    ((ISubject*)this)->MarkForDestruction(flag);
    g_scene.Remove(*(ISceneNode*)this);
}
