// A fragment of compartment.cpp (0x800775e0): the weak (empty) destructor of
// CSPRStack. CSPRStack is named by the mangled symbol; the destructor is a weak
// copy of a header inline emitted in this file, so it is defined
// __declspec(weak). The rest of the file is not part of this unit.
class CSPRStack {
public:
    ~CSPRStack();
};

__declspec(weak) CSPRStack::~CSPRStack() {
}
