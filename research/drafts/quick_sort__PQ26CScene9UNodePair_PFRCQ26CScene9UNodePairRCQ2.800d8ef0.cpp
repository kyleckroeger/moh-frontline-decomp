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
template <class I, class T, class C>
void sort_(I first, I last, C comp, T*);

template <class I1, class I2>
inline void iter_swap(I1 a, I2 b) {
    CScene::UNodePair tmp = *a;
    *a = *b;
    *b = tmp;
}

template <class I, class T, class C>
inline void uli2(I last, I next, T value, C comp) {
    while (comp(value, *next)) {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

template <class I, class T, class C>
inline void uli3(I last, T value, C comp, I next) {
    while (comp(value, *next)) {
        *last = *next;
        last = next;
        --next;
    }
    *last = value;
}

template <class I, class C>
inline void fin(I i, I last, C comp) {
    for (; i != last; ++i)
        unguarded_linear_insert(i, *i, comp);
}

template <class I, class C>
inline void fin2(I mid, I last, C comp) {
    for (I i = mid; i != last; ++i)
        unguarded_linear_insert(i, *i, comp);
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
inline void insertion_sort(I first, I last, C comp) {
    I i;
    if (first + 1 < last) {
        if (first + 2 == last) {
            if (comp(*(last - 1), *first))
                iter_swap(first, last - 1);
        } else {
            for (i = last - 1; i != first; --i) {
                I prev = i - 1;
                if (comp(*i, *prev))
                    iter_swap(i, prev);
            }
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
