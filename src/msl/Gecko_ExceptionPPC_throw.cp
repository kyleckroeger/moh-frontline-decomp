// Runtime Gecko_ExceptionPPC.cp: ExPPC_ThrowHandler, which handles a throw:
// it looks up the thrower's exception record (none terminates) and sets the
// frame pointer; a rethrow (no throw type) walks the actions to the active
// catch block and takes over its exception object, type and location (any
// other stop terminates); then the actions are walked for a catch block
// (16- or 32-bit) whose type matches the thrown type, unwinding a
// specification that does not allow the type into its unexpected handler (an
// inline: unwind, then its catch information gets the exception and the
// specification before the long jump), and the matching catch block gets
// the exception object (through a pointer copy for pointer types) before the
// long jump to the handler. Reconstructed from
// the disassembly; the record and catch layouts are inferred from the
// offsets. The rest of the file is in other units.
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
    void* catchinfo;
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

typedef struct ex_catchblock {
    unsigned char action;
    unsigned char unused;
    unsigned short unused2;
    char* catch_type;
    unsigned short catch_pcoffset;
    short cinfo_ref;
} ex_catchblock;

typedef struct ex_catchblock_32 {
    unsigned char action;
    unsigned char unused;
    unsigned short unused2;
    char* catch_type;
    long catch_pcoffset;
    long cinfo_ref;
} ex_catchblock_32;

typedef struct ex_specification_full {
    unsigned char action;
    unsigned char unused;
    unsigned short specs;
    long pcoffset;
    long cinfo_ref;
    char* spec[1];
} ex_specification_full;

typedef struct CatchInfoFull {
    void* location;
    void* typeinfo;
    void* dtor;
    void* sublocation;
    long pointercopy;
    void* stacktop;
} CatchInfoFull;

extern "C" char __throw_catch_compare(const char*, const char*, long*);

void ExPPC_FindExceptionRecord(char* returnaddr, MWExceptionInfo* info);
exaction_type ExPPC_NextAction(ActionIterator* iter);
void ExPPC_UnwindStack(ThrowContext* context, MWExceptionInfo* info, void* catcher);
void ExPPC_LongJump(ThrowContext* context, void* TOC, void* target);

static inline exaction_type ExPPC_CurrentAction(const ActionIterator* iter) {
    if (iter->info.action_pointer == 0)
        return 0;
    return *(exaction_type*)iter->info.action_pointer & 0x7F;
}

static inline int ExPPC_IsInSpecification(char* extype, ex_specification_full* spec) {
    long offset;
    int i;

    for (i = 0; i < spec->specs; i++) {
        if (__throw_catch_compare(extype, spec->spec[i], &offset))
            return 1;
    }
    return 0;
}

static inline void ExPPC_HandleUnexpected(ThrowContext* context, MWExceptionInfo* info, ex_specification_full* spec) {
    CatchInfoFull* catchinfo;

    ExPPC_UnwindStack(context, info, spec);
    catchinfo = (CatchInfoFull*)(context->FP + spec->cinfo_ref);
    catchinfo->location = context->location;
    catchinfo->typeinfo = context->throwtype;
    catchinfo->dtor = context->dtor;
    catchinfo->stacktop = spec;
    ExPPC_LongJump(context, info->TOC, info->current_function + spec->pcoffset);
}

static void ExPPC_ThrowHandler(ThrowContext* context) {
    ActionIterator iter;
    MWExceptionInfo info;
    exaction_type action;
    CatchInfoFull* catchinfo;
    long offset;

    ExPPC_FindExceptionRecord(context->returnaddr, &info);
    if (info.exception_record == 0)
        std::terminate();
    context->FP = ((*(unsigned short*)info.exception_record >> 4) & 1) ? (char*)context->GPR[31] : context->SP;

    if (context->throwtype == 0) {
        iter.info = info;
        iter.current_SP = context->SP;
        iter.current_FP = context->FP;
        iter.current_R31 = context->GPR[31];
        for (action = ExPPC_CurrentAction(&iter);; action = ExPPC_NextAction(&iter)) {
            switch (action) {
            case 13:
                break;
            case 0:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 15:
            case 16:
                continue;
            case 1:
            case 14:
            default:
                std::terminate();
            }
            break;
        }
        catchinfo = (CatchInfoFull*)(iter.current_FP + ((ex_activecatchblock*)iter.info.action_pointer)->cinfo_ref);
        context->throwtype = (char*)catchinfo->typeinfo;
        context->location = catchinfo->location;
        context->dtor = 0;
        context->catchinfo = catchinfo;
    } else {
        context->catchinfo = 0;
    }

    iter.info = info;
    iter.current_SP = context->SP;
    iter.current_FP = context->FP;
    iter.current_R31 = context->GPR[31];
    for (action = ExPPC_CurrentAction(&iter);; action = ExPPC_NextAction(&iter)) {
        switch (action) {
        case 16:
            if (__throw_catch_compare(context->throwtype, ((ex_catchblock_32*)iter.info.action_pointer)->catch_type, &offset))
                break;
            continue;
        case 12:
            if (__throw_catch_compare(context->throwtype, ((ex_catchblock*)iter.info.action_pointer)->catch_type, &offset))
                break;
            continue;
        case 15:
            if (!ExPPC_IsInSpecification(context->throwtype, (ex_specification_full*)iter.info.action_pointer))
                ExPPC_HandleUnexpected(context, &info, (ex_specification_full*)iter.info.action_pointer);
            continue;
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 13:
            continue;
        case 1:
        case 14:
        default:
            std::terminate();
        }
        break;
    }

    if (action == 16) {
        ex_catchblock_32* catchblock = (ex_catchblock_32*)iter.info.action_pointer;
        ExPPC_UnwindStack(context, &info, catchblock);
        catchinfo = (CatchInfoFull*)(context->FP + catchblock->cinfo_ref);
        catchinfo->location = context->location;
        catchinfo->typeinfo = context->throwtype;
        catchinfo->dtor = context->dtor;
        if (*context->throwtype == '*') {
            catchinfo->sublocation = &catchinfo->pointercopy;
            catchinfo->pointercopy = *(long*)context->location + offset;
        } else {
            catchinfo->sublocation = (char*)context->location + offset;
        }
        ExPPC_LongJump(context, info.TOC, info.current_function + catchblock->catch_pcoffset);
    } else {
        ex_catchblock* catchblock = (ex_catchblock*)iter.info.action_pointer;
        ExPPC_UnwindStack(context, &info, catchblock);
        catchinfo = (CatchInfoFull*)(context->FP + catchblock->cinfo_ref);
        catchinfo->location = context->location;
        catchinfo->typeinfo = context->throwtype;
        catchinfo->dtor = context->dtor;
        if (*context->throwtype == '*') {
            catchinfo->sublocation = &catchinfo->pointercopy;
            catchinfo->pointercopy = *(long*)context->location + offset;
        } else {
            catchinfo->sublocation = (char*)context->location + offset;
        }
        ExPPC_LongJump(context, info.TOC, info.current_function + catchblock->catch_pcoffset);
    }
}
