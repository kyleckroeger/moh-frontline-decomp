// A fragment of pumhandler.cpp (0x800b913c): UpdateSubtitles. The subtitle
// queue is given a pool of 20 nodes on first use; then each queued subtitle
// whose delay has run out is shown as a pop-up (at 140 and 402 scaled by the
// screen's scale factors, for its on-screen time, in its colour) and erased,
// and the others wait the frame time less. The dwi templates,
// QueuedSubtitle and the globals are named by the symbols; the list view
// and its inline erase follow pumhandler_list_reserve.cpp and
// alarm_register.cpp, and the subtitle members, the screen's scale factors,
// the handler view (four pointers, the first used) and the fixed pop-up
// arguments are inferred. The rest of the file is not part of this unit.
extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);
inline void* operator new(unsigned long, void* p) { return p; }

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/* inferred: a queued subtitle's delay, on-screen time, colour and text */
struct QueuedSubtitle {
    float m_delay;
    float m_time;
    CColor m_color;
    char m_text[48];
};

namespace dwi {
template <class T>
struct list_node {
    list_node(list_node* prev, list_node* next, const T& data) : m_prev(prev), m_next(next), m_data(data) {}

    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list_iter {
public:
    list_iter(list_node<T>* node) : m_node(node) {}
    list_iter(const list_iter& other) : m_node(other.m_node) {}
    T& operator*() const { return m_node->m_data; }
    list_iter& operator++() {
        m_node = m_node->m_next;
        return *this;
    }
    bool operator!=(const list_iter& other) const { return m_node != other.m_node; }

    list_node<T>* m_node;
};

template <class T>
class list {
public:
    template <class I>
    list(int, I, I, char*);
    list_node<T>* alloc_node() {
        list_node<T>* node = m_free;
        m_free = node->m_next;
        return node;
    }
    list_iter<T> begin() { return list_iter<T>(m_head.m_next); }
    list_iter<T> end() { return list_iter<T>(&m_head); }
    ~list() {
        list_node<T>* node = m_head.m_next;
        list_node<T>* next;

        while (node != &m_head) {
            next = node->m_next;
            node->~list_node<T>();
            node->m_prev = 0;
            node->m_next = m_free;
            m_free = node;
            node = next;
        }
        m_head.m_next = &m_head;
        m_head.m_prev = &m_head;
        m_count = 0;
        if (m_owned)
            MEM_free(m_pool);
        m_pool = 0;
    }

    void swap(list& other) {
        int count = m_count;
        m_count = other.m_count;
        other.m_count = count;
        int capacity = m_capacity;
        m_capacity = other.m_capacity;
        other.m_capacity = capacity;
        list_node<T>* pool = m_pool;
        m_pool = other.m_pool;
        other.m_pool = pool;
        list_node<T>* free = m_free;
        m_free = other.m_free;
        other.m_free = free;
        bool owned = m_owned;
        m_owned = other.m_owned;
        other.m_owned = owned;
        if (m_count != 0) {
            if (other.m_count != 0) {
                list_node<T>* prev = m_head.m_prev;
                m_head.m_prev = other.m_head.m_prev;
                other.m_head.m_prev = prev;
                list_node<T>* next = m_head.m_next;
                m_head.m_next = other.m_head.m_next;
                other.m_head.m_next = next;
                m_head.m_prev->m_next = &m_head;
                other.m_head.m_prev->m_next = &other.m_head;
            } else {
                other.m_head.m_prev = m_head.m_prev;
                other.m_head.m_next = m_head.m_next;
                other.m_head.m_prev->m_next = &other.m_head;
                m_head.m_next = &m_head;
                m_head.m_prev = &m_head;
            }
        } else if (other.m_count != 0) {
            m_head.m_prev = other.m_head.m_prev;
            m_head.m_next = other.m_head.m_next;
            m_head.m_prev->m_next = &m_head;
            other.m_head.m_next = &other.m_head;
            other.m_head.m_prev = &other.m_head;
        }
    }

    void reserve(int, char*);
    list_node<T>* erase(list_node<T>* node) {
        list_node<T>* prev = node->m_prev;
        list_node<T>* next = node->m_next;
        prev->m_next = next;
        next->m_prev = prev;
        node->m_prev = 0;
        node->m_next = m_free;
        m_free = node;
        m_count--;
        return next;
    }

    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
}

class PopUpMessageHandler {
public:
    void AddPopUpMessage(char*, unsigned char, unsigned short, unsigned short, unsigned char, unsigned char,
                         unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
};

/* inferred: the screen's scale factors at +40 and +44 */
struct ScreenView {
    unsigned char unknown00[40];
    float m_scaleX;
    float m_scaleY;
    unsigned char unknown30[0xe0];
};

extern ScreenView g_screen;
extern PopUpMessageHandler* g_pPopUpMessageHandler[4];
extern dwi::list<QueuedSubtitle> g_subtitle_queue;
extern bool g_subtitle_queue_initialized;

void UpdateSubtitles(float elapsed) {
    if (!g_subtitle_queue_initialized) {
        g_subtitle_queue.reserve(20, "Subtitle queue");
        g_subtitle_queue_initialized = true;
    }
    dwi::list_node<QueuedSubtitle>* node = g_subtitle_queue.m_head.m_next;
    ScreenView* screen = &g_screen;
    dwi::list_node<QueuedSubtitle>* end = &g_subtitle_queue.m_head;
    while (node != end) {
        if (node->m_data.m_delay <= elapsed) {
            CColor color = node->m_data.m_color;
            g_pPopUpMessageHandler[0]->AddPopUpMessage(node->m_data.m_text, 1, 140.0f * screen->m_scaleX,
                                                       402.0f * screen->m_scaleY, 1, node->m_data.m_time, 22,
                                                       color.r, color.g, color.b, color.a);
            node = g_subtitle_queue.erase(node);
        } else {
            node->m_data.m_delay -= elapsed;
            node = node->m_next;
        }
    }
}
