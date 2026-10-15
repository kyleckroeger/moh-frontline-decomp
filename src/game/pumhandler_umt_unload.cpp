// A fragment of pumhandler.cpp (0x800b8544): CUserMessageTable::Unload frees
// the message records and closes the message text and index files, clearing
// each pointer and the message count. CUserMessageTable is named by the
// mangled symbols; its members and the record layout (a text pointer, a
// length and a byte at +5) are inferred from offsets. Load, after this
// function, is not part of this unit.
void TLT_CloseFile(void*);

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

void CUserMessageTable::Unload() {
    if (m_messages) {
        delete[] m_messages;
        m_messages = 0;
    }
    if (m_text) {
        TLT_CloseFile(m_text);
        m_text = 0;
    }
    if (m_index) {
        TLT_CloseFile(m_index);
        m_index = 0;
    }
    m_count = 0;
}
