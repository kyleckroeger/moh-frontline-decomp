extern void g(int); extern int h(); extern void g3(int,int,int);
static const int tab[8] = {3,5,7,11,13,17,19,23};
struct S { int n, m; S(); S(int); void I1(); };
S::S() { n = h(); for (int i = 0; i < n; i++) g3(tab[1], tab[2], i); m = 0; }
S::S(int k) { n = k; for (int i = 0; i < n; i++) g3(tab[1], tab[2], i); m = 0; }
void S::I1() { n = h(); for (int i = 0; i < n; i++) g3(tab[1], tab[2], i); m = 0; }
