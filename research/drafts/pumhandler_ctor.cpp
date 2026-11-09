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
};
class CColor {
public:
    CColor(unsigned int rgba) : m_rgba(rgba) {}
    CColor(const CColor& other) : m_rgba(other.m_rgba) {}
    unsigned int m_rgba;
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
    UserMessage* m_table;
    char* m_strings;
    unsigned char* m_lengths;
    int m_count;
};
CUserMessageTable umTable;

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
    if (umTable.m_strings || umTable.m_lengths || umTable.m_table)
        umTable.Unload();
    umTable.m_strings = (char*)TLT_LoadFileNormal(stringFile, 0, 0);
    umTable.m_lengths = (unsigned char*)TLT_LoadFileNormal(lengthFile, 0, 0);
    char* text = umTable.m_strings;
    unsigned char* lengths = umTable.m_lengths;
    if (!text || !lengths) {
        DebugMsg("Can't find User message files: %s and %s!!!\n", stringFile, lengthFile);
    } else {
        umTable.m_count = (lengths[1] << 8) | lengths[0];
        if (umTable.m_table)
            delete[] umTable.m_table;
        umTable.m_table = new UserMessage[umTable.m_count];
        int offset = 2;
        for (unsigned short i = 0; i < umTable.m_count; i++) {
            unsigned char* entry = lengths + offset;
            offset += 2;
            umTable.m_table[i].m_length = entry[0];
            unsigned char padding = entry[1];
            char* end = text + padding;
            umTable.m_table[i].m_text = text;
            text = end + entry[0] + 1;
            umTable.m_table[i].m_padding = padding;
            *end = 0;
        }
    }

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
    CColor color(0x3f3f3f80);
    m_font->SetColor(color);
    CScreen* screen = CScreen::GetCurrent();
    if (screen)
        m_font->SetDepth(screen->GetDepthBufferMax());
    m_subtitleFont = new CFont(subtitleData, (ERenderPriority)0x70);
    CColor subtitleColor(0x3f3f3f3f);
    m_subtitleFont->SetColor(subtitleColor);
    m_subtitleFont->SetScale(1.0f, 1.0f);
    if (screen)
        m_subtitleFont->SetDepth(screen->GetDepthBufferMax());
    if (!m_back)
        m_back = new (DWI_alloc(0, 100, 0x400)) CSprite("data\\bitmaps\\popupBack.gsh", (CSprite::ESpriteLoadType)1, (ERenderPriority)0x6e);
    if (!m_subtitleBack)
        m_subtitleBack = new (DWI_alloc(0, 100, 0x400)) CSprite("data\\bitmaps\\popupSubtitleBack.gsh", (CSprite::ESpriteLoadType)1, (ERenderPriority)10);
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
