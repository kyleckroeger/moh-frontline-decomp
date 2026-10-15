// A fragment of animmorph.cpp (0x80064c9c): CAnimMorph::AsAnimMorph returns
// the object itself. It is a weak copy of a header inline emitted in this
// file, so it is defined __declspec(weak); the result type is inferred.
// CAnimMorph is named by the symbol. The rest of the file is in other units.
class CAnimMorph {
public:
    CAnimMorph* AsAnimMorph();
};

__declspec(weak) CAnimMorph* CAnimMorph::AsAnimMorph() {
    return this;
}
