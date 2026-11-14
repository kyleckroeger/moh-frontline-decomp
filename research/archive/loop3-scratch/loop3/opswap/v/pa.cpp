// loop knots2 (2026-11-12): 88 -> 82 real lines with all locals hoisted to the top in a hill-climbed
// order (scratch/loop3/knots2/pu/hc.py); i now in r29 as the original. Left: hist/IV/pointer colouring.
// Earlier: best WIP (harvest1, 2026-11-12): 88 real lines (was 113; index said 196), from
// scratch/loop3/struct1/v/pcube.cpp: 'move = move * (move * move); g_fGraphCursor += move;' (unfused,
// matches the original's length). Left: registers.
// A fragment of debug_prof.cpp (0x800f34f0): the SProfHistBin constructor
// (six halfwords set to -1, then 300 profile bins built by __construct_array)
// and the weak SProfileBin constructor, which sets the first word to -1 and
// clears the five after it. SProfHistBin and SProfileBin are named by the
// mangled symbols; their members are inferred. The SProfileBin constructor is a
// weak copy of a header inline emitted in this file, so it is defined
// __declspec(weak). The rest of the file is not part of this unit.
struct SProfileBin {
    SProfileBin();

    int m_id;
    int m_values[5];
};

struct SProfHistBin {
    SProfHistBin();

    short m_value0;
    short m_value2;
    short m_value4;
    short m_value6;
    short m_value8;
    short m_valueA;
    SProfileBin m_bins[300];
};

SProfHistBin::SProfHistBin()
    : m_value0(-1), m_value2(-1), m_value4(-1), m_value6(-1), m_value8(-1), m_valueA(-1) {
}

__declspec(weak) SProfileBin::SProfileBin() : m_id(-1) {
    m_values[0] = 0;
    m_values[1] = 0;
    m_values[2] = 0;
    m_values[3] = 0;
    m_values[4] = 0;
}

#include "game/CInputManager.h"


extern CInputManager g_inputMgr;
extern SProfileBin g_ProfileBins[31];
extern SProfHistBin g_ProfileBinsHistory[31];
extern bool g_bProfileUpdateHistory;
extern int g_HistoryCursor;
extern int g_GraphCursor;
extern float g_fGraphCursor;
extern int g_ProfileBinTOP;
extern int g_ProfileBinSelect;

void PROFILE_UPDATE() {
    short* min1;
    short* max2;
    int k;
    short* min2;
    SProfHistBin* hist;
    short* min3;
    int j;
    short* max1;
    int* value;
    short* max3;
    int i;

    if (g_bProfileUpdateHistory)
        g_HistoryCursor = (g_HistoryCursor + 1) % 300;
    for (i = 0; i < 31; i++) {
        hist = &g_ProfileBinsHistory[i];
        hist->m_bins[g_HistoryCursor] = g_ProfileBins[i];
        min1 = &hist->m_value0;
        max1 = &hist->m_value2;
        min2 = &hist->m_value4;
        max2 = &hist->m_value6;
        min3 = &hist->m_value8;
        max3 = &hist->m_valueA;
        *min1 = g_HistoryCursor;
        *max1 = g_HistoryCursor;
        *min2 = g_HistoryCursor;
        *max2 = g_HistoryCursor;
        *min3 = g_HistoryCursor;
        *max3 = g_HistoryCursor;
        j = (g_HistoryCursor + 1) % 300;
        for (k = 0; k < 300; k++) {
            value = &hist->m_bins[j].m_values[1];
            if (*value < hist->m_bins[*min1].m_values[1])
                *min1 = j;
            if (*value > hist->m_bins[*max1].m_values[1])
                *max1 = j;
            value = &hist->m_bins[j].m_values[2];
            if (*value < hist->m_bins[*min2].m_values[2])
                *min2 = j;
            if (*value > hist->m_bins[*max2].m_values[2])
                *max2 = j;
            value = &hist->m_bins[j].m_values[3];
            if (*value < hist->m_bins[*min3].m_values[3])
                *min3 = j;
            if (*value > hist->m_bins[*max3].m_values[3])
                *max3 = j;
            j = (j + 1) % 300;
        }
        g_ProfileBins[i].m_values[4] = 0;
        g_ProfileBins[i].m_values[3] = 0;
        g_ProfileBins[i].m_values[2] = 0;
        g_ProfileBins[i].m_values[1] = 0;
    }
    if (g_inputMgr.GetValue(0, 123)) {
        if (--g_ProfileBinSelect < -1)
            g_ProfileBinSelect = -1;
    }
    if (g_inputMgr.GetValue(0, 124)) {
        if (++g_ProfileBinSelect >= 31)
            g_ProfileBinSelect = 30;
    }
    if (g_inputMgr.GetValue(0, 125)) {
        if (g_ProfileBinTOP != -1) {
            int parent = g_ProfileBins[g_ProfileBinTOP].m_id;
            if (parent != -1)
                g_ProfileBinTOP = parent;
            else
                g_ProfileBinTOP = -1;
        }
    }
    if (g_inputMgr.GetValue(0, 126))
        g_ProfileBinTOP = g_ProfileBinSelect;
    if (g_inputMgr.GetValue(0, 122))
        g_bProfileUpdateHistory = !g_bProfileUpdateHistory;
    if (!g_bProfileUpdateHistory) {
        float move = (float)g_inputMgr.GetValue(0, 137);
        move *= 0.0078125f;
        move = move * (move * move);
        g_fGraphCursor += move;
        if (g_fGraphCursor < 0.0f)
            g_fGraphCursor = 299.0f + g_fGraphCursor;
        else if (g_fGraphCursor >= 300.0f)
            g_fGraphCursor = g_fGraphCursor - 300.0f;
        g_GraphCursor = (int)g_fGraphCursor;
    }
}
