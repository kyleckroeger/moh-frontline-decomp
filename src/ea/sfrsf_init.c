// The resampling filter's initialisation (sfrsf.c, 0x801674c4): the state's
// process function is SFILTER_rsf, its input and position words and two
// halfwords are cleared, the coefficient table is rsflc unless the second
// argument is set, and the four history samples are cleared. RSFSTATE and
// rsflc are named by the symbols; the field meanings are not established.
struct RSFSTATE {
    int (*process)(void*, int, void*, void*, int);
    int field04;
    unsigned char unknown08[20];
    int pitch;
    int field20;
    const float* table;
    short field28;
    short field2a;
    float history[4];
};

int SFILTER_rsf(void*, int, void*, void*, int);
extern const float rsflc[];

void SFILTER_rsfinit(RSFSTATE* state, int, int custom) {
    state->process = SFILTER_rsf;
    state->field04 = 0;
    state->field20 = 0;
    state->field28 = 0;
    state->field2a = 0;
    if (custom == 0)
        state->table = rsflc;
    state->history[0] = 0.0f;
    state->history[1] = 0.0f;
    state->history[2] = 0.0f;
    state->history[3] = 0.0f;
}
