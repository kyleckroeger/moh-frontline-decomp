struct CBuddySoldier;
CBuddySoldier* GetS();
int GetP();
struct BS { BS() { BS* self = this; self->moving = 0; self->active = false; self->path = 0; }
  BS(const BS& o) { moving = o.moving; active = o.active; path = o.path; } int path; int moving; bool active; };
struct BB { BB() : soldier(0), progress(0) {} BB(const BB& o) : soldier(o.soldier), progress(o.progress) {} CBuddySoldier* soldier; int progress; };
struct BI1 { BI1() {} BI1(const BI1& o) : key(o.key), state(o.state) {} BB key; BS state; };
struct BI2 : BB { BI2() {} BI2(const BI2& o) : BB(o), state(o.state) {} BS state; };
struct BI3 { BI3() : soldier(0), progress(0) {} BI3(const BI3& o) : soldier(o.soldier), progress(o.progress), state(o.state) {} CBuddySoldier* soldier; int progress; BS state; };
void Use1(BI1, bool); void Use2(BI2, bool); void Use3(BI3, bool);
void T_N1() { BI1 info; info.key.soldier = GetS(); info.state.moving = 0; info.state.active = true; info.key.progress = GetP(); GetP(); Use1(info, false); }
void T_N2() { BI2 info; info.soldier = GetS(); info.state.moving = 0; info.state.active = true; info.progress = GetP(); GetP(); Use2(info, false); }
void T_N3() { BI3 info; CBuddySoldier* s = GetS(); info.state.moving = 0; info.state.active = true; int p = GetP(); GetP(); info.soldier = s; info.progress = p; Use3(info, false); }
