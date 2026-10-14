// A fragment of scene.cpp (0x800d8ecc): the file-local dwi::quick_sort for the
// scene's node pairs, which hands the range to quick_sort_ with the first
// element as the value-type tag. CScene, UNodePair and the dwi templates are
// named by the mangled symbols; the pair view is that of scene_query.cpp. The
// template is file-local (static) in the original, as its local symbol shows,
// so it is static here and instantiated explicitly; quick_sort_ (the next
// function) is only declared. The rest of the file is not part of this unit.
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
template <class I, class C, class T>
void quick_sort_(I first, I last, C comp, T&);

template <class I, class C>
static void quick_sort(I first, I last, C comp) {
    quick_sort_(first, last, comp, *first);
}
}

typedef bool (*NodePairCompare)(const CScene::UNodePair&, const CScene::UNodePair&);

template void dwi::quick_sort(CScene::UNodePair*, CScene::UNodePair*, NodePairCompare);
