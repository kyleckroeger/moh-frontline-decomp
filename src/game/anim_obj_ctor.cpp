// A fragment of anim_obj.cpp (0x80095850): CAnimObject::Init (the skinned
// animation initialised for this object; three words cleared; the transform
// at +9024 and the 80 transforms reset to identity; three vectors set to zero
// through an inferred inline setter that stores z first; words, flags and a
// halfword reset; the collision list pointed at the object's own 128-record
// storage through dwi::fast_vec::reserve(T*, int) (the body of the original's
// weak copy at 0x800770a4), freeing owned storage first; a
// float cleared) and the CAnimObject default
// constructor (the scene-node base constructors: table pointers and eight
// cleared words; then the members in order: the skinned animation at +48,
// 80 transforms at +3904 through the CMatrix array wrapper, a transform at
// +9024, the collision list at +9272 (an empty dwi::fast_vec that owns its
// storage, growth -1; the list's constructor is inferred), a transform at
// +9312, the light-volume manager at +9400 and 128 collision records at
// +9432; then the animated volume pointer cleared) and the weak, empty
// CAnimObject::CClsnInfo constructor the record array uses, then the weak
// dwi::fast_vec<CAnimObject::CClsnInfo> destructor (frees owned storage) and
// the compiler's __defctor__7CMatrixFv array wrapper (CMatrix's inline
// constructor with its default argument), emitted after them. The file name is
// this project's; the original record is anim_obj.cpp. The classes and
// functions are named by the mangled symbols; the member positions come from
// the code (the transform at +9312 from the gap Init leaves), while their
// names, the record's contents and the space between members are inferred,
// and only the virtuals these functions need are declared. The 0.0f
// constant is an item of the file's .sdata2 pool, linked at its original
// address. CAnimObject
// declares MarkForDestruction (defined elsewhere) before its destructor so
// its global virtual table is not emitted here. CMatrix's default constructor
// takes an unknown default argument. The scene-node bases' weak tables and
// inline destructors are weak duplicates, linked to the original copies.
// Formerly also the anim_obj_fastvec unit.
extern "C" void MEM_free(void*);

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
    void reserve(T* storage, int capacity) {
        if (m_owned && m_data)
            MEM_free(m_data);
        m_data = storage;
        m_capacity = capacity;
        m_owned = false;
    }
    fast_vec() : m_data(0), m_unknown4(0), m_size(0), m_capacity(0), m_owned(true), m_growth(-1) {}
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

class CVector3 {
public:
    void Set(float nx, float ny, float nz) {
        z = nz;
        y = ny;
        x = nx;
    }

    float x, y, z, w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;

    void Ident();

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class CLightVolumeManager {
public:
    CLightVolumeManager();
    ~CLightVolumeManager();

    unsigned char unknown00[4];
};

class CAnimObject;
struct CharFrame_t;

class CAnimSkinned {
public:
    CAnimSkinned();
    void Init(CAnimObject*, unsigned short, CharFrame_t*);
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
    struct CClsnInfo {
        CClsnInfo() {}

        unsigned char unknown00[24];
    };
    virtual void MarkForDestruction(int);
    virtual ~CAnimObject();
    CAnimObject();
    void Init();

    int data24;
    int data28;
    int data2c;
    CAnimSkinned m_skinned;
    unsigned char unknown0060[3808];
    CMatrix m_matrices[80];
    CMatrix m_matrix2340;
    int data2380;
    int data2384;
    int data2388;
    int data238c;
    int data2390;
    int data2394;
    CAnimatedVolume* m_volume;
    unsigned char unknown239c[20];
    int data23b0;
    unsigned char unknown23b4[4];
    CVector3 m_vector23b8;
    CVector3 m_vector23c8;
    CVector3 m_vector23d8;
    unsigned char unknown23e8[80];
    dwi::fast_vec<CClsnInfo> m_collisions;
    unsigned char unknown2450;
    unsigned char flag2451;
    unsigned char flag2452;
    unsigned char flag2453;
    unsigned char flag2454;
    unsigned char flag2455;
    unsigned char flag2456;
    unsigned char flag2457;
    unsigned char unknown2458;
    unsigned char flag2459;
    unsigned char flag245a;
    unsigned char flag245b;
    unsigned char unknown245c[4];
    CMatrix m_matrix2460;
    unsigned char flag24a0;
    unsigned char unknown24a1[15];
    unsigned char flag24b0;
    unsigned char flag24b1;
    unsigned char flag24b2;
    unsigned char flag24b3;
    unsigned char unknown24b4;
    unsigned char flag24b5;
    unsigned char unknown24b6[2];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknown24bc[20];
    short data24d0;
    unsigned char unknown24d2[6];
    CClsnInfo m_collisionStorage[128];
    float data30d8;
};

void CAnimObject::Init() {
    m_skinned.Init(this, 0, 0);
    data24 = 0;
    data28 = 0;
    data2c = 0;
    m_matrix2340.Ident();
    m_vector23c8.Set(0.0f, 0.0f, 0.0f);
    m_vector23b8.Set(0.0f, 0.0f, 0.0f);
    m_vector23d8.Set(0.0f, 0.0f, 0.0f);
    data23b0 = 0;
    for (int i = 0; i < 80; i++)
        m_matrices[i].Ident();
    data2380 = 0;
    data2384 = 0;
    data2388 = 0;
    data238c = 0;
    data2394 = -1;
    m_collisions.reserve(m_collisionStorage, 128);
    data2390 = 0;
    flag245a = 0;
    flag245b = 0;
    flag2459 = 0;
    flag24b1 = 0;
    flag2452 = 1;
    flag2453 = 0;
    flag24b2 = 1;
    flag24b3 = 1;
    flag2455 = 0;
    flag2456 = 0;
    flag2454 = 1;
    flag24a0 = 0;
    flag24b0 = 0;
    flag2451 = 0;
    data24d0 = 1;
    flag24b5 = 0;
    data30d8 = 0.0f;
}

CAnimObject::CAnimObject() {
    m_volume = 0;
}
