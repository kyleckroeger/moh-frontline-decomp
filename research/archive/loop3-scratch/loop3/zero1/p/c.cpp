extern "C" int rand();
struct S { unsigned int loc[7]; unsigned char ev; void E(); void F(); void G(); void H(); void I(); void J(); void K();void L(); void M(); };
#define LOOP(V) for (int j = 0; j < 7; j++) { if (V[j] > hits) { hits = V[j]; location = j; } }
static inline void MH_E(const unsigned int* values, int& out) { unsigned int hits = 0; int location = 0; LOOP(values) out = location; }
void S::E() { int location; MH_E(loc, location); ev = location * 5 + rand() % 5; }
static inline void MH_F(const unsigned int* values, int& location) { unsigned int hits = 0; location = 0; LOOP(values) }
void S::F() { int location; MH_F(loc, location); ev = location * 5 + rand() % 5; }
static inline int MH_G(const unsigned int* values) { int location = 0; unsigned int hits = 0; LOOP(values) return location; }
void S::G() { int location = MH_G(loc); ev = location * 5 + rand() % 5; }
static inline int MH_K(const unsigned int* values) { int hits = 0; int location = 0; LOOP(((int*)values)) return location; }
void S::K() { int location = MH_K(loc); ev = location * 5 + rand() % 5; }
void S::I() { int location; { unsigned int hits = 0; int l = 0; for (int j = 0; j < 7; j++) { if (loc[j] > hits) { hits = loc[j]; l = j; } } location = l; } ev = location * 5 + rand() % 5; }
void S::J() { int hits = 0; int location = 0; LOOP(((int*)loc)) ev = location * 5 + rand() % 5; }
int MH_L(const unsigned int* values) { unsigned int hits = 0; int location = 0; LOOP(values) return location; }
void S::L() { int location = MH_L(loc); ev = location * 5 + rand() % 5; }
static inline int MH_M(const unsigned int* values, unsigned int hits) { int location = 0; LOOP(values) return location; }
void S::M() { int location = MH_M(loc, 0); ev = location * 5 + rand() % 5; }
static inline int MH_H(const unsigned int* values, int location) { unsigned int hits = 0; LOOP(values) return location; }
void S::H() { int location = MH_H(loc, 0); ev = location * 5 + rand() % 5; }
