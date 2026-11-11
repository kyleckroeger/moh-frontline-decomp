// A fragment of Soldier_object.cpp (0x800ae72c): the CSoldierObject default
// constructor (CAnimObject's constructor, the table pointer, then the members
// in order: the AI soldier doodad at +12512, the morph animation at +12656,
// two transforms, an array of eight transforms at +16512 and another
// transform (class initialisation only), four sphere volumes at +17184 to
// +17280 through their inline constructors, the character shadow at +17360
// and the jaw controller at +17472). The file name is this project's; the
// original record is Soldier_object.cpp. The classes and functions are named
// by the mangled symbols; the member positions come from the code, while
// their names, the sizes of the classes and the space between them (where
// the two transforms after the morph lie is not known) are inferred, and only
// the virtuals the constructor needs are declared. CSoldierObject declares
// Destroy (its first own virtual, defined elsewhere) first so its global
// virtual table is not emitted here. The compiler's weak array constructor
// for CMatrix (__defctor__7CMatrixFv) and its copies of IVolume's inline destructor and weak table are
// weak duplicates, linked to the original copies.
#include "game/CVector3.h"

class CMatrix {
public:
    /* The original's default constructor takes a default argument (the
       compiler made the __defctor wrapper for the array); what it is, is not
       known, and it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
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

class CVolSphere : public IVolume {
public:
    CVolSphere() {}
    virtual IVolume* Create() const;
    virtual ~CVolSphere();

    unsigned char unknown04[28];
};

class CAnimObject {
public:
    CAnimObject();
    virtual ~CAnimObject();

    unsigned char unknown0004[12508];
};

class CAISoldierDoodad {
public:
    CAISoldierDoodad();
    ~CAISoldierDoodad();

    unsigned char unknown00[144];
};

class CAnimMorph {
public:
    CAnimMorph();
    ~CAnimMorph();

    unsigned char unknown00[16];
};

class CCharacterShadow {
public:
    CCharacterShadow();
    ~CCharacterShadow();

    unsigned char unknown00[112];
};

class CJawController {
public:
    CJawController();

    unsigned char unknown00[4];
};

class CSoldierObject : public CAnimObject {
public:
    virtual void Destroy();
    virtual ~CSoldierObject();
    CSoldierObject();

    CAISoldierDoodad m_doodad;
    CAnimMorph m_morph;
    CMatrix m_matrixA;
    CMatrix m_matrixB;
    unsigned char unknown3200[3712];
    CMatrix m_matrices[8];
    CMatrix m_matrixC;
    unsigned char unknown4340[96];
    CVolSphere m_sphereA;
    CVolSphere m_sphereB;
    CVolSphere m_sphereC;
    CVolSphere m_sphereD;
    unsigned char unknown43a0[48];
    CCharacterShadow m_shadow;
    CJawController m_jaw;
};

CSoldierObject::CSoldierObject() {
}
