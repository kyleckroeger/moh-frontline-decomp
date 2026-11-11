// dvdStatus_base.c: its zero-initialised globals (0x5c bytes), written by research/tools/bss_decls.py
// and checked by check_data.py. Views marked "view" hold the size only; real layouts come with the shared headers.

struct GXRenderModeObj { unsigned char data[0x3c]; }; // view: size from the original
struct OSFontHeader;
typedef short s16;
struct GXFifoObj;
typedef unsigned long u32;

GXRenderModeObj gRMode; // file-local in the original // address: gRMode 0x803168e0
static OSFontHeader* FontData; // address: FontData 0x8034ef40
void* LastSheet; // file-local in the original // address: LastSheet 0x8034ef44
s16 FontSize; // file-local in the original // address: FontSize 0x8034ef48
s16 FontSpace; // file-local in the original // address: FontSpace 0x8034ef4a
GXFifoObj* gDefaultFifoObj; // file-local in the original // address: gDefaultFifoObj 0x8034ef4c
void* gFrameBuffer; // file-local in the original // address: gFrameBuffer 0x8034ef50
u32 gHeapPtr; // file-local in the original // address: gHeapPtr 0x8034ef54
void* gFifoBuf; // file-local in the original // address: gFifoBuf 0x8034ef58
int gGxIsInit;
