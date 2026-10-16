// EA's case-insensitive string compare: returns the first difference of the
// lowered characters, or 0 at the end of the first string. The upper-case
// test macro and the lowering helper are inferred from the code (the test and
// the value promote the character separately; the second string is read
// first); their names are this project's.
#define IsUpper(c) ((c) >= 'A' && (c) <= 'Z')

static inline int Lower(const char& c) {
    return IsUpper(c) ? c + 32 : c;
}

extern "C" int stricmp(const char* a, const char* b) {
    int diff;
    while (!(diff = Lower(*a) - Lower(*b)) && *a) {
        a++;
        b++;
    }
    return diff;
}
