// The end of buddy.cpp (0x80058ccc..0x800590ec): DestroyBuddyDataStructures,
// which clears the buddy initialised flag and the player checkpoint and
// empties the attached-buddy vector, the ready-buddy list and the buddy
// path-base vector; the file's static initialiser, which constructs those three
// containers and registers their destructors; and the weak dwi::vector
// destructors CW emits after it. The unit owns the file's .bss block
// (0x801c6c20: the three containers, each after its destructor-chain record)
// and its .sbss flag and checkpoint. The rest of the file is not part of this
// unit; the views of the buddy records are inferred (as in buddy_head.cpp).
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

class CBuddySoldier;

/* inferred: the state part of an attached buddy (path base, moving flag, in use). Its assignment
   copies the fields in layout order, as the original's element assignments do. */
struct BuddyState {
    int path;
    int moving;
    bool active;
    BuddyState& operator=(const BuddyState& o) {
        path = o.path;
        moving = o.moving;
        active = o.active;
        return *this;
    }
};

/* inferred: an empty check of a buddy state, taken by value (it reads like a compiled-out debug
   check). Called first in BuddyInfo's constructor: the whole-state read makes CW 1.3.2 keep only
   the state part of a BuddyInfo temporary or local in its stack slot and split soldier/progress
   into registers, as the original's list range constructor, push_back(BuddyInfo()) and
   BIFunc_AddBuddy show. */
inline void Check(BuddyState) {}

/* inferred: an attached buddy (soldier, checkpoint progress, state) */
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) {
        Check(state);
        state.moving = 0;
        state.active = false;
        state.path = 0;
    }
    BuddyInfo(const BuddyInfo& other) : soldier(other.soldier), progress(other.progress) {
        state.moving = other.state.moving;
        state.active = other.state.active;
        state.path = other.state.path;
    }

    CBuddySoldier* soldier;
    int progress;
    BuddyState state;
};

#include "game/DwiContainers.h"

/* inferred: a buddy path base entry */
struct BuddyPathInfo {
    BuddyPathInfo() : base(0) {}

    int base;
};

dwi::vector<BuddyInfo> g_attached_buddies;
dwi::list<BuddyInfo> g_ready_buddies;
dwi::vector<BuddyPathInfo> g_buddy_path_base;
bool g_bInitialized;
int g_player_checkpoint;

void DestroyBuddyDataStructures() {
    g_bInitialized = false;
    g_player_checkpoint = 0;
    g_attached_buddies.clear();
    g_ready_buddies.clear();
    g_buddy_path_base.clear();
}
