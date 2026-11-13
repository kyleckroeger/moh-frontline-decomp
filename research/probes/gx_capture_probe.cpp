// settings: config/GMFE69/particlesystem_link.json
// replaces: Link__23CPropertyParticleSystemFP7CDmaTagR10CDmaPacket
//
// Test harness, not game code. CPropertyParticleSystem::Link (body as in
// src/game/particlesystem_link.cpp) with its virtual RenderSystem call wrapped:
// the write-gather pipe is redirected into a buffer for the call
// (GXRedirectWriteGatherPipe), the bytes are hashed and logged
// ("GXC call this bytes hash"), then replayed to the GPU with GXCallDisplayList so
// the frame still draws. With DUMP_FROM >= 0 the calls DUMP_FROM..DUMP_FROM+DUMP_COUNT-1
// are also dumped as hex ("GXD call offset words"). Each call's buffer is pre-filled
// with a sentinel; the captured length is where the first all-sentinel 32-byte block
// starts (the restore pads to 32 bytes). Edit the #defines in a copy to change what is
// logged; SCRIBBLE overwrites 4 KB of stack below the caller after each call, and DELAY
// busy-waits that many loop steps (the 2026-11-13 timing experiments, docs/Dolphin.md).
//
// Link it with `link_drafts.py --probe` and keep it in the control
// (`dolphin_boot.py control --keep ... Link__23CPropertyParticleSystemFP7CDmaTagR10CDmaPacket`),
// then compare the GXC lines of the two runs. The same wrapping works for any draft
// reached through one call site that draws through the FIFO.
extern "C" {
void OSReport(const char*, ...);
volatile void* GXRedirectWriteGatherPipe(void*);
void GXRestoreWriteGatherPipe(void);
void DCInvalidateRange(void*, unsigned long);
void DCFlushRange(void*, unsigned long);
void GXCallDisplayList(void*, unsigned long);
}

#ifndef LOG_FROM
#define LOG_FROM 0
#endif
#ifndef LOG_COUNT
#define LOG_COUNT 6000
#endif
#ifndef DUMP_FROM
#define DUMP_FROM -1
#endif
#ifndef DUMP_COUNT
#define DUMP_COUNT 1
#endif

class CDmaTag;
class CDmaPacket;
class CRenderList;
class CTexture {
public:
    void Load(CDmaPacket&, CRenderList*);
};
class CPropertyParticleSystem {
public:
    void Link(CDmaTag*, CDmaPacket&);
};

#ifdef SCRIBBLE
// Overwrites the stack below the caller with a constant, so later code can't read
// what RenderSystem left there.
#pragma dont_inline on
void ProbeScribble() {
    volatile unsigned int a[1024];
    for (int i = 0; i < 1024; i++)
        a[i] = 0x5a5a5a5a;
}
#pragma dont_inline reset
#endif

typedef void (*RenderFn)(void*, CDmaPacket&);

static const unsigned int BUF = 0x40000;
static const unsigned int SENT = 0xabababab;
static unsigned int s_buf[BUF / 4 + 8];
static unsigned int s_used = BUF; // first call fills everything
static unsigned int s_call;
#ifndef DELAY
#define DELAY 0
#endif
static volatile int s_delay = DELAY; // busy-wait after each call (timing experiments)

static void Fill(unsigned int* p, unsigned int bytes) {
    for (unsigned int i = 0; i < bytes / 4; i++)
        p[i] = SENT;
    DCFlushRange(p, bytes);
}

void CPropertyParticleSystem::Link(CDmaTag*, CDmaPacket& packet) {
    char* self = (char*)this;
    char* prev = *(char**)(self + 0);
    char* shape = *(char**)(self + 0x74);
    bool load = true;
    if (prev && shape == *(char**)(prev + 0x74))
        load = false;
    if (load)
        ((CTexture*)(shape + 0x20))->Load(packet, 0);
    RenderFn fn = *(RenderFn*)(*(char**)(self + 0x1c) + 0xc4);

    unsigned int* buf = (unsigned int*)(((unsigned int)s_buf + 31) & ~31u);
    Fill(buf, s_used);
    GXRedirectWriteGatherPipe(buf);
    fn(this, packet);
#ifdef SCRIBBLE
    ProbeScribble();
#endif
    for (volatile int d = 0; d < s_delay; d++) {
    }
    GXRestoreWriteGatherPipe();
    DCInvalidateRange(buf, BUF);
    unsigned int n = 0;
    while (n < BUF) {
        unsigned int* b = buf + n / 4;
        bool all = true;
        for (int i = 0; i < 8; i++)
            if (b[i] != SENT) {
                all = false;
                break;
            }
        if (all)
            break;
        n += 32;
    }
    s_used = n + 32 <= BUF ? n + 32 : BUF;
    unsigned int h = 2166136261u;
    for (unsigned int i = 0; i < n / 4; i++)
        h = (h ^ buf[i]) * 16777619u;
    unsigned int call = s_call++;
    if (call >= LOG_FROM && call < LOG_FROM + LOG_COUNT)
        OSReport("GXC %u %08x %u %08x\n", call, (unsigned int)this, n, h);
    if ((int)call >= DUMP_FROM && (int)call < DUMP_FROM + DUMP_COUNT) {
        for (unsigned int o = 0; o < n / 4; o += 8)
            OSReport("GXD %u %u %08x %08x %08x %08x %08x %08x %08x %08x\n", call, o * 4, buf[o], buf[o + 1],
                     buf[o + 2], buf[o + 3], buf[o + 4], buf[o + 5], buf[o + 6], buf[o + 7]);
    }
    if (n >= BUF)
        OSReport("GXC overflow %u\n", call);
    else if (n)
        GXCallDisplayList(buf, n);
}
