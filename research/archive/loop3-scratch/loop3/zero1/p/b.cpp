extern "C" int rand();
struct S { unsigned int loc[7]; unsigned char ev; void A(); void B(); void C(); void D(); };
void S::A() {
    unsigned int hits = 0;
    int location = 0;
    for (int j = 0; j < 7; j++) { if (loc[j] > hits) { hits = loc[j]; location = j; } }
    ev = location * 5 + rand() % 5;
}
void S::B() {
    unsigned int hits = 0;
    int location = hits;
    for (int j = 0; j < 7; j++) { if (loc[j] > hits) { hits = loc[j]; location = j; } }
    ev = location * 5 + rand() % 5;
}
static inline int MostHit(const unsigned int* values) {
    unsigned int hits = 0; int location = 0;
    for (int j = 0; j < 7; j++) { unsigned int v = values[j]; if (v > hits) { hits = v; location = j; } }
    return location;
}
void S::C() {
    int location = MostHit(loc);
    ev = location * 5 + rand() % 5;
}
void S::D() {
    unsigned int hits = 0;
    int location = 0;
    for (int j = 0; j < 7; j++) { unsigned int v = loc[j]; if (v > hits) { hits = v; location = j; } }
    ev = location * 5 + rand() % 5;
}
