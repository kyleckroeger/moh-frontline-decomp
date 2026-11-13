// The end of pumhandler.cpp (0x800ba7ac): the file's static initialiser.
// It constructs the user message table and the subtitle queue in the file's
// .bss block and registers their destructors. The CUserMessageTable
// constructor (placed by pumhandler_umt_dtor, a file duplicate here) is
// compiled first and inlined into the initialiser. The queue's sentinel is
// built from a default QueuedSubtitle whose times are never set: the
// compiler keeps them in f31, saved but never loaded, as in the original.
// CUserMessageTable, QueuedSubtitle, the dwi list and the globals are named
// by the mangled symbols; the members, the inline list constructor and the
// QueuedSubtitle view (its colour as four bytes, inferred: a CColor member
// keeps the times on the stack) are inferred. lastString is a size view of
// AddSubtitle's 50-byte static buffer, which sits between the two objects in
// the original block. The rest of the file is not part of this unit.

/* inferred: a queued subtitle's delay, on-screen time, colour and text */
struct QueuedSubtitle {
    QueuedSubtitle() : m_r(0), m_g(0), m_b(0), m_a(128) {}

    float m_delay;
    float m_time;
    unsigned char m_r;
    unsigned char m_g;
    unsigned char m_b;
    unsigned char m_a;
    char m_text[48];
};

#include "game/DwiContainers.h"

struct SUserMessage {
    char* m_text;
    unsigned char m_length;
    unsigned char m_extra;
};

class CUserMessageTable {
public:
    ~CUserMessageTable();
    CUserMessageTable();

    SUserMessage* m_messages;
    char* m_text;
    unsigned char* m_index;
    int m_count;
};

CUserMessageTable::CUserMessageTable() {
    m_messages = 0;
    m_index = 0;
    m_text = 0;
    m_count = 0;
}

CUserMessageTable umTable;
/* view: AddSubtitle's static buffer (lastString), placed by pumhandler_add */
static char s_lastStringView[50];
dwi::list<QueuedSubtitle> g_subtitle_queue;
