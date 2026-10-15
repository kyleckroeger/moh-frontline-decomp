// A fragment of objcreate.cpp (0x8003d128): CStaticObject::SetScript keeps the script object (+280). It is a weak copy of a
// header inline emitted in this file, so it is defined __declspec(weak); the
// member is inferred. The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
struct BSObject;

class CStaticObject {
public:
    void SetScript(BSObject*);

    unsigned char unknown000[280];
    BSObject* m_script;
};

__declspec(weak) void CStaticObject::SetScript(BSObject* script) {
    m_script = script;
}
