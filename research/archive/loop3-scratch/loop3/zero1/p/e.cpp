extern "C" int rand(); void msg(int, unsigned); void use(void*, void*); void useb(bool, bool);
struct S { unsigned int w[45]; unsigned int loc[7]; unsigned char wp, ev; void* pa[4]; void R8(); void R9(); void P1(); void B1(); void R10(); void R11(); };
void S::R8() { unsigned hits = w[0]; int location = w[1]; msg(0,0); w[2] = hits + location;
  hits = 0; location = 0; for (int j=0;j<7;j++){ if (loc[j]>hits){hits=loc[j];location=j;} } ev = location*5 + rand()%5; }
void S::R9() { unsigned most = 0; int weapon = 0; for (unsigned i=0;i<45;i++){ msg(i,w[i]); if (w[i]>most){most=w[i];weapon=i;} } wp = weapon;
  weapon = 0; most = 0; for (int j=0;j<7;j++){ if (loc[j]>most){most=loc[j];weapon=j;} } ev = weapon*5 + rand()%5; }
// pointers reused
void S::P1() { void* a = pa[0]; void* b = pa[1]; use(a, b); msg(0,0); use(a, b); a = 0; b = 0; for (int j=0;j<4;j++){ if (pa[j]) { a = pa[j]; b = &pa[j]; } } msg(0,0); use(a, b); }
// bools reused
void S::B1() { bool a = w[0] != 0; bool b = w[1] != 0; useb(a, b); msg(0,0); useb(a, b); a = 0; b = 0; for (int j=0;j<4;j++){ if (w[j]) { a = 1; b = loc[j] != 0; } } msg(0,0); useb(a, b); }
// one var reused twice with zero (loop then loop): same variable in two webs vs other var fresh-but-multi-web
void S::R10() { int a = 0; int b = 0; for (int j=0;j<7;j++){ msg(a,b); if (loc[j]) { a = j; b = loc[j]; } } msg(a,b);
  a = 0; b = 0; for (int j=0;j<7;j++){ if (loc[j] > b) { a = j; b = loc[j]; } } msg(a,b); }
// pointers, fresh
void S::R11() { msg(0,0); void* a = 0; void* b = 0; for (int j=0;j<4;j++){ if (pa[j]) { a = pa[j]; b = &pa[j]; } } msg(0,0); use(a, b); }
