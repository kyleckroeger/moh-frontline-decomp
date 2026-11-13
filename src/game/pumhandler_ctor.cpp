// A fragment of pumhandler.cpp (0x800ba3d0): the PopUpMessageHandler
// constructor. It loads the user-message table (CUserMessageTable::Load, here
// inlined; its out-of-line copy is placed by pumhandler_umt_unload.cpp, whose
// table view this file shares), loads the pop-up and subtitle fonts from the
// level's font resource (the pop-up font differs in multiplayer, which also
// sets POPUPS_IN_SECTION to 2), colours and depth-sorts both fonts, creates the
// two background sprites if missing and copies the shell's subtitle setting.
// PopUpMessageHandler, CUserMessageTable and the called functions are named by
// the symbols; members, CFont/CSprite/LevelFileContentsStruct_ views and the
// shell field are inferred from offsets. In the record loop the extra-byte
// count is read once through 'extra = record.m_extra = entry[0]' (the chained
// assignment gives the original's register order). pumhandler_block.h places
// the head of the file's constant block so the names land at their original
// offsets. The rest of the file is not part of this unit.
#include "pumhandler_block.h"

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
#include "game/CScreen.h"

class CFont {
public:
    CFont(const char*, ERenderPriority);
    void SetColor(CColor);
    void SetScale(float, float);
    void SetDepth(unsigned long);
    unsigned char unknown[0x28];
};

enum TLTResourceID {};
struct LevelFileContentsStruct_ {
    unsigned char unknown00[8];
    char** m_data;
};
LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
const char* TLT_LoadFileFromLevelBigFile(const char*, int*);

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

extern CUserMessageTable umTable;

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

inline void CUserMessageTable::Unload() {
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
        extra = m_messages[i].m_extra = entry[0];
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
