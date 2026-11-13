// CInputManager: the game's input (key maps, per-player feedback switches) over the device
// manager, shared by every file that uses it (PortReadiness step 3.3).
//
// Evidence: out of line in the original: Init (input_init), Update (draft
// research/drafts/Update.80084c80.cpp), Reset, VerifyAnalog, IsReady, SetFeedback,
// ResetActuatorState, GetValueAnyPlayer, GetValue, SetKeymap, Shutdown (input_manager). The
// destructor is weak (the implicit one; input.cpp, which defines g_inputMgr: 0x2450 bytes, emits
// it). No virtuals.
// The device manager is a member at +0, not a base: the weak destructor destroys it with -1
// (a base would get 0), as input.cpp's matched view has it. input_init.cpp and input_manager.cpp
// view it as a base, which gives the same code for their calls. CInputManager's own fields
// start at +0x2430.
// Inferred: the member's name (m_deviceMgr), return types (the matched definitions';
// VerifyAnalog's bool is Moh2.cpp's, the one caller), field names and types (input_init.cpp, input_manager.cpp; m_zooming, one byte per
// player at +0x244b, and m_inverted at +0x244a are player_beginupdate.cpp's). Init clears
// +0x244a..+0x244e.
#ifndef GAME_CINPUTMANAGER_H
#define GAME_CINPUTMANAGER_H

#include "game/CDeviceManager.h"

struct KEYMAP;

class CInputManager {
public:
    void Init();
    void Update(float);
    void Reset();
    bool VerifyAnalog();
    bool IsReady();
    void SetFeedback(unsigned long, long, long, float, float);
    void ResetActuatorState(long);
    bool GetValueAnyPlayer(unsigned long);
    int GetValue(unsigned long, unsigned long);
    void SetKeymap(unsigned long, KEYMAP*);
    void Shutdown();

    CDeviceManager m_deviceMgr;
    KEYMAP* m_keymap;
    KEYMAP* m_playerKeymaps[4];
    unsigned char field2444;
    bool m_feedback;
    bool m_feedbackOff[4];
    bool m_inverted;
    unsigned char m_zooming[4];
};

#endif
