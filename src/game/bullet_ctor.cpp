// A fragment of bullet.cpp (0x800ccc00): CBullet::Draw (empty); Shutdown (the
// float at +144 cleared, -1 at +36 and FLT_MAX at +148); Init (the transform
// at +80 reset to identity, Shutdown's stores through a direct call the
// compiler inlines, two words cleared); the destructor (its virtual table
// pointer, then the inline IMovingSceneNode and ISceneNode destructors and
// IObserver's, and the object freed when asked) and the default constructor
// (the scene-node base constructors: table pointers and eight cleared words;
// the transform's class initialisation; the word at +40 cleared). The file
// name is this project's; the original record is bullet.cpp. The classes and
// functions are named by the mangled symbols; the members and their names are
// inferred views (as in static_obj_dtor.cpp), and only the virtuals these
// functions need are declared. CBullet declares GetTMLocalToWorld (its first
// own virtual, defined elsewhere) first so its global virtual table is not
// emitted here. IMovingSceneNode's and ISceneNode's destructors are inline
// and their tables weak in the original, so the compiler's copies are weak
// duplicates, linked to the original copies. The float constants are items of
// the file's .sdata2 pool, linked at their original addresses.
class CDrawContext;

class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

class ISceneNode : public IObserver {
public:
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}

    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class IMovingSceneNode : public ISceneNode {
public:
    IMovingSceneNode() {}
    virtual ~IMovingSceneNode() {}
};

#include "game/CVector3.h"

#include "game/CMatrix.h"


class CBullet : public IMovingSceneNode {
public:
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual ~CBullet();
    virtual void Draw(CDrawContext&);
    virtual void Init();
    virtual void Shutdown();
    CBullet();

    int data24;
    int data28;
    unsigned char unknown2c[36];
    CMatrix m_tm;
    float data90;
    float data94;
};

void CBullet::Draw(CDrawContext&) {
}

void CBullet::Shutdown() {
    data90 = 0.0f;
    data24 = -1;
    data94 = 3.4028235e38f;
}

void CBullet::Init() {
    m_tm.Ident();
    CBullet::Shutdown();
    data08 = 0;
    data04 = 0;
}

CBullet::~CBullet() {
}

CBullet::CBullet() {
    data28 = 0;
}
