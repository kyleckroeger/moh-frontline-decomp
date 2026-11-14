extern void g(int); extern void g2(int,int); extern int h(); extern void gp(void*);
struct T { int x[100]; };
extern T gt;
struct S { int n, m; void D2(); void D5(); void D6(); void D7(); };
void D1(int a, int b) { g(a); g(b); g(a); g(b); }
void S::D2() { int j = h(); g(n); g(j); g(m); g(j); }
void D3(int a) { g(gt.x[1]); g(a); g(gt.x[2]); g(a); g(gt.x[3]); }
void S::D5() { g(gt.x[1]); g(n); g(gt.x[2]); g(m); g(gt.x[3]); }
inline void inl(S* s) { int j = h(); g(s->n); g(j); g(s->m); g(j); }
void S::D6() { inl(this); }
inline void inl2(S* s, int q) { g(s->n); g(q); g(s->m); g(q); }
void S::D7() { int k = h(); inl2(this, k); }
