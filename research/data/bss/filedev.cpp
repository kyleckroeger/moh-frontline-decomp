// filedev.cpp: its zero-initialised globals (0x8060 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct READREQUEST { unsigned char data[0x8060]; }; // view: size from the original

READREQUEST gCurRead; // file-local in the original // address: gCurRead 0x80336d00
