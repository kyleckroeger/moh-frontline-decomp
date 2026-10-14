// A fragment of scene.cpp (0x800d9c10): three file-local instances of the dwi
// sort helpers used to order the scene's node pairs: unguarded_partition_
// (Hoare partition around a pivot, swapping out-of-place pairs), median (the
// median of three pairs under the comparison) and iter_swap (swaps two
// pairs). CScene, UNodePair and the dwi templates are named by the mangled
// symbols; the pair view is that of scene_query.cpp. The templates are
// file-local (static) in the original, as their local symbols show, so they
// are static here and instantiated explicitly (in reverse emission order).
// quick_sort, quick_sort_ and sort_ before them and the rest of the file are
// not part of this unit.
class ISceneNode;

class CScene {
public:
    union UNodePair {
        struct {
            ISceneNode* first;
            ISceneNode* second;
            signed char order;
        } nodes;
        unsigned long long key;
        unsigned char data[16];
    };
};

namespace dwi {
template <class I1, class I2>
static void iter_swap(I1 a, I2 b) {
    CScene::UNodePair tmp = *a;
    *a = *b;
    *b = tmp;
}

template <class T, class C>
static T median(T a, T b, T c, C comp) {
    if (comp(a, b))
        return comp(b, c) ? b : (comp(a, c) ? c : a);
    else
        return comp(a, c) ? a : (comp(b, c) ? c : b);
}

template <class I, class T, class C>
static I unguarded_partition_(I first, I last, T pivot, C comp) {
    for (;;) {
        while (comp(*first, pivot))
            ++first;
        do
            --last;
        while (comp(pivot, *last));
        if (!(last > first))
            return first;
        iter_swap(first, last);
        ++first;
    }
}
}

typedef bool (*NodePairCompare)(const CScene::UNodePair&, const CScene::UNodePair&);

template void dwi::iter_swap(CScene::UNodePair*, CScene::UNodePair*);
template CScene::UNodePair dwi::median(CScene::UNodePair, CScene::UNodePair, CScene::UNodePair, NodePairCompare);
template CScene::UNodePair* dwi::unguarded_partition_(CScene::UNodePair*, CScene::UNodePair*, CScene::UNodePair,
                                                      NodePairCompare);
