// settings: config/GMFE69/ar.json
// replaces: ARStartDMA
// Test harness (W1, pure image): ARStartDMA as ar.c has it, reporting main-memory-to-ARAM transfers
// that land in the animation pool (ARAM 0x800000..0x1000000) other than the animation upload itself
// (main memory 0x8236ca70.. in both images), with the caller.
typedef unsigned long u32;
typedef unsigned short u16;
typedef int BOOL;
extern "C" {
}
#define __DSPRegs ((volatile u16*)0xCC005000)
extern "C" {
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL);
void OSReport(const char*, ...);
}
static int s_reports;
extern "C" void ARStartDMA(u32 type, u32 mainmem_addr, u32 aram_addr, u32 length) {
    register unsigned long lr;
    asm { mflr lr }
    if (type == 0 && aram_addr + length > 0x800000 && aram_addr < 0x1000000 &&
        !(mainmem_addr >= 0x8236ca60 && mainmem_addr < 0x8236ca60 + 4162784) && s_reports < 40) {
        s_reports++;
        OSReport("ARDMA write mram %08x aram %08x len %d lr %08x\n", mainmem_addr, aram_addr, length, lr);
    }
    static int s_reads;
    if (type == 1 && mainmem_addr >= 0x80446090 && mainmem_addr < 0x80446090 + 0xc800 && s_reads < 10) {
        s_reads++;
        OSReport("ARDMA read mram %08x aram %08x len %d lr %08x\n", mainmem_addr, aram_addr, length, lr);
    }
    BOOL old = OSDisableInterrupts();
    __DSPRegs[16] = (__DSPRegs[16] & 0xFFFFFC00 | (mainmem_addr >> 0x10));
    __DSPRegs[17] = (__DSPRegs[17] & 0xFFFF001F | ((u16)mainmem_addr));
    __DSPRegs[18] = (__DSPRegs[18] & 0xFFFFFC00 | (aram_addr >> 0x10));
    __DSPRegs[19] = (__DSPRegs[19] & 0xFFFF001F | ((u16)aram_addr));
    __DSPRegs[20] = __DSPRegs[20] & ~0x8000 | ((type << 0xF) & ~0x7FFF);
    __DSPRegs[20] = (__DSPRegs[20] & 0xFFFFFC00) | (length >> 0x10);
    __DSPRegs[21] = (__DSPRegs[21] & 0xFFFF001F) | (length & 0x0000FFFF);
    OSRestoreInterrupts(old);
}
