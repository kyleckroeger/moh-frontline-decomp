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

#include "game/DwiContainers.h"

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
