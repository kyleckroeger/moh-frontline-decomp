// A fragment of projectilebullet.cpp (0x800d1a44): CProjectileBullet::InitClass
// (keeps the render list and screen in the file's statics and registers the
// bullet render bin, a file-local object owned by the projectilebullet unit,
// with the list), the CProjectileBullet
// destructor (its virtual table pointer, the two fiber volumes destroyed in
// reverse order, then CBullet's destructor, and the object freed when asked)
// and the default constructor (CBullet's constructor, the table pointer, the
// two fibers through their inline constructors, and the word at +40 set to 1).
// The file name is this project's; the original record is projectilebullet.cpp.
// The classes and functions are named by the mangled symbols; the members,
// their names and the space between them are inferred views, and only the
// virtuals these functions need are declared. CProjectileBullet declares
// MarkForDestruction (its first virtual, defined elsewhere) before its
// destructor, so its global virtual table is not emitted here. IVolume's
// destructor is inline (weak in the original), so the compiler's copies of it
// and of IVolume's weak virtual table are weak duplicates, linked to the
// original copies.
class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
};

/* inferred: a 16-byte vector and the fiber's line, as in fiber_dtor.cpp */
class CVector3 {
public:
    union {
        double pair[2];
        float v[4];
    };
} __attribute__((aligned(8)));

class CLine3 {
public:
    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
};

class CVolFiber : public IVolume {
public:
    CVolFiber() {}
    virtual IVolume* Create() const;
    virtual ~CVolFiber();

    unsigned char unknown04[12];
    CLine3 m_line;
};

class CBullet {
public:
    CBullet();
    virtual void MarkForDestruction(int);
    virtual ~CBullet();

    unsigned char unknown04[36];
    int data28;
};

class CRenderBin;
class CScreen;

class CRenderList {
public:
    void Register(CRenderBin&);
};

class CProjectileBullet : public CBullet {
public:
    static void InitClass(CRenderList*, CScreen*);
    virtual void MarkForDestruction(int);
    virtual ~CProjectileBullet();
    CProjectileBullet();

    unsigned char unknown2c[148];
    CVolFiber m_fiberA;
    CVolFiber m_fiberB;
};

static CRenderList* g_pRenderList;
static CScreen* g_pScreen;
extern CRenderBin g_BulletBin; // file-local in the original

void CProjectileBullet::InitClass(CRenderList* list, CScreen* screen) {
    g_pRenderList = list;
    g_pScreen = screen;
    list->Register(g_BulletBin);
}

CProjectileBullet::~CProjectileBullet() {
}

CProjectileBullet::CProjectileBullet() {
    data28 = 1;
}
