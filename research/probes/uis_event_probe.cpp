// settings: config/GMFE69/IStudio.json
// replaces: iSirenEventProcessFnc__7IStudioFv
//
// Test harness (dol4), not game code: IStudio::iSirenEventProcessFnc (src/game/IStudio.cpp)
// with a log line per studio event sent ("UISEV controller event value").
extern "C" void OSReport(const char*, ...);
struct UISInfo_t;
extern "C" int UISProcessEvent(UISInfo_t*, unsigned long, int, int, float*, int);
#include "game/CInputManager.h"
extern CInputManager g_inputMgr;
extern int g_PauseScreenLockedController;

class IStudio {
public:
    void iSirenEventProcessFnc();
    UISInfo_t* m_info;
};

void IStudio::iSirenEventProcessFnc() {
    float value;
    for (int i = 0; i < 4; i++) {
        if (g_PauseScreenLockedController != -1 && i != g_PauseScreenLockedController)
            continue;
        for (int e = 0; e < 20; e++) {
            float v = g_inputMgr.GetValue(i, 121 + e);
            if (v) {
                value = e < 16 ? 1.0f : v;
                OSReport("UISEV %d %d %d\n", i, e, (int)(v * 100.0f));
                UISProcessEvent(m_info, i + 1, e, 1, &value, 0);
            }
        }
    }
}
