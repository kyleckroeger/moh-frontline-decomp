// A fragment of collisionvolume.cpp (0x80098200): CCollisionVolume::Init
// keeps the volume, copies the transform and keeps the collision ID; the
// destructor and the constructor (the scene-node base chain's virtual table
// pointers, eight cleared words, the transform's class initialisation and a
// null volume). The file name is this project's; the original record is
// collisionvolume.cpp, and these functions lie between collisionvolume_rows.cpp
// and hearingvolume.cpp. The class names, the base chain (IDestructible,
// ISubject, IObserver, ISceneNode) and the member functions come from the
// mangled symbols and the virtual tables; the members, their names and the
// inline base constructors' member initialisation are inferred views, and
// only the virtuals these functions need are declared.
//
// ISceneNode's destructor is inline, so its virtual table is weak in the
// original (kept from another file) and the compiler emits a weak copy of the
// table and of the destructor here; both are weak duplicates, linked to the
// original copies.
class IVolume;

#include "game/CVector3.h"

#include "game/CMatrix.h"

#include "game/ISceneNode.h"

class CCollisionVolume : public ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~CCollisionVolume();
    CCollisionVolume();
    void Init(IVolume*, CMatrix*, EClsnId);

    unsigned char unknown24[12];
    CMatrix m_tm;
    IVolume* m_volume;
    EClsnId m_collisionId;
};

void CCollisionVolume::Init(IVolume* volume, CMatrix* matrix, EClsnId id) {
    m_volume = volume;
    m_tm = *matrix;
    m_collisionId = id;
}

CCollisionVolume::~CCollisionVolume() {
}

CCollisionVolume::CCollisionVolume() {
    m_volume = 0;
}
