extern void g(int); extern int h();
struct S { int n, m; };
void L1(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); j = h(); g(p->n); g(j); g(p->m); }
void L2(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); j = h(); g(p->n); g(j); g(p->m); }
void L3(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); j = h(); g(p->n); g(j); g(p->m); }
void L4(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); g(v[6] ^ v[7]); j = h(); g(p->n); g(j); g(p->m); }
void L5(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); g(v[6] ^ v[7]); g(v[8] ^ v[9]); j = h(); g(p->n); g(j); g(p->m); }
void L6(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); g(v[6] ^ v[7]); g(v[8] ^ v[9]); g(v[10] ^ v[11]); j = h(); g(p->n); g(j); g(p->m); }
void L7(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); g(v[6] ^ v[7]); g(v[8] ^ v[9]); g(v[10] ^ v[11]); g(v[12] ^ v[13]); j = h(); g(p->n); g(j); g(p->m); }
void L8(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); g(v[6] ^ v[7]); g(v[8] ^ v[9]); g(v[10] ^ v[11]); g(v[12] ^ v[13]); g(v[14] ^ v[15]); j = h(); g(p->n); g(j); g(p->m); }
void L9(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); g(v[6] ^ v[7]); g(v[8] ^ v[9]); g(v[10] ^ v[11]); g(v[12] ^ v[13]); g(v[14] ^ v[15]); g(v[16] ^ v[17]); j = h(); g(p->n); g(j); g(p->m); }
void L10(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); g(v[6] ^ v[7]); g(v[8] ^ v[9]); g(v[10] ^ v[11]); g(v[12] ^ v[13]); g(v[14] ^ v[15]); g(v[16] ^ v[17]); g(v[18] ^ v[19]); j = h(); g(p->n); g(j); g(p->m); }
void L11(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); g(v[6] ^ v[7]); g(v[8] ^ v[9]); g(v[10] ^ v[11]); g(v[12] ^ v[13]); g(v[14] ^ v[15]); g(v[16] ^ v[17]); g(v[18] ^ v[19]); g(v[20] ^ v[21]); j = h(); g(p->n); g(j); g(p->m); }
void L12(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); g(v[6] ^ v[7]); g(v[8] ^ v[9]); g(v[10] ^ v[11]); g(v[12] ^ v[13]); g(v[14] ^ v[15]); g(v[16] ^ v[17]); g(v[18] ^ v[19]); g(v[20] ^ v[21]); g(v[22] ^ v[23]); j = h(); g(p->n); g(j); g(p->m); }
void L13(S* p, volatile int* v) { int j; g(v[0] ^ v[1]); g(v[2] ^ v[3]); g(v[4] ^ v[5]); g(v[6] ^ v[7]); g(v[8] ^ v[9]); g(v[10] ^ v[11]); g(v[12] ^ v[13]); g(v[14] ^ v[15]); g(v[16] ^ v[17]); g(v[18] ^ v[19]); g(v[20] ^ v[21]); g(v[22] ^ v[23]); g(v[24] ^ v[25]); j = h(); g(p->n); g(j); g(p->m); }
