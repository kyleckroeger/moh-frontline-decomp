// animemot.cpp: its zero-initialised globals (0x40 bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct ARQRequest { unsigned char data[0x20]; }; // view: size from the original

ARQRequest g_fromARAM_request; // file-local in the original // address: g_fromARAM_request 0x801d3620
ARQRequest g_toARAM_request; // file-local in the original // address: g_toARAM_request 0x801d3640
