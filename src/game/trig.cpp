// Angle helpers on 24-bit fixed-point angles (one turn is 0x1000000) and the
// tables of Taylor coefficients filled in by MathLLAngleInit.
extern "C" {
double atan2(double, double);
double sin(double);
double cos(double);
double pow(double, double);
}

static float _Math_TaylorConst[8];
static float _Math_Taylor[9];
// The original .bss leaves 12 bytes before this table; inferred as alignment.
static float _Math_InvTaylor[9] __attribute__((aligned(16)));

long MathArcTan2(float y, float x) {
    return 2670176.8f * (float)atan2(y, x);
}

void MathSinCos(long angle, float* s, float* c) {
    float radians = 6.2831855f * angle / 16777216.0f;

    *s = sin(radians);
    *c = cos(radians);
}

float MathCosf(float x) {
    return cos(x);
}

float MathSinf(float x) {
    return sin(x);
}

void MathLLAngleInit() {
    int i = 0;
    _Math_TaylorConst[i] = 0.16666667f;
    _Math_TaylorConst[1] = 0.008333334f;
    _Math_TaylorConst[2] = 0.0001984127f;
    _Math_TaylorConst[3] = 2.7557319e-06f;
    _Math_TaylorConst[4] = 2.5052108e-08f;
    _Math_TaylorConst[5] = 1.6059044e-10f;
    _Math_TaylorConst[6] = 7.647164e-13f;
    _Math_Taylor[i] = -_Math_TaylorConst[i] * pow(3.1415927f, 3.0);
    _Math_Taylor[1] = _Math_TaylorConst[1] * pow(3.1415927f, 5.0);
    _Math_Taylor[2] = -_Math_TaylorConst[2] * pow(3.1415927f, 7.0);
    _Math_Taylor[3] = _Math_TaylorConst[3] * pow(3.1415927f, 9.0);
    _Math_Taylor[4] = -_Math_TaylorConst[4] * pow(3.1415927f, 11.0);
    _Math_Taylor[5] = _Math_TaylorConst[5] * pow(3.1415927f, 13.0);
    _Math_Taylor[6] = -_Math_TaylorConst[6] * pow(3.1415927f, 15.0);
    _Math_Taylor[7] = pow(3.1415927f, 1.0);
    _Math_Taylor[8] = 0.00390625f;
    _Math_InvTaylor[i] = 0.16666667f;
    _Math_InvTaylor[1] = 0.025f;
    _Math_InvTaylor[2] = 0.008928572f;
    _Math_InvTaylor[3] = 0.004340278f;
    _Math_InvTaylor[4] = 0.0024857954f;
    _Math_InvTaylor[5] = 0.001577524f;
    _Math_InvTaylor[6] = 0.0010742188f;
    _Math_InvTaylor[7] = 0.00077012007f;
    _Math_InvTaylor[8] = 1.0f;
}
