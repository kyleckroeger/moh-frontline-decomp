struct CBuddySoldier;
CBuddySoldier* GetS();
int GetP();
void Use(struct BuddyInfo, bool);
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) { volatile int x = 0; moving = 0; active = false; path = 0; }
    CBuddySoldier* soldier; int progress; int path; int moving; bool active;
};
void T_F() {
    BuddyInfo info;
    info.soldier = GetS();
    info.moving = 0;
    info.active = true;
    info.progress = GetP();
    GetP();
    Use(info, false);
}
