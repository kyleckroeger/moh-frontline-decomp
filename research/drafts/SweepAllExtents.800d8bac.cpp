extern "C" unsigned long OSGetTick();

class ISceneNode {
public:
    struct SExtent {
        ISceneNode* m_node;
        bool m_max;
        unsigned char unknown05[3];
        float m_value;
        unsigned char unknown0c[4];

        bool operator<(const SExtent& other) const {
            bool less = false;
            if (m_value < other.m_value || (m_value == other.m_value && m_max && !other.m_max))
                less = true;
            return less;
        }
    };
};

namespace dwi {
template <class T>
struct list_node {
    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list {
public:
    void sort() {
        if (m_count > 1) {
            list_node<T>* first = m_head.m_next;
            list_node<T>* p;
            for (p = m_head.m_next; p != &m_head; p = p->m_next) {
                if (p->m_data < first->m_data)
                    first = p;
            }
            list_node<T>* next = first->m_next;
            list_node<T>* prev = first->m_prev;
            prev->m_next = next;
            next->m_prev = prev;
            list_node<T>* front = m_head.m_next;
            first->m_next = front;
            front->m_prev = first;
            m_head.m_next = first;
            first->m_prev = &m_head;
            for (p = first->m_next; p != &m_head; p = p->m_next) {
                list_node<T>* q = p;
                do {
                    q = q->m_prev;
                } while (p->m_data < q->m_data);
                list_node<T>* before = p->m_prev;
                if (q != before) {
                    list_node<T>* after = p->m_next;
                    before->m_next = after;
                    after->m_prev = before;
                    list_node<T>* qnext = q->m_next;
                    p->m_next = qnext;
                    qnext->m_prev = p;
                    q->m_next = p;
                    p->m_prev = q;
                    p = q;
                }
            }
        }
    }

    int m_count;
    int m_unknown4;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

template <class T>
class fast_vec {
public:
    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

class CScene {
public:
    void SweepAllExtents();
    struct SExclusionEntry;
    union UNodePair {
        struct {
            ISceneNode* first;
            ISceneNode* second;
            signed char order;
        } nodes;
        unsigned long long key;
        unsigned char data[16];
    };

    unsigned char unknown000[128];
    dwi::list<ISceneNode::SExtent> m_extents128;
    dwi::list<ISceneNode::SExtent> m_extents172;
    dwi::fast_vec<UNodePair> m_pairs;
    dwi::fast_vec<UNodePair> m_pairs2;
    unsigned char unknown108[16];
    dwi::fast_vec<SExclusionEntry> m_exclusions; /* +0x118, as in src/game/scene_visit.cpp */
};

void SweepExtents(dwi::list<ISceneNode::SExtent>&, dwi::fast_vec<CScene::UNodePair>&,
                  dwi::fast_vec<CScene::SExclusionEntry>&);
bool CmpNodePairs(const CScene::UNodePair&, const CScene::UNodePair&);

namespace dwi {
template <class I, class C>
void quick_sort(I first, I last, C comp);
}

typedef bool (*NodePairCompare)(const CScene::UNodePair&, const CScene::UNodePair&);

void CScene::SweepAllExtents() {
    long times[3];
    m_pairs.m_size = 0;
    times[0] = -OSGetTick();
    m_extents128.sort();
    times[0] += OSGetTick();
    times[1] = -OSGetTick();
    SweepExtents(m_extents128, m_pairs, m_exclusions);
    times[1] += OSGetTick();
    times[2] = -OSGetTick();
    dwi::quick_sort(m_pairs.m_data, m_pairs.m_data + m_pairs.m_size, CmpNodePairs);
    times[2] += OSGetTick();
    m_pairs2.m_size = 0;
    times[0] -= OSGetTick();
    m_extents172.sort();
    times[0] += OSGetTick();
    times[1] -= OSGetTick();
    SweepExtents(m_extents172, m_pairs2, m_exclusions);
    times[1] += OSGetTick();
    times[2] -= OSGetTick();
    dwi::quick_sort(m_pairs2.m_data, m_pairs2.m_data + m_pairs2.m_size, CmpNodePairs);
    times[2] += OSGetTick();
}
