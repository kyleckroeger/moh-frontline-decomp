// A fragment of static_obj.cpp (0x800b1d64): the CStaticObject destructor
// (its virtual table pointer; the light-volume manager at +608 and the
// spline-path traversals at +176 and +136 destroyed; then the inline
// IMovingSceneNode and ISceneNode destructors and IObserver's; the object
// freed when asked) and the two constructors. Both run the scene-node base
// constructors (table pointers, eight cleared words), the transform's class
// initialisation, the two traversals and the light-volume manager; the mesh
// constructor keeps the mesh, its five integer arguments (in two groups of
// fields, with a flag bit set when the fifth is non-zero) and the motion
// frame, and the default one clears its own set of fields. The same body
// follows in both: the transform reset to identity, 1.9979998f (bits
// 0x3fffbe75) at +128, 0.5f at +232, -1 at +476 and further fields and flag
// bits cleared. The file name is this project's; the original record is
// static_obj.cpp, after static_obj_bounds.cpp. The classes, the nested
// MotionFrame type and the functions are named by the mangled symbols; the
// member positions come from the code, while their names, their types (the
// flags are one-bit bool fields: one is set from "!= 0") and the space
// between them are inferred. Only the virtuals these functions need are
// declared. CStaticObject declares MarkForDestruction (defined elsewhere)
// before its destructor so its global virtual table is not emitted here.
// IMovingSceneNode's and ISceneNode's destructors are inline and their
// tables weak in the original, so the compiler's copies are weak duplicates,
// linked to the original copies. The float constants are items of the file's
// .sdata2 pool, linked at their original addresses.
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

class CAISplinePathTraversal {
public:
    CAISplinePathTraversal();
    ~CAISplinePathTraversal();

    unsigned char unknown00[40];
};

class CLightVolumeManager {
public:
    CLightVolumeManager();
    ~CLightVolumeManager();

    unsigned char unknown00[4];
};

class CStaticMesh;

#include "game/CVector3.h"


#include "game/CMatrix.h"


class CStaticObject : public IMovingSceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~CStaticObject();
    struct MotionFrame;
    CStaticObject(CStaticMesh*, int, int, int, int, int, MotionFrame*);
    CStaticObject();

    int data24;
    int data28;
    int data2c;
    CStaticMesh* m_mesh;
    unsigned char unknown34[12];
    CMatrix m_tm;
    float data80;
    unsigned char unknown84[4];
    CAISplinePathTraversal m_traversalA;
    CAISplinePathTraversal m_traversalB;
    float datad8;
    unsigned char unknowndc[4];
    float datae0;
    float datae4;
    float datae8;
    int dataec;
    float dataf0;
    unsigned char unknownf4[16];
    int data104;
    int data108;
    int data10c;
    int data110;
    int data114;
    int data118;
    unsigned char unknown11c[68];
    float data160;
    float data164;
    float data168;
    unsigned char unknown16c[76];
    float data1b8;
    float data1bc;
    unsigned char unknown1c0[28];
    int data1dc;
    bool flag1e0_7 : 1;
    bool flag1e0_6 : 1;
    bool flag1e0_5 : 1;
    bool flag1e0_4 : 1;
    bool flag1e0_3 : 1;
    bool flag1e0_2 : 1;
    bool flag1e0_1 : 1;
    bool flag1e0_0 : 1;
    bool flag1e1_7 : 1;
    bool flag1e1_6 : 1;
    bool flag1e1_5 : 1;
    int data1e4;
    unsigned char unknown1e8[20];
    int data1fc;
    int data200;
    int data204;
    int data208;
    float data20c;
    int data210;
    int data214;
    int data218;
    int data21c;
    int data220;
    int data224;
    float data228;
    int data22c;
    int data230;
    unsigned char unknown234[20];
    MotionFrame* m_motionFrame;
    unsigned char unknown24c[20];
    CLightVolumeManager m_lightVolumes;
};

CStaticObject::~CStaticObject() {
}

CStaticObject::CStaticObject(CStaticMesh* mesh, int a, int b, int c, int d, int flag, MotionFrame* frame)
    : data24(0), data28(0), data2c(0), m_mesh(mesh), data118(0), flag1e0_7(1), flag1e0_3(flag != 0),
      flag1e0_2(0), flag1e0_1(0), data1fc(-1), data200(0), data204(0), data208(a), data20c(0.0f), data210(b),
      data214(0), data218(-1), data21c(0), data220(0), data224(c), data228(0.0f), data22c(d), data230(0),
      m_motionFrame(frame) {
    m_tm.Ident();
    data80 = 1.9979998f; /* bits 0x3fffbe75 */
    data110 = 0;
    data114 = 0;
    data108 = 0;
    data10c = 0;
    flag1e0_5 = 0;
    flag1e0_6 = 0;
    data1bc = 0.0f;
    data1b8 = 0.0f;
    data1dc = -1;
    data104 = 0;
    flag1e0_0 = 0;
    datae8 = 0.5f;
    flag1e1_7 = 0;
    data168 = 0.0f;
    data164 = 0.0f;
    data160 = 0.0f;
    data1e4 = 0;
    flag1e1_6 = 0;
    flag1e1_5 = 0;
}

CStaticObject::CStaticObject()
    : data24(0), data28(0), data2c(0), m_mesh(0), datad8(0.0f), datae0(0.0f), datae4(0.0f), dataec(0), dataf0(0.0f), data104(0), data118(0), flag1e0_7(1),
      flag1e0_1(0), data1fc(-1), data200(0), data204(0), data208(0), data210(0), data214(0) {
    m_tm.Ident();
    data80 = 1.9979998f; /* bits 0x3fffbe75 */
    data110 = 0;
    data114 = 0;
    data108 = 0;
    data10c = 0;
    flag1e0_5 = 0;
    flag1e0_6 = 0;
    data1bc = 0.0f;
    data1b8 = 0.0f;
    data1dc = -1;
    data104 = 0;
    flag1e0_0 = 0;
    datae8 = 0.5f;
    flag1e1_7 = 0;
    data168 = 0.0f;
    data164 = 0.0f;
    data160 = 0.0f;
    data1e4 = 0;
    flag1e1_6 = 0;
    flag1e1_5 = 0;
}
