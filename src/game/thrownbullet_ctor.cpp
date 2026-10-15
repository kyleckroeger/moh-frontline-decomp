// A fragment of thrownbullet.cpp (0x800d4de0): the CThrownBullet destructor
// (the table pointer, the two fiber volumes and the light-volume manager
// destroyed, then CBullet's destructor, and the object freed when asked) and
// the default constructor (CBullet's constructor; the table pointer; the dummy script
// object at +176 (its three words cleared before its table pointer, as in
// thrown_obj_ctor.cpp); the light-volume manager at +212; two fiber volumes at
// +336 and +416 through their inline constructors; then 2 stored at +40). The
// file name is this project's; the original record is thrownbullet.cpp. The
// classes and functions are named by the mangled symbols; the members and
// their names are inferred, and only the virtuals the constructor needs are
// declared. CThrownBullet declares MarkForDestruction (its own override,
// defined elsewhere) first so its global virtual table is not emitted here.
// IVolume's destructor is inline and its table weak in the original, so the
// compiler's copies are weak duplicates, linked to the original copies.
class BSGO_Basic {
    int m_field0;
    int m_field4;
    int m_field8;

public:
    BSGO_Basic() : m_field0(0), m_field4(0), m_field8(0) {}
    virtual void unknown08();
};

class BSGO_Dummy : public BSGO_Basic {
public:
    virtual void unknown08();
};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume() {}
};

class CVolFiber : public IVolume {
public:
    CVolFiber() {}
    virtual IVolume* Create() const;
    virtual ~CVolFiber();

    unsigned char unknown04[76];
} __attribute__((aligned(8)));

class CLightVolumeManager {
public:
    CLightVolumeManager();
    ~CLightVolumeManager();

    unsigned char unknown00[4];
};

class CBullet {
public:
    CBullet();
    virtual void MarkForDestruction(int);
    virtual ~CBullet();

    unsigned char unknown04[36];
    int data28;
    unsigned char unknown2c[132];
};

class CThrownBullet : public CBullet {
public:
    virtual void MarkForDestruction(int);
    virtual ~CThrownBullet();
    CThrownBullet();

    BSGO_Dummy m_scriptObject;
    unsigned char unknownc0[20];
    CLightVolumeManager m_lightVolumes;
    unsigned char unknownd8[120];
    CVolFiber m_fiberA;
    CVolFiber m_fiberB;
};

CThrownBullet::~CThrownBullet() {
}

CThrownBullet::CThrownBullet() {
    data28 = 2;
}
