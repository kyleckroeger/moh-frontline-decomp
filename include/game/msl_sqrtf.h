// sqrtf as MSL's math.h defines it for the Gekko: three Newton steps from frsqrte, always
// inlined. 33 matched game files carried this same definition; they share it from here.
#ifndef GAME_MSL_SQRTF_H
#define GAME_MSL_SQRTF_H

#ifndef _MATH_H_ // the SDK's math.h (include/dolphin-sdk/libc) has the same definition

extern inline float sqrtf(float x) {
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f) {
        double guess = __frsqrte((double)x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        y = (float)(x * guess);
        return y;
    }
    return x;
}
#endif

#endif
