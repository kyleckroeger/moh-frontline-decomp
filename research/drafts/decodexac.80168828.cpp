// late2c (2026-11-13): 4 table spellings x 4 in-increment forms ('*in++', split ++, += 1, in[0]) x 6 orders of
// count/c0/c1: best stays 28.
// best WIP (harvest1, 2026-11-12): 28 real lines (was 32; index said 32), from
// scratch/loop3/harvest1/dx_merge.cpp: 'table = &xatablef[(*packet->in++ & 0xF) * 16]' in place (from
// scratch/lib/coda_wip.cpp). Left: FPR colouring in the loop.
namespace SND {
/* inferred: a decode request */
struct MXAPACKETF {
    int count;
    float previous;
    float previous2;
    unsigned char* in;
    float* out;
};

extern float xafilterf[];
extern float xatablef[];

void decodexac(MXAPACKETF* packet);
}

void SND::decodexac(SND::MXAPACKETF* packet) {
    using namespace SND;
    unsigned char* start = packet->in;

    packet->out[-2] = packet->previous2;
    packet->out[-1] = packet->previous;
    while (packet->count > 0) {
        float* filter;
        float* table;
        float c0;
        float c1;
        int i;

        filter = &xafilterf[*packet->in >> 4];
        packet->count -= 28;
        c0 = filter[0];
        table = &SND::xatablef[(*packet->in++ & 0xF) * 16];
        c1 = filter[4];
        for (i = 0; i < 14; i++) {
            unsigned char b = packet->in[i];
            packet->out[0] = packet->out[-2] * c1 + (packet->out[-1] * c0 + table[b >> 4]);
            packet->out[1] = packet->out[-1] * c1 + (packet->out[0] * c0 + table[b & 0xF]);
            packet->out += 2;
        }
        packet->in += 14;
    }
    packet->previous2 = packet->out[-2];
    packet->previous = packet->out[-1];
    packet->in = start;
}

// SND::CEAXABLKDecf, the EA-XA block decoder to float samples: allocation
// through the CODA hooks, construction and feeding a block of input.
// CEAXABLKDecf and XAFSTATE are named by the mangled symbols; the members are
// inferred from offsets and are not original.
namespace SND {
extern void* (*CODANew)(unsigned long);
extern void (*CODADelete)(void*);

struct XAFSTATE {
    float previous0;
    float previous1;
};

class CEAXABLKDecf {
public:
    CEAXABLKDecf();
    void* operator new(unsigned long);
    void operator delete(void*);
    int Feed(void*, int, int);

    int field0;
    int field4;
    int m_frames;
    int m_bytes;
    char field10[136];
    XAFSTATE m_state;
    void* m_data;
    int fieldA4;
};

// decodexac comes first in the original file, and Decode, GetState and
// SetState follow Feed; they are not reconstructed, so this unit covers the
// allocation, constructor and Feed only.

void* CEAXABLKDecf::operator new(unsigned long size) {
    return CODANew(size);
}

void CEAXABLKDecf::operator delete(void* p) {
    CODADelete(p);
}

CEAXABLKDecf::CEAXABLKDecf() {
    m_bytes = 0;
    m_frames = 0;
    field4 = 0;
    m_state.previous0 = 0.0f;
    m_state.previous1 = 0.0f;
}

int CEAXABLKDecf::Feed(void* data, int bytes, int frames) {
    if (!data)
        return -1;
    if (m_frames)
        return -1;
    m_frames = frames;
    m_bytes = bytes;
    m_data = data;
    return 0;
}
}
