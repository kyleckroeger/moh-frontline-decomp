extern void g(int); extern int h(); extern void gs(const char*); extern void gs2(const char*, int);
struct S { int n, m; void G1(); void G2(); void G3(); void G4(int); };
void S::G1() { gs("aaa"); g(n); gs("bbb"); g(m); gs("ccc"); }
void S::G2() { int j = h(); gs("aaa"); g(n); g(j); gs("bbb"); g(m); gs("ccc"); g(j); }
void G3(int p) { gs("aaa"); g(p); gs("bbb"); g(p); gs("ccc"); }
void S::G3() { for (int i = 0; i < n; i++) gs2("aaa", i); gs("bbb"); g(m); }
void S::G4(int q) { gs("aaa"); g(n); gs2("bbb",q); g(m); gs("ccc"); g(q); }
