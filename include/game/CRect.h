// CRect: an integer rectangle, shared by every file that uses it (PortReadiness step 3.3).
//
// Evidence: SetClearRect__7CScreenFRC5CRect; four words, set together by CScreen's constructor.
// Inferred: the member names and the inline constructors (src/game/Screen.cpp).
#ifndef GAME_CRECT_H
#define GAME_CRECT_H

struct CRect {
    CRect() {}
    CRect(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
    int left;
    int top;
    int right;
    int bottom;
};

#endif
