struct CBuddySoldier;
CBuddySoldier* GetS();
int GetP();
struct BS { BS() { BS* self = this; self->moving = 0; self->active = false; self->path = 0; }
  BS(const BS& o) { moving = o.moving; active = o.active; path = o.path; } int path; int moving; bool active; };
struct BI3 { BI3() : soldier(0), progress(0) {} BI3(const BI3& o) : soldier(o.soldier), progress(o.progress), state(o.state) {} CBuddySoldier* soldier; int progress; BS state; };
struct BI4 { BI4() : soldier(0), progress(0) { BI4* self = this; self->moving = 0; self->active = false; self->path = 0; }
  BI4(const BI4& o) : soldier(o.soldier), progress(o.progress) { moving = o.moving; active = o.active; path = o.path; } CBuddySoldier* soldier; int progress; int path; int moving; bool active; };
void Use3(BI3, bool); void Use4(BI4, bool);
void UseR3(const BI3&); void UseR4(const BI4&);
void T_P3() { GetP(); Use3(BI3(), false); }
void T_P4() { GetP(); Use4(BI4(), false); }
