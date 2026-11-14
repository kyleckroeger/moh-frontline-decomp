struct CBuddySoldier;
CBuddySoldier* GetS();
int GetP();
void Use(struct BuddyInfo, bool);
struct BuddyInfo {
    BuddyInfo() : soldier(0), progress(0) { BuddyInfo* self = this; self->moving = 0; self->active = false; self->path = 0; }
    BuddyInfo(const BuddyInfo& o) : soldier(o.soldier), progress(o.progress) { moving = o.moving; active = o.active; path = o.path; }
    CBuddySoldier* soldier; int progress; int path; int moving; bool active;
};
void T_H() {
    BuddyInfo info;
    info.soldier = GetS();
    info.moving = 0;
    info.active = true;
    info.progress = GetP();
    GetP();
    Use(info, false);
}
