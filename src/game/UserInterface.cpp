// UserInterface's destructor: outside multiplayer it deletes the HUD sprites
// and the bullet-count font. UserInterface, CSprite, CFont and CRenderBin are
// named by the mangled symbols; the sprite and font pointers are the class's
// static members (defined with the rest of the file, not part of this unit).
// CRenderBin is an inferred view: 28 bytes of members, then its virtual table
// pointer, with the virtual destructor in the first slot. The constructor
// after it sets the HUD defaults (a counter of 100, unit scales, flags
// cleared, 4.5 timers; members inferred from offsets, constants from the
// file's .sdata2 pool), and the static initialiser builds the four team
// colours.
#include "game/CRenderBin.h"


class CSprite : public CRenderBin {
public:
    virtual ~CSprite();
};

class CFont : public CRenderBin {
public:
    virtual ~CFont();
};

extern bool g_bInMultiplayerMode;

// Inferred: a colour of four bytes built by an inline constructor.
class CColor {
public:
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class UserInterface {
public:
    ~UserInterface();
    UserInterface();

    unsigned char unknown000[8];
    float data008;
    float data00c;
    float data010;
    unsigned char unknown014[1936];
    unsigned char flag7a4_7 : 1;
    unsigned char flag7a4_6 : 1;
    unsigned char flag7a4_5 : 1;
    unsigned char flag7a4_4 : 1;
    unsigned char flag7a4_low : 4;
    unsigned char unknown7a5[23];
    int data7bc;
    float data7c0;
    float data7c4;
    float data7c8;
    float data7cc;
    unsigned char unknown7d0[4];
    int data7d4;
    int data7d8;
    float data7dc;
    float data7e0;
    float data7e4;
    unsigned char unknown7e8[4];
    float data7ec;
    bool data7f0;
    unsigned char unknown7f1[3];
    float data7f4;
    unsigned char unknown7f8[12];
    int data804;
    int data808;
    int data80c;
    int data810;
    bool data814;

    static CSprite* healthSprite;
    static CSprite* compassSprite;
    static CSprite* compassEdgeSprite;
    static CSprite* hitMeterSprite;
    static CSprite* blackSprite;
    static CFont* bulletText;
};

UserInterface::~UserInterface() {
    if (!g_bInMultiplayerMode) {
        delete healthSprite;
        delete compassSprite;
        delete hitMeterSprite;
        delete compassEdgeSprite;
        delete blackSprite;
        delete bulletText;
    }
}

UserInterface::UserInterface() {
    data7bc = 100;
    data008 = 0.0f;
    data00c = 1.0f;
    data010 = 0.0f;
    flag7a4_4 = 0;
    flag7a4_7 = 0;
    flag7a4_5 = 0;
    flag7a4_6 = 0;
    data7c0 = 1.0f;
    data7c4 = 0.0f;
    data7c8 = 1.0f;
    data7cc = 1.0f;
    data7d4 = 0;
    data7d8 = 0;
    data7dc = 4.5f;
    data7e0 = 4.5f;
    data7e4 = 4.5f;
    data7f4 = 0.0f;
    data7ec = 4.5f;
    data7f0 = true;
    data804 = 0;
    data808 = 0;
    data80c = 0;
    data810 = 0;
    data814 = false;
}

CColor teamColors[4] = {
    CColor(96, 32, 32, 128),
    CColor(96, 96, 0, 128),
    CColor(0, 96, 0, 128),
    CColor(32, 32, 128, 128),
};
