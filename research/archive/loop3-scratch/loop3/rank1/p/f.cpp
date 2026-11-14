extern void g(int); extern int h(); extern void gp(void*);
struct T { int x[100]; };
extern T gt;
static T st;
struct S { int n, m; void F1(); void F2(); void F3(); void F4(); };
void S::F1() { g(st.x[1]); g(n); g(st.x[2]); g(m); g(st.x[3]); }
void S::F2() { int j = h(); g(st.x[1]); g(n); g(j); g(st.x[2]); g(m); g(st.x[3]); g(j); }
void F3(int p) { g(st.x[1]); g(p); g(st.x[2]); g(p); g(st.x[3]); }
void S::F3() { for (int i = 0; i < 10; i++) g(st.x[i]); g(n); }
void S::F4() { g(n); g(st.x[1]); g(st.x[2]); g(m); g(st.x[3]); }
