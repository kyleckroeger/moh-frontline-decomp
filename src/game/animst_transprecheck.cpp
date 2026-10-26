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
// callback table cleared when set, 0 returned), AnimStInit (returns 0) and
// _AnimStGetFreeSlotIndex: compacts the four slots, returns the first free
// one; when all are used it ends the first slot with a delay (callback with
// event 3, channels stopped) and retries, else ends slot 0 and returns 3.
// The slot record now starts at +4 of the owner (its first word is not
// used here), which lets the compaction copy whole slots; the compaction,
// callback and kill helpers are inferred inlines. The rest of the file is in
// other units.
struct AnimStateSlotView {
    unsigned short state;
    unsigned short status;
    unsigned short flags;
    unsigned char field06[2];
    float delay;
    unsigned char field0C[92];
};

struct AnimStateOwnerView {
    unsigned char field00[4];
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
    int GetStateCallbackID(unsigned long, unsigned short);

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

struct AnimChannel_t;
struct AnimStInfo_t : AnimStateOwnerView {
};

struct AnimStCallbackEntryView {
    unsigned long id;
    void* callback;
};

struct AnimStCallbackTableView {
    unsigned long count;
    AnimStCallbackEntryView* entries;
};

typedef int (*AnimStateSelFunc)(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
typedef void* AnimStFinishCallback;
typedef AnimStCallbackTableView* AnimStCallbackTable;

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

extern "C" void* memcpy(void*, const void*, unsigned long);
extern "C" void AnimChanStopByStateIndex(AnimObjHdr_t*, AnimChannel_t*, unsigned short);

/* inferred: moves the used slots down over the free ones */
inline void _AnimStCompactSlots(AnimStInfo_t* owner) {
    int last = 3;
    if (!owner->slots[3].status) {
        last = 2;
        if (!owner->slots[2].status) {
            last = 1;
            if (!owner->slots[1].status) {
                last = 0;
                if (!owner->slots[0].status)
                    last = -1;
            }
        }
    }
    if (last > 0) {
        int i = 0;
        while (i < last) {
            if (!owner->slots[i].status) {
                if (i != last) {
                    for (int k = i; k < last; k++) {
                        memcpy(&owner->slots[k], &owner->slots[k + 1], sizeof(AnimStateSlotView));
                        owner->slots[k + 1].status = 0;
                        owner->slots[k + 1].delay = 0.0f;
                    }
                }
                last--;
            } else {
                i++;
            }
        }
    }
}

/* inferred: runs a state's selection callback (skipped for flagged states) */
inline void _AnimStCallStateCallback(unsigned short state, AnimObjHdr_t* object, AnimChannel_t* channel, void* user, unsigned long event, void* extra, float frames) {
    unsigned short id = object->id;
    if (!(state & 0x8000)) {
        g_AnimDB.GetStateAnimList(id, state, &_AnimSt_pStatAnimList);
        int index = g_AnimDB.GetStateCallbackID(state, id);
        ((AnimStateSelFunc)_AnimSt_CallbackTable->entries[index].callback)(&_AnimSt_pStatAnimList, state, object, channel, user, event, extra, frames);
    }
}

/* inferred: ends the state in a slot and compacts the slots */
inline void _AnimStKillSlot(AnimStInfo_t* owner, int slot, AnimObjHdr_t* object, AnimChannel_t* channel, void* user) {
    _AnimStCallStateCallback(owner->slots[slot].state, object, channel, user, 3, 0, 1.0f);
    AnimChanStopByStateIndex(object, channel, owner->slots[slot].state);
    owner->slots[slot].status = 0;
    _AnimStCompactSlots(owner);
}

static unsigned short _AnimStGetFreeSlotIndex(AnimStInfo_t* owner, AnimObjHdr_t* object, AnimChannel_t* channel, void* user, unsigned short) {
    int result = -1;
    bool killed = false;
    int i;

    _AnimStCompactSlots(owner);
    i = 0;
    if (!owner->slots[0].status) {
        result = 0;
    } else {
        i = 1;
        if (!owner->slots[1].status) {
            result = 1;
        } else {
            i = 2;
            if (!owner->slots[2].status) {
                result = 2;
            } else {
                i = 3;
                if (!owner->slots[3].status)
                    result = 3;
                else
                    i = 4;
            }
        }
    }
    if (i == 4) {
        for (i = 0; i < 4; i++) {
            if (owner->slots[i].delay != 0.0f) {
                _AnimStKillSlot(owner, i, object, channel, user);
                killed = true;
                break;
            }
        }
        if (killed == true) {
            if (!owner->slots[0].status)
                result = 0;
            else if (!owner->slots[1].status)
                result = 1;
            else if (!owner->slots[2].status)
                result = 2;
            else if (!owner->slots[3].status)
                result = 3;
        }
    }
    if (result == -1) {
        _AnimStKillSlot(owner, 0, object, channel, user);
        result = 3;
    }
    return result;
}
