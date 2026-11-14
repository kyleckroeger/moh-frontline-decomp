extern void g(int); extern int h();
struct S { int n, m; void N1(); void N2(); void N3(); void N4(); inline void in1(int q); inline void in2(); };
inline void S::in1(int q) { int z = h(); g(n); g(q); g(z); g(m); g(q); g(z); }
inline void S::in2() { int z = h(); g(n); g(z); g(m); g(z); }
void S::N1() { int a = h(); in1(a); g(a); }
void S::N2() { int a = h(); in2(); g(a); }
void S::N3() { int a = h(); for (int i = 0; i < 10; i++) { g(i); } in2(); g(a); }
inline void fi(S* s, int q) { int z = h(); g(s->n); g(q); g(z); g(s->m); g(q); g(z); }
void S::N4() { int a = h(); fi(this, a); g(a); }
