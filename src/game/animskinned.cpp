// The end of animskinned.cpp (0x80066e98): the CAnimSkinned constructor and
// the static initialisation of the skinned-animation matrix cache
// (constructed with 16, 384 and 192; the meaning of the arguments is not
// known). The constructor builds the CAnimated part (its table, 16 animation
// channels, two matrices whose inline constructors initialise the matrix
// class once, and two vector arrays), then sets its own table, a count of 80
// at +3280 and two pointers to its own storage. The rest of the file is in
// other units. The classes, functions and globals are named by the symbols;
// the object size comes from the symbol, while the members and their offsets
// are inferred from the constructor's stores and array constructions. The
// classes declare UpdateAnimation (defined elsewhere) first so that their
// tables stay elsewhere; CAnimated's inline destructor (exception clean-up)
// is a weak duplicate.
#include "game/CMatrix.h"


class CVector4 {
public:
    CVector4();

    float x;
    float y;
    float z;
    float w;
};

struct AnimChannel_t {
    AnimChannel_t();

    unsigned char unknown00[136];
};

class CAnimated {
public:
    virtual void UpdateAnimation(float); /* result type not known */
    virtual ~CAnimated() {}

    unsigned char unknown004[12];
    AnimChannel_t m_channels[16];
    unsigned char unknown890[16];
    CMatrix m_matrix0;
    CMatrix m_matrix1;
    unsigned char unknown920[504];
    CVector4 m_vectors0[3];
    CVector4 m_vectors1[4];
};

class CAnimSkinned : public CAnimated {
public:
    virtual void UpdateAnimation(float); /* result type not known */
    CAnimSkinned();
    virtual ~CAnimSkinned();

    unsigned char unknownb90[300];
    int* data0cbc;
    unsigned char unknown0cc0[16];
    int m_count;
    unsigned char unknown0cd4[52];
    void* data0d08;
    unsigned char unknown0d0c[4];
    unsigned char m_storage[4];
};

class CAnimMatrixCache {
public:
    CAnimMatrixCache(int, int, int);

    unsigned char unknown00[36];
};

CAnimSkinned::CAnimSkinned() {
    m_count = 80;
    data0d08 = m_storage;
    data0cbc = &m_count;
}

CAnimMatrixCache g_animSkinnedCache(16, 384, 192);
