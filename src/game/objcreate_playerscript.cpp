// A fragment of objcreate.cpp (0x80040294): CPlayerObject::GetScriptObject returns the script object (+592). It is a
// weak copy of a header inline emitted in this file, so it is defined
// __declspec(weak); the member and result type are inferred. The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
struct BSObject;

class CPlayerObject {
public:
    BSObject* GetScriptObject() const;

    unsigned char unknown000[592];
    BSObject* m_script;
};

__declspec(weak) BSObject* CPlayerObject::GetScriptObject() const {
    return m_script;
}
