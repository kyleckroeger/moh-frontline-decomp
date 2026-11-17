// settings: config/GMFE69/animmorph.json
// replaces: UpdateAnimation__10CAnimMorphFf
// Test harness (W1, pure image): CAnimMorph::UpdateAnimation as animmorph.cpp has it, with
// LLCharObjCompositeMorph's decoding inline (byte offsets from animmorph.cpp's view: m_frames +0xa34,
// m_render +0xa40, m_morph +0xcd0, m_weightsSet +0xde0; frames at list+4, 0x50 bytes each, weight
// +8, morph data +0x4c). Reports frames whose morph data has target index 63 (the decoder then writes
// weights[64], the morph object's count) and puts the count back to 64.
extern "C" void OSReport(const char*, ...);
struct DMRenderObject_T;
void DMRenderSetFlags(DMRenderObject_T*, unsigned long);
class CAnimated {
public:
    virtual ~CAnimated();
    virtual void UpdateAnimation(float);
};
class CAnimMorph : public CAnimated {
public:
    virtual ~CAnimMorph();
    virtual void UpdateAnimation(float);
};
static int s_reports;
static int s_seen;
extern unsigned char g_frame_cache[0xc800];
extern int g_frame_cache_used;
static inline unsigned short swap16(unsigned short v) { return (unsigned short)((v >> 8) | (v << 8)); }
static void decode(float* weights, unsigned short* data) {
    for (int i = 0; i < 64; i++)
        weights[i] = 0.0f;
    unsigned short count = swap16(data[0]);
    for (unsigned int i = 0; i < count; i++) {
        unsigned short entry = swap16(data[i + 1]);
        weights[(entry & 63) + 1] = 2.0f * ((unsigned int)(entry >> 6) / 1024.0f) - 0.5f;
    }
}
void CAnimMorph::UpdateAnimation(float time) {
    char* self = (char*)this;
    CAnimated::UpdateAnimation(time);
    char* list = *(char**)(self + 0xa34);
    float* weights = (float*)(self + 0xcd0);
    unsigned long* count = (unsigned long*)(self + 0xdd0);
    bool* set = (bool*)(self + 0xde0);
    unsigned short frames = *(unsigned short*)list;
    if (frames) {
        if (s_seen < 20) {
            s_seen++;
            for (unsigned int f = 0; f < frames; f++) {
                char* frame = list + 4 + f * 0x50;
                unsigned short* data = *(unsigned short**)(frame + 0x4c);
                OSReport("MORPHSEQ %d this %08x f %d/%d off %d n %d used %d w %08x\n", s_seen, (unsigned long)self, f,
                         frames, (int)((unsigned char*)data - g_frame_cache), swap16(data[0]), g_frame_cache_used,
                         *(unsigned long*)(frame + 8));
            }
        }
        for (unsigned int f = 0; f < frames; f++) {
            char* frame = list + 4 + f * 0x50;
            unsigned short* data = *(unsigned short**)(frame + 0x4c);
            unsigned short n = swap16(data[0]);
            for (unsigned int k = 0; k < n; k++) {
                unsigned short e = swap16(data[k + 1]);
                if ((e & 63) == 63 && s_reports < 30) {
                    s_reports++;
                    OSReport("MORPH63 this %08x list %08x frames %d f %d frame %08x w %08x data %08x n %d e %04x w0 %08x %08x %08x %08x\n",
                             (unsigned long)self, (unsigned long)list, frames, f, (unsigned long)frame,
                             *(unsigned long*)(frame + 8), (unsigned long)data, n, e,
                             *(unsigned long*)frame, *(unsigned long*)(frame + 4), *(unsigned long*)(frame + 0x48),
                             *(unsigned long*)(data + 2));
                }
            }
        }
        decode(weights, *(unsigned short**)(list + 4 + 0x4c));
        for (unsigned int f = 1; f < frames; f++) {
            char* frame = list + 4 + f * 0x50;
            float tmp[64];
            decode(tmp, *(unsigned short**)(frame + 0x4c));
            float w = *(float*)(frame + 8);
            for (int j = 0; j < 64; j++)
                weights[j] = weights[j] + w * (tmp[j] - weights[j]);
        }
        if (*count != 64) {
            if (s_reports < 30) {
                s_reports++;
                OSReport("MORPHCOUNT this %08x count %08x\n", (unsigned long)self, *count);
            }
            *count = 64;
        }
        DMRenderSetFlags((DMRenderObject_T*)(self + 0xa40), 1);
        *set = true;
    } else if (*set) {
        for (int i = 0; i < 64; i++)
            weights[i] = 0.0f;
        *set = false;
    }
}
