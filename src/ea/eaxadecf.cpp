// decodexac and SND::CEAXABLKDecf, the EA-XA block decoder to float samples.
// decodexac decodes a request's 15-byte blocks (a filter/shift byte and 14
// bytes of two 4-bit codes) into float samples through the filter
// coefficients (xafilterf) and the per-shift sample table (xatablef), keeping
// the last two samples in the request. CEAXABLKDecf: allocation
// through the CODA hooks, construction, feeding a block of input and decoding,
// and the decoder state accessors (GetState returns the two previous samples
// by value, SetState restores them). CEAXABLKDecf, XAFSTATE, MXAPACKETF and
// decodexac are named by the mangled symbols; the members are inferred from
// offsets and are not original.
namespace SND {
extern void* (*CODANew)(unsigned long);
extern void (*CODADelete)(void*);

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

void decodexac(MXAPACKETF* packet) {
    unsigned char* start = packet->in;
    packet->out[-2] = packet->previous2;
    packet->out[-1] = packet->previous;
    while (packet->count > 0) {
        float* filter;
        int ix;
        float c0;
        float c1;
        int i;

        filter = &xafilterf[*packet->in >> 4];
        packet->count -= 28;
        c0 = filter[0];
        ix = (*packet->in & 0xF) * 16;
        c1 = filter[4];
        packet->in++;
        for (i = 0; i < 14; i++) {
            unsigned char b = packet->in[i];
            float* out = packet->out;
            out[0] = out[-2] * c1 + (out[-1] * c0 + (&xatablef[ix])[b >> 4]);
            out = packet->out;
            out[1] = out[-1] * c1 + (out[0] * c0 + (&xatablef[ix])[b & 0xF]);
            packet->out += 2;
        }
        packet->in += 14;
    }
    packet->previous2 = packet->out[-2];
    packet->previous = packet->out[-1];
    packet->in = start;
}

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
    int Decode(float**, int);
    XAFSTATE GetState();
    void SetState(XAFSTATE*);

    int field0;
    int field4;
    int m_frames;
    int m_bytes;
    float* m_read;
    char field14[16];
    float m_buffer[28];
    int m_count;
    XAFSTATE m_state;
    void* m_data;
    float* m_out;
};

// GetState and SetState follow Decode in their own unit.

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

// The original calls decodexac here; the pragma keeps it out of line.
#pragma dont_inline on
int CEAXABLKDecf::Decode(float** out, int count) {
    int const wanted = count;
    int done = 0;
    int remaining;
    int rest;
    int i;
    m_out = *out;
    if (!m_frames)
        return 0;
    remaining = wanted < m_frames ? wanted : m_frames;
    if (field4) {
        int n = field4 <= remaining ? field4 : remaining;
        for (i = 0; i < n; i++) {
            *m_out = *m_read;
            m_out++;
            m_read++;
        }
        done = n;
        remaining -= n;
        m_frames -= n;
        field4 -= n;
    }
    {
        int samples = remaining / 28 * 28;
        m_count = samples;
        rest = remaining - m_count;
        if (m_count > 0) {
            decodexac((MXAPACKETF*)&m_count);
            done += samples;
            m_frames -= samples;
            m_data = (char*)m_data + samples / 28 * 15;
        }
    }
    if (rest > 0) {
        float* save = m_out;
        m_out = m_buffer;
        m_count = rest;
        decodexac((MXAPACKETF*)&m_count);
        m_frames -= rest;
        m_data = (char*)m_data + 15;
        m_read = m_out + m_count;
        field4 = -m_count;
        m_out -= 28;
        for (i = 0; i < rest; i++)
            save[i] = m_out[i];
        done += rest;
    }
    if (m_frames <= 0)
        field4 = 0;
    return done;
}

#pragma dont_inline reset

XAFSTATE CEAXABLKDecf::GetState() {
    XAFSTATE state;

    state.previous0 = m_state.previous0;
    state.previous1 = m_state.previous1;
    return state;
}

void CEAXABLKDecf::SetState(XAFSTATE* state) {
    m_state = *state;
}
}
