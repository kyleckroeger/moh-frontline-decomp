// The end of animmorph.cpp (0x800655c4): the CAnimMorph constructor, the weak
// empty CVector4 and AnimChannel_t constructors (emitted for the array
// constructions), and the static initialisation of the morph animation
// matrix cache (constructed with 16, 384 and 8, as the skinned one in
// animskinned.cpp; the meaning of the arguments is not known). The
// constructor builds the CAnimated part as in animskinned.cpp (its table, 16
// animation channels, two matrices whose inline constructors initialise the
// matrix class once, and two vector arrays), then sets its own table. The
// rest of the file is in other units. The classes, functions and globals are
// named by the symbols; the cache's size comes from its symbol, while the
// members and their offsets are the inferred view of animskinned.cpp. The
// classes declare UpdateAnimation (defined elsewhere) first so that their
// tables stay elsewhere; CAnimated's inline destructor (exception clean-up)
// is a weak duplicate.
#include "game/CMatrix.h"


class CVector4 {
public:
    CVector4() {}

    float x;
    float y;
    float z;
    float w;
};

struct AnimChannel_t {
    AnimChannel_t() {}

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

class CAnimMorph : public CAnimated {
public:
    virtual void UpdateAnimation(float); /* result type not known */
    CAnimMorph();
    virtual ~CAnimMorph();
};

class CAnimMatrixCache {
public:
    CAnimMatrixCache(int, int, int);

    unsigned char unknown00[36];
};

CAnimMorph::CAnimMorph() {
}

CAnimMatrixCache g_animMorphCache(16, 384, 8);
