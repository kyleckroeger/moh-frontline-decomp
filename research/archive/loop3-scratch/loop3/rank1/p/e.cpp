extern void g(int); extern int h();
void E1() { int a = h(); int b = h(); g(a); g(b); g(a); g(b); }
void E2() { int b; int a; a = h(); b = h(); g(a); g(b); g(a); g(b); }
void E3() { int a = h(); int b = h(); int c = h(); g(a); g(b); g(c); g(a); g(b); g(c); }
void E4() { int a = h(); int b = h(); g(b); g(a); g(b); g(a); }
void E5(int p, int q, int r) { g(p); g(q); g(r); g(p); g(q); g(r);}
void E6(int p, int q) { int a = h(); g(p); g(q); g(a); g(p); g(q); g(a);}
void E7(int p, int q) { int a = h(); g(a); g(p); g(q); g(a); g(p); g(q); g(a);}
void E8(int p) { int a = h(); int b = h(); g(a); g(p); g(b); g(a); g(p); g(b);}
