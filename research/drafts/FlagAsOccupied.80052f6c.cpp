// best WIP (harvest1, 2026-11-12): 3 real lines (was 4; index said 4), from
// scratch/loop3/airender/fao/BEST3.cpp: 'types = types & ...' instead of '&='. Left: 3 lines (the 1|2
// 'or' order); the sweep1 FAO_BEST3 inline-view variant is State question 8 and not used.
enum COVERPOINT_TYPE {};

void DebugMsg(const char*, ...);

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

struct CoverPointTypes {
    int m_bits;
    CoverPointTypes() {}
    CoverPointTypes(const int& bits) : m_bits(bits) {}
    CoverPointTypes& operator&=(const int& bits) { m_bits = bits & m_bits; return *this; }
    CoverPointTypes& operator&=(const CoverPointTypes& o) { m_bits = o.m_bits & m_bits; return *this; }
    CoverPointTypes operator&(const int& bits) const { return CoverPointTypes(m_bits & bits); }
    CoverPointTypes operator&(const CoverPointTypes& o) const { return CoverPointTypes(m_bits & o.m_bits); }
    CoverPointTypes& operator|=(const int& bits) { m_bits |= bits; return *this; }
    CoverPointTypes& operator=(const CoverPointTypes& o) { m_bits = o.m_bits; return *this; }
    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t = *this; t.m_bits = t.m_bits | bits; return t; }
};
inline CoverPointTypes Mask(unsigned int v) { CoverPointTypes t; t.m_bits = v; return t; }

struct CoverPointDataView {
    unsigned char unknown00[112];
    int m_flags;
};

inline unsigned int NotMasked(unsigned int v, unsigned int mask) { return ~(v & mask); }
struct OccView { unsigned int m_bits; OccView(unsigned int b) : m_bits(b) {} unsigned int operator~() const { return ~(m_bits & 0xff); } };

class CCoverPoint {
public:
    void FlagAsUnoccupied(COVERPOINT_TYPE);
    bool FlagAsOccupied(COVERPOINT_TYPE);
    COVERPOINT_TYPE GetAvailablePeekPosition() const;

    CoverPointTypes GetTypes() const {
        CoverPointTypes types;
        types.m_bits = m_data->m_flags;
        ChangeEndian(*(int*)&types.m_bits);
        struct { unsigned int mask; unsigned int type; } table[8] = {
            {0x01, 1}, {0x02, 2}, {0x04, 4}, {0x08, 4}, {0x10, 1}, {0x20, 2}, {0x40, 1}, {0x80, 2}};
        unsigned int bits = 0;
        for (int i = 0; i < 8; i++) {
            if (types.m_bits & table[i].mask)
                bits |= table[i].type;
        }
        types.m_bits = bits;
        types = types & 0xff;
        types = types & (CoverPointTypes(1) | 2 | 4);
        return types;
    }

    unsigned int m_occupied;
    CoverPointDataView* m_data;
};

void CCoverPoint::FlagAsUnoccupied(COVERPOINT_TYPE type) {
    m_occupied &= ~type;
}

bool CCoverPoint::FlagAsOccupied(COVERPOINT_TYPE type) {
    CoverPointTypes types = GetTypes();
    if (type & ~types.m_bits) {
        DebugMsg("Coverpoint flag error! cptTypes = %x cptPosition = %x\n", types.m_bits, type);
        return false;
    }
    m_occupied |= type;
    return true;
}

COVERPOINT_TYPE CCoverPoint::GetAvailablePeekPosition() const {
    CoverPointTypes types = GetTypes();
    int avail = ~(m_occupied & 0xff);
    avail &= types.m_bits;
    if (avail <= 1)
        return (COVERPOINT_TYPE)avail;
    avail &= 1 - avail;
    return (COVERPOINT_TYPE)avail;
}
