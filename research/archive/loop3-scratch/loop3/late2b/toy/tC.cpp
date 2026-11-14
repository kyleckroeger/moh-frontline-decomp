struct CBuddySoldier;
CBuddySoldier* GetS();
int GetP();
void Use(struct BuddyInfo, bool);
struct BS { BS() { BS* self = this; self->moving = 0; self->active = false; self->path = 0; } int path; int moving; bool active; };
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) {}
    CBuddySoldier* soldier; int progress; BS st;
};
void T_C() {
    BuddyInfo info;
    info.soldier = GetS();
    info.st.moving = 0;
    info.st.active = true;
    info.progress = GetP();
    GetP();
    Use(info, false);
}
