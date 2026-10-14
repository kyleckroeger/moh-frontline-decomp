// A fragment of locatbig.cpp (0x8014a9a8): BIG_locate (the named entry's data
// in a BIG archive, or null when the name is missing or not found, through
// BIG_locateentryz) and BIG_count (the entry count from the header: a 16-bit
// count at +4 in 0xC0FB headers, a 32-bit count at +8 in "BIGF"/"BIG"
// headers). The functions are named by the symbols; the parameter names are
// inferred. BIG_locateentryz before them is not part of this unit; the header
// type test is locatbig.cpp's BIG_typeofheader (another unit), inlined here as
// in the original, so this unit declares a static inline copy.
extern "C" const char* BIG_locateentryz(const void* header, const char* name, int index, unsigned int* offset,
                                        unsigned int* size);

static inline int BIG_typeofheader(const void* header) {
    int type = 0;

    if (*(unsigned short*)header == 0xc0fb)
        type = 1;
    else if (*(unsigned int*)header == 0x42494746)
        type = 2;
    else if ((*(unsigned int*)header & ~0xff) == 0x42494700)
        type = 3;
    return type;
}

extern "C" const void* BIG_locate(const void* header, const char* name) {
    const void* data = 0;
    int offset = 0;

    if (name)
        BIG_locateentryz(header, name, 0, (unsigned int*)&offset, 0);
    if (offset)
        data = (const char*)header + offset;
    return data;
}

extern "C" int BIG_count(const void* header) {
    int count = 0;

    switch (BIG_typeofheader(header)) {
    case 1:
        count = ((unsigned short*)header)[2];
        break;
    case 2:
    case 3:
        count = ((int*)header)[2];
        break;
    }
    return count;
}
