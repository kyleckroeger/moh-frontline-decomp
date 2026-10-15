// A fragment of weapon.cpp (0x800d64e8): the CWeapon destructor (the table
// pointer, then CStaticObject's destructor, and the object freed when asked),
// the CWeapon default constructor
// (CStaticObject's constructor; the table pointer; the dummy script object at
// +728 (its three words cleared before its table pointer, as in
// thrown_obj_ctor.cpp); then two halfwords cleared, 0.0f at +648, 1 at +652 and
// a cleared word at +640) and CWeapon's array operator new (DWI_alloc with the
// given name, the size and the 1024 flag). The file name is this project's; the original
// record is weapon.cpp. The classes and functions are named by the mangled
// symbols; the members and their names are inferred, and only the virtuals the
// constructor needs are declared. CWeapon declares BeginUpdate (its own
// override, defined elsewhere) first so its global virtual table is not
// emitted here. The 0.0f constant is an item of the file's .sdata2 pool.
void* DWI_alloc(const char*, int, int);

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

class CStaticObject {
public:
    CStaticObject();
    virtual ~CStaticObject();

    unsigned char unknown004[636];
};

class CWeapon : public CStaticObject {
public:
    virtual void BeginUpdate(float);
    virtual ~CWeapon();
    CWeapon();
    static void* operator new[](unsigned long, char*);

    int data280;
    short data284;
    short data286;
    float data288;
    int data28c;
    unsigned char unknown290[72];
    BSGO_Dummy m_scriptObject;
};

CWeapon::~CWeapon() {
}

CWeapon::CWeapon() {
    data284 = 0;
    data286 = 0;
    data288 = 0.0f;
    data28c = 1;
    data280 = 0;
}

void* CWeapon::operator new[](unsigned long size, char* name) {
    return DWI_alloc(name, size, 1024);
}
