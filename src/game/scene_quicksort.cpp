// A fragment of scene.cpp (0x800d8ecc): the file-local dwi::quick_sort for the
// scene's node pairs, which hands the range to quick_sort_ with the first
// element as the value-type tag, and quick_sort_ itself (0x800d8ef0): ranges of
// up to 16 pairs get an insertion sort, longer ones are partitioned by sort_
// (declared only) and then finished by an insertion sort of the first 16 pairs
// and unguarded linear inserts for the rest. CScene, UNodePair and the dwi
// templates are named by the mangled symbols; the pair view is that of
// scene_query.cpp. The templates are file-local (static) in the original, as
// their local symbols show, so they are static here and instantiated
// explicitly. The helper bodies are inferred: the insertion sort's backward
// pass is its own inline (backward_pass) with the swap written out, which gives
// the original's register and stack-slot order (an inline's locals are
// numbered before the locals of the inlines it calls). The rest of the file is
// not part of this unit.
class ISceneNode;

#include "game/CScene.h"


namespace dwi {
template <class I, class T, class C>
void sort_(I first, I last, C comp, T*);

template <class I1, class I2>
inline void iter_swap(I1 a, I2 b) {
    CScene::UNodePair tmp = *a;
    *a = *b;
    *b = tmp;
}

template <class I, class T, class C>
inline void unguarded_linear_insert(I last, T value, C comp) {
    I next = last - 1;
    while (comp(value, *next)) {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

template <class I, class C>
inline void backward_pass(I first, I last, C comp) {
    for (I i = last - 1; i != first; --i) {
        I prev = i - 1;
        if (comp(*i, *prev)) {
            CScene::UNodePair tmp = *i;
            *i = *prev;
            *prev = tmp;
        }
    }
}

template <class I, class C>
inline void insertion_sort(I first, I last, C comp) {
    I i;
    if (first + 1 < last) {
        if (first + 2 == last) {
            if (comp(*(last - 1), *first))
                iter_swap(first, last - 1);
        } else {
            backward_pass(first, last, comp);
            for (i = first + 2; i != last; ++i)
                unguarded_linear_insert(i, *i, comp);
        }
    }
}

template <class I, class C, class T>
static void quick_sort_(I first, I last, C comp, T&) {
    if (last - first <= 16) {
        insertion_sort(first, last, comp);
    } else {
        sort_(first, last, comp, (T*)0);
        I mid = first + 16;
        insertion_sort(first, mid, comp);
        for (I i = mid; i != last; ++i)
            unguarded_linear_insert(i, *i, comp);
    }
}

template <class I, class C>
static void quick_sort(I first, I last, C comp) {
    quick_sort_(first, last, comp, *first);
}
}

typedef bool (*NodePairCompare)(const CScene::UNodePair&, const CScene::UNodePair&);

template void dwi::quick_sort(CScene::UNodePair*, CScene::UNodePair*, NodePairCompare);
