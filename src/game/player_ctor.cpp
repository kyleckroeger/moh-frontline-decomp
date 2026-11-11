// A fragment of player.cpp (0x800a7528): the CPlayerObject default
// constructor. Base chain (table pointers, eight cleared words); then the
// members in order: the AI doodad at +36, two transforms, two boxes and two
// capsules (inline constructors), an empty owning collision list at +680, the
// hearing volume at +704, a word at +1084, three transforms, a word at +1780,
// a transform, the player script object at +1904 (BSGO_Player over
// BSGO_Basic, whose three words precede its table pointer, as in
// thrown_obj_ctor.cpp), the camera at +2000 (its whole constructor inline:
// scene-node bases, two identity transforms and two more, three flags, ten
// planes through the array constructor, two cleared words) and the user
// interface at +2672. The body gives the collision list 128 records from
// DWI_allocalign (freeing owned storage first) and resets the script
// object's fields. The file name is this project's; the original record is
// player.cpp. The classes and functions are named by the mangled symbols; the
// member positions come from the code, while their names, the class sizes,
// the space between members and the inline helpers (the list's Allocate, the
// script object's Reset) are inferred; CMatrix's default argument is taken to
// be an identity flag because the camera's first two transforms are reset to
// identity right after their class initialisation. Only the virtuals the
// constructor needs are declared; each class declares one of its own virtual
// functions first so its global virtual table is not emitted here. The
// compiler's copies of the scene-node and IVolume weak tables and inline
// destructors, CPlane's weak constructor and destructor and the collision
// list's weak destructor (kept right after this constructor in the original)
// are weak duplicates, linked to the original copies.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
class CDrawContext;
struct BSObject;

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

namespace dwi {
template <class T>
class fast_vec {
public:
    fast_vec() : m_data(0), m_unknown4(0), m_size(0), m_capacity(0), m_owned(true), m_growth(-1) {}
    void Allocate(int capacity) {
        if (m_owned && m_data)
            MEM_free(m_data);
        m_data = (T*)DWI_allocalign(0, capacity * sizeof(T), 16, 1024);
        m_capacity = capacity;
        m_owned = true;
    }
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

class CAnimObject;
class CPlayerWeaponObject {
public:
    void Shutdown();
};

class CAnimObjectFactory {
public:
    void ReleaseObject(CAnimObject*);

    unsigned char data[16];
};

extern CAnimObjectFactory g_AnimObjFactory;

#include "game/CVector3.h"

class CMatrix {
public:
    /* The default argument's meaning is inferred: where it is non-zero the
       matrix is reset to identity straight after the class initialisation. */
    CMatrix(int identity = 0) {
        if (!s_ClassInit)
            InitClass();
        if (identity)
            Ident();
    }
    static void InitClass();
    void Ident();
    static bool s_ClassInit;

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual IVolume* Create() const;
    virtual ~CVolBox();

    unsigned char unknown04[76];
} __attribute__((aligned(8)));

class CVolCapsule : public IVolume {
public:
    CVolCapsule() {}
    virtual IVolume* Create() const;
    virtual ~CVolCapsule();

    unsigned char unknown04[76];
} __attribute__((aligned(8)));

class CAIPlayerDoodad {
public:
    CAIPlayerDoodad();
    ~CAIPlayerDoodad();

    unsigned char unknown00[108];
};

class CHearingVolume {
public:
    CHearingVolume();
    virtual ~CHearingVolume();

    unsigned char unknown004[196];
} __attribute__((aligned(8)));

class CPlane {
public:
    CPlane() {}
    ~CPlane() {}

    unsigned char unknown00[32];
};

class CCamera : public IMovingSceneNode {
public:
    CCamera() : m_matrix40(1), m_matrix80(1), data141(true), data142(false), data143(false), m_script(0), data294(0) {}
    virtual void Draw(CDrawContext&); /* result type not known */
    virtual ~CCamera();

    unsigned char unknown24[28];
    CMatrix m_matrix40;
    CMatrix m_matrix80;
    CMatrix m_matrixc0;
    CMatrix m_matrix100;
    unsigned char unknown140;
    bool data141;
    bool data142;
    bool data143;
    unsigned char unknown144[12];
    CPlane m_planes[10];
    BSObject* m_script;
    int data294;
    unsigned char unknown298[8];
};

class BSGO_Basic {
    int m_field0;
    int m_field4;
    int m_field8;

public:
    BSGO_Basic() : m_field0(0), m_field4(0), m_field8(0) {}
    virtual void Destroy();

    void ResetFields() {
        m_field0 = 0;
        m_field4 = 0;
        m_field8 = 0;
    }
};

class BSGO_Player : public BSGO_Basic {
public:
    BSGO_Player() : data2c(0) {}
    virtual void Destroy();

    void Reset() {
        data2c = 0;
        ResetFields();
    }

    unsigned char unknown10[28];
    int data2c;
};

class UserInterface {
public:
    UserInterface();
    ~UserInterface();

    unsigned char unknown00[4];
};

class CPlayerObject : public IMovingSceneNode {
public:
    struct CClsnInfo {
        unsigned char unknown00[24];
    };

    virtual void Destroy();
    virtual ~CPlayerObject();
    CPlayerObject();

    CAIPlayerDoodad m_doodad;
    CMatrix m_matrix90;
    CMatrix m_matrixd0;
    CVolBox m_boxA;
    CVolBox m_boxB;
    CVolCapsule m_capsuleA;
    CVolCapsule m_capsuleB;
    unsigned char unknown250[88];
    dwi::fast_vec<CClsnInfo> m_collisions;
    CHearingVolume m_hearing;
    unsigned char unknown388[180];
    int data43c;
    unsigned char unknown440[32];
    CPlayerWeaponObject* m_weapons[45];
    unsigned char unknown514[4];
    CMatrix m_matrix518;
    CMatrix m_matrix558;
    CMatrix m_matrix598;
    unsigned char unknown5d8[284];
    int data6f4;
    CMatrix m_matrix6f8;
    unsigned char unknown738[56];
    BSGO_Player m_scriptObject;
    unsigned char unknown7b0[48];
    CCamera m_camera;
    UserInterface m_ui;
};

CPlayerObject::CPlayerObject() : data43c(0), data6f4(0) {
    m_collisions.Allocate(128);
    m_scriptObject.Reset();
}
