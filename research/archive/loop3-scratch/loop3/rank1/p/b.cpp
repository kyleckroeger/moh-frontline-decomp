extern void g(int);
void K1(int k) { int i; g(k); for (i = 0; i < 10; i++) g(i); g(k); }
void K2(int k) { int i; g(k); g(k); for (i = 0; i < 10; i++) g(i); g(k); }
void K4(int k) { int i; g(k); g(k); g(k); g(k); for (i = 0; i < 10; i++) g(i); g(k); }
void K6(int k) { int i; g(k); g(k); g(k); g(k); g(k); g(k); for (i = 0; i < 10; i++) g(i); g(k); }
void K8(int k) { int i; g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); for (i = 0; i < 10; i++) g(i); g(k); }
void K12(int k) { int i; g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); for (i = 0; i < 10; i++) g(i); g(k); }
void K20(int k) { int i; g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); g(k); for (i = 0; i < 10; i++) g(i); g(k); }
