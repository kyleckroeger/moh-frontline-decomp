// A fragment of Moh2.cpp (0x8001a108): SendRenderList kicks the global render
// list, timing it in profile bin 19 (the tick count subtracted before and
// added after, then the bin's call count incremented). The function, class
// and globals are named by the symbols; the profile-bin layout (as in
// debug_prof_profilebin_ctor.cpp) and the render-list view are inferred, and
// the bin index is not named. The rest of the file is not part of this unit.
extern "C" unsigned long OSGetTick();

struct SProfileBin {
    int m_id;
    int m_values[5];
};

class CRenderList {
public:
    void Kick();

    unsigned char unknown00[36];
};

extern SProfileBin g_ProfileBins[31];
extern CRenderList theRenderList;

void SendRenderList() {
    g_ProfileBins[19].m_values[1] -= OSGetTick();
    theRenderList.Kick();
    g_ProfileBins[19].m_values[1] += OSGetTick();
    g_ProfileBins[19].m_values[4]++;
}
