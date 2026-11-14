extern void g(int); extern int h(); extern void gs(const char*, int);
static const unsigned long primes[27] = {1,2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101};
static const int tab2[4] = {7,8,9,10};
static inline int NextPrime(int n) { const unsigned long* p = primes; const unsigned long* end = primes + 27; for (; p != end; p++) if ((int)*p >= n) return *p; return *(end-1); }
struct S { int n, m; int* d; void Q1(); void Q2(); };
void S::Q1() { for (int i = 0; i < n; i++) g(d[i]); g(NextPrime(n)); for (int i = 0; i < m; i++) { int t[4] = {tab2[0], tab2[1], tab2[2], tab2[3]}; g(t[i&3] + d[i]); } }
void S::Q2() { g(NextPrime(n)); for (int i = 0; i < m; i++) { int t[4] = {tab2[0], tab2[1], tab2[2], tab2[3]}; g(t[i&3] + d[i]); } }
