// A fragment of IStudio.cpp (0x8008b8b8): the empty IStudio destructor and
// the constructor, which clears its six words (the members are inferred). The
// classes and functions are named by the symbols. The rest of the file is in
// other units.
class IStudio {
public:
    IStudio();
    ~IStudio();

    int m_value00;
    int m_value04;
    int m_value08;
    int m_value0c;
    int m_value10;
    int m_value14;
};

IStudio::~IStudio() {
}

IStudio::IStudio() {
    m_value00 = 0;
    m_value04 = 0;
    m_value08 = 0;
    m_value0c = 0;
    m_value10 = 0;
    m_value14 = 0;
}
