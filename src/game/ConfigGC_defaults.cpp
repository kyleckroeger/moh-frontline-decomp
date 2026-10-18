// A fragment of ConfigGC.cpp (0x800f77ac): CDualConfig::
// SetupButtonsForMultiplayerGame takes the shell's custom buttons (configs
// above 5) or the default layout, translates each of the 13 buttons through
// its configArray row (recording the visible config text) and appends the
// mapped ones to the player's key map from entry 19 with their key codes,
// then four fixed entries (22 and 23, and 17 and 18 with the second and third
// mapped buttons) and an end marker. CDualConfig::CopyDefaultConfig
// writes one of the five default button layouts into the current config text
// (as SetUpDefaultConfig does; other values leave it) and copies the text,
// terminator included, into the buffer. InitializeButtonMovementArray
// copies the movement texts (ids 741 to 755
// into movementText, one row more than its 14 rows, as the code does, and a
// selection into MovementText) from the text table at +60 (inferred; the
// bounds-checked copy is an inferred inline helper), clearing an unused local
// buffer before each of the first copies, then clears the table pointer.
// CDualConfig::GetDefaultCustomControllerValues fills a custom controller
// layout with its 13 default button assignments. The class and function are
// named by the mangled symbol; what each value means is not known. The rest
// of the file is in other units.
extern "C" {
void* memcpy(void*, const void*, unsigned long);
void* memset(void*, int, unsigned long);
char* strcpy(char*, const char*);
}

/* inferred: a text table (8-byte entries, text first; count at +12) */
struct TextTableView {
    struct Entry {
        char* text;
        int unknown04;
    };

    Entry* entries;
    unsigned char unknown04[8];
    int count;

    char* GetText(int id) { return id < count ? entries[id].text : 0; }
};

inline void CopyText(char* dest, TextTableView* table, int id) {
    strcpy(dest, id < table->count ? table->entries[id].text : 0);
}

extern char movementText[14][24];
extern char MovementText[15][24];

extern char currConfigText[14];
extern char visualConfigText[14];
extern char configArray[14][15];

/* inferred: a player's key map (button and action pairs after a word) */
struct KeyMapEntry {
    unsigned short button;
    unsigned short action;
};

struct KeyMapView {
    int unknown00;
    KeyMapEntry entries[64];
};

extern KeyMapView g_config_keymap[4];

class CShellMenu {
public:
    void RetrieveShellControllerButtons(char*, int);

    unsigned char unknown0000[0x19dc];
};

extern CShellMenu g_Shell;

class CDualConfig {
public:
    void SetupButtonsForMultiplayerGame();
    void CopyDefaultConfig(int, char*);
    void GetDefaultCustomControllerValues(char*);
    void InitializeButtonMovementArray();

    unsigned char unknown00[56];
    int m_player;
    TextTableView* m_texts;
    int m_config;
};

void CDualConfig::SetupButtonsForMultiplayerGame() {
    unsigned short keys[13];
    char buttons[16];
    unsigned char mapped[13];
    int i;
    int count;

    if (m_config > 5)
        g_Shell.RetrieveShellControllerButtons(buttons, m_config - 6);
    else
        CopyDefaultConfig(m_config, buttons);
    for (i = 0; i < 13; i++)
        mapped[i] = 123;
    keys[0] = 7;
    keys[1] = 10;
    keys[2] = 9;
    keys[3] = 8;
    keys[4] = 11;
    keys[5] = 12;
    keys[6] = 16;
    keys[7] = 14;
    keys[8] = 15;
    keys[9] = 19;
    keys[10] = 20;
    keys[11] = 21;
    keys[12] = 13;
    for (i = 0; i < 13; i++) {
        mapped[i] = configArray[i][buttons[i]];
        visualConfigText[i] = buttons[i];
    }
    count = 19;
    for (i = 0; i < 13; i++) {
        if (mapped[i] != 123) {
            g_config_keymap[m_player].entries[count].button = keys[i];
            g_config_keymap[m_player].entries[count].action = mapped[i];
            count++;
        }
    }
    g_config_keymap[m_player].entries[count].button = 22;
    g_config_keymap[m_player].entries[count].action = 0;
    count++;
    g_config_keymap[m_player].entries[count].button = 23;
    g_config_keymap[m_player].entries[count].action = 1;
    count++;
    g_config_keymap[m_player].entries[count].button = 17;
    g_config_keymap[m_player].entries[count].action = mapped[1];
    count++;
    g_config_keymap[m_player].entries[count].button = 18;
    g_config_keymap[m_player].entries[count].action = mapped[2];
    count++;
    g_config_keymap[m_player].entries[count].button = 0xFFFF;
    g_config_keymap[m_player].entries[count].action = 0xFFFF;
}

void CDualConfig::CopyDefaultConfig(int config, char* buttons) {
    switch (config) {
    case 1:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 2;
        currConfigText[3] = 0;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 6;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 3;
        currConfigText[12] = 2;
        break;
    case 2:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 0;
        currConfigText[3] = 4;
        currConfigText[4] = 0;
        currConfigText[5] = 9;
        currConfigText[6] = 6;
        currConfigText[7] = 2;
        currConfigText[8] = 3;
        currConfigText[9] = 8;
        currConfigText[10] = 7;
        currConfigText[11] = 1;
        currConfigText[12] = 3;
        break;
    case 3:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 2;
        currConfigText[3] = 0;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 6;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 2;
        currConfigText[12] = 3;
        break;
    case 4:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 2;
        currConfigText[3] = 0;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 2;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 6;
        currConfigText[12] = 3;
        break;
    case 5:
        currConfigText[0] = 1;
        currConfigText[1] = 3;
        currConfigText[2] = 0;
        currConfigText[3] = 2;
        currConfigText[4] = 0;
        currConfigText[5] = 1;
        currConfigText[6] = 4;
        currConfigText[7] = 5;
        currConfigText[8] = 6;
        currConfigText[9] = 9;
        currConfigText[10] = 8;
        currConfigText[11] = 3;
        currConfigText[12] = 2;
        break;
    }
    memcpy(buttons, currConfigText, 14);
}


void CDualConfig::GetDefaultCustomControllerValues(char* values) {
    values[0] = 1;
    values[1] = 3;
    values[2] = 2;
    values[3] = 0;
    values[4] = 0;
    values[5] = 1;
    values[6] = 4;
    values[7] = 5;
    values[8] = 6;
    values[9] = 9;
    values[10] = 8;
    values[11] = 3;
    values[12] = 2;
}

void CDualConfig::InitializeButtonMovementArray() {
    int i = 0;
    char buffer[80];
    for (; i <= 14; i++) {
        memset(buffer, 0, 80);
        CopyText(movementText[i], m_texts, 741 + i);
    }
    CopyText(MovementText[0], m_texts, 741);
    CopyText(MovementText[1], m_texts, 742);
    CopyText(MovementText[2], m_texts, 743);
    CopyText(MovementText[3], m_texts, 744);
    CopyText(MovementText[4], m_texts, 745);
    CopyText(MovementText[5], m_texts, 746);
    CopyText(MovementText[6], m_texts, 747);
    CopyText(MovementText[7], m_texts, 748);
    CopyText(MovementText[8], m_texts, 749);
    CopyText(MovementText[9], m_texts, 759);
    CopyText(MovementText[10], m_texts, 760);
    CopyText(MovementText[11], m_texts, 752);
    CopyText(MovementText[12], m_texts, 753);
    CopyText(MovementText[13], m_texts, 762);
    m_texts = 0;
}
