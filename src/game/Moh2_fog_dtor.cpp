// A fragment of Moh2.cpp (0x8001acb4): the weak (empty) destructor of CFog.
// CFog is named by the mangled symbol; the destructor is a weak copy of a
// header inline emitted in this file, so it is defined __declspec(weak). The
// rest of the file is not part of this unit.
class CFog {
public:
    ~CFog();
};

__declspec(weak) CFog::~CFog() {
}
