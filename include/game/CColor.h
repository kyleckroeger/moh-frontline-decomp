// CColor: an 8-bit RGBA colour, shared by every file that uses it (PortReadiness step 3.3).
//
// Evidence: the mangled names take it by const reference (SetClearColor__7CScreenFRC6CColor);
// the layout is four bytes r, g, b, a (layout lint: the PopUpMessageHandler constructor stores
// the four bytes one by one and copies the whole colour as one word, so the copy is the
// implicit one). Inferred: the member names and the two inline constructors (src/game/Screen.cpp,
// the matched unit that builds CScreen's clear colour).
#ifndef GAME_CCOLOR_H
#define GAME_CCOLOR_H

struct CColor {
    CColor() {}
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

#endif
