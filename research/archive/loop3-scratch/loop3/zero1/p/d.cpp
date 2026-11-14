extern "C" int rand(); void msg(int, unsigned);
struct S { unsigned int w[45]; unsigned int loc[7]; unsigned char wp, ev; void R1(); void R2(); void R3(); void R4(); void R5(); void R6(); void R7(); };
// R1: both reused
void S::R1() { unsigned most = 0; int weapon = 0; for (unsigned i=0;i<45;i++){ msg(i,w[i]); if (w[i]>most){most=w[i];weapon=i;} } wp = weapon;
  most = 0; weapon = 0; for (int j=0;j<7;j++){ if (loc[j]>most){most=loc[j];weapon=j;} } ev = weapon*5 + rand()%5; }
// R2: only weapon reused (fresh hits)
void S::R2() { unsigned most = 0; int weapon = 0; for (unsigned i=0;i<45;i++){ msg(i,w[i]); if (w[i]>most){most=w[i];weapon=i;} } wp = weapon;
  unsigned hits = 0; weapon = 0; for (int j=0;j<7;j++){ if (loc[j]>hits){hits=loc[j];weapon=j;} } ev = weapon*5 + rand()%5; }
// R3: only most reused (fresh location)
void S::R3() { unsigned most = 0; int weapon = 0; for (unsigned i=0;i<45;i++){ msg(i,w[i]); if (w[i]>most){most=w[i];weapon=i;} } wp = weapon;
  most = 0; int location = 0; for (int j=0;j<7;j++){ if (loc[j]>most){most=loc[j];location=j;} } ev = location*5 + rand()%5; }
// R4: fresh both, no first loop
void S::R4() { unsigned hits = 0; int location = 0; for (int j=0;j<7;j++){ if (loc[j]>hits){hits=loc[j];location=j;} } ev = location*5 + rand()%5; }
// R5: both declared at top uninitialised, assigned zero later, no earlier defs
void S::R5() { unsigned hits; int location; msg(0,0); hits = 0; location = 0; for (int j=0;j<7;j++){ if (loc[j]>hits){hits=loc[j];location=j;} } ev = location*5 + rand()%5; }
// R6: earlier def is a plain non-loop assignment
void S::R6() { unsigned hits = w[0]; int location = w[1]; msg(hits, location); hits = 0; location = 0; for (int j=0;j<7;j++){ if (loc[j]>hits){hits=loc[j];location=j;} } ev = location*5 + rand()%5; }
// R7: earlier def constant, then call, then zero again
void S::R7() { unsigned hits = 0; int location = 0; msg(hits, location); hits = 0; location = 0; for (int j=0;j<7;j++){ if (loc[j]>hits){hits=loc[j];location=j;} } ev = location*5 + rand()%5; }
