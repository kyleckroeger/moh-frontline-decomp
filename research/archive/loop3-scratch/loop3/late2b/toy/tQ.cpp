struct CBuddySoldier;
CBuddySoldier* GetS();
int GetP();
struct BS { BS() { moving = 0; active = false; path = 0; }
  BS(const BS& o) { moving = o.moving; active = o.active; path = o.path; } int path; int moving; bool active; unsigned char pad[3]; };
struct BI3 { BI3() : soldier(0), progress(0) {} BI3(const BI3& o) : soldier(o.soldier), progress(o.progress), state(o.state) {} CBuddySoldier* soldier; int progress; BS state; };
struct BI5 { BI5() : soldier(0), progress(0) { moving = 0; active = false; path = 0; }
  BI5(const BI5& o) : soldier(o.soldier), progress(o.progress) { moving = o.moving; active = o.active; path = o.path; } CBuddySoldier* soldier; int progress; int path; int moving; bool active; unsigned char pad[3]; };
void Use3(BI3, bool); void Use5(BI5, bool);
void T_Q3() { BI3 info; info.soldier = GetS(); info.state.moving = 0; info.state.active = true; info.progress = GetP(); GetP(); Use3(info, false); }
void T_Q5() { BI5 info; info.soldier = GetS(); info.moving = 0; info.active = true; info.progress = GetP(); GetP(); Use5(info, false); }
