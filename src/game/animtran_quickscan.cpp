// A fragment of animtran.cpp (0x800681b0, the start of the file):
// _AnimTranQuickScan checks whether an object's animation set has a
// transition for its current state (the database fills the transition list
// array): a direct match of from/to (a 0xFFFF from matches any entry whose
// from has no 0x8000 bit, otherwise the 0x8000 wildcard), then the wildcard
// from, each with the given state (or with any state, 0xFFFF) returns -1;
// otherwise every state 14250..14299 reachable from a wildcard entry is
// collected and the first one whose own list has a wildcard transition to the
// target is returned (-1 when none). CAnimDatabase, the globals, the list
// array type and the function are named by the symbols; the entry/list
// layouts, the search helper (inlined four times; its frame argument is 0
// here) and the state range are inferred. The rest of the file is in other
// units.
struct AnimObjHdr_t {
    unsigned short kind;
    unsigned short set;
};

struct AnimTranEntry_t {
    unsigned short from;
    unsigned short to;
    unsigned short state;
    unsigned short low;
    unsigned short high;
    unsigned short pad;
    void* transition;
};

struct AnimTranList_t {
    int count;
    AnimTranEntry_t entries[1];
};

struct AnimTranListRef_t {
    int unknown00;
    int unknown04;
    AnimTranList_t* list;
};

struct AnimTranTransListArray_t {
    unsigned short count;
    unsigned short pad;
    int unknown04;
    AnimTranListRef_t lists[8];
};

class CAnimDatabase {
public:
    void GetTransList(unsigned long, unsigned long, AnimTranTransListArray_t*);

    unsigned char unknown00[176];
};

extern CAnimDatabase g_AnimDB;
extern AnimTranTransListArray_t AnimTran_TransListArray;

static inline void* FindTransition(AnimTranTransListArray_t* array, unsigned short from, unsigned short to,
                                   unsigned short state, unsigned short frame) {
    unsigned short target;
    int i;
    void* found;
    unsigned short key;
    found = 0;
    target = from;
    if (from != 0xffff) {
        key = 0x8000;
    } else {
        target = to;
        key = 0xffff;
    }
    for (i = 0; i < array->count; i++) {
        AnimTranList_t* list = array->lists[i].list;
        for (int j = 0; j < list->count; j++) {
            AnimTranEntry_t* entry = &list->entries[j];
            if (entry->from == key || (key == 0xffff && !(entry->from & 0x8000))) {
                if (entry->to == target && entry->state == state) {
                    if (state != 0xffff) {
                        if (entry->low <= frame && entry->high >= frame)
                            found = entry->transition;
                    } else {
                        found = entry->transition;
                    }
                }
            }
        }
    }
    return found;
}

int _AnimTranQuickScan(AnimObjHdr_t* object, unsigned short to, unsigned short from, unsigned short current,
                       unsigned short state) {
    AnimTranTransListArray_t* array = &AnimTran_TransListArray;
    unsigned short candidates[68];
    unsigned short count;
    int i;

    g_AnimDB.GetTransList(object->set, current, array);
    if (array->count != 0) {
        if (FindTransition(array, from, to, state, 0))
            return -1;
        if (FindTransition(array, 0xffff, to, state, 0))
            return -1;
        if (FindTransition(array, from, to, 0xffff, 0))
            return -1;
        if (FindTransition(array, 0xffff, to, 0xffff, 0))
            return -1;
        for (i = 0, count = 0; i < array->count; i++) {
            AnimTranList_t* list = array->lists[i].list;
            for (int j = 0; j < list->count; j++) {
                AnimTranEntry_t* entry = &list->entries[j];
                if (entry->from == 0xffff || !(entry->from & 0x8000)) {
                    unsigned short id = entry->to;
                    if (id >= 14250 && id <= 14299)
                        candidates[count++] = id;
                }
            }
        }
        for (i = 0; i < count; i++) {
            g_AnimDB.GetTransList(object->set, candidates[i], &AnimTran_TransListArray);
            if (array->count != 0 && FindTransition(array, 0xffff, to, 0xffff, 0))
                return candidates[i];
        }
    }
    return -1;
}
