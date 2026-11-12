// anim_callback.cpp: its initialised data (.sdata, .rodata). _AnimObj_StateDef is the
// state definition handed to AnimStSetCallbackTable: the state count (9) and the table of
// state-select functions (research/data/bss/anim_callback.cpp); its layout is inferred.
// _floco_anim_ranges are ten animation index bounds. Both are file statics in the
// original, defined global here because the file's fragments reach them with extern.
// Checked by research/tools/check_data.py.
// settings: config/GMFE69/anim_callback_init.json

typedef int (*AnimStateSelFunc)(); // view: the real signature is in anim_callback_init.cpp

struct AnimStateSelView { // view from src/game/anim_callback_init.cpp
    int state;
    AnimStateSelFunc callback;
};

extern AnimStateSelView _AnimObj_StateSelFuncArray[9];

struct AnimStateDefView { // inferred layout
    int count;
    AnimStateSelView* table;
};

AnimStateDefView _AnimObj_StateDef = {9, _AnimObj_StateSelFuncArray}; // file-local in the original

extern const signed char _floco_anim_ranges[10];
const signed char _floco_anim_ranges[10] = {0, 3, 6, 9, 12, 14, 16, 18, 20, 25}; // file-local in the original
