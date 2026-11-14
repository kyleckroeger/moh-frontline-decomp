struct CBuddySoldier;
CBuddySoldier* GetS();
int GetP();
void Use(struct BuddyInfo, bool);
struct BS { BS() { BS* self = this; self->moving = 0; self->active = false; self->path = 0; }
  BS(const BS& o) { moving = o.moving; active = o.active; path = o.path; } int path; int moving; bool active; };
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) {}
    BuddyInfo(CBuddySoldier* s, int p, const BS& st) : soldier(s), progress(p), state(st) {}
    BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress), state(o.state) {}
    CBuddySoldier* soldier; int progress; BS state;
};
void T_M() {
    BS st;
    CBuddySoldier* s = GetS();
    st.moving = 0;
    st.active = true;
    int p = GetP();
    GetP();
    Use(BuddyInfo(s, p, st), false);
}
