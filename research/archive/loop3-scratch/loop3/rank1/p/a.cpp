extern void g(int);
extern void g2(int,int);
extern int h();
struct S {
    int a[40]; int n; int m;
    void f1();  // this used before and after loop
    void f2();  // this used only before loop, loop over own index
    void f3();  // this used inside loop
    void f4();  // this used only before; loop count heavy
};
void S::f1() { g(n); for (int i = 0; i < 10; i++) g(i); g(m); }
void S::f2() { g(n); for (int i = 0; i < 10; i++) g(i); }
void S::f3() { for (int i = 0; i < 10; i++) g2(i, n); }
void S::f4() { g(n); g(m); for (int i = 0; i < 10; i++) { g(i); g(i); g(i); } }
void S1(S* s) { g(s->n); for (int i = 0; i < 10; i++) g(i); g(s->m); }
void S2(S* s) { g(s->n); for (int i = 0; i < 10; i++) g(i); }
void S3(int k) { int i; g(k); for (i = 0; i < 10; i++) g(i); g(k); }
