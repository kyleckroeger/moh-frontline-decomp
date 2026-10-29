// UISStack.c (0x80085a8c, whole file): UISStackProcess, the UI script
// interpreter. A screen script is byte code run on a downward-addressed
// parameter stack (UISStackInfo_t: pc at +16, stack top at +12); each opcode
// byte selects one case. The opcode numbers, the called functions and the
// type names come from the mangled symbols and the original jump table; the
// operand meanings, record views and field names are inferred. Operands are
// big-endian immediates read byte by byte from the script. The case order is
// the original code layout order. Case 0x6a is a no-op (an int-to-pointer
// conversion of the top slot).
#include <string.h>

struct UISScrData_t {
    char field0[8];
    unsigned long count;
    char* table;
};

struct UISString_t {
    long field0;
    unsigned long length;
    char* data;
};

union UISParam_t {
    long i;
    unsigned long u;
    float f;
    void* p;
    UISParam_t* pp;
    long* lp;
    char* cp;
    UISString_t* s;
};

struct UISScreen_t {
    unsigned short id0;
    unsigned short id1;
    unsigned short u4;
    unsigned short u6;
    UISScrData_t* data;
};

struct UISControlInfo_t {
    long field0;
    unsigned long field4;
};

struct UISStackInfo_t {
    UISParam_t* field0;
    UISParam_t* base;
    UISParam_t* field8;
    UISParam_t* sp;
    unsigned char* pc;
};

struct UISThreadGroupInfoT {
    unsigned short u0;
    unsigned short u2;
    union {
        struct {
            unsigned short u4;
            unsigned short u6;
        } h;
        long l4;
        void* p4;
    };
    long l8;
    unsigned short u12;
    unsigned short u14;
};

enum UISThreadActionT { UISThreadActionUnknown = 0 };

struct UISSavedStack_t {
    UISStackInfo_t stack;
    UISControlInfo_t* control;
    UISScreen_t* screen;
    UISParam_t* param;
    UISStackInfo_t* parent;
    unsigned short u36;
    unsigned short u38;
};

typedef void (*UISCallFnc)(long, long, long, UISParam_t*, UISParam_t*);
typedef void (*UISExtFnc)(long, unsigned short, unsigned short, long, UISParam_t*, UISParam_t*);

struct UISInfo_t {
    char field00[12];
    UISExtFnc extFnc;
    char field10[16];
    unsigned long flags;
    char field24[24];
    UISCallFnc* fncTable;
    char field40[4];
    unsigned long rateFnc;
    char field48[4];
    unsigned long savedMax;
    unsigned long savedCount;
    UISSavedStack_t* saved;
};

void UISAddThreadAction(UISInfo_t*, UISThreadActionT, UISThreadGroupInfoT*, long, UISParam_t*);
void UISLoadRateFnc(UISInfo_t*, UISScreen_t*, UISControlInfo_t*, unsigned long, unsigned char*, unsigned long);
void UISUnloadRateFnc(UISInfo_t*, UISControlInfo_t*, unsigned long);
void UISLoadAdvRateFnc(UISInfo_t*, UISScreen_t*, UISControlInfo_t*, UISControlInfo_t*, unsigned long, unsigned char*,
                       unsigned char*, unsigned long, float, unsigned long);
unsigned long UISFindRateFnc(UISInfo_t*, UISControlInfo_t*, unsigned long);
UISControlInfo_t* UISFindSiblingEnableControl(UISScreen_t*, UISControlInfo_t*);
void UISStringFormat(UISScrData_t*, UISString_t*, UISString_t*, unsigned long, UISParam_t*);
void UISUpdateVisibility(UISInfo_t*, UISScreen_t*, unsigned long, void*, unsigned long);

static UISParam_t uisFormatStringStack[20];
extern unsigned char uisEventNotProcess;

#define LONGC(v)                           \
    {                                      \
        unsigned char b0 = *stack->pc;     \
        stack->pc++;                       \
        unsigned char b1 = *stack->pc;     \
        stack->pc++;                       \
        unsigned char b2 = *stack->pc;     \
        stack->pc++;                       \
        unsigned char b3 = *stack->pc;     \
        stack->pc++;                       \
        v = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3; \
    }
#define LONG(v)                            \
    {                                      \
        unsigned long b0 = *stack->pc;     \
        stack->pc++;                       \
        unsigned long b1 = *stack->pc;     \
        stack->pc++;                       \
        unsigned long b2 = *stack->pc;     \
        stack->pc++;                       \
        unsigned long b3 = *stack->pc;     \
        stack->pc++;                       \
        v = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3; \
    }
inline short ReadShort(UISStackInfo_t* stack) {
    unsigned long b1;
    unsigned long b0 = *stack->pc;
    stack->pc++;
    b1 = *stack->pc;
    stack->pc++;
    return (b0 << 8) | b1;
}
#define SHORT() ReadShort(stack)
#define POP() (*--stack->sp)
#define PUSH(m, x) stack->sp->m = (x); stack->sp++

inline unsigned char* UISScrAddr(UISScreen_t* screen, long offset) {
    return (unsigned char*)screen->data + offset;
}
long UISStackProcess(UISInfo_t* info, UISParam_t* param, UISStackInfo_t* stack, UISScreen_t* screen,
                     UISControlInfo_t* control) {
    UISThreadGroupInfoT group;
    long bits;
    UISParam_t arg;
    UISParam_t value;
    while (stack->pc != 0) {
        UISParam_t* sp = stack->sp;
        unsigned char op = *stack->pc;
        stack->pc++;
        switch (op) {
        case 2:
        case 0x71: {
            unsigned char count = *stack->pc;
            stack->pc++;
            UISParam_t* params = stack->sp - count;
            group.u0 = params[-1].u;
            group.u2 = params[-1].u >> 16;
            group.h.u4 = screen->id0;
            group.h.u6 = screen->id1;
            UISAddThreadAction(info, (UISThreadActionT)0, &group, count, params);
            while (count--)
                stack->sp--;
            stack->sp--;
            break;
        }
        case 3: {
            long l8 = POP().i;
            unsigned long v = POP().u;
            unsigned short lo = v;
            unsigned short hi = v >> 16;
            if (lo == 0xffff) {
                lo = screen->id0;
                hi = screen->id1;
            }
            group.u0 = lo;
            group.u2 = hi;
            group.l8 = l8;
            UISAddThreadAction(info, (UISThreadActionT)1, &group, 0, 0);
            break;
        }
        case 8: {
            unsigned long v = POP().u;
            unsigned short lo = v;
            unsigned short hi = v >> 16;
            if (lo == 0xffff) {
                lo = screen->id0;
                hi = screen->id1;
            }
            group.u0 = lo;
            group.u2 = hi;
            UISAddThreadAction(info, (UISThreadActionT)3, &group, 0, 0);
            break;
        }
        case 88: {
            unsigned char kind = *stack->pc;
            stack->pc++;
            unsigned char* a = 0;
            unsigned char* b = 0;
            UISControlInfo_t* c0 = (UISControlInfo_t*)POP().p;
            UISControlInfo_t* c1 = (UISControlInfo_t*)POP().p;
            if (kind >= 5) {
                if (kind >= 6)
                    b = UISScrAddr(screen, POP().i);
                a = UISScrAddr(screen, POP().i);
            }
            unsigned long u0 = POP().u;
            float f = POP().f;
            unsigned long u1 = POP().u;
            unsigned long x = POP().u;
            UISLoadAdvRateFnc(info, screen, c0, c1, x, a, b, u0, f, u1);
            break;
        }
        case 6: {
            UISControlInfo_t* c = (UISControlInfo_t*)POP().p;
            unsigned long b = POP().u;
            unsigned char* x = UISScrAddr(screen, POP().i);
            unsigned long a = POP().u;
            UISLoadRateFnc(info, screen, c, a, x, b);
            break;
        }
        case 7: {
            UISControlInfo_t* a = (UISControlInfo_t*)POP().p;
            UISUnloadRateFnc(info, a, POP().u);
            break;
        }
        case 10: {
            long n = POP().i;
            long idx = POP().i;
            long b = POP().i;
            long a = POP().i;
            UISParam_t* params = stack->sp - n;
            info->fncTable[idx](a, b, n, params, params - 1);
            break;
        }
        case 0x76: {
            long n = POP().i;
            stack->sp--;
            UISParam_t* params = stack->sp - n;
            *(long*)&group = params[0].i;
            UISAddThreadAction(info, (UISThreadActionT)9, &group, n, params + 1);
            break;
        }
        case 11: {
            unsigned short id0 = screen->id0;
            unsigned short id1 = screen->id1;
            long n = POP().i;
            stack->sp--;
            UISParam_t* params = stack->sp - n;
            info->extFnc(params[0].i, id0, id1, n, params + 1, params - 1);
            break;
        }
        case 12: {
            long n = POP().i;
            *(long*)&group = POP().i;
            group.l4 = POP().i;
            UISAddThreadAction(info, (UISThreadActionT)7, &group, n, stack->sp - n);
            break;
        }
        case 13:
            sp->p = screen->data;
            stack->sp++;
            break;
        case 14: {
            UISString_t* dst = POP().s;
            UISString_t* src = POP().s;
            unsigned long n = POP().u;
            if (dst->length < n)
                n = dst->length;
            if (src->length < n)
                n = src->length;
            strncpy(dst->data, src->data, n);
            break;
        }
        case 15:
        case 16: {
            long v;
            LONG(v);
            PUSH(i, v);
            break;
        }
        case 17: {
            LONG(bits);
            PUSH(f, *(float*)&bits);
            break;
        }
        case 18:
            sp[-1].i = (long)sp[-1].f;
            break;
        case 19:
            sp[-1].f = (float)sp[-1].i;
            break;
        case 20:
            sp[-2].i = (long)sp[-2].f;
            break;
        case 21:
            sp[-2].f = (float)sp[-2].i;
            break;
        case 106:
            sp[-1].p = (void*)sp[-1].i;
            break;
        case 24:
            sp[-1].i = *sp[-1].lp;
            break;
        case 25:
            *sp[-1].lp = sp[-2].i;
            stack->sp -= 2;
            break;
        case 105:
            sp[-1].p = &sp[sp[-1].i - 1];
            break;
        case 26:
            sp[-1] = (sp - 1)[sp[-1].i];
            break;
        case 27:
            {
                long idx = sp[-1].i - 1;
                sp[idx].i = sp[-2].i;
            }
            stack->sp -= 2;
            break;
        case 28: {
            unsigned long a = POP().u;
            unsigned long b = POP().u;
            PUSH(u, a & b);
            break;
        }
        case 29: {
            unsigned long a = POP().u;
            unsigned long b = POP().u;
            PUSH(u, a | b);
            break;
        }
        case 30: {
            unsigned long a = POP().u;
            PUSH(u, ~a);
            break;
        }
        case 31: {
            long a = POP().i;
            long b = POP().i;
            PUSH(i, a && b);
            break;
        }
        case 32: {
            long a = POP().i;
            long b = POP().i;
            PUSH(i, a || b);
            break;
        }
        case 33: {
            long a = POP().i;
            PUSH(i, !a);
            break;
        }
        case 34:
            sp[-1].i = __abs(sp[-1].i);
            break;
        case 35: {
            float a = POP().f;
            PUSH(f, a < 0.0f ? -a : a);
            break;
        }
        case 36:
            stack->sp[-1].i = -sp[-1].i;
            break;
        case 37:
            sp[-2].i += sp[-1].i;
            stack->sp--;
            break;
        case 38:
            stack->sp[-2].i -= stack->sp[-1].i;
            stack->sp--;
            break;
        case 39:
            sp[-2].i *= sp[-1].i;
            stack->sp--;
            break;
        case 40:
            sp[-2].i /= sp[-1].i;
            stack->sp--;
            break;
        case 41: {
            float a = POP().f;
            PUSH(f, -a);
            break;
        }
        case 42:
            sp[-2].f += sp[-1].f;
            stack->sp--;
            break;
        case 43:
            sp[-2].f -= sp[-1].f;
            stack->sp--;
            break;
        case 44:
            sp[-2].f *= sp[-1].f;
            stack->sp--;
            break;
        case 45:
            sp[-2].f /= sp[-1].f;
            stack->sp--;
            break;
        case 46:
            sp[-1].i++;
            break;
        case 47:
            sp[-1].i--;
            break;
        case 48:
            sp[-1].f += 1.0f;
            break;
        case 49:
            sp[-1].f -= 1.0f;
            break;
        case 50:
            sp[-2].i = sp[-1].i <= sp[-2].i;
            stack->sp--;
            break;
        case 51:
            sp[-2].i = sp[-1].i >= sp[-2].i;
            stack->sp--;
            break;
        case 52:
            sp[-2].i = sp[-1].i < sp[-2].i;
            stack->sp--;
            break;
        case 53:
            sp[-2].i = sp[-1].i > sp[-2].i;
            stack->sp--;
            break;
        case 54:
            sp[-2].i = sp[-1].i == sp[-2].i;
            stack->sp--;
            break;
        case 55:
            sp[-2].i = sp[-1].i != sp[-2].i;
            stack->sp--;
            break;
        case 56:
            sp[-2].i = sp[-1].f <= sp[-2].f;
            stack->sp--;
            break;
        case 57:
            sp[-2].i = sp[-1].f >= sp[-2].f;
            stack->sp--;
            break;
        case 58:
            sp[-2].i = sp[-1].f < sp[-2].f;
            stack->sp--;
            break;
        case 59:
            sp[-2].i = sp[-1].f > sp[-2].f;
            stack->sp--;
            break;
        case 60:
            sp[-2].i = sp[-1].f == sp[-2].f;
            stack->sp--;
            break;
        case 61:
            sp[-2].i = sp[-1].f != sp[-2].f;
            stack->sp--;
            break;
        case 62: {
            long offset = POP().i;
            if (POP().u != 0)
                stack->pc += offset;
            break;
        }
        case 63: {
            long offset = POP().i;
            if (POP().u == 0)
                stack->pc += offset;
            break;
        }
        case 64:
            {
                long offset = POP().i;
                stack->pc += offset;
            }
            break;
        case 65:
            sp[0] = sp[-1];
            stack->sp++;
            break;
        case 66:
            stack->sp--;
            break;
        case 67: {
            long offset;
            LONG(offset);
            PUSH(p, stack->pc);
            stack->pc = (unsigned char*)screen->data + offset;
            break;
        }
        case 68:
            stack->pc = (unsigned char*)POP().p;
            break;
        case 69: {
            unsigned char count = *stack->pc;
            stack->pc++;
            unsigned long id = *(unsigned long*)(*POP().lp + (long)screen->data);
            if (id != -1) {
                group.u0 = id;
                group.u2 = id >> 16;
                group.h.u4 = screen->id0;
                group.h.u6 = screen->id1;
                UISAddThreadAction(info, (UISThreadActionT)0, &group, count, stack->sp - count);
                while (count--)
                    stack->sp--;
            }
            break;
        }
        case 70: {
            long v;
            long* entry = (long*)(*POP().lp + (long)screen->data);
            if (POP().i < entry[0]) {
                v = entry[1] + (long)screen->data;
                group.u0 = 0;
                group.u2 = 0;
                group.l8 = v;
                group.p4 = entry;
                group.u14 = screen->id0;
                group.u12 = screen->id1;
                UISAddThreadAction(info, (UISThreadActionT)6, &group, 0, 0);
            }
            break;
        }
        case 9:
            group.l8 = (long)UISFindSiblingEnableControl(screen, control);
            group.u0 = 0;
            group.u2 = 0;
            group.l4 = 0;
            group.u14 = screen->id0;
            group.u12 = screen->id1;
            UISAddThreadAction(info, (UISThreadActionT)5, &group, 0, 0);
            break;
        case 71: {
            long v = POP().i;
            long* list = POP().lp;
            arg.i = POP().i;
            long* entry = (long*)(*list + (long)screen->data);
            if (arg.i < entry[0]) {
                long c = entry[arg.i + 2];
                UISControlInfo_t* ctl = (UISControlInfo_t*)(c + (long)screen->data);
                if (control != ctl || ctl->field4 == 0) {
                    group.u0 = v;
                    group.u2 = 0;
                    group.l8 = (long)ctl;
                    group.p4 = entry;
                    group.u14 = screen->id0;
                    group.u12 = screen->id1;
                    UISAddThreadAction(info, (UISThreadActionT)5, &group, 1, &arg);
                }
            }
            break;
        }
        case 72:
            group.u0 = screen->u4;
            group.u2 = screen->u6;
            UISAddThreadAction(info, (UISThreadActionT)3, &group, 0, 0);
            break;
        case 73: {
            UISString_t* s = POP().s;
            if (s != 0)
                stack->sp[-1].i = s->length;
            else
                stack->sp[-1].i = 0;
            break;
        }
        case 74: {
            long c = 0;
            unsigned long index = POP().u;
            UISString_t* s = POP().s;
            if (s != 0 && index < s->length)
                c = s->data[index];
            stack->sp[-1].i = c;
            break;
        }
        case 75: {
            long c = POP().i;
            unsigned long index = POP().u;
            UISString_t* s = POP().s;
            if (s != 0 && index < s->length)
                s->data[index] = c;
            break;
        }
        case 76: {
            unsigned long n = POP().u;
            for (unsigned long i = 0; i < n; i++) {
                if (i < 20)
                    uisFormatStringStack[n - i - 1] = POP();
                else
                    stack->sp--;
            }
            UISString_t* fmt = POP().s;
            UISString_t* dst = POP().s;
            UISStringFormat(screen->data, dst, fmt, n, uisFormatStringStack);
            break;
        }
        case 77:
            value = POP();
            *(long*)&group = POP().i;
            UISAddThreadAction(info, (UISThreadActionT)2, &group, 1, &value);
            break;
        case 78:
            stack->pc += 4;
            break;
        case 82: {
            long t = stack->sp[-1].i;
            stack->sp[-1].i = stack->sp[-2].i;
            stack->sp[-2].i = t;
            break;
        }
        case 79:
            uisEventNotProcess = 0;
            break;
        case 80:
            uisEventNotProcess = 1;
            break;
        case 81:
        case 0x70: {
            unsigned long n;
            unsigned char count = *stack->pc;
            stack->pc++;
            n = info->savedCount;
            if (n < info->savedMax) {
                UISParam_t* params = stack->sp - count;
                group.u0 = params[-1].u;
                group.u2 = params[-1].u >> 16;
                group.h.u4 = screen->id0;
                group.h.u6 = screen->id1;
                UISAddThreadAction(info, (UISThreadActionT)0, &group, count, params);
                while (count--)
                    stack->sp--;
                stack->sp--;
                UISSavedStack_t* saved = &info->saved[n];
                saved->stack.pc = stack->pc;
                saved->stack.sp = stack->sp;
                saved->stack.base = stack->base;
                saved->stack.field8 = stack->field8;
                saved->stack.field0 = stack->field0;
                saved->param = param;
                saved->parent = stack;
                stack->base = saved->stack.sp;
                saved->u38 = group.u0;
                saved->u36 = group.u2;
                saved->screen = screen;
                saved->control = control;
                info->savedCount++;
                return 3;
            }
            while (count--)
                stack->sp--;
            break;
        }
        case 84:
            {
                long v = POP().i;
                group.u0 = 0;
                group.u2 = 0;
                group.l8 = v;
                group.l4 = 0;
            }
            group.u14 = screen->id0;
            group.u12 = screen->id1;
            UISAddThreadAction(info, (UISThreadActionT)5, &group, 0, 0);
            break;
        case 85:
        case 0x6b: {
            long offset;
            LONG(offset);
            short index = SHORT();
            UISParam_t* top = stack->sp;
            offset += top[index].i;
            if (op == 0x6b) {
                top->i = offset;
                stack->sp++;
            } else {
                *top = *(UISParam_t*)offset;
                stack->sp++;
            }
            break;
        }
        case 86: {
            long offset;
            LONG(offset);
            short index = SHORT();
            offset += stack->sp[index].i;
            *(long*)offset = sp[-1].i;
            stack->sp--;
            break;
        }
        case 87: {
            long offset;
            LONG(offset);
            short index = SHORT();
            offset += stack->sp[index].i;
            stack->sp->i = offset;
            stack->sp++;
            break;
        }
        case 90:
            sp[-2].i %= sp[-1].i;
            stack->sp--;
            break;
        case 91:
        case 93:
        case 95:
        case 97:
        case 0x6c:
        case 0x6d:
        case 0x6e: {
            long* array = 0;
            long index = 0;
            long scale = 1;
            long dims;
            long extra = 0;
            switch (op) {
            case 97:
                extra = 1;
                array = sp[sp[-1].i - 1].lp;
                break;
            case 91:
            case 0x6c:
                extra = 1;
                array = &sp[sp[-1].i - 1].i;
                break;
            case 93:
            case 0x6d:
                array = sp[-1].lp;
                extra = 1;
                break;
            case 95:
            case 0x6e: {
                extra = 0;
                long offset;
                LONG(offset);
                short slot = SHORT();
                offset += stack->sp[slot].i;
                array = (long*)offset;
                break;
            }
            }
            dims = array[0];
            for (long i = 1; i <= dims; i++) {
                long size = array[i];
                long sub = sp[-(i + extra)].i;
                if (sub >= size || sub < 0)
                    sub = size - 1;
                index += scale * sub;
                scale *= size;
            }
            if (op == 0x6c || op == 0x6d || op == 0x6e)
                sp[-(dims + extra)].p = &array[1 + dims + index];
            else
                sp[-(dims + extra)].i = array[1 + dims + index];
            stack->sp -= dims - (1 - extra);
            break;
        }
        case 92:
        case 94:
        case 96:
        case 98: {
            long i;
            long* array = 0;
            long index = 0;
            long scale = 1;
            long extra = 0;
            long dims;
            switch (op) {
            case 98:
                extra = 1;
                array = sp[sp[-1].i - 1].lp;
                break;
            case 92:
                extra = 1;
                array = &sp[sp[-1].i - 1].i;
                break;
            case 94:
                array = sp[-1].lp;
                extra = 1;
                break;
            case 96: {
                extra = 0;
                long offset;
                LONG(offset);
                short slot = SHORT();
                offset += stack->sp[slot].i;
                array = (long*)offset;
                break;
            }
            }
            dims = array[0];
            for (i = 1; i <= dims; i++) {
                long size = array[i];
                long sub = sp[-(1 + i + extra)].i;
                if (sub >= size || sub < 0)
                    sub = size - 1;
                index += scale * sub;
                scale *= size;
            }
            array[1 + dims + index] = sp[-1 - extra].i;
            stack->sp -= 1 + dims + extra;
            break;
        }
        case 99: {
            long n;
            LONGC(n);
            for (long i = 0; i < n; i++)
                sp[i] = sp[-1];
            stack->sp += n;
            break;
        }
        case 100:
        {
            long n;
            LONGC(n);
            stack->sp -= n;
        }
            break;
        case 101: {
            long i;
            long n;
            LONGC(n);
            for (i = 0; i < n; i++)
                sp[i] = sp[i - n];
            stack->sp += n;
            break;
        }
        case 102:
        case 103:
        case 104: {
            long* array = 0;
            long dims;
            long total = 1;
            long extra = 0;
            int i;
            switch (op) {
            case 102:
                extra = 1;
                array = &sp[sp[-1].i - 1].i;
                break;
            case 103:
                array = sp[-1].lp;
                extra = 1;
                break;
            case 104: {
                extra = 0;
                long offset;
                LONG(offset);
                short slot = SHORT();
                offset += stack->sp[slot].i;
                array = (long*)offset;
                break;
            }
            }
            dims = array[0];
            for (i = 1; i <= dims; i++)
                total *= array[i];
            for (i = 0; i < total; i++)
                array[1 + dims + i] = sp[-1 - extra].i;
            stack->sp -= extra + 1;
            break;
        }
        case 111:
            {
                unsigned long n = sp[-1].u;
                if (n < screen->data->count)
                    sp[-1].p = screen->data->table + n * 12;
                else
                    sp[-1].i = 0;
            }
            break;
        case 89: {
            long offset;
            LONG(offset);
            short index = SHORT();
            unsigned char show = *stack->pc;
            stack->pc++;
            offset += stack->sp[index].i;
            UISUpdateVisibility(info, screen, show, (void*)offset, sp[-1].u);
            stack->sp--;
            break;
        }
        case 0x73: {
            UISControlInfo_t* c = (UISControlInfo_t*)POP().p;
            unsigned long r = UISFindRateFnc(info, c, POP().u);
            PUSH(i, info->rateFnc > r);
            break;
        }
        case 0x72:
            {
                long l12 = POP().i;
                unsigned long v = POP().u;
                group.u0 = v;
                group.u2 = v >> 16;
                group.h.u4 = screen->id0;
                group.h.u6 = screen->id1;
                *(long*)&group.u12 = l12;
            }
            UISAddThreadAction(info, (UISThreadActionT)8, &group, 0, 0);
            break;
        case 0x74: {
            long on = POP().i;
            long bit = POP().i;
            unsigned long mask = bit >= 0 ? 1 << bit : 0xffffffff;
            if (on != 0)
                info->flags |= mask;
            else
                info->flags &= ~mask;
            break;
        }
        case 0x75: {
            long bit = POP().i;
            PUSH(u, info->flags & (1 << bit));
            break;
        }
        }
    }
    return 0;
}
