// Runtime Gecko_ExceptionPPC.cp: ExPPC_NextAction, which returns the next
// unwind action (an action list ends with bit 7; at its end the caller's
// frame is popped: its saved r31 is read below the saved FPRs when the record
// saves GPRs, the caller's record is looked up from the return address (no
// record terminates), and the frame pointer is r31 or the stack pointer by
// record flag bit 4; the known actions are stepped over by their sizes,
// others terminate, and a branch action (1) jumps within the record), and
// ExPPC_FindExceptionRecord, which finds the
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

typedef struct ActionIterator {
    MWExceptionInfo info;
    char* current_SP;
    char* current_FP;
    long current_R31;
} ActionIterator;

typedef unsigned char exaction_type;

namespace std {
void terminate();
}

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

static exaction_type ExPPC_NextAction(ActionIterator* iter) {
    char* SP;
    char* callers_SP;
    char* return_addr;
    exaction_type action;
    unsigned short flags;

    for (;;) {
        if (iter->info.action_pointer == 0
            || ((action = *(exaction_type*)iter->info.action_pointer) & 0x80) != 0) {
            flags = *(unsigned short*)iter->info.exception_record;
            SP = iter->current_SP;
            callers_SP = *(char**)SP;
            if ((flags >> 11) != 0)
                iter->current_R31 = *(long*)(callers_SP - ((flags >> 3) & 0xF8) - 4);
            return_addr = *(char**)(callers_SP + 4);
            ExPPC_FindExceptionRecord(return_addr, &iter->info);
            if (iter->info.exception_record == 0)
                std::terminate();
            iter->current_SP = callers_SP;
            iter->current_FP = ((*(unsigned short*)iter->info.exception_record >> 4) & 1) ? (char*)iter->current_R31
                                                                                         : iter->current_SP;
            if (iter->info.action_pointer == 0)
                continue;
        } else {
            switch (action) {
            case 2:
                iter->info.action_pointer += 8;
                break;
            case 3:
                iter->info.action_pointer += 12;
                break;
            case 4:
                iter->info.action_pointer += 8;
                break;
            case 5:
                iter->info.action_pointer += 12;
                break;
            case 6:
            case 7:
                iter->info.action_pointer += 12;
                break;
            case 8:
                iter->info.action_pointer += 16;
                break;
            case 9:
                iter->info.action_pointer += 20;
                break;
            case 10:
                iter->info.action_pointer += 8;
                break;
            case 11:
                iter->info.action_pointer += 12;
                break;
            case 12:
                iter->info.action_pointer += 12;
                break;
            case 16:
                iter->info.action_pointer += 16;
                break;
            case 13:
                iter->info.action_pointer += 4;
                break;
            case 15:
                iter->info.action_pointer += 12 + ((unsigned short*)iter->info.action_pointer)[1] * 4;
                break;
            default:
                std::terminate();
            }
        }

        action = *(exaction_type*)iter->info.action_pointer & 0x7F;
        if (action == 1) {
            iter->info.action_pointer = iter->info.exception_record + ((unsigned short*)iter->info.action_pointer)[1];
            action = *(exaction_type*)iter->info.action_pointer & 0x7F;
        }
        return action;
    }
}
