// A fragment of objcreate.cpp (0x8003f1e4): CCollisionVolume::SetScript keeps the script object (+120). It is a weak copy of a
// header inline emitted in this file, so it is defined __declspec(weak); the
// member is inferred. The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
struct BSObject;

class CCollisionVolume {
public:
    void SetScript(BSObject*);

    unsigned char unknown000[120];
    BSObject* m_script;
};

__declspec(weak) void CCollisionVolume::SetScript(BSObject* script) {
    m_script = script;
}
