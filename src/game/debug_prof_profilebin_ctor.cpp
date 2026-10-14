// A fragment of debug_prof.cpp (0x800f3554): the weak SProfileBin constructor,
// which sets the first word to -1 and clears the five after it. SProfileBin is
// named by the mangled symbol; its members are inferred. The constructor is a
// weak copy of a header inline emitted in this file, so it is defined
// __declspec(weak). The rest of the file is not part of this unit.
struct SProfileBin {
    SProfileBin();

    int m_id;
    int m_values[5];
};

__declspec(weak) SProfileBin::SProfileBin() : m_id(-1) {
    m_values[0] = 0;
    m_values[1] = 0;
    m_values[2] = 0;
    m_values[3] = 0;
    m_values[4] = 0;
}
