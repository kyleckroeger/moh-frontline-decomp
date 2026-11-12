// pumhandler.cpp: pop-up message handler construction and the file's globals, as
// partial source. Message text comes from two files: one holds all the strings,
// the other a big-endian count then a (padding, length) byte pair per message.
inline void* operator new(unsigned long, void* p) { return p; }
void* DWI_alloc(const char*, int, int);
void DebugMsg(const char*, ...);
void* TLT_LoadFileNormal(const char*, int*, int);
void TLT_CloseFile(void*);
extern bool g_bInMultiplayerMode;
extern int POPUPS_IN_SECTION;

enum ERenderPriority {};
class CSprite {
public:
    enum ESpriteLoadType {};
    CSprite(const char*, ESpriteLoadType, ERenderPriority);
    void* operator new(unsigned long size) { return DWI_alloc(0, size, 0x400); }
    unsigned char unknown[100];
};
class CColor {
public:
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};
class CFont {
public:
    CFont(const char*, ERenderPriority);
    void SetColor(CColor);
    void SetScale(float, float);
    void SetDepth(unsigned long);
    unsigned char unknown[0x28];
};
class CScreen {
public:
    static CScreen* GetCurrent();
    unsigned long GetDepthBufferMax();
};
enum TLTResourceID {};
struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    char** m_data;
};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
const char* TLT_LoadFileFromLevelBigFile(const char*, int*);

struct UserMessage {
    char* m_text;
    unsigned char m_padding;
    unsigned char m_length;
};
class CUserMessageTable {
public:
    CUserMessageTable() : m_table(0), m_lengths(0), m_strings(0), m_count(0) {}
    ~CUserMessageTable();
    void Unload() {
        if (m_table) {
            delete[] m_table;
            m_table = 0;
        }
        if (m_strings) {
            TLT_CloseFile(m_strings);
            m_strings = 0;
        }
        if (m_lengths) {
            TLT_CloseFile(m_lengths);
            m_lengths = 0;
        }
        m_count = 0;
    }
    void Load(char*, char*);
    UserMessage* m_table;
    char* m_strings;
    unsigned char* m_lengths;
    int m_count;
};
CUserMessageTable umTable;

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

inline void CUserMessageTable::Load(char* textFile, char* indexFile) {
    int pos;
    unsigned char* index;
    char* text;
    if (m_strings || m_lengths || m_table)
        Unload();
    m_strings = (char*)TLT_LoadFileNormal(textFile, 0, 0);
    m_lengths = (unsigned char*)TLT_LoadFileNormal(indexFile, 0, 0);
    text = m_strings;
    if (!text || !(index = m_lengths)) {
        DebugMsg("Can't find User message files: %s and %s!!!\n", textFile, indexFile);
        return;
    }
    pos = 2;
    unsigned short count = *(unsigned short*)index;
    ChangeEndian(count);
    m_count = count;
    if (m_table)
        delete[] m_table;
    m_table = new UserMessage[m_count];
    for (unsigned short i = 0; i < m_count; i++, pos += 2) {
        unsigned char* entry = index + pos;
        unsigned char extra = entry[0];
        m_table[i].m_length = extra;
        unsigned char length = entry[1];
        m_table[i].m_text = text;
        char* end = text + length;
        text = end + 1 + extra;
        m_table[i].m_padding = length;
        *end = 0;
    }
}

class PopUpMessage {
public:
    PopUpMessage();
    ~PopUpMessage();
    unsigned char unknown[0x4c];
};
struct ShellView {
    unsigned char unknown[0x19a6];
    unsigned char m_subtitles;
};
extern ShellView g_Shell;

class PopUpMessageHandler {
public:
    PopUpMessageHandler(char*, char*);
    PopUpMessage m_messages[20];
    unsigned char unknown5f0[4];
    CFont* m_font;          // +0x5f4
    CFont* m_subtitleFont;  // +0x5f8
    CSprite* m_back;        // +0x5fc
    CSprite* m_subtitleBack;// +0x600
    unsigned char unknown604[4];
    unsigned char m_subtitles;
};

PopUpMessageHandler::PopUpMessageHandler(char* stringFile, char* lengthFile) {
    umTable.Load(stringFile, lengthFile);

    LevelFileContentsStruct_* fonts = TLT_FindNextResourceByType((TLTResourceID)11, 0);
    const char* fontData;
    if (!g_bInMultiplayerMode)
        fontData = TLT_LoadFileFromLevelBigFile(((char**)fonts->m_data)[0x164 / 4], 0);
    else
        fontData = TLT_LoadFileFromLevelBigFile(((char**)fonts->m_data)[0x1c / 4], 0);
    const char* subtitleData = TLT_LoadFileFromLevelBigFile(((char**)fonts->m_data)[0x14 / 4], 0);
    if (g_bInMultiplayerMode)
        POPUPS_IN_SECTION = 2;

    m_font = new CFont(fontData, (ERenderPriority)0x6f);
    m_font->SetColor(CColor(63, 63, 63, 128));
    CScreen* screen = CScreen::GetCurrent();
    if (screen)
        m_font->SetDepth(screen->GetDepthBufferMax());
    m_subtitleFont = new CFont(subtitleData, (ERenderPriority)0x70);
    m_subtitleFont->SetColor(CColor(63, 63, 63, 63));
    m_subtitleFont->SetScale(1.0f, 1.0f);
    if (screen)
        m_subtitleFont->SetDepth(screen->GetDepthBufferMax());
    if (!m_back)
        m_back = new CSprite("data\\bitmaps\\popupBack.gsh", (CSprite::ESpriteLoadType)1, (ERenderPriority)0x6e);
    if (!m_subtitleBack)
        m_subtitleBack = new CSprite("data\\bitmaps\\popupSubtitleBack.gsh", (CSprite::ESpriteLoadType)1, (ERenderPriority)10);
    m_subtitles = g_Shell.m_subtitles;
}

// The subtitle queue. Only its sentinel's first fields are initialised; the rest
// of the temporary is copied uninitialised, as in the original.
struct QueuedSubtitle {
    QueuedSubtitle() : m_x(0.0f), m_y(0.0f), m_r(0), m_g(0), m_b(0), m_a(0x80) {}
    float m_x;
    float m_y;
    unsigned char m_r, m_g, m_b, m_a;
    unsigned char m_rest[48];
};
namespace dwi {
template <class T> struct list_node {
    list_node(list_node* prev, list_node* next, const T& data) : m_prev(prev), m_next(next), m_data(data) {}
    list_node* m_prev;
    list_node* m_next;
    T m_data;
};
template <class T> class list {
public:
    list() : m_count(0), m_capacity(0), m_pool(0), m_free(0), m_head(&m_head, &m_head, T()), m_owned(true) {}
    ~list();
    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};
}
dwi::list<QueuedSubtitle> g_subtitle_queue;
