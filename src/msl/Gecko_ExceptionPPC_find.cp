// Runtime Gecko_ExceptionPPC.cp: ExPPC_UnwindStack, which runs the unwind
// actions up to the catcher (destroying locals, members and arrays - each
// operand in a saved register or a frame slot by the record flags - deleting
// pointers, handing an active catch block's object to the throw context or
// destroying it, and popping frames whose action lists end, terminating on
// an unknown action); ExPPC_PopStackFrame, which restores the
// caller's saved FPRs (16-byte paired-single slots when record flag bit 1 is
// set, else doubles; the count is in bits 6-10) and GPRs (count in the top 5
// bits, saved below the FPRs) into the throw context through three inline
// copy loops, pops the stack pointer and returns the caller's return address;
// ExPPC_NextAction, which returns the next
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

typedef struct FPRSave {
    double d;
    float ps[2];
} FPRSave;

typedef struct ThrowContext {
    FPRSave FPR[32];
    long GPR[32];
    long CR;
    char* SP;
    char* FP;
    char* throwSP;
    char* returnaddr;
    char* throwtype;
    void* location;
    void* dtor;
} ThrowContext;

typedef struct CatchInfo {
    void* location;
    void* typeinfo;
    void* dtor;
} CatchInfo;

typedef void (*DtorFunc)(void*, short);
typedef void (*DeleteFunc)(void*);

/* the action records (the first byte is the action, bit 7 ends the list) */
typedef struct ex_branch {
    unsigned char action;
    unsigned char unused;
    unsigned short target;
} ex_branch;

typedef struct ex_destroylocal {
    unsigned char action;
    unsigned char unused;
    short local;
    void* dtor;
} ex_destroylocal;

typedef struct ex_destroylocalcond {
    unsigned char action;
    unsigned char flags;
    short cond;
    short local;
    short unused;
    void* dtor;
} ex_destroylocalcond;

typedef struct ex_destroylocalpointer {
    unsigned char action;
    unsigned char flags;
    short pointer;
    void* dtor;
} ex_destroylocalpointer;

typedef struct ex_destroylocalarray {
    unsigned char action;
    unsigned char unused;
    short localarray;
    unsigned short elements;
    unsigned short element_size;
    void* dtor;
} ex_destroylocalarray;

typedef struct ex_destroymember {
    unsigned char action;
    unsigned char flags;
    short objectptr;
    long offset;
    void* dtor;
} ex_destroymember;

typedef struct ex_destroymembercond {
    unsigned char action;
    unsigned char flags;
    short cond;
    short objectptr;
    short unused;
    long offset;
    void* dtor;
} ex_destroymembercond;

typedef struct ex_destroymemberarray {
    unsigned char action;
    unsigned char flags;
    short objectptr;
    long offset;
    long elements;
    long element_size;
    void* dtor;
} ex_destroymemberarray;

typedef struct ex_deletepointer {
    unsigned char action;
    unsigned char flags;
    short objectptr;
    void* deletefunc;
} ex_deletepointer;

typedef struct ex_deletepointercond {
    unsigned char action;
    unsigned char flags;
    short cond;
    short objectptr;
    short unused;
    void* deletefunc;
} ex_deletepointercond;

typedef struct ex_activecatchblock {
    unsigned char action;
    unsigned char unused;
    short cinfo_ref;
} ex_activecatchblock;

typedef struct ex_specification {
    unsigned char action;
    unsigned char unused;
    unsigned short specs;
    long pcoffset;
    long cinfo_ref;
} ex_specification;

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

static inline void RestorePSFPRs(ThrowContext* context, FPRSave* save, int count) {
    int i;
    for (i = 32 - count; i < 32; i++, save++) {
        context->FPR[i].ps[0] = save->ps[0];
        context->FPR[i].ps[1] = save->ps[1];
        context->FPR[i].d = save->d;
    }
}

static inline void RestoreFPRs(ThrowContext* context, double* save, int count) {
    int i;
    for (i = 32 - count; i < 32; i++)
        context->FPR[i].d = *save++;
}

static inline void RestoreGPRs(ThrowContext* context, long* save, int count) {
    int i;
    for (i = 32 - count; i < 32; i++)
        context->GPR[i] = *save++;
}

static char* ExPPC_PopStackFrame(ThrowContext* context, MWExceptionInfo* info) {
    char* SP;
    char* callers_SP;
    char* FPR_save_area;
    long* GPR_save_area;
    int has_ps;
    FPRSave* ps_area;
    int saved_FPRs;
    int saved_GPRs;

    SP = context->SP;
    callers_SP = *(char**)SP;
    has_ps = (*(unsigned short*)info->exception_record >> 1) & 1;
    saved_FPRs = (*(unsigned short*)info->exception_record >> 6) & 0x1F;
    if (has_ps) {
        ps_area = (FPRSave*)(callers_SP - saved_FPRs * 16);
        FPR_save_area = (char*)ps_area;
    } else {
        FPR_save_area = callers_SP - saved_FPRs * 8;
    }

    if (has_ps)
        RestorePSFPRs(context, ps_area, saved_FPRs);
    else
        RestoreFPRs(context, (double*)FPR_save_area, saved_FPRs);

    GPR_save_area = (long*)FPR_save_area;
    saved_GPRs = *(unsigned short*)info->exception_record >> 11;
    GPR_save_area -= saved_GPRs;
    RestoreGPRs(context, GPR_save_area, saved_GPRs);

    context->SP = callers_SP;
    return *(char**)(callers_SP + 4);
}

static void ExPPC_UnwindStack(ThrowContext* context, MWExceptionInfo* info, void* catcher) {
    char* ap;
    int action;

    for (;;) {
        if ((ap = info->action_pointer) == 0) {
            ExPPC_FindExceptionRecord(ExPPC_PopStackFrame(context, info), info);
            if (info->exception_record == 0)
                std::terminate();
            context->FP = ((*(unsigned short*)info->exception_record >> 4) & 1) ? (char*)context->GPR[31]
                                                                               : context->SP;
            continue;
        }

        action = *(exaction_type*)ap;
        switch (action & 0x7F) {
        case 1:
            info->action_pointer = info->exception_record + ((ex_branch*)ap)->target;
            break;
        case 2: {
            ex_destroylocal* p = (ex_destroylocal*)ap;
            ((DtorFunc)p->dtor)(context->FP + p->local, -1);
            info->action_pointer += sizeof(ex_destroylocal);
            break;
        }
        case 3: {
            ex_destroylocalcond* p = (ex_destroylocalcond*)ap;
            char cond;
            if (p->flags >> 7)
                cond = (char)context->GPR[p->cond];
            else
                cond = *(char*)(context->FP + p->cond);
            if (cond)
                ((DtorFunc)p->dtor)(context->FP + p->local, -1);
            info->action_pointer += sizeof(ex_destroylocalcond);
            break;
        }
        case 4: {
            ex_destroylocalpointer* p = (ex_destroylocalpointer*)ap;
            void* pointer;
            if (p->flags >> 7)
                pointer = (void*)context->GPR[p->pointer];
            else
                pointer = *(void**)(context->FP + p->pointer);
            ((DtorFunc)p->dtor)(pointer, -1);
            info->action_pointer += sizeof(ex_destroylocalpointer);
            break;
        }
        case 5: {
            ex_destroylocalarray* p = (ex_destroylocalarray*)ap;
            int size;
            int elements;
            elements = p->elements;
            size = p->element_size;
            char* ptr = context->FP + p->localarray;
            ptr += size * elements;
            while (elements > 0) {
                ptr -= size;
                ((DtorFunc)p->dtor)(ptr, -1);
                elements--;
            }
            info->action_pointer += sizeof(ex_destroylocalarray);
            break;
        }
        case 6: {
            ex_destroymember* p = (ex_destroymember*)ap;
            char* object;
            if (p->flags >> 7)
                object = (char*)context->GPR[p->objectptr];
            else
                object = *(char**)(context->FP + p->objectptr);
            ((DtorFunc)p->dtor)(object + p->offset, 0);
            info->action_pointer += sizeof(ex_destroymember);
            break;
        }
        case 7: {
            ex_destroymember* p = (ex_destroymember*)ap;
            char* object;
            if (p->flags >> 7)
                object = (char*)context->GPR[p->objectptr];
            else
                object = *(char**)(context->FP + p->objectptr);
            ((DtorFunc)p->dtor)(object + p->offset, -1);
            info->action_pointer += sizeof(ex_destroymember);
            break;
        }
        case 8: {
            ex_destroymembercond* p = (ex_destroymembercond*)ap;
            char* object;
            short cond;
            if ((p->flags >> 6) & 1)
                object = (char*)context->GPR[p->objectptr];
            else
                object = *(char**)(context->FP + p->objectptr);
            if (p->flags >> 7)
                cond = (short)context->GPR[p->cond];
            else
                cond = *(short*)(context->FP + p->cond);
            if (cond)
                ((DtorFunc)p->dtor)(object + p->offset, 0);
            info->action_pointer += sizeof(ex_destroymembercond);
            break;
        }
        case 9: {
            ex_destroymemberarray* p = (ex_destroymemberarray*)ap;
            long size;
            long elements;
            char* ptr;
            if (p->flags >> 7)
                ptr = (char*)context->GPR[p->objectptr];
            else
                ptr = *(char**)(context->FP + p->objectptr);
            elements = p->elements;
            size = p->element_size;
            ptr += p->offset;
            ptr += size * elements;
            while (elements > 0) {
                ptr -= size;
                ((DtorFunc)p->dtor)(ptr, -1);
                elements--;
            }
            info->action_pointer += sizeof(ex_destroymemberarray);
            break;
        }
        case 10: {
            ex_deletepointer* p = (ex_deletepointer*)ap;
            void* pointer;
            if (p->flags >> 7)
                pointer = (void*)context->GPR[p->objectptr];
            else
                pointer = *(void**)(context->FP + p->objectptr);
            ((DeleteFunc)p->deletefunc)(pointer);
            info->action_pointer += sizeof(ex_deletepointer);
            break;
        }
        case 11: {
            ex_deletepointercond* p = (ex_deletepointercond*)ap;
            void* pointer;
            char cond;
            if ((p->flags >> 6) & 1)
                pointer = (void*)context->GPR[p->objectptr];
            else
                pointer = *(void**)(context->FP + p->objectptr);
            if (p->flags >> 7)
                cond = (char)context->GPR[p->cond];
            else
                cond = *(char*)(context->FP + p->cond);
            if (cond)
                ((DeleteFunc)p->deletefunc)(pointer);
            info->action_pointer += sizeof(ex_deletepointercond);
            break;
        }
        case 12:
            if (catcher == ap)
                return;
            info->action_pointer = ap + 12;
            break;
        case 16:
            if (catcher == ap)
                return;
            info->action_pointer = ap + 16;
            break;
        case 13: {
            CatchInfo* catchinfo = (CatchInfo*)(context->FP + ((ex_activecatchblock*)ap)->cinfo_ref);
            if (catchinfo->dtor) {
                if (context->location == catchinfo->location)
                    context->dtor = catchinfo->dtor;
                else
                    ((DtorFunc)catchinfo->dtor)(catchinfo->location, -1);
            }
            info->action_pointer += sizeof(ex_activecatchblock);
            break;
        }
        case 15:
            if (catcher == ap)
                return;
            info->action_pointer = ap + (sizeof(ex_specification) + ((ex_specification*)ap)->specs * 4);
            break;
        default:
            std::terminate();
        }

        if (action & 0x80)
            info->action_pointer = 0;
    }
}
