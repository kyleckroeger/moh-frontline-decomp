// A fragment of compartment.cpp (0x80076f58): weak copies emitted in this
// file: CCompartment's bounding-volume getters (the embedded volume at +40
// for every type) and the dwi::fast_vec<CLight*> constructor (empty, owning
// its storage, with a growth value), push_back, begin, end and size.
// The classes, templates and functions are named by the mangled symbols; the
// fast_vec members are inferred (the layout differs from the view in
// scene.cpp, which only uses the first three words), CCompartment is a
// non-virtual view, the record types are left incomplete, and explicit
// instantiations reproduce the order of the weak copies. The rest of the file
// is not part of this unit.
extern "C" void MEM_free(void*);

class CLight;
class IVolume;
struct ShapeFile;
struct CPTTexSwap;
struct gcVertex;
struct CPTNode;
struct CPTChunk;
class CCPTMatBin;

#include "game/ISceneNode.h"


/* inferred: storage of the embedded volume (its class is not known) */
struct EmbeddedVolumeView {
    unsigned char data[16];
};

class CCompartment {
public:
    IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;

    unsigned char unknown00[40];
    EmbeddedVolumeView m_volume;
};

namespace dwi {
template <class T>
class fast_vec {
public:
    fast_vec(int);
    ~fast_vec();
    void push_back(T);
    T* begin();
    T* end();
    int size() const;
    void reserve(T*, int);

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};

template <class T> __declspec(weak) fast_vec<T>::fast_vec(int growth) {
    m_data = 0;
    m_unknown4 = 0;
    m_size = 0;
    m_capacity = 0;
    m_owned = true;
    m_growth = growth;
}

template <class T> __declspec(weak) void fast_vec<T>::push_back(T value) {
    m_data[m_size++] = value;
}

template <class T> __declspec(weak) T* fast_vec<T>::begin() {
    return m_data;
}

template <class T> __declspec(weak) T* fast_vec<T>::end() {
    return m_data + m_size;
}

template <class T> __declspec(weak) int fast_vec<T>::size() const {
    return m_size;
}
}

__declspec(weak) IVolume* CCompartment::GetLocalBoundingVolume(ISceneNode::EVolumeType) const {
    return (IVolume*)&m_volume;
}

__declspec(weak) IVolume* CCompartment::GetWorldBoundingVolume(ISceneNode::EVolumeType) const {
    return (IVolume*)&m_volume;
}

template class dwi::fast_vec<CLight*>;
