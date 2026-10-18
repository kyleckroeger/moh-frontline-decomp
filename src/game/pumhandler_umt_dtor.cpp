// A fragment of pumhandler.cpp (0x800b8770): the CUserMessageTable destructor
// (the records freed and both files closed, without clearing the pointers;
// the count cleared) and constructor, then DisplaySubtitle: the subtitle
// queue (reserved for 20 entries on first use) is cleared and the text is
// split into lines of up to 25 characters at spaces (a word may run the line
// to 29), each queued with the colour and time; the time is raised to 1.5
// seconds per line and divided by the number of four-line pages, and each
// line is delayed by its page index times that time, in frames (the time
// divided by 0.016683). CUserMessageTable, QueuedSubtitle and the
// globals are named by the mangled symbols; the members, the subtitle and
// list views (as in pumhandler_subtitles.cpp) and the inline list helpers
// are inferred. The queue name is a .rodata and the constants .sdata2 pool
// entries. The rest of the file is not part of this unit.
extern "C" unsigned long strlen(const char*);
extern "C" char* strchr(const char*, int);
extern "C" char* strncpy(char*, const char*, unsigned long);
extern "C" char* strcpy(char*, const char*);
extern "C" void* memset(void*, int, unsigned long);
void TLT_CloseFile(void*);

inline void* operator new(unsigned long, void* p) { return p; }

struct CColor {
    CColor() : r(0), g(0), b(0), a(128) {}

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
    list_node<T>* alloc_node() {
        list_node<T>* node = m_free;
        m_free = node->m_next;
        return node;
    }
    list_iter<T> begin() { return list_iter<T>(m_head.m_next); }
    list_iter<T> end() { return list_iter<T>(&m_head); }
    void clear() {
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
    }
    void push_back(const T& value) {
        list_node<T>* prev = m_head.m_prev;
        list_node<T>* pos = &m_head;
        list_node<T>* node = alloc_node();
        new (node) list_node<T>(0, 0, value);
        m_count++;
        prev->m_next = node;
        node->m_prev = prev;
        node->m_next = pos;
        pos->m_prev = node;
    }
    void reserve(int, char*);

    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
}

extern dwi::list<QueuedSubtitle> g_subtitle_queue;
extern bool g_subtitle_queue_initialized;


struct SUserMessage {
    char* m_text;
    unsigned char m_length;
    unsigned char m_extra;
};

class CUserMessageTable {
public:
    void Unload();
    void Load(char*, char*);
    ~CUserMessageTable();
    CUserMessageTable();

    SUserMessage* m_messages;
    char* m_text;
    unsigned char* m_index;
    int m_count;
};

CUserMessageTable::~CUserMessageTable() {
    if (m_messages)
        delete[] m_messages;
    if (m_text)
        TLT_CloseFile(m_text);
    if (m_index)
        TLT_CloseFile(m_index);
    m_count = 0;
}

CUserMessageTable::CUserMessageTable() {
    m_messages = 0;
    m_index = 0;
    m_text = 0;
    m_count = 0;
}

void DisplaySubtitle(const char* text, float time, CColor color) {
    int length = strlen(text);
    signed char lines = 0;
    if (!g_subtitle_queue_initialized) {
        g_subtitle_queue.reserve(20, "Subtitle Queue");
        g_subtitle_queue_initialized = true;
    }
    g_subtitle_queue.clear();
    while (length > 25) {
        const char* end = text;
        while (end - text <= 25) {
            const char* space = strchr(end, ' ');
            if (space) {
                space++;
                if (space - text > 29)
                    break;
                end = space;
            } else {
                if (length <= 29)
                    end = text + length;
                break;
            }
        }
        QueuedSubtitle line;
        memset(line.m_text, 0, sizeof(line.m_text));
        strncpy(line.m_text, text, end - text);
        line.m_time = time;
        line.m_color = color;
        line.m_delay = 0.0f;
        g_subtitle_queue.push_back(line);
        while (*end == ' ')
            end++;
        text = end;
        length = strlen(end);
        lines++;
    }
    if (length > 0) {
        QueuedSubtitle line;
        memset(line.m_text, 0, sizeof(line.m_text));
        strcpy(line.m_text, text);
        line.m_time = time;
        line.m_color = color;
        line.m_delay = 0.0f;
        g_subtitle_queue.push_back(line);
        lines++;
    }
    float minimum = 1.5f * lines;
    if (time < minimum)
        time = minimum;
    if (lines >= 4)
        time /= lines / 4;
    int i = 0;
    for (dwi::list_iter<QueuedSubtitle> it = g_subtitle_queue.begin(); it != g_subtitle_queue.end(); ++it, i++) {
        (*it).m_time = time;
        (*it).m_delay = (i / 4) * time / 0.016683351f;
    }
}
