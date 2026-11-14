extern void f(int); extern void g(int, int); extern int h();
void t1(int* a, int n) { int x = 0; int y = 0; for (int i=0;i<n;i++){ if (a[i]>x){x=a[i]; y=i;} } g(x,y); }
