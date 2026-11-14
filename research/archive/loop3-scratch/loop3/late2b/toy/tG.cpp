struct CBuddySoldier;
CBuddySoldier* GetS();
int GetP();
void Use(struct BuddyInfo, bool);
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) { moving = 0; active = false; path = 0; }
    BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress) { moving = o.moving; active = o.active; path = o.path; }
    BuddyInfo& operator=(const BuddyInfo& o) { soldier = o.soldier; progress = o.progress; moving = o.moving; active = o.active; path = o.path; return *this; }
    CBuddySoldier* soldier; int progress; int path; int moving; bool active;
};
void T_G() {
    BuddyInfo info;
    info.soldier = GetS();
    info.moving = 0;
    info.active = true;
    info.progress = GetP();
    GetP();
    Use(info, false);
}
