// A fragment of compartment.cpp (0x80075b88): CCompartment::GetSharedSize returns the shared compartment size (a global
// named by its symbol, file-local in the original and declared extern here;
// its type is inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CCompartment {
public:
    static int GetSharedSize();
};

extern int g_SharedCompartmentSize; // file-local in the original

int CCompartment::GetSharedSize() {
    return g_SharedCompartmentSize;
}
