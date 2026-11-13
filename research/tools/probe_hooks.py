#!/usr/bin/env python3
"""Add per-update test hooks to an input probe written by input_script.py (dol4).

  probe_hooks.py PROBE.cpp HOOKS.txt

HOOKS.txt lines (# starts a comment); all hooks run inside iPAD_update, before the trace:
  UPDATE X Y Z YAW              set the player's position (m_tm and the previous position)
                                and yaw at that update
  FLASH UPDATE A1 .. A9         call the built-in PlayerScreenFlash with these arguments (in
                                the order a script pushes them) at that update
  SNDTR FIRST LAST EVERY        log SNDtimeremaining for every active voice ("SNDTR")
  XALF UPDATE                   run the looping EA-XA unpacker on a synthetic sample ("XALF")
  BOXTRI UPDATE COUNT           run CCDBVolBox::TestCollision(CTriangle) on COUNT random
                                box and triangle pairs ("BOXTRI")
Build the probe into both the test image and its control (the hooks are part of the
iPAD_update probe, so `control --keep iPAD_update__Fv` keeps them). docs/Dolphin.md, "Probe
routes (dol4)", has the routes that use them.
"""
import sys
p, t = sys.argv[1], sys.argv[2]
lines = [l.split() for l in open(t) if l.strip() and not l.startswith('#')]
rows = [l for l in lines if l[0] not in ('FLASH', 'SNDTR', 'XALF', 'BOXTRI')]
flash = [l[1:] for l in lines if l[0] == 'FLASH']
sndtr = [l[1:] for l in lines if l[0] == 'SNDTR']
xalf = [l[1:] for l in lines if l[0] == 'XALF']
boxtri = [l[1:] for l in lines if l[0] == 'BOXTRI']
boxtri_def = '#define BOXTRI_AT %s\n#define BOXTRI_COUNT %s' % tuple(boxtri[0]) if boxtri else '#define BOXTRI_AT 0\n#define BOXTRI_COUNT 0'
xalf_def = '#define XALF_AT %s' % xalf[0][0] if xalf else '#define XALF_AT 0'
sndtr_def = '#define SNDTR_RANGE %s, %s, %s' % tuple(sndtr[0]) if sndtr else '#define SNDTR_RANGE 0, 0, 1'
tab = ",\n".join("    {%s, %sf, %sf, %sf, %sf}" % tuple(r) for r in rows) or '    {0, 0.0f, 0.0f, 0.0f, 0.0f}'
flashtab = ',\n'.join('    {%s, {%s}}' % (f[0], ', '.join(x + 'f' if '.' in x else x + '.0f' for x in f[1:])) for f in flash) or '    {0, {0}}'
code = open(p).read()
hook = '''
// dol4 teleports (identical in test and control): set m_tm's position (0xb0), the previous position (0x288) and m_yaw (0x29c).
struct Teleport { unsigned int frame; float x, y, z, yaw; };
static const Teleport s_tp[] = {
%s
};
// dol4: call the script built-in PlayerScreenFlash at an update, with the arguments a level
// script pushes (in push order), as the BS machine would (g_iCurrentBIFIndex set to its entry).
struct BSBuiltinEntry { void (*function)(int**, void*); unsigned char u04[6]; short argumentCount; short parameterCount; unsigned char u0e[2]; };
extern BSBuiltinEntry* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;
void BIFunc_PlayerScreenFlash(int**, void*);
struct Flash { unsigned int frame; float args[9]; };
static const Flash s_flash[] = {
%s
};
static void DoFlash(void) {
    for (unsigned int k = 0; k < sizeof(s_flash) / sizeof(s_flash[0]); k++) {
        if (s_flash[k].frame != s_update || !s_flash[k].frame)
            continue;
        int index = -1;
        for (int i = 0; i < 2048; i++) {
            if (g_pBuiltInFunctions[i].function == BIFunc_PlayerScreenFlash) {
                index = i;
                break;
            }
        }
        if (index < 0)
            return;
        int buf[16];
        int count = g_pBuiltInFunctions[index].parameterCount;
        for (int i = 1; i <= count && i <= 9; i++) {
            float v = s_flash[k].args[i - 1];
            if (i == 9)
                buf[12 - count + i] = *(int*)&v;
            else
                buf[12 - count + i] = (int)v;
        }
        int* sp = &buf[12];
        int saved = g_iCurrentBIFIndex;
        g_iCurrentBIFIndex = index;
        OSReport("FLASH %%u index %%d count %%d\\n", s_update, index, count);
        BIFunc_PlayerScreenFlash(&sp, 0);
        g_iCurrentBIFIndex = saved;
    }
}

// dol4: every EVERY updates from FIRST to LAST, ask SNDtimeremaining about every active voice
// (sndgs + 0x1d4: the voice records, 0x80 bytes, handle at +0, active flag at +0x5d;
// sndgs + 0x170: the voice count, a short) and log the answers. It only reads.
%s
extern "C" char sndgs[];
extern "C" int SNDtimeremaining(int);
static const unsigned int s_sndtr[3] = { SNDTR_RANGE };
static void DoSndTimeRemaining(void) {
    if (s_update < s_sndtr[0] || s_update > s_sndtr[1] || (s_update - s_sndtr[0]) %% s_sndtr[2] != 0)
        return;
    int count = *(short*)(sndgs + 0x170);
    char* voices = *(char**)(sndgs + 0x1d4);
    for (int v = 0; v < count; v++) {
        char* rec = voices + v * 0x80;
        if (rec[0x5d]) {
            int handle = *(int*)rec;
            OSReport("SNDTR %%u voice %%d handle %%08x left %%d\\n", s_update, v, handle, SNDtimeremaining(handle));
        }
    }
}

// dol4: at update XALF_AT, run the looping EA-XA unpacker (the mixer's filter for an
// in-memory XA voice with a loop start: SFILTER_unpackxalfinit, then the state's unpack
// function, SFILTER_unpackxalf) over a fixed synthetic sample (40 blocks of 15 bytes, loop
// from sample 300 to 1119) for 24 mixer-sized calls of 256 samples, and log each call's
// result, position and an FNV-1a hash of its output.
%s
struct UNPACKINITPARAMS {
    void* data;
    int field4;
    int field8;
    int length;
    int loopstart;
    int loopend;
    int field18[3];
    int (*getframe)(void*);
};
void SFILTER_unpackxalfinit(void*, UNPACKINITPARAMS*);
typedef int (*XAUnpackFn)(void*, int, void*, void*, int);
typedef void (*XARestoreFn)(void*);
static unsigned char s_xa[40 * 15];
static int s_xastate[0x80 / 4];
static float s_xaout[256];
static void DoXalf(void) {
    if (s_update != XALF_AT || !XALF_AT)
        return;
    unsigned int seed = 12345;
    for (int i = 0; i < 40 * 15; i++) {
        seed = seed * 1103515245u + 12345u;
        unsigned char b = (unsigned char)(seed >> 16);
        if (i %% 15 == 0)
            b = (unsigned char)(((i / 15) %% 4) << 4 | (seed >> 24) %% 12);
        s_xa[i] = b;
    }
    UNPACKINITPARAMS params;
    unsigned char* pp = (unsigned char*)&params;
    for (unsigned int i = 0; i < sizeof(params); i++)
        pp[i] = 0;
    params.data = s_xa;
    params.length = 40 * 28;
    params.loopstart = 300;
    params.loopend = 40 * 28 - 1;
    for (int i = 0; i < 0x80 / 4; i++)
        s_xastate[i] = 0;
    SFILTER_unpackxalfinit(s_xastate, &params);
    XAUnpackFn unpack = (XAUnpackFn)s_xastate[0];
    for (int call = 0; call < 24; call++) {
        for (int i = 0; i < 256; i++)
            s_xaout[i] = 0.0f;
        int n = unpack(s_xastate, 256, 0, s_xaout, 0);
        unsigned int h = 2166136261u;
        const unsigned char* o = (const unsigned char*)s_xaout;
        for (int i = 0; i < 256 * 4; i++)
            h = (h ^ o[i]) * 16777619u;
        OSReport("XALF %%u call %%d n %%d pos %%d frame %%d hash %%08x\\n", s_update, call, n, s_xastate[0x28 / 4],
                 params.getframe ? params.getframe(s_xastate) : -1, h);
    }
    ((XARestoreFn)s_xastate[1])(s_xastate);
}

// dol4: at update BOXTRI_AT, call CCDBVolBox::TestCollision(const CTriangle&) on
// BOXTRI_COUNT pseudo-random box and triangle pairs (no game code calls it, see
// docs/Dolphin.md) and log every 100th result, the counts of each result and an FNV-1a hash
// of the result sequence. The box is laid out as the draft's view: vtable pointer, half
// sizes at +4, three axes at +0x10 and the centre at +0x40 (16-byte vectors).
%s
struct BTVec { float x, y, z, w; };
struct BTTri { BTVec v[3]; };
struct BTBox { void* vt; float half[3]; BTVec axes[3]; BTVec center; };
class CTriangle;
class CCollision;
class CCDBVolBox {
public:
    int TestCollision(const CTriangle&, CCollision&, bool) const;
};
static float BTSqrt(float v) {
    if (v <= 0.0f) return 0.0f;
    float r = v > 1.0f ? v : 1.0f;
    for (int i = 0; i < 30; i++) r = 0.5f * (r + v / r);
    return r;
}
static unsigned int s_btseed = 2463534242u;
static float BTRand(float lo, float hi) {
    s_btseed ^= s_btseed << 13;
    s_btseed ^= s_btseed >> 17;
    s_btseed ^= s_btseed << 5;
    return lo + (hi - lo) * (float)(s_btseed & 0xffffff) / 16777216.0f;
}
static void BTNorm(BTVec& a) {
    float l = BTSqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    if (l > 0.0f) { a.x /= l; a.y /= l; a.z /= l; }
}
static void DoBoxTri(void) {
    if (s_update != BOXTRI_AT || !BOXTRI_AT)
        return;
    static BTBox box;
    static BTTri tri;
    static int collision[16];
    int counts[5] = {0, 0, 0, 0, 0};
    unsigned int h = 2166136261u;
    for (int n = 0; n < BOXTRI_COUNT; n++) {
        // orthonormal axes from two random vectors (Gram-Schmidt), some axis-aligned
        BTVec a = {BTRand(-1, 1), BTRand(-1, 1), BTRand(-1, 1), 0};
        BTVec b = {BTRand(-1, 1), BTRand(-1, 1), BTRand(-1, 1), 0};
        if (n %% 4 == 0) { a.x = 1; a.y = 0; a.z = 0; b.x = 0; b.y = 1; b.z = 0; }
        BTNorm(a);
        float d = a.x * b.x + a.y * b.y + a.z * b.z;
        b.x -= d * a.x; b.y -= d * a.y; b.z -= d * a.z;
        BTNorm(b);
        BTVec c = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, 0};
        box.axes[0] = a; box.axes[1] = b; box.axes[2] = c;
        for (int k = 0; k < 3; k++) box.half[k] = BTRand(0.1f, 2.0f);
        box.center.x = BTRand(-1, 1); box.center.y = BTRand(-1, 1); box.center.z = BTRand(-1, 1); box.center.w = 0;
        float spread = (n %% 3 == 0) ? 1.0f : 4.0f;
        for (int k = 0; k < 3; k++) {
            tri.v[k].x = BTRand(-spread, spread);
            tri.v[k].y = BTRand(-spread, spread);
            tri.v[k].z = BTRand(-spread, spread);
            tri.v[k].w = 0;
        }
        if (n %% 50 == 7) tri.v[2] = tri.v[1];  // degenerate
        int r = ((const CCDBVolBox*)&box)->TestCollision(*(const CTriangle*)&tri, *(CCollision*)collision, true);
        counts[(unsigned int)r < 4 ? r : 4]++;
        h = (h ^ (unsigned int)r) * 16777619u;
        if (n %% 100 == 0)
            OSReport("BOXTRI %%d r %%d\\n", n, r);
    }
    OSReport("BOXTRI done %%d counts %%d %%d %%d %%d %%d hash %%08x\\n", BOXTRI_COUNT, counts[0], counts[1], counts[2], counts[3], counts[4], h);
}

static void DoTeleports(void) {
    DoFlash();
    DoBoxTri();
    DoXalf();
    DoSndTimeRemaining();
    unsigned char* p = g_pPlayers[0];
    if (!p) return;
    for (unsigned int i = 0; i < sizeof(s_tp) / sizeof(s_tp[0]); i++) {
        if (s_tp[i].frame == s_update) {
            *(float*)(p + 0xb0) = s_tp[i].x;
            *(float*)(p + 0xb4) = s_tp[i].y;
            *(float*)(p + 0xb8) = s_tp[i].z;
            *(float*)(p + 0x29c) = s_tp[i].yaw;
            // SetTMLocalToWorld also copies the position to the previous position (0x288).
            *(float*)(p + 0x288) = s_tp[i].x;
            *(float*)(p + 0x28c) = s_tp[i].y;
            *(float*)(p + 0x290) = s_tp[i].z;
            OSReport("TELEPORT %%u %%.3f %%.3f %%.3f %%.3f\\n", s_update, s_tp[i].x, s_tp[i].y, s_tp[i].z, s_tp[i].yaw);
        }
    }
}

void iPAD_update(void) {''' % (tab, flashtab, sndtr_def, xalf_def, boxtri_def)
assert code.count('void iPAD_update(void) {') == 1
code = code.replace('void iPAD_update(void) {', hook, 1)
code = code.replace('    if (s_update % TRACE_EVERY == 0) {', '    DoTeleports();\n    if (s_update % TRACE_EVERY == 0) {', 1)
open(p, 'w').write(code)
