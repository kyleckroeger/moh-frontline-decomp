// A fragment of AIFilter.cpp (0x8005b408): the CAIFilterTankObject constructor,
// which places its tank AI object (a CAITankObject told its owner) in its own
// buffer at +20, rounded up to 16 bytes, through CAITankObject's placement
// operator new, and keeps it at +8. The file name is this project's; the
// original record is AIFilter.cpp. The classes and functions are named by the
// mangled symbols; the members follow AIFilter.cpp's view (whose first word is
// the table pointer), the buffer's size is inferred and the CAITankObject size
// comes from the allocation. CAIFilterObject's destructor is defined later in
// the original file, so it is only declared here (the constructor's cleanup
// calls it). Each class declares ChooseTarget (defined elsewhere; the result
// type is not known) first so the global virtual tables are not emitted here.
class CAIObject {
public:
    virtual ~CAIObject();
};

class CAITankObject : public CAIObject {
public:
    CAITankObject(void*);
    static void* operator new(unsigned long, void*);

    unsigned char data004[1956];
};

class CAIFilterObject {
public:
    CAIFilterObject() {}
    virtual void ChooseTarget(float); /* result type not known */
    virtual ~CAIFilterObject();

    void* m_gameObject;
    CAIObject* m_aiObject;
};

class CAIFilterTankObject : public CAIFilterObject {
public:
    CAIFilterTankObject();
    virtual void ChooseTarget(float); /* result type not known */
    virtual ~CAIFilterTankObject();

    unsigned char unknown0c[8];
    char m_aiBuffer[1976];
};

CAIFilterTankObject::CAIFilterTankObject() {
    char* buffer = m_aiBuffer;
    if ((unsigned long)buffer & 15)
        buffer = (char*)(((unsigned long)buffer & ~15) + 16);
    m_aiObject = new (buffer) CAITankObject(this);
}
