// A fragment of toilet.cpp (0x8001cca8): TLT_LoadFromRepository loads a file
// from the level big file (falling back to a plain file load, both inlined
// as in toilet_files.cpp) and reports whether the loaded data is already in
// the repository hash map, adding it when it is not. The function and the
// repository are named by the symbols; the repository test is written as a
// small inline helper with the iterator in it (InRepository, inferred): the
// found pointer then colours like the original's (written in the caller,
// p/end/data sit in r7/r9/r8 instead of r9/r8/r7). The repository is
// file-local in the original and declared extern here. The rest of the file
// is not part of this unit.
#include "game/DwiHashMap.h"

extern "C" {
int sprintf(char*, const char*, ...);
char* strchr(const char*, int);
int FILE_sizez(const char*);
char* FILE_loadz(const char*, int);
void* BIG_locateentryz(void*, const char*, int, int*, int*);
char* BIG_locate(void*, const char*);
}
void DebugMsg(const char*, ...);

extern char* g_pLevelBigFile;

extern dwi::hash_map<char*, char*> g_Repository;

inline char* LoadFromBig(char* big, const char* name, int* size) {
    const char* q = strchr(name, '\\');
    while (q) {
        name = q + 1;
        q = strchr(name, '\\');
    }
    int offset;
    if (big) {
        if (BIG_locateentryz(big, name, 0, &offset, size))
            return BIG_locate(big, name);
    }
    return 0;
}

inline char* TLT_LoadFileNormal(const char* name, int* size, int flags) {
    char path[260];
    const char* file = strchr(name, ':');
    if (file == 0)
        file = name;
    else
        file = file + 1;
    sprintf(path, "%s", file);
    if (size)
        *size = FILE_sizez(path);
    return FILE_loadz(path, flags | 1024);
}

inline char* TLT_LoadFileFromLevelBigFile(const char* name, int* size) {
    const char* p = strchr(name, ':');
    {
        const char* n = name;
        if (!p) {
            n = name;
        } else {
            n = p;
            n++;
            DebugMsg("WARNING: When loading from a big file, strip off the %s part.\nSince it's a DEBUG build, I'll do it for you\n", "");
        }
        name = n;
    }
    char* file = LoadFromBig(g_pLevelBigFile, name, size);
    if (!file)
        file = TLT_LoadFileNormal(name, size, 0);
    return file;
}

static inline bool InRepository(char* data) {
    dwi::hash_map<char*, char*>::iterator it = g_Repository.find(data);
    return it != g_Repository.end();
}
char* TLT_LoadFromRepository(const char* name, int* size, bool* cached) {
    char* data = TLT_LoadFileFromLevelBigFile(name, size);
    *cached = false;
    if (data) {
        *cached = InRepository(data);
        if (!*cached)
            g_Repository.insert(data, data);
    }
    return data;
}
