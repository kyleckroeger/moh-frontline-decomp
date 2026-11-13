// late2c (2026-11-13): owner r30 / slot r31 swap (orig owner r31): 'self' copies (const or not, before/after slot), slot
// set in the for-init, &owner->slots[0]: inert (23); indexed owner->slots[i] with the slot dropped from the inline: 215-239.
// Animation states: each object carries four 104-byte state slots. This unit
// covers the slot accessors at the start of the file. The slot, owner and
// callback-table views are inferred from offsets; their names are not
// original.
/* inferred: an animation weight block (its current weight first) */
struct AnimWgtInfo_t {
    float weight;
    unsigned char field04[20];
};

struct AnimStateSlotView {
    unsigned char field00[4];
    unsigned short state;
    unsigned short status;
    unsigned short flags;
    unsigned char field0A[2];
    float delay;
    unsigned char userMemory[64];
    AnimWgtInfo_t weight;
};

struct AnimStateOwnerView {
    AnimStateSlotView slots[4];
};

struct AnimStCallbackEntryView {
    unsigned long id;
    void* callback;
};

struct AnimStCallbackTableView {
    unsigned long count;
    AnimStCallbackEntryView* entries;
};

extern AnimStCallbackTableView* _AnimSt_CallbackTable;

struct AnimObjHdr_t {
    unsigned short field00;
    unsigned short id;
};

struct AnimChannel_t;

struct AnimFileStateAnimList_t {
    unsigned char field00[0xbbc];
};

class CAnimDatabase {
public:
    void GetStateAnimList(unsigned long, unsigned long, AnimFileStateAnimList_t*);
    int GetStateCallbackID(unsigned long, unsigned short);

    unsigned char unknown00[176];
};

extern CAnimDatabase g_AnimDB;
extern AnimFileStateAnimList_t _AnimSt_pStatAnimList; // file-local in the original

typedef int (*AnimStateSelFunc)(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
typedef void (*AnimStFinishFunc)(unsigned short, void*);

extern AnimStFinishFunc _AnimSt_FinishCb; // file-local in the original

extern "C" void AnimWgtProcess(AnimWgtInfo_t*, float);
extern "C" unsigned short AnimChanGetNumActiveByState(AnimObjHdr_t*, AnimChannel_t*, unsigned short);
extern "C" void AnimChanStopByStateIndex(AnimObjHdr_t*, AnimChannel_t*, unsigned short);
extern "C" void* memcpy(void*, const void*, unsigned long);

/* inferred: runs a state's selection callback (skipped for flagged states) */
inline void _AnimStCallStateCallback(unsigned short state, AnimObjHdr_t* object, AnimChannel_t* channel, void* user, unsigned long event, float frames) {
    unsigned short id = object->id;
    if (!(state & 0x8000)) {
        g_AnimDB.GetStateAnimList(id, state, &_AnimSt_pStatAnimList);
        int index = g_AnimDB.GetStateCallbackID(state, id);
        ((AnimStateSelFunc)_AnimSt_CallbackTable->entries[index].callback)(&_AnimSt_pStatAnimList, state, object, channel, user, event, 0, frames);
    }
}

extern "C" void AnimStSetStateDelay(AnimStateOwnerView* owner, unsigned short state, float delay) {
    for (int i = 0; i < 4; i++) {
        AnimStateSlotView* slot = &owner->slots[i];
        if (slot->status != 0 && slot->state == state)
            slot->delay = delay;
    }
}

extern "C" void AnimStSetStateFlags(AnimStateOwnerView* owner, unsigned short state, unsigned short flags) {
    for (int i = 0; i < 4; i++) {
        AnimStateSlotView* slot = &owner->slots[i];
        if (slot->status != 0 && slot->state == state)
            slot->flags |= flags;
    }
}

extern "C" unsigned short AnimStGetCurrentState(AnimStateOwnerView* owner) {
    unsigned short current = 0xFFFF;

    for (int i = 3; i >= 0; i--) {
        if (owner->slots[i].status == 3)
            current = owner->slots[i].state;
    }
    return current;
}

extern "C" void* AnimStGetStateUserMemory(AnimStateOwnerView* owner, unsigned short state) {
    for (int i = 0; i < 4; i++) {
        AnimStateSlotView* slot = &owner->slots[i];
        if (slot->status != 0 && slot->state == state)
            return owner->slots[i].userMemory;
    }
    return 0;
}

extern "C" float AnimStGetStateWeight(AnimStateOwnerView* owner, unsigned short state) {
    for (int i = 0; i < 4; i++) {
        AnimStateSlotView* slot = &owner->slots[i];
        if (slot->status != 0 && slot->state == state)
            return owner->slots[i].weight.weight;
    }
    return 0.0f;
}

extern "C" int AnimStGetCallbackIndexFromID(unsigned long id) {
    AnimStCallbackTableView* table = _AnimSt_CallbackTable;

    for (int i = 0; i < table->count; i++) {
        if (id == table->entries[i].id)
            return i;
    }
    return -1;
}

/* inferred: moves the used slots down over the free ones */
inline void _AnimStCompactSlots(AnimStateOwnerView* owner) {
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
                        memcpy(&owner->slots[k].state, &owner->slots[k + 1].state, sizeof(AnimStateSlotView));
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

/* inferred: runs a finished state's callback and the finish callback */
inline void _AnimStFinishState(unsigned short state, AnimObjHdr_t* object, AnimChannel_t* channel, void* user, float frames) {
    unsigned short id = object->id;
    if (!(state & 0x8000)) {
        g_AnimDB.GetStateAnimList(id, state, &_AnimSt_pStatAnimList);
        int index = g_AnimDB.GetStateCallbackID(state, id);
        ((AnimStateSelFunc)_AnimSt_CallbackTable->entries[index].callback)(&_AnimSt_pStatAnimList, state, object, channel, user, 1, 0, frames);
        _AnimSt_FinishCb(state, user);
    }
}

/* inferred: ends the state in a slot */
inline void _AnimStStopSlot(AnimStateOwnerView* owner, unsigned int slot, AnimObjHdr_t* object, AnimChannel_t* channel, void* user, float frames) {
    _AnimStCallStateCallback(owner->slots[slot].state, object, channel, user, 3, frames);
    AnimChanStopByStateIndex(object, channel, owner->slots[slot].state);
    owner->slots[slot].status = 0;
}

inline void _AnimStProcessSlot(AnimStateOwnerView* owner, unsigned int i, AnimStateSlotView* slot, AnimObjHdr_t* object, AnimChannel_t* channel, void* user, float frames) {
        if ((slot->delay -= frames) < 0.0f) {
            slot->delay = 0.0f;
            AnimWgtProcess(&slot->weight, frames);
            if (slot->weight.weight > 1e-07f) {
                if (slot->flags & 1)
                    _AnimStCallStateCallback(slot->state, object, channel, user, 2, frames);
                if (AnimChanGetNumActiveByState(object, channel, slot->state) == 0) {
                    if (slot->status == 3) {
                        _AnimStFinishState(slot->state, object, channel, user, frames);
                    } else {
                        _AnimStStopSlot(owner, i, object, channel, user, frames);
                    }
                }
            } else {
                _AnimStStopSlot(owner, i, object, channel, user, frames);
            }
        }
}

extern "C" void AnimStProcess(AnimStateOwnerView* owner, AnimObjHdr_t* object, AnimChannel_t* channel, void* user, float frames) {
    int i;
    AnimStateSlotView* slot = owner->slots;
    for (i = 0; i < 4; slot++, i++) {
        if (slot->status == 0)
            break;
        _AnimStProcessSlot(owner, i, slot, object, channel, user, frames);
    }
    _AnimStCompactSlots(owner);
}
