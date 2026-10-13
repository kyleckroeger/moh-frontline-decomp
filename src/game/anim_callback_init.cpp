// A fragment of anim_callback.cpp (0x800608b4): AnimCallback_Init, which
// fills the file's state-selection table (nine state ids with their
// CAnimCallbacks selection callbacks, pathing_cb serving two states), passes
// the file's state definition to the animation system and registers the
// finish callback. The table and the definition are file-local in the
// original. The callbacks are named by the mangled symbols; the table entry
// layout (id, callback) and the definition view are inferred, and the
// callback result types are not known.
struct AnimFileStateAnimList_t;
struct AnimObjHdr_t;
struct AnimChannel_t;

class CAnimCallbacks {
public:
    static int pathing_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int random_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int turn_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int death_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int weapon_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int aim_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int face_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int floco_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static void animfinish_cb(unsigned short, void*); /* result type not known */
};

typedef int (*AnimStateSelFunc)(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);

// Inferred: a state id and its selection callback (8 bytes each, nine
// entries); the 8-byte state definition passed to the animation system.
struct AnimStateSelView {
    int state;
    AnimStateSelFunc callback;
};

struct AnimStateDefView {
    unsigned char unknown00[8];
};

extern AnimStateSelView _AnimObj_StateSelFuncArray[9];
extern AnimStateDefView _AnimObj_StateDef;

extern "C" void AnimStSetCallbackTable(AnimStateDefView*);
extern "C" void AnimStRegisterFinishCallback(void (*)(unsigned short, void*));

void AnimCallback_Init() {
    _AnimObj_StateSelFuncArray[0].state = 1000;
    _AnimObj_StateSelFuncArray[0].callback = CAnimCallbacks::pathing_cb;
    _AnimObj_StateSelFuncArray[1].state = 1100;
    _AnimObj_StateSelFuncArray[1].callback = CAnimCallbacks::random_cb;
    _AnimObj_StateSelFuncArray[2].state = 1001;
    _AnimObj_StateSelFuncArray[2].callback = CAnimCallbacks::turn_cb;
    _AnimObj_StateSelFuncArray[3].state = 1200;
    _AnimObj_StateSelFuncArray[3].callback = CAnimCallbacks::death_cb;
    _AnimObj_StateSelFuncArray[4].state = 1300;
    _AnimObj_StateSelFuncArray[4].callback = CAnimCallbacks::weapon_cb;
    _AnimObj_StateSelFuncArray[5].state = 1002;
    _AnimObj_StateSelFuncArray[5].callback = CAnimCallbacks::aim_cb;
    _AnimObj_StateSelFuncArray[6].state = 1010;
    _AnimObj_StateSelFuncArray[6].callback = CAnimCallbacks::face_cb;
    _AnimObj_StateSelFuncArray[7].state = 1020;
    _AnimObj_StateSelFuncArray[7].callback = CAnimCallbacks::pathing_cb;
    _AnimObj_StateSelFuncArray[8].state = 1400;
    _AnimObj_StateSelFuncArray[8].callback = CAnimCallbacks::floco_cb;
    AnimStSetCallbackTable(&_AnimObj_StateDef);
    AnimStRegisterFinishCallback(CAnimCallbacks::animfinish_cb);
}
