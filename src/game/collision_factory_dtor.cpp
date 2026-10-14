// A fragment of collision.cpp (0x800c7edc): the weak destructor of
// dwi::CFactory<SClsnContact, SClsnContact*>, which frees the factory's block
// when it owns it and clears its four words. The template and SClsnContact
// are named by the mangled symbols; the factory's members are inferred (as
// for CMemFactory in world_object.cpp). The destructor is a weak template
// copy emitted in this file, defined __declspec(weak) out of the class and
// instantiated explicitly. The rest of the file is not part of this unit.
extern "C" void MEM_free(void*);

struct SClsnContact;

namespace dwi {
template <class T, class P>
class CFactory {
public:
    ~CFactory();

    void* m_memory;
    int data04;
    int data08;
    int data0c;
    bool m_owned;
};

template <class T, class P> __declspec(weak) CFactory<T, P>::~CFactory() {
    if (m_owned)
        MEM_free(m_memory);
    m_memory = 0;
    data04 = 0;
    data08 = 0;
    data0c = 0;
}
}

template class dwi::CFactory<SClsnContact, SClsnContact*>;
