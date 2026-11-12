// A fragment of hearingvolume.cpp (0x80098c8c): CHearingVolume's destructor
// (the two sphere volumes, then the inline ISceneNode destructor and
// IObserver's) and constructor (the scene-node base chain's virtual table
// pointers, eight cleared words, the transform's class initialisation, the
// two sphere volumes through their inline constructors, a null owner). The
// file name is this project's; the original record is hearingvolume.cpp, and
// these functions lie between hearingvolume_init.cpp and hierobject_weak.cpp.
// The class names, the base chains (IDestructible, ISubject, IObserver,
// ISceneNode; IVolume, CVolSphere) and the member functions come from the
// mangled symbols and the virtual tables; the members, their names and the
// inline base constructors' member initialisation are inferred views (as in
// collisionvolume_ctor.cpp and hearingvolume_init.cpp), and only the virtuals
// these functions need are declared.
//
// ISceneNode's and IVolume's destructors are inline, so their virtual tables
// are weak in the original (kept from other files) and the compiler emits weak
// copies of the tables and destructors here; all four are weak duplicates,
// linked to the original copies.

#include "game/CVector3.h"

#include "game/CMatrix.h"


class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual void MarkForDestruction(int);
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

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
};

class CVolSphere : public IVolume {
public:
    CVolSphere() {}
    virtual ~CVolSphere();

    unsigned char unknown04[28];
};

class CStaticObject;

class CHearingVolume : public ISceneNode {
public:
    virtual void BeginUpdate(float);
    virtual ~CHearingVolume();
    CHearingVolume();

    CStaticObject* m_owner;
    unsigned char unknown28[8];
    CMatrix m_tm;
    CVector3 m_lastPosition;
    CVolSphere m_localVolume;
    CVolSphere m_worldVolume;
};

CHearingVolume::~CHearingVolume() {
}

CHearingVolume::CHearingVolume() {
    m_owner = 0;
}
