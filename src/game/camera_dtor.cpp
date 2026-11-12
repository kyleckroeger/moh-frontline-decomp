// A fragment of camera.cpp (0x8007a534): CCamera::BeginUpdate (empty) and the
// CCamera destructor (its virtual table pointer; the script object at +656
// destroyed and cleared when there is one; the ten planes at +336 destroyed
// through the array destructor; then the inline IMovingSceneNode and
// ISceneNode destructors and IObserver's; the object freed when asked). The
// file name is this project's; the original record is camera.cpp, after
// camera_rows.cpp. The classes and functions are named by the mangled
// symbols; the members and their names are inferred, and only the virtuals
// these functions need are declared. CCamera declares Draw (defined in
// camera_rows.cpp; the result type is not known) first so its global virtual
// table is not emitted here. IMovingSceneNode's, ISceneNode's and CPlane's
// destructors are inline (the scene-node tables weak) in the original, so
// the compiler's copies are weak duplicates, linked to the original copies.
class CDrawContext;
struct BSObject;
void DestroyBSObject(BSObject*);







#include "game/ISceneNode.h"


class IMovingSceneNode : public ISceneNode {
public:
    IMovingSceneNode() {}
    virtual ~IMovingSceneNode() {}
};

class CPlane {
public:
    ~CPlane() {}

    unsigned char unknown00[32];
};

/* inferred: the code checks the planes' address before the array
   destructor, which a member object with its own inline destructor
   reproduces. This is uncertain: the original has no weak destructor for
   such a wrapper, and the inlined CCamera constructor in player_ctor.cpp
   matches with a plain CPlane array, so the check may have another cause. */
struct CameraPlanesView {
    ~CameraPlanesView() {}

    CPlane m_planes[10];
};

class CCamera : public IMovingSceneNode {
public:
    virtual void Draw(CDrawContext&); /* result type not known */
    virtual void BeginUpdate(float);
    virtual ~CCamera();

    unsigned char unknown24[300];
    CameraPlanesView m_planes;
    BSObject* m_script;
};

void CCamera::BeginUpdate(float) {
}

CCamera::~CCamera() {
    if (m_script) {
        DestroyBSObject(m_script);
        m_script = 0;
    }
}
