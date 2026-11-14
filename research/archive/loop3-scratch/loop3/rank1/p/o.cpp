extern void g(int); extern int h(); extern void gf(float); extern bool above(void*, void*);
struct S { int n, m; float f; void O1(void* node); void O2(void* node); void O3(void* node); void O4(void* node); };
void S::O1(void* node) { g(n); g((int)node); int sign; if (!above(this, node)) sign = 1; else sign = -1; gf(sign * f); g(n); gf(sign * f); g((int)node); }
void S::O2(void* node) { g(n); g((int)node); int sign; if (!above(this, node)) sign = 1; else sign = -1; g(sign); g(n); g(sign); g((int)node); }
void S::O3(void* node) { g(n); g((int)node); int sign; if (!above(this, node)) sign = 1; else sign = -1; gf(sign * f); g(n); gf(sign * f); }
void S::O4(void* node) { g(n); g((int)node); int sign = h(); gf(sign * f); g(n); gf(sign * f); g((int)node); }
