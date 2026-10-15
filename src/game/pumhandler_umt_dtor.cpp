// A fragment of pumhandler.cpp (0x800b8770): the CUserMessageTable destructor
// (the records freed and both files closed, without clearing the pointers;
// the count cleared) and constructor. CUserMessageTable is named by the
// mangled symbols; its members are inferred from offsets (as in
// pumhandler_umt_unload.cpp). The rest of the file is not part of this unit.
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
