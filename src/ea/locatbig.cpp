// BIG archive headers: format detection (0xC0FB, "BIGF", "BIG\0"), header
// size, and the optional trailing debug tag (a letter and a three-digit
// version, then a 32-bit value); then BIG_locateentryz (an entry of a BIG
// archive by name, compared case-insensitively, or by index: its name, with
// the entry's offset and size fields read big-endian from the entry record,
// or null with both cleared), BIG_locate (the named entry's data, or null
// when the name is missing or not found, through BIG_locateentryz) and
// BIG_count (the entry count from the header: a 16-bit count at +4 in 0xC0FB
// headers, a 32-bit count at +8 in "BIGF"/"BIG" headers). The whole file; the
// compiler inlines the header type and size tests into the later functions.
// The functions are named by the symbols; the parameter names are inferred.
// The offset field is read as a 64-bit value whose high part (fields wider
// than 4 bytes) is discarded when it is stored: its 1-to-4-byte read leaves
// only the comparisons behind. BIG_debuginfo is called, not inlined, in the
// original: '#pragma auto_inline off' around its definition reproduces that.
extern "C" unsigned char __lower_map[256];
extern "C" unsigned long strlen(const char*);

extern "C" int BIG_typeofheader(const void* header) {
    int type = 0;

    if (*(unsigned short*)header == 0xc0fb)
        type = 1;
    else if (*(unsigned int*)header == 0x42494746)
        type = 2;
    else if ((*(unsigned int*)header & ~0xff) == 0x42494700)
        type = 3;
    return type;
}

extern "C" int BIG_sizeofheader(const void* header) {
    int size = 0;

    switch (BIG_typeofheader(header)) {
    case 1:
        size = ((unsigned short*)header)[1] + 4;
        break;
    case 2:
    case 3:
        size = ((int*)header)[3];
        break;
    }
    return size;
}

#pragma auto_inline off
static int BIG_debuginfo(const void* header, int* version, unsigned int* value) {
    int result = 0;
    const char* tag = (const char*)header + BIG_sizeofheader(header) - 8;

    if (((tag[0] >= 'A' && tag[0] <= 'Z') || (tag[0] >= 'a' && tag[0] <= 'z')) && tag[1] >= '0' && tag[1] <= '9' &&
        tag[2] >= '0' && tag[2] <= '9' && tag[3] >= '0' && tag[3] <= '9') {
        result = 8;
        if (version)
            *version = (tag[1] - '0') * 100 + (tag[2] - '0') * 10 + (tag[3] - '0');
        if (value)
            *value = *(unsigned int*)(tag + 4);
    }
    return result;
}
#pragma auto_inline reset

static inline unsigned int BIG_getsmall(const unsigned char* p, int bytes) {
    return bytes == 1 ? *p
         : bytes == 2 ? *(unsigned short*)p
         : bytes == 3 ? *(unsigned int*)p >> 8
         : bytes == 4 ? *(unsigned int*)p : 0;
}

static inline unsigned long long BIG_getnum(const unsigned char* p, int bytes) {
    unsigned long long value;
    if (bytes <= 4)
        value = BIG_getsmall(p, bytes);
    else {
        value = BIG_getsmall(p, bytes - 4);
        value <<= 32;
        p += bytes - 4;
        value |= *(unsigned int*)p;
    }
    return value;
}
static inline int BIG_tolower(int c) { return c == -1 ? -1 : __lower_map[(unsigned char)c]; }

static inline int BIG_compare(const char* entry, const char* name) {
    int diff = BIG_tolower(*entry) - BIG_tolower(*name);
    while (!diff && *entry) {
        name++;
        entry++;
        diff = BIG_tolower(*entry) - BIG_tolower(*name);
    }
    return diff;
}

extern "C" const char* BIG_locateentryz(const void* header, const char* name, int index, unsigned int* offset,
                                        unsigned int* size) {
    int count = 0;
    int debug = BIG_debuginfo(header, 0, 0);
    const unsigned char* entry = (const unsigned char*)header + 16;
    int offsetBytes = 4;
    int sizeBytes = 4;
    const unsigned char* end = (const unsigned char*)header + BIG_sizeofheader(header) - debug;

    switch (BIG_typeofheader(header)) {
    case 2:
        break;
    case 3:
        offsetBytes = ((const char*)header)[3] - '0';
        sizeBytes = offsetBytes;
        if (sizeBytes > 4)
            sizeBytes = 4;
        break;
    case 1:
        entry = (const unsigned char*)header + 6;
        offsetBytes = 3;
        sizeBytes = 3;
        break;
    }

    if (name) {
        while (entry < end) {
            const char* entryName = (const char*)entry + offsetBytes + sizeBytes;
            if (!BIG_compare(entryName, name)) {
                if (offset)
                    *offset = BIG_getnum(entry, offsetBytes);
                if (size)
                    *size = BIG_getsmall(entry + offsetBytes, sizeBytes);
                return entryName;
            }
            entry = (const unsigned char*)entryName + strlen(entryName) + 1;
        }
    } else {
        while (entry < end) {
            const char* entryName = (const char*)entry + offsetBytes + sizeBytes;
            if (count == index) {
                if (offset)
                    *offset = BIG_getnum(entry, offsetBytes);
                if (size)
                    *size = BIG_getsmall(entry + offsetBytes, sizeBytes);
                return entryName;
            }
            entry = (const unsigned char*)entryName + strlen(entryName) + 1;
            count++;
        }
    }
    if (offset)
        *offset = 0;
    if (size)
        *size = 0;
    return 0;
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
