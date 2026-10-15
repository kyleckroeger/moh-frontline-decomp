// A fragment of objcreate.cpp (0x8003f8b0): CAnimObject::SetScript keeps the script object (+9104). It is a weak copy of a
// header inline emitted in this file, so it is defined __declspec(weak); the
// member is inferred. The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
struct BSObject;

class CAnimObject {
public:
    void SetScript(BSObject*);

    unsigned char unknown000[9104];
    BSObject* m_script;
};

__declspec(weak) void CAnimObject::SetScript(BSObject* script) {
    m_script = script;
}
