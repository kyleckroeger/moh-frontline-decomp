struct CBuddySoldier;
CBuddySoldier* GetS();
int GetP();
void Use(struct BuddyInfo, bool);
struct BS { BS() { moving = 0; active = false; path = 0; } int path; int moving; bool active; };
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) {}
    BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress), st(o.st) {}
    CBuddySoldier* soldier; int progress; BS st;
};
void T_K() {
    BuddyInfo info;
    info.soldier = GetS();
    info.st.moving = 0;
    info.st.active = true;
    info.progress = GetP();
    GetP();
    Use(info, false);
}
