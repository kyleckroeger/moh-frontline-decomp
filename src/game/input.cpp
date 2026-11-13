// The end of input.cpp (0x80085210): the weak MathFunClamp<long> instance
// (the template as in player.cpp), then the static initialisation: the global input
// manager is built through its device manager's constructor and its
// destructor registered, then the weak CInputManager destructor (the device
// manager destroyed as a member), emitted for the registration. The rest of the file is in other units. g_inputMgr,
// CInputManager and CDeviceManager are named by the symbols; the object size
// comes from the symbol; the device manager is a member (its destructor is
// called for a complete object), and the rest of the layout is opaque here.
template <class T> __declspec(weak) T MathFunClamp(T value, T low, T high) {
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

template long MathFunClamp<long>(long, long, long);



#include "game/CInputManager.h"


CInputManager g_inputMgr;
