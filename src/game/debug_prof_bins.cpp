// A fragment of debug_prof.cpp (0x800f451c): the profiling history and debug statistics
// render bins' Render functions, which both ask the screen for its width and height and
// return 0. CDbgStatsRenderBin::Render is that class's key function (its destructor is
// inline, weak in the original), so this unit compiles with RTTI on and also owns
// debug_prof.cpp's CDbgStatsRenderBin virtual table and RTTI records, as the original file
// did, while the rest of the file stays in its other fragments (a stepping stone, accepted
// 2026-11-13: docs/Remaining.md). The file name is this project's; the original record is
// debug_prof.cpp.
//
// CRenderBin, CProfHistRenderBin, CDbgStatsRenderBin and CDmaPacket are named by the
// symbols; the classes and their slots follow __vt__18CDbgStatsRenderBin (CRenderBin's Init,
// Link and IsUsed), as in debug_prof_register.cpp. CProfHistRenderBin declares its destructor
// (defined elsewhere, debug_prof_cprofhistrenderbin_dtor.cpp) so its table, which
// debug_prof_register.cpp places, is not emitted here. The compiled copy of
// CDbgStatsRenderBin's inline destructor is a file duplicate of
// debug_prof_cdbgstatsrenderbin_dtor.cpp's, and those of CRenderBinData's and CRenderBin's
// RTTI records (with CRenderBin's base list) and their names are file duplicates of
// debug_prof_register.cpp's (the names link at their .rodata pool addresses).
class CColor;
class CRect;

#include "game/CScreen.h"

class CDmaTag;
class CDmaPacket;

class CRenderBinData {
    unsigned char unknown00[28];
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CProfHistRenderBin : public CRenderBin {
public:
    virtual ~CProfHistRenderBin();
    virtual int Render(CDmaPacket&, void*);
};

class CDbgStatsRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
};

extern CScreen* g_pScreen;

int CProfHistRenderBin::Render(CDmaPacket&, void*) {
    g_pScreen->GetWidth();
    g_pScreen->GetHeight();
    return 0;
}

int CDbgStatsRenderBin::Render(CDmaPacket&, void*) {
    g_pScreen->GetWidth();
    g_pScreen->GetHeight();
    return 0;
}
