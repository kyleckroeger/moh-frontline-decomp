// A fragment of UIStudio.c (0x80088c34): UISInit lays out a studio in one
// block: after its 172-byte header come the screens (12 bytes each), a table
// of pointers, records of 52 and of 40 bytes, and two word stacks; each
// table's capacity is kept with a zero count, the stacks' pointers are set
// up, the value given is stored, the current screens are none (-1), the
// counters are cleared and so is RuntimeErrorFnc. UISInfo_t is named by the
// mangled symbol, the magic is 'UIS_'; the members (named by offset) and
// the parameter meanings are inferred. The rest of the file is in other
// units.
typedef void (*UISErrorFnc_t)(void);

extern UISErrorFnc_t RuntimeErrorFnc;

struct UISInfo_t {
    unsigned long magic;
    unsigned long flags;
    unsigned long value08;
    unsigned long count0C;
    unsigned long count10;
    unsigned long count14;
    unsigned long count18;
    unsigned long count1C;
    long screen20;
    long screen24;
    unsigned long screenCapacity;
    unsigned long screenCount;
    void* screens;
    unsigned long capacity34;
    unsigned long count38;
    void* table3C;
    unsigned long capacity40;
    unsigned long count44;
    void* table48;
    unsigned long capacity4C;
    unsigned long count50;
    void* table54;
    unsigned long* stack58;
    unsigned long* stack5C;
    unsigned long* stack60;
    unsigned long* stack64;
    char field68[4];
    unsigned long* stack6C;
    unsigned long* stack70;
    unsigned long* stack74;
    unsigned long* stack78;
    char field7C[36];
    unsigned long* stackA0;
    unsigned long* stackA4;
    unsigned long** stackA8;
};

extern "C" void UISInit(UISInfo_t* info, unsigned long screens, unsigned long count34, unsigned long count40,
                        unsigned long count4C, unsigned long stack1, unsigned long stack2, unsigned long value) {
    unsigned long offset;
    info->magic = 0x5549535f;
    info->flags = 0;
    offset = sizeof(UISInfo_t);
    info->screenCapacity = screens;
    info->screenCount = 0;
    info->screens = (char*)info + offset;
    offset += screens * 12;
    info->capacity34 = count34;
    info->count38 = 0;
    info->table3C = (char*)info + offset;
    offset += count34 * 4;
    info->capacity40 = count40;
    info->count44 = 0;
    info->table48 = (char*)info + offset;
    offset += count40 * 52;
    info->capacity4C = count4C;
    info->count50 = 0;
    info->table54 = (char*)info + offset;
    offset += count4C * 40;
    info->stack58 = info->stack5C = info->stack64 = (unsigned long*)((char*)info + offset);
    offset += stack1 * 4;
    info->stack60 = info->stack5C + stack1;
    info->stackA0 = info->stack60 - 1;
    info->stackA4 = info->stack60 - 1;
    info->stackA8 = &info->stack5C;
    info->stack6C = info->stack70 = info->stack78 = (unsigned long*)((char*)info + offset);
    offset += stack2 * 4;
    info->stack74 = info->stack70 + stack2;
    info->value08 = value;
    info->screen24 = -1;
    info->count0C = 0;
    info->count18 = 0;
    info->count1C = 0;
    info->count10 = 0;
    info->count14 = 0;
    RuntimeErrorFnc = 0;
    info->screen20 = -1;
}
