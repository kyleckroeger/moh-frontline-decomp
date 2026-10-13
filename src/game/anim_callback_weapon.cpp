// A fragment of anim_callback.cpp (0x8005fab4): CAnimCallbacks::weapon_cb,
// which on event 0 starts the state's list entry whose record matches the
// callback's last pointer argument (compared as a number; the first entry
// when none does), passing 1.0 as the last argument of AnimChanStartAnim (its
// meaning is unknown; the constant is a .sdata2 pool entry), and returns 0.
// The empty cases 1 and 2 are inferred from the switch's comparison tree.
// The class and callback are named by the mangled symbols; the list, entry
// and record views are inferred (as in anim_callback_init.cpp), and the
// record word compared here is the one random_cb reads as a weight.
struct AnimRecordView {
    unsigned char unknown00[4];
    unsigned int value;
};

struct AnimListEntryView {
    int anim;
    unsigned short flags;
    AnimRecordView* record;
};

struct AnimFileStateAnimList_t {
    unsigned short count;
    AnimListEntryView entries[1];
};

struct AnimObjHdr_t;
struct AnimChannel_t;

class CAnimCallbacks {
public:
    static int weapon_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
};

extern "C" int AnimChanStartAnim(AnimObjHdr_t*, AnimChannel_t*, int, unsigned short, unsigned short, void*, float);

int CAnimCallbacks::weapon_cb(AnimFileStateAnimList_t* list, unsigned short state, AnimObjHdr_t* header,
                              AnimChannel_t* channel, void* user, unsigned long event, void* weapon, float) {
    switch (event) {
    case 0: {
        unsigned int i;
        for (i = 0; i < list->count; i++) {
            if ((unsigned int)weapon == list->entries[i].record->value)
                break;
        }
        if (i >= list->count)
            i = 0;
        AnimChanStartAnim(header, channel, list->entries[i].anim, list->entries[i].flags, state, user, 1.0f);
        break;
    }
    case 1:
    case 2:
        break;
    }
    return 0;
}
