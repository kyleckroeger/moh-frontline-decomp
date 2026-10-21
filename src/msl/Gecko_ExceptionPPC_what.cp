// Runtime Gecko_ExceptionPPC.cp, file tail: std::bad_exception::what, the
// weak copy of the <exception> inline the file emits after its functions (it
// returns the class name). The class is declared here with only what's needed;
// what() is defined weak out of class so this unit emits it. The rest of the
// file is in other units.
namespace std {
class exception {
public:
    virtual ~exception();
    virtual const char* what() const;
};

class bad_exception : public exception {
public:
    virtual ~bad_exception();
    virtual const char* what() const;
};

__declspec(weak) const char* bad_exception::what() const {
    return "bad_exception";
}
}
