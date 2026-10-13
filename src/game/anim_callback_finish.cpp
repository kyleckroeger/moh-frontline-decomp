// A fragment of anim_callback.cpp (0x8005e9bc): CAnimCallbacks::animfinish_cb,
// the finish callback AnimCallback_Init registers: it passes the finished
// state to the animated object's FinishAnimationState and returns 0. The
// names come from the mangled symbols; the result types are inferred.
class CAnimated {
public:
    void FinishAnimationState(unsigned short); /* result type not known */
};

class CAnimCallbacks {
public:
    static int animfinish_cb(unsigned short, void*);
};

int CAnimCallbacks::animfinish_cb(unsigned short state, void* user) {
    ((CAnimated*)user)->FinishAnimationState(state);
    return 0;
}
