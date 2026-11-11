// targimpl.c: its zero-initialised globals (0x578 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct TRKState { unsigned char data[0xa4]; }; // view: size from the original
struct ProcessorState_PPC { unsigned char data[0x430]; }; // view: size from the original
struct Default_PPC { unsigned char data[0x94]; }; // view: size from the original
struct u128 { unsigned char data[0x10]; }; // view: size from the original

TRKState gTRKState;
ProcessorState_PPC gTRKCPUState;
Default_PPC gTRKSaveState;
u128 TRKvalue128_temp;
