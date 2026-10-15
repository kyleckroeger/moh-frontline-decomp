// A fragment of animskinned.cpp (0x80066a04): CAnimSkinned::AsAnimSkinned returns the object itself. It is a weak copy of
// a header inline emitted in this file, so it is defined __declspec(weak);
// the result type is inferred. The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CAnimSkinned {
public:
    CAnimSkinned* AsAnimSkinned();
};

__declspec(weak) CAnimSkinned* CAnimSkinned::AsAnimSkinned() {
    return this;
}
