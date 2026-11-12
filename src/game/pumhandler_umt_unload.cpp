// A fragment of pumhandler.cpp (0x800b8544): CUserMessageTable::Unload frees
// the message records and closes the message text and index files, clearing
// each pointer and the message count; Load unloads any loaded table, loads
// the text and index files and builds the message records from the index
// (a byte-swapped count, then per message an extra-byte count and a length;
// each message is terminated in place). CUserMessageTable is named by the
// mangled symbols; its members and the record layout (a text pointer, a
// length and a byte at +5) are inferred from offsets. The record building is
// the inline member CreateUMStructFromFile, named and bounded as Rising Sun's
// out-of-line copy (CreateUMStructFromFile__17CUserMessageTable).
void TLT_CloseFile(void*);
void* TLT_LoadFileNormal(const char*, int*, int);
void DebugMsg(const char*, ...);

/* the ChangeEndian overloads as in matrix_endian.cpp */
inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

struct SUserMessage {
    char* m_text;
    unsigned char m_length;
    unsigned char m_extra;
};

class CUserMessageTable {
public:
    void Unload();
    void CreateUMStructFromFile();
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

inline void CUserMessageTable::CreateUMStructFromFile() {
    unsigned char* entry;
    char* end;
    unsigned short i;
    unsigned char extra;
    unsigned char length;
    unsigned char* index = m_index;
    int pos = 2;
    char* text = m_text;
    unsigned short count = *(unsigned short*)index;
    ChangeEndian(count);
    m_count = count;
    if (m_messages)
        delete[] m_messages;
    m_messages = new SUserMessage[m_count];
    for (i = 0; i < m_count; i++, pos += 2) {
        entry = index + pos;
        extra = entry[0];
        m_messages[i].m_extra = extra;
        length = entry[1];
        m_messages[i].m_text = text;
        end = text + length;
        text = end + 1 + extra;
        m_messages[i].m_length = length;
        *end = 0;
    }
}

void CUserMessageTable::Load(char* textFile, char* indexFile) {
    if (m_text || m_index || m_messages)
        Unload();
    m_text = (char*)TLT_LoadFileNormal(textFile, 0, 0);
    m_index = (unsigned char*)TLT_LoadFileNormal(indexFile, 0, 0);
    if (!m_text || !m_index) {
        DebugMsg("Can't find User message files: %s and %s!!!\n", textFile, indexFile);
        return;
    }
    CreateUMStructFromFile();
}
