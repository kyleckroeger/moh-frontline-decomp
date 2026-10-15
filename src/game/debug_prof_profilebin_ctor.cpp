// A fragment of debug_prof.cpp (0x800f34f0): the SProfHistBin constructor
// (six halfwords set to -1, then 300 profile bins built by __construct_array)
// and the weak SProfileBin constructor, which sets the first word to -1 and
// clears the five after it. SProfHistBin and SProfileBin are named by the
// mangled symbols; their members are inferred. The SProfileBin constructor is a
// weak copy of a header inline emitted in this file, so it is defined
// __declspec(weak). The rest of the file is not part of this unit.
struct SProfileBin {
    SProfileBin();

    int m_id;
    int m_values[5];
};

struct SProfHistBin {
    SProfHistBin();

    short m_value0;
    short m_value2;
    short m_value4;
    short m_value6;
    short m_value8;
    short m_valueA;
    SProfileBin m_bins[300];
};

SProfHistBin::SProfHistBin()
    : m_value0(-1), m_value2(-1), m_value4(-1), m_value6(-1), m_value8(-1), m_valueA(-1) {
}

__declspec(weak) SProfileBin::SProfileBin() : m_id(-1) {
    m_values[0] = 0;
    m_values[1] = 0;
    m_values[2] = 0;
    m_values[3] = 0;
    m_values[4] = 0;
}
