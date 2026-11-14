extern void g(int); extern void g2(int,int); extern int h();
void C1(int k) { int j = h(); g(k); g(j); g(k); g(j); }
void C1b(int k) { int j = h(); g(j); g(k); g(j); g(k); g(j); g(j); g(j); }
void C2(int k) { int i; for (i = 0; i < 10; i++) g2(i, k); }
void C2b(int k) { int i; for (i = 0; i < 10; i++) {g2(k, k); g(k); g(i);} }
void C3() { int i, j; for (i = 0; i < 10; i++) { g(i); for (j = 0; j < 10; j++) g(j); } }
void C3b() { int i, j; for (i = 0; i < 10; i++) { g(i);g(i);g(i);g(i);g(i);g(i); for (j = 0; j < 10; j++) g(j); } }
void C4(int k) { int i; g(k); g(k); for (i = 0; i < 10; i++) g(i); for (i = 0; i < 10; i++) g(k); }
void C5(int k) { int j = h(); int i; g(k); for (i = 0; i < 10; i++) g(i); g(j); g(k); }
