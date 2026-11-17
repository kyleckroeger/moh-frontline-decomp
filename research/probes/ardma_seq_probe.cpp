// settings: config/GMFE69/animemot.json
// replaces: ARStartDMA
// replaces: DMAFromARAMCallback__FUl
// Test harness (W1, pure image): with frame_async_probe.cpp. From the first animation frame read
// on, logs each AR DMA start (type, addresses, length, caller, tick, whether a DMA was already in
// flight) and each call of the frame read's completion callback (tick, DMA still in flight?).
typedef unsigned long u32;
typedef unsigned short u16;
typedef int BOOL;
#define __DSPRegs ((volatile u16*)0xCC005000)
extern "C" {
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL);
void OSReport(const char*, ...);
u32 OSGetTick(void);
}
int g_frseq_on;
static int s_logged;
extern "C" void ARStartDMA(u32 type, u32 mainmem_addr, u32 aram_addr, u32 length) {
    register unsigned long lr;
    asm { mflr lr }
    BOOL old = OSDisableInterrupts();
    u32 busy = __DSPRegs[5] & 0x200;
    if (g_frseq_on && s_logged < 80) {
        s_logged++;
        OSReport("ARSEQ start t%d mram %08x aram %08x len %d lr %08x tick %u busy %x\n", type, mainmem_addr,
                 aram_addr, length, lr, OSGetTick(), busy);
    }
    __DSPRegs[16] = (__DSPRegs[16] & 0xFFFFFC00 | (mainmem_addr >> 0x10));
    __DSPRegs[17] = (__DSPRegs[17] & 0xFFFF001F | ((u16)mainmem_addr));
    __DSPRegs[18] = (__DSPRegs[18] & 0xFFFFFC00 | (aram_addr >> 0x10));
    __DSPRegs[19] = (__DSPRegs[19] & 0xFFFF001F | ((u16)aram_addr));
    __DSPRegs[20] = __DSPRegs[20] & ~0x8000 | ((type << 0xF) & ~0x7FFF);
    __DSPRegs[20] = (__DSPRegs[20] & 0xFFFFFC00) | (length >> 0x10);
    __DSPRegs[21] = (__DSPRegs[21] & 0xFFFF001F) | (length & 0x0000FFFF);
    OSRestoreInterrupts(old);
}
extern unsigned long end;
extern volatile bool g_bDMAToMRAM;
void DMAFromARAMCallback(unsigned long request) {
    register unsigned long lr;
    asm { mflr lr }
    if (g_frseq_on && s_logged < 80) {
        s_logged++;
        OSReport("ARSEQ frcb req %08x tick %u busy %x lr %08x\n", request, OSGetTick(), __DSPRegs[5] & 0x200, lr);
    }
    end = OSGetTick();
    g_bDMAToMRAM = false;
}
