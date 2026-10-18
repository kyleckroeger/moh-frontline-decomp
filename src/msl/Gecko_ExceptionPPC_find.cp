// Runtime Gecko_ExceptionPPC.cp: ExPPC_FindExceptionRecord, which finds the
// exception record for a return address: the registered fragment whose code
// range holds the address (an inline search of the fragment's exception table
// index ranges, only the index bounds, TOC and active flag are kept), then a
// binary search of the fragment's function index (12-byte entries: function
// offset, size with the "record inline" flag in the top bit, record offset),
// then the record's code ranges (large 8-byte ranges when bit 3 of the
// record flags is set, else small 6-byte ranges) for the action at the
// offset. Reconstructed from the disassembly; the structure layouts are
// inferred from the offsets. The rest of the file is in other units.
#include "__ppc_eabi_linker.h"

typedef struct ProcessInfo {
    __eti_init_info* exception_info;
    char* TOC;
    int active;
} ProcessInfo;

typedef struct FragmentInfo {
    struct ExceptionTableIndex* exception_start;
    struct ExceptionTableIndex* exception_end;
    char* code_start;
    char* code_end;
    char* data_start;
    char* data_end;
    char* TOC;
    int active;
} FragmentInfo;

typedef struct ExceptionTableIndex {
    unsigned long functionoffset;
    unsigned long functionsize;
    unsigned long exceptionoffset;
} ExceptionTableIndex;

typedef struct ExceptionRangeSmall {
    unsigned short start;
    unsigned short end;
    unsigned short action;
} ExceptionRangeSmall;

typedef struct ExceptionRangeLarge {
    unsigned long start;
    unsigned short size;
    unsigned short action;
} ExceptionRangeLarge;

typedef struct MWExceptionInfo {
    char* exception_record;
    char* current_function;
    char* action_pointer;
    char* code_start;
    char* data_start;
    char* TOC;
} MWExceptionInfo;

extern ProcessInfo fragmentinfo[1];

static inline int ExPPC_FindExceptionFragment(char* returnaddr, FragmentInfo* frag) {
    ProcessInfo* f;
    int i;
    __eti_init_info* eti_info;

    for (i = 0, f = fragmentinfo; i < 1; ++i, ++f) {
        if (f->active) {
            for (eti_info = f->exception_info;; ++eti_info) {
                if (eti_info->code_size == 0)
                    break;
                if (returnaddr >= (char*)eti_info->code_start
                    && returnaddr < (char*)eti_info->code_start + eti_info->code_size) {
                    frag->exception_start = (ExceptionTableIndex*)eti_info->eti_start;
                    frag->exception_end = (ExceptionTableIndex*)eti_info->eti_end;
                    frag->code_start = 0;
                    frag->code_end = 0;
                    frag->data_start = 0;
                    frag->data_end = 0;
                    frag->TOC = f->TOC;
                    frag->active = f->active;
                    return 1;
                }
            }
        }
    }
    return 0;
}

static void ExPPC_FindExceptionRecord(char* returnaddr, MWExceptionInfo* info) {
    FragmentInfo* fragment;
    FragmentInfo frag;
    ExceptionTableIndex *exceptionindex, *p;
    unsigned long returnoffset;
    long i, m, n;

    info->exception_record = 0;
    info->action_pointer = 0;

    if (ExPPC_FindExceptionFragment(returnaddr, &frag) == 0)
        return;
    fragment = &frag;

    info->code_start = fragment->code_start;
    info->data_start = fragment->data_start;
    info->TOC = fragment->TOC;

    returnoffset = returnaddr - fragment->code_start;
    exceptionindex = fragment->exception_start;
    for (i = 0, n = fragment->exception_end - fragment->exception_start;;) {
        if (i > n)
            return;
        p = &exceptionindex[m = (i + n) / 2];

        if (returnoffset < p->functionoffset) {
            n = m - 1;
        } else if (returnoffset > p->functionoffset + (p->functionsize & 0x7FFFFFFF)) {
            i = m + 1;
        } else {
            break;
        }
    }

    info->current_function = fragment->code_start + p->functionoffset;
    info->exception_record = (p->functionsize >> 31) ? (char*)&p->exceptionoffset
                                                     : fragment->data_start + p->exceptionoffset;

    returnoffset -= p->functionoffset;

    if ((*(unsigned short*)info->exception_record >> 3) & 1) {
        ExceptionRangeLarge* range = (ExceptionRangeLarge*)(info->exception_record + 4);
        for (; range->start != 0; range++) {
            unsigned long start = range->start;
            unsigned long end = start + range->size * 4;
            if (start <= returnoffset && end >= returnoffset) {
                info->action_pointer = info->exception_record + range->action;
                break;
            }
        }
    } else {
        ExceptionRangeSmall* range = (ExceptionRangeSmall*)(info->exception_record + 2);
        for (; range->start != 0; range++) {
            if (range->start <= returnoffset && range->end >= returnoffset) {
                info->action_pointer = info->exception_record + range->action;
                break;
            }
        }
    }
}
