// pumhandler.cpp: the pop-up message handler, the user-message table and the subtitle queue.
// CUserMessageTable::Unload/Load (the record building in the inline CreateUMStructFromFile), the
// table's destructor and constructor, DisplaySubtitle (splits a text into queued subtitle lines),
// the weak dwi::list<QueuedSubtitle> reserve/~list/range constructor, UpdateSubtitles, the two
// PromptUserMessage overloads, PopUpMessageHandler's AddPopUpMessage, PopUpMessageHandler_Loop,
// destructor and constructor, and the static initialiser of umTable and g_subtitle_queue.
// Names come from the mangled symbols; the class layouts, views and local names are inferred.
//
// Compiled with deferred inlining and written last to first: the original numbers the function
// statics and the two dtor-chain records (umTable before the loop's and AddPopUpMessage's
// statics, g_subtitle_queue after them) before every extab label, and the .text order is the
// reverse of the source order. With deferred inlining the table's constructor is inlined into
// the static initialiser, Unload into Load and Load into the handler's constructor, as in the
// original. The list members are all defined in the class: a weak copy of an in-class member is
// emitted where its first user stands (here reserve, ~list and the range constructor right
// before UpdateSubtitles), while out-of-class template members go to the end of the file.
// ~list goes through clear() so the out-of-line copy and the one inlined into reserve colour
// alike (an expanded inline numbers its locals in reverse declaration order).
// QueuedSubtitle's constructor sets its colour bytes in its body (a CColor initialiser takes
// the colour's address and keeps the temporary's times on the stack in the initialiser);
// UpdateSubtitles passes the colour by value to a small inline (ShowSubtitle, inferred), which
// gives the original's word copy of it. 12.0f has its own .sdata2 label between UpdateSubtitles'
// and PromptUserMessage's extab labels (probably a function the original linker removed), so .sdata2 is
// pooled.
extern "C" unsigned long strlen(const char*);
extern "C" char* strchr(const char*, int);
extern "C" char* strncpy(char*, const char*, unsigned long);
extern "C" char* strcpy(char*, const char*);
extern "C" void* memset(void*, int, unsigned long);
extern "C" void MEM_free(void*);
extern "C" void FONT_getrecta(void*, const char*, float*, float*, float*, float*);
void* DWI_allocalign(const char*, int, int, int);
void* DWI_alloc(const char*, int, int);
void DebugMsg(const char*, ...);
void* TLT_LoadFileNormal(const char*, int*, int);
void TLT_CloseFile(void*);
inline void* operator new(unsigned long, void* p) { return p; }

#include "game/DwiHashMap.h"
#include "game/CScreen.h"

/* inferred: a queued subtitle's delay, on-screen time, colour and text */
struct QueuedSubtitle {
    QueuedSubtitle() {
        m_color.r = 0;
        m_color.g = 0;
        m_color.b = 0;
        m_color.a = 128;
    }
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
    list() : m_count(0), m_capacity(0), m_pool(0), m_free(0), m_head(&m_head, &m_head, T()), m_owned(true) {}
    template <class I>
    list(int capacity, I first, I last, char* name)
        : m_count(0), m_capacity(capacity),
          m_pool((list_node<T>*)DWI_allocalign(name, m_capacity * sizeof(list_node<T>), 16, 1024)), m_free(0),
          m_head(&m_head, &m_head, T()), m_owned(true) {
        init_free();
        link_range(end(), first, last);
    }
    template <class I2> void link_range(I2 end, I2 first, I2 last) {
        list_node<T>* head = end.m_node;
        list_node<T>* stop = last.m_node;
        list_node<T>* tail = m_head.m_prev;
        for (I2 it = first; it.m_node != stop; ++it) {
            list_node<T>* added = new (alloc_node()) list_node<T>(0, 0, *it);
            tail->m_next = added;
            added->m_prev = tail;
            tail = added;
            m_count++;
        }
        tail->m_next = head;
        head->m_prev = tail;
    }
    void init_free() {
        list_node<T>* next = 0;
        list_node<T>* node = m_pool + (m_capacity - 1);
        while (node != m_pool) {
            node->m_next = next;
            next = node;
            node--;
        }
        node->m_next = next;
        m_free = node;
    }
    list_node<T>* alloc_node() {
        list_node<T>* node = m_free;
        m_free = node->m_next;
        return node;
    }
    list_iter<T> begin() { return list_iter<T>(m_head.m_next); }
    list_iter<T> end() { return list_iter<T>(&m_head); }
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
    ~list() {
        clear();
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

    void reserve(int capacity, char* name) {
        if (capacity > m_capacity) {
            list<T> grown(capacity, begin(), end(), name);
            swap(grown);
        }
    }

    int m_count;
    int m_capacity;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

}

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
    /* inferred inline lookup */
    char* GetMessage(unsigned int id) {
        if ((int)id < m_count)
            return m_messages[id].m_text;
        return 0;
    }

    SUserMessage* m_messages;
    char* m_text;
    unsigned char* m_index;
    int m_count;
};

enum ERenderPriority {};
enum TLTResourceID {};
struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    char** m_data;
};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
const char* TLT_LoadFileFromLevelBigFile(const char*, int*);

/* inferred: 28 bytes of members, then the virtual table pointer (destructor in the first slot) */
class CRenderBin {
public:
    unsigned char unknown00[28];

    virtual ~CRenderBin();
};

struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};

class CSprite : public CRenderBin {
public:
    enum ESpriteLoadType {};
    enum EPolyPrimType {};
    CSprite(const char*, ESpriteLoadType, ERenderPriority);
    virtual ~CSprite();
    void* operator new(unsigned long size) { return DWI_alloc(0, size, 0x400); }
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);

    unsigned char unknown20[0x44];
};

typedef CSprite::EPolyPrimType EPolyPrimType;

class CFont : public CRenderBin {
public:
    CFont(const char*, ERenderPriority);
    virtual ~CFont();
    void SetScale(float, float);
    void GetScale(float&, float&);
    void SetColor(CColor);
    void SetDepth(unsigned long);
    void DrawText(float, float, const char*);
    void DrawTextCentered(float, const char*);

    void* m_font;
    unsigned char unknown24[4];
};

/* inferred: the viewport's scale and offsets */
struct ViewportView {
    float m_x;
    float m_y;
    unsigned char unknown08[8];
    float m_scaleX;
    float m_scaleY;
    unsigned char unknown18[88];
};

class Pause {
public:
    bool bIsGamePaused();

    unsigned char unknown00[152];
};

class CPlayerObject {
public:
    unsigned char unknown0000[4720];
    bool m_hideMessages;
};

#include "game/CScene.h"

struct ShellView {
    unsigned char unknown[0x19a6];
    unsigned char m_subtitles;
};

class PopUpMessage {
public:
    PopUpMessage();
    ~PopUpMessage();
    void Reset();
    void Set_placement(unsigned char);
    void Set_screenLocs(unsigned short, unsigned short);
    void Set_property(unsigned char);
    void Set_onScreenTime(float);
    void Set_textSize(unsigned char);
    void Set_rgba(unsigned char, unsigned char, unsigned char, unsigned char);
    void Set_top(unsigned short);
    void Set_bottom(unsigned short);
    void ChangeAvailability(unsigned char);
    void SetString(const char*);
    unsigned char Available();
    unsigned char Get_placement();
    unsigned short Get_screenLocX();
    unsigned short Get_screenLocY();
    unsigned char Get_property();
    float Get_onScreenTime();
    unsigned char Get_r();
    unsigned char Get_g();
    unsigned char Get_b();
    unsigned char Get_a();
    unsigned short Get_top();
    char* GetString();
    void MoveTop();
    void Scroll();
    void UpdateDisplayTime(float);

    unsigned char unknown00[76];
};

class PopUpMessageHandler {
public:
    PopUpMessageHandler(char*, char*);
    ~PopUpMessageHandler();
    void AddPopUpMessage(char*, unsigned char, unsigned short, unsigned short, unsigned char, unsigned char,
                         unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
    void PopUpMessageHandler_Loop();

    PopUpMessage m_messages[5][4];
    unsigned char unknown5f0[4];
    CFont* m_font;
    CFont* m_subtitleFont;
    CSprite* m_background;
    CSprite* m_background2;
    float m_speed;
    unsigned char m_showBackground;
};

extern CScreenFieldMode g_screen;
extern ViewportView g_viewport;
extern Pause g_Pause;
extern CScene g_scene;
extern ShellView g_Shell;
extern bool g_bInMultiplayerMode;
extern int g_NumPlayers;
extern PopUpMessageHandler* g_pPopUpMessageHandler[4];

static int POPUPS_IN_SECTION = 4;
static float MP_PUM_TEXT_SCALE_X_2P = 1.0f;
static float MP_PUM_TEXT_SCALE_Y_2P = 1.0f;
static float MP_PUM_TEXT_SCALE_X_4P = 0.9f;
static float MP_PUM_TEXT_SCALE_Y_4P = 0.9f;

extern CUserMessageTable umTable;
extern dwi::list<QueuedSubtitle> g_subtitle_queue;
extern bool g_subtitle_queue_initialized;

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
        extra = m_messages[i].m_extra = entry[0];
        length = entry[1];
        m_messages[i].m_text = text;
        end = text + length;
        text = end + 1 + extra;
        m_messages[i].m_length = length;
        *end = 0;
    }
}

static inline short FindSlot(PopUpMessage* section, unsigned char placement) {
    short slot;
    short i;
    for (i = 0; i < POPUPS_IN_SECTION; i++) {
        if (section[i].Available()) {
            slot = (unsigned short)i;
            return slot;
        }
    }
    if (placement == 3) {
        slot = -1;
    } else {
        unsigned short top;
        unsigned short best = 0;
        top = section[0].Get_top();
        switch (placement) {
        case 0:
        case 4:
            for (short j = 1; j < POPUPS_IN_SECTION; j++) {
                unsigned short t = section[j].Get_top();
                if (t > top) {
                    best = j;
                    top = t;
                }
            }
            break;
        case 1:
        case 2:
            for (short j = 1; j < POPUPS_IN_SECTION; j++) {
                unsigned short t = section[j].Get_top();
                if (t < top) {
                    best = j;
                    top = t;
                }
            }
            break;
        }
        section[best].Reset();
        slot = best;
    }
    return slot;
}

CUserMessageTable umTable;

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
    if (!m_background)
        m_background = new CSprite("data\\bitmaps\\popupBack.gsh", (CSprite::ESpriteLoadType)1, (ERenderPriority)0x6e);
    if (!m_background2)
        m_background2 = new CSprite("data\\bitmaps\\popupSubtitleBack.gsh", (CSprite::ESpriteLoadType)1, (ERenderPriority)10);
    m_showBackground = g_Shell.m_subtitles;
}

PopUpMessageHandler::~PopUpMessageHandler() {
    delete m_background2;
    delete m_background;
    delete m_font;
    m_font = 0;
}

void PopUpMessageHandler::PopUpMessageHandler_Loop() {
    float scaleX;
    float sy0;
    float sy1;
    float sy2;
    float sy3;
    CScreen* screen;
    int screenX;
    int screenY;
    float left;
    float right;
    float top;
    float bottom;
    float scaleY;
    float x;
    float y;
    float w;
    float h;
    float fontScaleX;
    float fontScaleY;
    int count;
    int i;
    int shrink;
    float elapsed;
    short minTop;
    short topSlot;
    short slot;
    int section;
    left = 0.0f;
    right = (float)g_screen.GetWidth();
    top = 322.0f * g_screen.m_heightScale;
    bottom = 448.0f * g_screen.m_heightScale;
    screen = CScreen::GetCurrent();
    screenX = screen->GetWidth();
    screenY = screen->GetHeight();
    scaleX = g_viewport.m_scaleX;
    elapsed = (1.0f / 59.94f) * m_speed;
    scaleY = g_viewport.m_scaleY;
    if (g_bInMultiplayerMode) {
        left = 10.0f * scaleX + (float)screenX * g_viewport.m_x;
        right = (right - 10.0f) * scaleX + (float)screenX * g_viewport.m_x;
        top = top * scaleY + (float)screenY * g_viewport.m_y;
        bottom = bottom * scaleY + (float)screenY * g_viewport.m_y;
    }
    switch (g_NumPlayers) {
    case 1:
        m_font->SetScale(1.0f, 1.0f);
        break;
    case 2:
        m_font->SetScale(MP_PUM_TEXT_SCALE_X_2P, MP_PUM_TEXT_SCALE_Y_2P);
        break;
    case 3:
    case 4:
        m_font->SetScale(MP_PUM_TEXT_SCALE_X_4P, MP_PUM_TEXT_SCALE_Y_4P);
        break;
    }
    if (m_showBackground) {
        count = 0;
        for (i = 0; i < POPUPS_IN_SECTION; i++) {
            if (!m_messages[4][i].Available())
                count++;
        }
        if (count > 0) {
            shrink = (4 - count) * 22;
            _spritepolyvert verts[4] = {
                { 0.0f, 0.0f, left, 0.0f, 0.0f, 0x80808040 },
                { 1.0f, 0.0f, right, 0.0f, 0.0f, 0x80808040 },
                { 1.0f, 1.0f, right, 110.0f * g_screen.m_heightScale, 0.0f, 0x80808040 },
                { 0.0f, 1.0f, left, 110.0f * g_screen.m_heightScale, 0.0f, 0x80808040 },
            };
            sy0 = g_viewport.m_y;
            verts[0].y += sy0 * (float)g_screen.GetHeight();
            sy1 = g_viewport.m_y;
            verts[1].y += sy1 * (float)g_screen.GetHeight();
            sy2 = g_viewport.m_y;
            verts[2].y += sy2 * (float)g_screen.GetHeight() - (float)shrink;
            sy3 = g_viewport.m_y;
            verts[3].y += sy3 * (float)g_screen.GetHeight() - (float)shrink;
            m_background->DrawPoly(verts, 4, (EPolyPrimType)5);
        }
        count = 0;
        for (int j = 0; j < POPUPS_IN_SECTION; j++) {
            if (!m_messages[1][j].Available())
                count++;
        }
        if (count > 0 && !g_scene.GetPlayer(0)->m_hideMessages && !g_bInMultiplayerMode) {
            _spritepolyvert verts[4] = {
                { 0.0f, 0.0f, left, top, 0.0f, 0x80808040 },
                { 1.0f, 0.0f, right, top, 0.0f, 0x80808040 },
                { 1.0f, 1.0f, right, bottom, 0.0f, 0x80808040 },
                { 0.0f, 1.0f, left, bottom, 0.0f, 0x80808040 },
            };
            m_background2->DrawPoly(verts, 4, (EPolyPrimType)5);
        }
    }
    topSlot = -1;
    minTop = 2345;
    PopUpMessage* topMessage = m_messages[4];
    for (slot = 0; slot < POPUPS_IN_SECTION; topMessage++, slot++) {
        if (!topMessage->Available()) {
            short messageTop = topMessage->Get_top();
            if (messageTop < minTop) {
                topSlot = slot;
                minTop = messageTop;
            }
        }
    }
    for (section = 0; section < 5; section++) {
        for (int i = 0; i < POPUPS_IN_SECTION; i++) {
            PopUpMessage* message = &m_messages[section][i];
            if (!message->Available()) {
                switch (message->Get_property()) {
                case 1:
                    if (!m_showBackground)
                        break;
                    if (section != 1) {
                        if (!g_scene.GetPlayer(0)->m_hideMessages) {
                            if (i != topSlot || g_bInMultiplayerMode) {
                                if (message->Get_r() != 60)
                                    m_font->SetColor(CColor(35, 35, 35, 128));
                                else
                                    m_font->SetColor(CColor(60, 60, 40, 128));
                            } else {
                                if (message->Get_r() != 60)
                                    m_font->SetColor(CColor(55, 55, 55, 128));
                                else
                                    m_font->SetColor(CColor(60, 60, 40, 128));
                            }
                            void* font = m_font->m_font;
                            FONT_getrecta(font, message->GetString(), &x, &y, &w, &h);
                            m_font->GetScale(fontScaleX, fontScaleY);
                            if (g_bInMultiplayerMode) {
                                static int MP_PUM_LEFT_JUSTIFY = 30;
                                float textX = scaleX * MP_PUM_LEFT_JUSTIFY + (float)screenX * g_viewport.m_x;
                                float locY = message->Get_screenLocY();
                                float textY = fontScaleY * locY + (float)screenY * g_viewport.m_y;
                                textY += 8.0f;
                                m_font->DrawText(textX, textY, message->GetString());
                            } else {
                                m_font->DrawTextCentered((float)message->Get_screenLocY(), message->GetString());
                            }
                        }
                    } else {
                        x = (float)message->Get_screenLocX();
                        m_subtitleFont->SetColor(CColor(255, 255, 255, message->Get_a()));
                        m_subtitleFont->DrawText(x, (float)message->Get_screenLocY(), message->GetString());
                    }
                    break;
                case 4:
                    message->Scroll();
                    break;
                case 7:
                    break;
                case 8:
                    /* Stands in for a statement the compiler removed: the original keeps a
                       separate empty case 8 node, which an empty case would drop
                       (identity statement, accepted 2026-11-12). */
                    message = (PopUpMessage*)(void*)message;
                }
                if (!g_Pause.bIsGamePaused()) {
                    if (message->Get_onScreenTime() < 0.0f)
                        message->Reset();
                    message->UpdateDisplayTime(elapsed);
                }
            }
        }
    }
}

void PopUpMessageHandler::AddPopUpMessage(char* text, unsigned char placement, unsigned short x, unsigned short y,
                                          unsigned char property, unsigned char time, unsigned char size,
                                          unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    static int lastUsedSlotOnTop = -1;
    static char lastString[50];
    PopUpMessage* section = m_messages[placement];
    short slot = FindSlot(m_messages[placement], placement);
    if (placement == 4) {
        lastUsedSlotOnTop = slot;
        if (strlen(text) > 49)
            text[49] = 0;
        strncpy(lastString, text, 50);
    }
    if (slot != -1) {
        short k;
        PopUpMessage* other;
        for (k = 0, other = section; k < POPUPS_IN_SECTION; other++, k++) {
            unsigned char current = other->Get_property();
            if (!other->Available()) {
                switch (current) {
                case 1:
                    other->Set_property(4);
                    break;
                case 2:
                    other->Set_property(5);
                    break;
                case 3:
                    other->Set_property(6);
                    break;
                case 7:
                    other->Set_property(8);
                    break;
                }
                other->MoveTop();
            }
        }
        PopUpMessage* message = &section[slot];
        message->SetString(text);
        message->Set_placement(placement);
        message->Set_rgba(r, g, b, a);
        switch (placement) {
        case 0:
            message->Set_screenLocs(320, 144.0f * g_screen.m_heightScale);
            message->Set_top(144.0f * g_screen.m_heightScale);
            message->Set_bottom(120.0f * g_screen.m_heightScale);
            message->Set_placement(0);
            break;
        case 1:
            message->Set_screenLocs(140.0f * g_screen.m_widthScale, 402.0f * g_screen.m_heightScale);
            message->Set_top(402.0f * g_screen.m_heightScale);
            message->Set_bottom(426.0f * g_screen.m_heightScale);
            message->Set_placement(1);
            break;
        case 2:
            message->Set_screenLocs(128, 296.0f * g_screen.m_heightScale);
            message->Set_top(296.0f * g_screen.m_heightScale);
            message->Set_bottom(320.0f * g_screen.m_heightScale);
            message->Set_placement(2);
            break;
        case 3:
            message->Set_screenLocs(x, y);
            break;
        case 4:
            message->Set_screenLocs(320, 12.0f * g_screen.m_heightScale);
            message->Set_top(12.0f * g_screen.m_heightScale);
            message->Set_bottom(88.0f * g_screen.m_heightScale);
            message->Set_placement(4);
            break;
        default:
            message->Set_screenLocs(320, 12.0f * g_screen.m_heightScale);
            break;
        }
        switch (property) {
        case 1:
            property = 4;
            break;
        case 2:
            property = 5;
            break;
        case 3:
            property = 6;
            break;
        case 7:
            property = 8;
            break;
        }
        message->Set_property(property);
        message->Set_onScreenTime(time);
        message->Set_textSize(size);
        message->ChangeAvailability(0);
    }
}

dwi::list<QueuedSubtitle> g_subtitle_queue;
bool g_subtitle_queue_initialized;

bool PromptUserMessage(unsigned int id, PopUpMessageHandler* handler) {
    char* text = umTable.GetMessage(id);
    if (text) {
        handler->AddPopUpMessage(text, 4, 320, 240, 1, 5, 22, 63, 63, 63, 128);
        return true;
    }
    return false;
}

void PromptUserMessage(char* text, PopUpMessageHandler* handler, PopUpMessage* message) {
    handler->AddPopUpMessage(text, message->Get_placement(), message->Get_screenLocX(), message->Get_screenLocY(),
                             message->Get_property(), message->Get_onScreenTime(), 22, message->Get_r(),
                             message->Get_g(), message->Get_b(), 35);
}

static inline void ShowSubtitle(PopUpMessageHandler* handler, char* text, unsigned short x, unsigned short y,
                                unsigned char time, CColor color) {
    handler->AddPopUpMessage(text, 1, x, y, 1, time, 22, color.r, color.g, color.b, color.a);
}
void UpdateSubtitles(float elapsed) {
    if (!g_subtitle_queue_initialized) {
        g_subtitle_queue.reserve(20, "Subtitle queue");
        g_subtitle_queue_initialized = true;
    }
    dwi::list_node<QueuedSubtitle>* node = g_subtitle_queue.m_head.m_next;
    CScreenFieldMode* screen = &g_screen;
    dwi::list_node<QueuedSubtitle>* end = &g_subtitle_queue.m_head;
    while (node != end) {
        if (node->m_data.m_delay <= elapsed) {
            ShowSubtitle(g_pPopUpMessageHandler[0], node->m_data.m_text, 140.0f * screen->m_widthScale,
                         402.0f * screen->m_heightScale, node->m_data.m_time, node->m_data.m_color);
            node = g_subtitle_queue.erase(node);
        } else {
            node->m_data.m_delay -= elapsed;
            node = node->m_next;
        }
    }
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

CUserMessageTable::CUserMessageTable() {
    m_messages = 0;
    m_index = 0;
    m_text = 0;
    m_count = 0;
}

CUserMessageTable::~CUserMessageTable() {
    if (m_messages)
        delete[] m_messages;
    if (m_text)
        TLT_CloseFile(m_text);
    if (m_index)
        TLT_CloseFile(m_index);
    m_count = 0;
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

