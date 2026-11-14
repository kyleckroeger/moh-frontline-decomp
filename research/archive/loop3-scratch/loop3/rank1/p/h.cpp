extern void g(int); extern int h(); extern void g3(int,int,int);
static const int tab[8] = {3,5,7,11,13,17,19,23};
struct S { int n, m; void H1(); void H2(); void H3(); void H4(); void H5(); void H6(); void H7(); void H8(); };
void S::H1() { for (int i = 0; i < n; i++) g3(tab[1], tab[2], i); g(m); }
void S::H2() { g(n); for (int i = 0; i < 10; i++) g3(tab[1], tab[2], i); }
void H3(int p) { for (int i = 0; i < p; i++) g3(tab[1], tab[2], i); g(p); }
void S::H3() { int k = h(); for (int i = 0; i < n; i++) g3(tab[1], tab[2], i); g(m); g(k); }
void S::H4() { g(tab[3]); for (int i = 0; i < n; i++) g3(tab[1], tab[2], i); g(m); }
void S::H5() { g(tab[3]); g(n); g(tab[4]); g(m); g(tab[5]); }
void S::H6() { int k = h(); g(tab[3]); g(n); g(tab[4]); g(m); g(tab[5]); g(k); }
void S::H7() { for (int i = 0; i < n; i++) g(i); for (int i = 0; i < n; i++) g3(tab[1], tab[2], i); g(m); }
void S::H8() { for (int i = 0; i < n; i++) { for (int j = 0; j < n; j++) g3(tab[1], tab[2], j); g(i);} g(m); }
