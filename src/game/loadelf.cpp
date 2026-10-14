// loadelf.cpp (0x80108a28, the whole file): LoadElf::Get, the fixed load
// address of a module (0x80003100), and LoadElf::RunModule, which loads an
// ELF file from the disc over the running program and jumps to it. The
// header (one 4 KB read into a 32-byte aligned buffer) must carry the ELF
// magic; each program header's segment is cleared to its memory size, read
// (its file size rounded up to 32 bytes) and flushed from both caches. Every
// DVD step adds CheckDVDError's result to HadError, and the whole load is
// retried while an error is pending; after any error the display is blacked
// out before the system is reset (shutdown, keeping the reset code) and the
// entry point is called. LoadElf, RunModule, Get, theCodePointer, HadError
// and the messages come from the symbols and strings; the read helper, the
// ELF header views (standard ELF field offsets) and local names are inferred.
// The entry number is an s32, as DVDConvertPathToEntrynum returns.
extern "C" {
#include <stdio.h>
#include <string.h>
}
#include <dolphin/dvd.h>
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <dolphin/vi.h>
#include <dolphin/base/PPCArch.h>

extern "C" int CheckDVDError(void);
extern "C" int gGxIsInit;

class LoadElf {
public:
    void* Get();
    void RunModule(const char* path);
};

struct ElfHeader {
    unsigned char ident[16];
    unsigned short type;
    unsigned short machine;
    unsigned long version;
    unsigned long entry;
    unsigned long phoff;
    unsigned long shoff;
    unsigned long flags;
    short ehsize;
    short phentsize;
    short phnum;
};

struct ElfProgramHeader {
    unsigned long type;
    unsigned long offset;
    unsigned long vaddr;
    unsigned long paddr;
    unsigned long filesz;
    unsigned long memsz;
    unsigned long flags;
    unsigned long align;
};

static unsigned char theCodePointer[0x1000 + 32];
static int HadError;

/* Inferred: read, retrying until the command finishes; -1 on a DVD error. */
inline int ReadWithRetry(DVDFileInfo* file, void* addr, int length, int offset) {
    bool done;
    do {
        HadError += CheckDVDError();
        DVDReadAsync(file, addr, length, offset, 0);
        int status;
        do {
            status = DVDGetCommandBlockStatus(&file->cb);
            if (status == 0) {
                done = true;
            } else {
                HadError += CheckDVDError();
                if (HadError) {
                    DVDCancel(&file->cb);
                    done = false;
                    break;
                }
            }
            if (HadError)
                return -1;
        } while (status != 0);
        if (HadError)
            return -1;
    } while (!done);
    return length;
}

void* LoadElf::Get() {
    return (void*)0x80003100;
}

void LoadElf::RunModule(const char* path) {
    unsigned long size;
    int length;
    ElfHeader* header;
    ElfProgramHeader* segment;
    void (*entry)();
    void* addr;
    s32 entrynum;
    int count;
    int errors;
    BOOL opened;
    int i;
    DVDFileInfo file;
    header = (ElfHeader*)(((unsigned long)theCodePointer + 31) & ~31);
    errors = 0;
    do {
        HadError = 0;
        printf("LOADER: Now parsing and loading %s\n", path);
        HadError += CheckDVDError();
        errors += HadError;
        do {
            entrynum = DVDConvertPathToEntrynum(path);
            HadError += CheckDVDError();
        } while (entrynum == -1);
        errors += HadError;
        if (HadError)
            continue;
        do {
            opened = DVDFastOpen(entrynum, &file);
            HadError += CheckDVDError();
        } while (!opened);
        errors += HadError;
        if (HadError)
            continue;
        int read = ReadWithRetry(&file, header, 0x1000, 0);
        errors += HadError;
        if (HadError)
            continue;
        if (read != 0x1000) {
            printf("LOADER: DVDRead failed loading ELF header\n");
            errors++;
            HadError = 1;
            continue;
        }
        if (*(unsigned long*)header->ident != 0x7F454C46) {
            printf("LOADER: Fatal Error: ELF Magic number not present in header\n");
            errors++;
            HadError = 1;
            continue;
        }
        entry = (void (*)())header->entry;
        segment = (ElfProgramHeader*)((unsigned char*)header + header->phoff);
        count = header->phnum;
        for (i = 0; i < count; i++) {
            addr = (void*)segment->vaddr;
            size = segment->memsz;
            length = (segment->filesz + 31) & ~31;
            printf("LOADER: Now loading segment %d at %08x, size %08x\n", i, addr, size);
            memset(addr, 0, size);
            if (length > 0) {
                HadError += CheckDVDError();
                errors += HadError;
                if (HadError)
                    continue;
                int read = ReadWithRetry(&file, addr, length, segment->offset);
                if (read != length) {
                    printf("LOADER: DVDRead failed loading segment %d\n", i);
                    HadError = 1;
                    break;
                }
            }
            printf("LOADER: Clearing I and D cache\n");
            ICInvalidateRange(addr, length);
            DCFlushRange(addr, length);
            segment++;
        }
        DVDClose(&file);
        if (HadError) {
            printf("LOADER: Had DVD error processing ELF, reloading...\n");
            errors = 1;
        }
    } while (HadError);
    if (errors) {
        printf("LOADER: Calling AppCLeanup due to DVD error\n");
        if (gGxIsInit) {
            GXDrawDone();
            VISetBlack(TRUE);
            VIFlush();
            VIWaitForRetrace();
        }
    }
    PPCSync();
    printf("LOADER: Calling entry point at %08x\n", entry);
    OSResetSystem(OS_RESET_SHUTDOWN, OSGetResetCode(), FALSE);
    entry();
}
