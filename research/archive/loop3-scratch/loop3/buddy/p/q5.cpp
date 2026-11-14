struct BuddyState { int path; int moving; bool active; };
struct BuddyInfo { BuddyInfo() : soldier(0), progress(0) { BuddyState unused = state; state.moving = 0; state.active = false; state.path = 0; } BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress), state(o.state) {} void* soldier; int progress; BuddyState state; };
