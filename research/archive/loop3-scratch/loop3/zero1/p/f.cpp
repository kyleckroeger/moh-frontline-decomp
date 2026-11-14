extern "C" int rand(); void msg(int, unsigned);
struct S { unsigned int w[45]; unsigned int loc[7]; unsigned char wp, ev; void T1(); void T2(); void T3(); void T4(); void T5(); };
#define FIRST unsigned most = w[0]; int weapon = w[1]; msg(0,0); w[2] = most + weapon;
// reused weapon first, fresh hits second
void S::T1() { FIRST weapon = 0; unsigned hits = 0; for (int j=0;j<7;j++){ if (loc[j]>hits){hits=loc[j];weapon=j;} } ev = weapon*5 + rand()%5; }
// both reused, a store between
void S::T2() { FIRST most = 0; wp = 7; weapon = 0; for (int j=0;j<7;j++){ if (loc[j]>most){most=loc[j];weapon=j;} } ev = weapon*5 + rand()%5; }
// both reused, a call between
void S::T3() { FIRST most = 0; msg(1,1); weapon = 0; for (int j=0;j<7;j++){ if (loc[j]>most){most=loc[j];weapon=j;} } ev = weapon*5 + rand()%5; }
// reused var copies from inline local? reused most + inline-local
static inline int Pick(const unsigned* v, unsigned& most) { int location = 0; for (int j=0;j<7;j++){ if (v[j]>most){most=v[j];location=j;} } return location; }
void S::T4() { FIRST most = 0; weapon = Pick(loc, most); ev = weapon*5 + rand()%5; }
// three reused
void S::T5() { unsigned a = w[0]; int b = w[1]; int c = w[3]; msg(0,0); w[2] = a + b + c; a = 0; b = 0; c = 0; for (int j=0;j<7;j++){ if (loc[j]>a){a=loc[j];b=j;c+=j;} } ev = b*5 + c + rand()%5; }
