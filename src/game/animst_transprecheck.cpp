// A fragment of animst.cpp (0x80067b5c): AnimStTransPreCheck finds the
// current state (as AnimStGetCurrentState, inlined), and unless there is
// none or it is the target state, looks up the animation lists of both
// states in g_AnimDB and quick-scans the transition between them. The
// functions, globals and structs are named by the symbols; the slot view and
// AnimStGetCurrentState's body follow animst.cpp (marked inline here so the
// unit inlines it without emitting it; it is an ordinary function earlier in
// the original file), and the object's id at +2, the list's value read at
// +8 and the result type are inferred. _AnimSt_pStatAnimList is file-local
// in the original and declared extern here. Then AnimStInitStructure (the
// state and status of the owner's four slots cleared), the setters of the
// finish callback and the callback table (file-local in the original,
// declared extern here; their types are opaque here), AnimStShutdown (the
// callback table cleared when set, 0 returned) and AnimStInit (returns 0).
// The rest of the file is in other units.
struct AnimStateSlotView {
    unsigned char field00[4];
    unsigned short state;
    unsigned short status;
    unsigned char field08[96];
};

struct AnimStateOwnerView {
    AnimStateSlotView slots[4];
};

struct AnimObjHdr_t {
    unsigned short field00;
    unsigned short id;
};

/* inferred: the list's value read at +8 */
struct AnimFileStateAnimList_t {
    unsigned char field00[8];
    unsigned short value08;
    unsigned char field0A[0xbbc - 10];
};

class CAnimDatabase {
public:
    void GetStateAnimList(unsigned long, unsigned long, AnimFileStateAnimList_t*);

    unsigned char unknown00[176];
};

extern CAnimDatabase g_AnimDB;
extern AnimFileStateAnimList_t _AnimSt_pStatAnimList; // file-local in the original

int _AnimTranQuickScan(AnimObjHdr_t*, unsigned short, unsigned short, unsigned short, unsigned short);

extern "C" inline unsigned short AnimStGetCurrentState(AnimStateOwnerView* owner) {
    unsigned short current = 0xFFFF;

    for (int i = 3; i >= 0; i--) {
        if (owner->slots[i].status == 3)
            current = owner->slots[i].state;
    }
    return current;
}

extern "C" int AnimStTransPreCheck(AnimStateOwnerView* owner, AnimObjHdr_t* object, unsigned short target) {
    unsigned short current = AnimStGetCurrentState(owner);
    if (current == 0xFFFF || current == target)
        return -1;
    g_AnimDB.GetStateAnimList(object->id, current, &_AnimSt_pStatAnimList);
    unsigned short currentCount = _AnimSt_pStatAnimList.value08;
    g_AnimDB.GetStateAnimList(object->id, target, &_AnimSt_pStatAnimList);
    return _AnimTranQuickScan(object, target, _AnimSt_pStatAnimList.value08, current, currentCount);
}

extern "C" void AnimStInitStructure(AnimStateOwnerView* owner) {
    owner->slots[0].state = 0;
    owner->slots[0].status = 0;
    owner->slots[1].state = 0;
    owner->slots[1].status = 0;
    owner->slots[2].state = 0;
    owner->slots[2].status = 0;
    owner->slots[3].state = 0;
    owner->slots[3].status = 0;
}

typedef void* AnimStFinishCallback;
typedef void* AnimStCallbackTable;

extern AnimStFinishCallback _AnimSt_FinishCb;     // file-local in the original
extern AnimStCallbackTable _AnimSt_CallbackTable; // file-local in the original

extern "C" void AnimStRegisterFinishCallback(AnimStFinishCallback callback) {
    _AnimSt_FinishCb = callback;
}

extern "C" void AnimStSetCallbackTable(AnimStCallbackTable table) {
    _AnimSt_CallbackTable = table;
}

extern "C" int AnimStShutdown() {
    if (_AnimSt_CallbackTable)
        _AnimSt_CallbackTable = 0;
    return 0;
}

extern "C" int AnimStInit() {
    return 0;
}
