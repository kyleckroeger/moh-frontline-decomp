// A fragment of scene.cpp (0x800dd070): the weak
// dwi::fast_vec<CScene::SModCommand> and
// dwi::fast_vec<CScene::SExclusionEntry> destructors, which frees the
// storage when the vector owns it. The template is named by the mangled
// symbols; its members are inferred (as in compartment_weak.cpp) and the
// element types are left incomplete. The destructor is a weak template copy
// emitted in this file, defined __declspec(weak) out of the class and
// instantiated explicitly. The rest of the file is not part of this unit.
#include "game/CScene.h"


extern "C" void MEM_free(void*);

namespace dwi {
template <class T>
class fast_vec {
public:
    ~fast_vec();

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};

template <class T> __declspec(weak) fast_vec<T>::~fast_vec() {
    if (m_owned && m_data)
        MEM_free(m_data);
}
}

template class dwi::fast_vec<CScene::SExclusionEntry>;
template class dwi::fast_vec<CScene::SModCommand>;
