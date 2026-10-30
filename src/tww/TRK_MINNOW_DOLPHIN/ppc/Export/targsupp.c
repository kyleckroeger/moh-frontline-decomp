/* targsupp.s: the file-access requests the host debugger services. Each entry
   traps ('twi 31,r0,0', TWW's 'twui r0,0') and returns. The original is an
   assembly file with four size-0 labels; one assembly function named after the
   first label gives the others as entry points. */
#include "TRK_MINNOW_DOLPHIN/ppc/Export/targsupp.h"

asm u32 TRKAccessFile(u32, u32, u32*, u8*) {
    nofralloc
    twi 31, r0, 0
    blr
    entry TRKOpenFile
    twi 31, r0, 0
    blr
    entry TRKCloseFile
    twi 31, r0, 0
    blr
    entry TRKPositionFile
    twi 31, r0, 0
    blr
}
