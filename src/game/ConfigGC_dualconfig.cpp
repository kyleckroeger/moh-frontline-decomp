// A fragment of ConfigGC.cpp (0x800f77ac): the file's initialised data (the
// axis, digital and analog button name tables, the default, current and
// visual configuration texts, the button types, the per-button configuration
// choices, the hotspot positions and the hotbox tables, in the original's
// order and with its values), then the controller configuration screen's
// functions in image order. SetupButtonsForMultiplayerGame takes the shell's
// custom buttons (configs above 5) or the default layout, translates each of
// the 13 buttons through its configArray row (recording the visible config
// text) and appends the mapped ones to the player's key map from entry 19,
// then four fixed entries and an end marker; CopyDefaultConfig writes one of
// the five default layouts into the current text and copies it out;
// GetDefaultCustomControllerValues fills a custom layout with its defaults;
// InitializeButtonMovementArray copies the movement texts (ids 741 to 755)
// from the text table at +60. RunNewConfigRendererer draws a quad (a
// four-vertex template) at each of the 17 hotspots sized from the sprite,
// draws the controls, handles the analog and button input, clamps the
// selection and its visual value, unassigns the previous holder and applies
// the changes when asked. DisplayControlsForNewConfigRendererer lists the
// selected movement (rotated row list, highlighted) and draws every button's
// assigned movement at its hotbox (axes at their plus and minus boxes,
// hidden when unassigned or in conflict with a held trigger), then the
// selected button's current assignment when it differs. OnEnter fetches the
// current configuration into both texts; SetUpDefaultConfig writes one of
// five layouts and applies it; Get/SetAppliedState read and write the flag at
// +12. SetupButtonsForGame builds the key map from the shell's controller
// setting (as the multiplayer variant does) and dumps all 64 entries;
// ApplyChanges does the same from the current text (the original appends the
// mapped buttons to the FIRST player's map); WillBeUnassigned finds the button
// whose assignment the selected one would take over; UnassignPreviousAssigned
// Button marks it unassigned and clears trigger conflicts; HandleButtonPress
// polls the four controllers per menu button (previous/next row, previous/
// next choice skipping unavailable ones, accept, apply, restore the shell's
// setting or its custom buttons, defaults); HandleAnalogButtonPress polls
// the two analog axes, accumulating each value and moving the selection or
// its choice once the total passes 775 (resetting it inside the dead zone).
// The data and the
// functions are named by the symbols; the members, the views, the text table
// helper, the drawing and conflict inlines and the comparison forms are
// inferred. The code addresses the file's data from one base, so this unit
// defines all of it; the strings and the vertex template are the original's
// pooled .rodata. The rest of the file is in other units.
extern "C" {
void* memcpy(void*, const void*, unsigned long);
void* memset(void*, int, unsigned long);
char* strcpy(char*, const char*);
}

void DebugMsg(const char*, ...);

char* axisText[15] = {
    "Left X Axis",
    "Left Y Axis",
    "Right X Axis",
    "Right Y Axis",
    "L / R Axis",
    "Unassigned",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
};

char* digitalText[15] = {
    "Y",
    "X",
    "A",
    "B",
    "L",
    "R",
    "Z",
    "Unassigned",
    "DPad Up",
    "DPad Down",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
};

char* analogText[15] = {
    "L",
    "R",
    "Unassigned",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
    "NO_TEXT",
};

char defaultConfigText[14] = { 1, 3, 2, 0, 0, 1, 4, 5, 6, 9, 8, 3, 2, 0 };

char currConfigText[14] = { 1, 3, 2, 0, 0, 1, 4, 5, 6, 9, 8, 3, 2, 0 };

char visualConfigText[14] = { 1, 3, 2, 0, 0, 1, 4, 5, 6, 9, 8, 3, 2, 0 };

char buttonType[14] = { 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char configArray[14][15] = {
    { 0, 1, 2, 3, 9, 123, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 2, 3, 9, 123, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 2, 3, 9, 123, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 0, 1, 2, 3, 9, 123, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, -1, -1, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { 11, 10, 8, 9, 6, 5, 4, 123, 3, 2, -1, -1, -1, -1, -1 },
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
};

short hotSpotXLoc[18] = { 150, 206, 150, 94, 150, 150, 500, 555, 500, 444, 538, 538, 538, 505, 208, 430, 538, 320 };

short hotSpotYLoc[18] = { 160, 190, 220, 190, 280, 305, 310, 340, 370, 340, 180, 213, 246, 280, 100, 100, 148, 302 };

char AxisPlusHotBoxLoc[15] = { 1, 0, 7, 6, 14, 17, 123, 123, 123, 123, 123, 123, 123, 123, 123 };

char AxisMinusHotBoxLoc[15] = { 3, 2, 9, 8, 15, 17, 123, 123, 123, 123, 123, 123, 123, 123, 123 };

char ButtonHotBoxLoc[15] = { 10, 11, 12, 13, 14, 15, 16, 17, 4, 5, 123, 123, 123, 123, 123 };


/* inferred: a player's key map (button and action pairs after a word) */
extern char movementText[14][24];
extern char MovementText[15][24];

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
    void Set_currentControllerSetting(unsigned char);
    void RetrieveShellControllerButtons(char*, int);
    unsigned char Get_currentControllerSetting();
    void CopyControllerButtons(char*, int);
    void PlayShellSound(int);

    unsigned char unknown0000[0x19dc];
};
extern CShellMenu g_Shell;

class CInputManager {
public:
    long GetValue(unsigned long, unsigned long);

    unsigned char unknown00[64];
};
extern CInputManager g_inputMgr;

/* inferred: the sprite's size bitfields at +0x60 */
struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned long color;
};

class CSprite {
public:
    enum EPolyPrimType {};
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);

    unsigned char unknown00[96];
    signed int m_width : 10;
    signed int m_height : 10;
};

struct CColor {
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct FONT;
extern "C" void FONT_getrecta(FONT*, const char*, float*, float*, float*, float*);

class CFont {
public:
    void SetColor(CColor);
    void DrawText(float, float, const char*);

    unsigned char unknown00[32];
    FONT* m_font;
};

class CScreenView {
public:
    unsigned char unknown00[44];
    float m_scale;
};
extern CScreenView g_screen;


class CDualConfig {
public:
    void SetupButtonsForMultiplayerGame();
    void CopyDefaultConfig(int, char*);
    void GetDefaultCustomControllerValues(char*);
    void InitializeButtonMovementArray();
    void RunNewConfigRendererer();
    void DrawControl(char index);
    void DrawCurrentControl(char index);
    bool IsTriggerSelected();
    bool ConflictsWithTrigger(int i);
    bool HiddenByTrigger(int i);
    void DisplayControlsForNewConfigRendererer();
    void OnEnter();
    void SetUpDefaultConfig(int);
    bool GetAppliedState();
    void SetAppliedState(bool);
    void SetupButtonsForGame();
    void ApplyChanges();
    int WillBeUnassigned();
    void UnassignPreviousAssignedButton();
    void HandleButtonPress();
    void HandleAnalogButtonPress();

    CSprite* m_sprite;
    CFont* m_buttonFont;
    CFont* m_rowFont;
    bool m_applied;
    unsigned char unknown0d;
    bool m_flag0e;
    unsigned char unknown0f[5];
    int m_selected;
    unsigned char unknown18[4];
    char m_rows[13];
    unsigned char unknown29[3];
    float m_horzAccum; /* inferred: analog value-change accumulator */
    float m_vertAccum; /* inferred: analog selection accumulator */
    int m_controllerSetting;
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

void CDualConfig::RunNewConfigRendererer() {
    _spritepolyvert verts[4] = {
        { 0.0f, 0.0f, 308.0f, 212.0f, 0.0f, 0x80808080 },
        { 1.0f, 0.0f, 332.0f, 212.0f, 0.0f, 0x80808080 },
        { 1.0f, 1.0f, 332.0f, 236.0f, 0.0f, 0x80808080 },
        { 0.0f, 1.0f, 308.0f, 236.0f, 0.0f, 0x80808080 },
    };
    int i;
    short* x = hotSpotXLoc;
    short* y = hotSpotYLoc;
    for (i = 0; i < 17; i++, x++, y++) {
        float halfHeight = 0.5f * m_sprite->m_height;
        float halfWidth = 0.5f * m_sprite->m_width;
        verts[0].x = *x - halfWidth;
        verts[1].x = halfWidth + *x;
        verts[2].x = halfWidth + *x;
        verts[3].x = *x - halfWidth;
        verts[0].y = *y - halfHeight;
        verts[1].y = *y - halfHeight;
        verts[2].y = halfHeight + *y;
        verts[3].y = halfHeight + *y;
        m_sprite->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
    }
    DisplayControlsForNewConfigRendererer();
    HandleAnalogButtonPress();
    HandleButtonPress();
    if (m_selected > 12)
        m_selected = 0;
    else if (m_selected < 0)
        m_selected = 12;
    if (visualConfigText[m_selected] > 14)
        visualConfigText[m_selected] = 0;
    else if (visualConfigText[m_selected] < 0)
        visualConfigText[m_selected] = 14;
    UnassignPreviousAssignedButton();
    if (m_applied)
        ApplyChanges();
}

inline void CDualConfig::DrawControl(char index) {
    float offsetX = 0.0f;
    float offsetY = -5.0f;
    float rx, ry, rw, rh;
    float x;
    float y;
    switch (buttonType[index]) {
    case 0: {
        char* text = MovementText[index];
        FONT_getrecta(m_buttonFont->m_font, text, &rx, &ry, &rw, &rh);
        x = hotSpotXLoc[ButtonHotBoxLoc[visualConfigText[index]]] - 0.5f * rw;
        y = hotSpotYLoc[ButtonHotBoxLoc[visualConfigText[index]]] - 0.5f * rh;
        if (visualConfigText[index] != 7)
            m_buttonFont->DrawText(x + offsetX, y + offsetY, text);
        break;
    }
    case 2: {
        char* text = MovementText[index];
        FONT_getrecta(m_buttonFont->m_font, text, &rx, &ry, &rw, &rh);
        x = hotSpotXLoc[AxisPlusHotBoxLoc[visualConfigText[index]]] - 0.5f * rw;
        y = hotSpotYLoc[AxisPlusHotBoxLoc[visualConfigText[index]]] - 0.5f * rh;
        if (visualConfigText[index] != 5)
            m_buttonFont->DrawText(x + offsetX, y + offsetY, text);
        x = hotSpotXLoc[AxisMinusHotBoxLoc[visualConfigText[index]]] - 0.5f * rw;
        y = hotSpotYLoc[AxisMinusHotBoxLoc[visualConfigText[index]]] - 0.5f * rh;
        if (visualConfigText[index] != 5)
            m_buttonFont->DrawText(x + offsetX, y + offsetY, text);
        break;
    }
    case 1: {
        char* text = MovementText[index];
        FONT_getrecta(m_buttonFont->m_font, text, &rx, &ry, &rw, &rh);
        x = hotSpotXLoc[ButtonHotBoxLoc[visualConfigText[index]]] - 0.5f * rw;
        y = hotSpotYLoc[ButtonHotBoxLoc[visualConfigText[index]]] - 0.5f * rh;
        if (visualConfigText[index] != 7)
            m_buttonFont->DrawText(x + offsetX, y + offsetY, text);
        break;
    }
    }
}

inline void CDualConfig::DrawCurrentControl(char index) {
    float offsetY = -5.0f;
    float rx, ry, rw, rh;
    float x;
    float y;
    switch (buttonType[m_selected]) {
    case 0: {
        char* text = MovementText[index];
        FONT_getrecta(m_buttonFont->m_font, text, &rx, &ry, &rw, &rh);
        x = hotSpotXLoc[ButtonHotBoxLoc[currConfigText[index]]] - 0.5f * rw;
        y = hotSpotYLoc[ButtonHotBoxLoc[currConfigText[index]]] - 0.5f * rh;
        if (currConfigText[index] != 7)
            m_buttonFont->DrawText(x, y + offsetY, text);
        break;
    }
    case 2: {
        char* text = MovementText[index];
        FONT_getrecta(m_buttonFont->m_font, text, &rx, &ry, &rw, &rh);
        x = hotSpotXLoc[AxisPlusHotBoxLoc[currConfigText[index]]] - 0.5f * rw;
        y = hotSpotYLoc[AxisPlusHotBoxLoc[currConfigText[index]]] - 0.5f * rh;
        if (currConfigText[index] != 5)
            m_buttonFont->DrawText(x, y + offsetY, text);
        x = hotSpotXLoc[AxisMinusHotBoxLoc[currConfigText[index]]] - 0.5f * rw;
        y = hotSpotYLoc[AxisMinusHotBoxLoc[currConfigText[index]]] - 0.5f * rh;
        if (currConfigText[index] != 5)
            m_buttonFont->DrawText(x, y + offsetY, text);
        break;
    }
    case 1: {
        char* text = MovementText[index];
        FONT_getrecta(m_buttonFont->m_font, text, &rx, &ry, &rw, &rh);
        x = hotSpotXLoc[ButtonHotBoxLoc[currConfigText[index]]] - 0.5f * rw;
        y = hotSpotYLoc[ButtonHotBoxLoc[currConfigText[index]]] - 0.5f * rh;
        if (currConfigText[index] != 7)
            m_buttonFont->DrawText(x, y + offsetY, text);
        break;
    }
    }
}

inline bool CDualConfig::IsTriggerSelected() {
    bool result;
    if (visualConfigText[m_selected] == 4 && buttonType[m_selected] == 2)
        result = true;
    else
        result = false;
    return result;
}

inline bool CDualConfig::ConflictsWithTrigger(int i) {
    if (i == m_selected)
        return false;
    if (visualConfigText[m_selected] == 4) {
        if (visualConfigText[i] == 4 && buttonType[i] == 2)
            return true;
        if (visualConfigText[i] == 4 && buttonType[i] == 1)
            return true;
        if (visualConfigText[i] == 4 && buttonType[i] == 0)
            return true;
        if (visualConfigText[i] == 5 && buttonType[i] == 1)
            return true;
        if (visualConfigText[i] == 5 && buttonType[i] == 0)
            return true;
    }
    return false;
}

inline bool CDualConfig::HiddenByTrigger(int i) {
    bool result;
    if (buttonType[m_selected] == 2)
        result = false;
    else if ((visualConfigText[m_selected] == 4 || visualConfigText[m_selected] == 5) && buttonType[i] == 2 &&
             visualConfigText[i] == 4)
        result = true;
    else
        result = false;
    return result;
}

void CDualConfig::DisplayControlsForNewConfigRendererer() {
    int k;
    int line = m_selected - 1;
    if (line < 0)
        line = 12;
    for (k = 0; k < 13; k++) {
        if (line > 12)
            line = 0;
        m_rows[k] = line;
        line++;
    }
    int unassigned = WillBeUnassigned();
    for (int row = 0; row < 3; row++) {
        if (row != 0 && row != 2) {
            char entry = m_rows[row];
            if (entry == m_selected)
                m_rowFont->SetColor(CColor(0, 0, 0, 128));
            float rx, ry, rw, rh;
            char* text = movementText[entry];
            FONT_getrecta(m_rowFont->m_font, text, &rx, &ry, &rw, &rh);
            float half = 0.5f;
            rx = 320.0f - rw * half;
            m_rowFont->DrawText(rx, half + (312.0f * g_screen.m_scale + (float)(row * 20 - 9)), text);
            if (entry == m_selected)
                m_rowFont->SetColor(CColor(64, 64, 64, 64));
        }
    }
    char button;
    for (int i = 0; i < 13; i++) {
        if (IsTriggerSelected() && ConflictsWithTrigger(i))
            continue;
        if (HiddenByTrigger(i))
            continue;
        button = i;
        if ((char)unassigned == button)
            continue;
        if (button == m_selected)
            m_buttonFont->SetColor(CColor(110, 110, 110, 64));
        if (buttonType[button] == 0)
            DrawControl(button);
        else if (buttonType[button] == 2)
            DrawControl(button);
        else if (buttonType[button] == 1)
            DrawControl(button);
        if (button == m_selected)
            m_buttonFont->SetColor(CColor(40, 40, 40, 40));
    }
    if (visualConfigText[m_selected] != currConfigText[m_selected])
        DrawCurrentControl(m_selected);
}

void CDualConfig::OnEnter() {
    char buttons[13];
    if (m_config > 5)
        g_Shell.RetrieveShellControllerButtons(buttons, m_config - 6);
    else
        CopyDefaultConfig(m_config, buttons);
    for (int i = 0; i < 13; i++) {
        currConfigText[i] = buttons[i];
        visualConfigText[i] = buttons[i];
    }
    m_controllerSetting = g_Shell.Get_currentControllerSetting();
    if (m_controllerSetting == 2) {
        currConfigText[8] = 7;
        visualConfigText[8] = 7;
    }
    currConfigText[13] = 0;
    visualConfigText[13] = 0;
    m_selected = 0;
    m_flag0e = false;
}

void CDualConfig::SetUpDefaultConfig(int config) {
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
    ApplyChanges();
}

bool CDualConfig::GetAppliedState() {
    return m_applied;
}

void CDualConfig::SetAppliedState(bool applied) {
    m_applied = applied;
}

void CDualConfig::SetupButtonsForGame() {
    char buttons[16];
    unsigned char mapped[13];
    unsigned short keys[13];
    int i;
    int count;

    int setting = g_Shell.Get_currentControllerSetting();
    if (setting > 5)
        g_Shell.RetrieveShellControllerButtons(buttons, setting - 6);
    else
        CopyDefaultConfig(setting, buttons);
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
    for (i = 0; i < 64; i++) {
        DebugMsg("--------------------------------------------------\n");
        DebugMsg("customConfigButton[%d].events.eventId = %d\n", i, g_config_keymap[m_player].entries[i].button);
        DebugMsg("customConfigButton[%d].events.inputBinding = %d\n", i, g_config_keymap[m_player].entries[i].action);
    }
}

void CDualConfig::ApplyChanges() {
    unsigned char mapped[13];
    unsigned short keys[13];
    int i;
    int count;

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
    if (m_config > 5)
        g_Shell.CopyControllerButtons(currConfigText, m_config - 6);
    for (i = 0; i < 13; i++) {
        mapped[i] = configArray[i][currConfigText[i]];
        visualConfigText[i] = currConfigText[i];
    }
    count = 19;
    for (i = 0; i < 13; i++) {
        if (mapped[i] != 123) {
            g_config_keymap->entries[count].button = keys[i];
            g_config_keymap->entries[count].action = mapped[i];
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

int CDualConfig::WillBeUnassigned() {
    int result = -1;
    for (int i = 0; (char)i < 13; i++) {
        if ((char)i == m_selected)
            continue;
        if (buttonType[i] == 0 && visualConfigText[m_selected] == 7)
            continue;
        if (buttonType[i] == 1 && visualConfigText[m_selected] == 7)
            continue;
        if (buttonType[i] == 2 && visualConfigText[m_selected] == 5)
            continue;
        if (currConfigText[i] != visualConfigText[m_selected])
            continue;
        if (buttonType[i] == 0 && buttonType[i] == buttonType[m_selected])
            result = i;
        else if (buttonType[i] == 2 && buttonType[i] == buttonType[m_selected])
            result = i;
        else if (buttonType[i] == 1 && buttonType[i] == buttonType[m_selected])
            result = i;
        else if (buttonType[i] == 1 && buttonType[m_selected] == 0)
            result = i;
        else if (buttonType[i] == 0 && buttonType[m_selected] == 1)
            result = i;
    }
    return result;
}

void CDualConfig::UnassignPreviousAssignedButton() {
    for (char i = 0; i < 13; i++) {
        if (i != m_selected && currConfigText[i] == currConfigText[m_selected]) {
            if (buttonType[i] == 0 && buttonType[i] == buttonType[m_selected]) {
                visualConfigText[i] = 7;
                currConfigText[i] = 7;
            } else if (buttonType[i] == 2 && buttonType[i] == buttonType[m_selected]) {
                visualConfigText[i] = 5;
                currConfigText[i] = 5;
            } else if (buttonType[i] == 1 && buttonType[i] == buttonType[m_selected]) {
                visualConfigText[i] = 7;
                currConfigText[i] = 7;
            } else if (buttonType[i] == 1 && buttonType[m_selected] == 0) {
                visualConfigText[i] = 7;
                currConfigText[i] = 7;
            } else if (buttonType[i] == 0 && buttonType[m_selected] == 1) {
                visualConfigText[i] = 7;
                currConfigText[i] = 7;
            }
        }
    }
    switch (buttonType[m_selected]) {
    case 2:
        if (currConfigText[m_selected] == 4) {
            int k;
            int value = 4;
            for (k = 0; k < 2; k++, value++) {
                char* type = buttonType;
                char* curr = currConfigText;
                char* visual = visualConfigText;
                for (int j = 0; j < 13; j++, type++, curr++, visual++) {
                    if (*type != 2 && *curr == value) {
                        *visual = 7;
                        *curr = 7;
                    }
                }
            }
        }
        return;
    case 0:
    case 1:
        if (currConfigText[m_selected] == 4 || currConfigText[m_selected] == 5) {
            for (int j = 0; j < 13; j++) {
                if (buttonType[j] == 2 && currConfigText[j] == 4) {
                    visualConfigText[j] = 5;
                    currConfigText[j] = 5;
                }
            }
        }
        break;
    }
}

void CDualConfig::HandleButtonPress() {
    float value;
    float value2;
    int i;

    for (i = 0; i < 4; i++) {
        value = g_inputMgr.GetValue(i, 123);
        if (value)
            break;
    }
    if (value) {
        visualConfigText[m_selected] = currConfigText[m_selected];
        m_selected--;
        return;
    }
    for (i = 0; i < 4; i++) {
        value = g_inputMgr.GetValue(i, 124);
        if (value)
            break;
    }
    if (value) {
        visualConfigText[m_selected] = currConfigText[m_selected];
        m_selected++;
        return;
    }
    for (i = 0; i < 4; i++) {
        value = g_inputMgr.GetValue(i, 125);
        if (value)
            break;
    }
    if (value) {
        char v = visualConfigText[m_selected];
        v--;
        if (v < 0)
            v = 14;
        visualConfigText[m_selected] = v;
        if (m_selected != 13 && configArray[m_selected][v] == -1) {
            int sel;
            while (configArray[sel = m_selected][v] == -1) {
                v--;
                visualConfigText[sel]--;
            }
        }
        return;
    }
    for (i = 0; i < 4; i++) {
        value = g_inputMgr.GetValue(i, 126);
        if (value)
            break;
    }
    if (value) {
        char v = visualConfigText[m_selected];
        v++;
        if (v > 14)
            v = 0;
        visualConfigText[m_selected] = v;
        if (m_selected != 13 && configArray[m_selected][v] == -1) {
            int sel;
            while (configArray[sel = m_selected][v] == -1) {
                v++;
                visualConfigText[sel]++;
            }
        }
        return;
    }
    for (i = 0; i < 4; i++) {
        value = g_inputMgr.GetValue(i, 128);
        if (value)
            break;
    }
    value2 = 0.0f;
    for (i = 0; i < 4; i++) {
        value2 = g_inputMgr.GetValue(i, 135);
        if (value2)
            break;
    }
    if (value || value2) {
        char c = visualConfigText[m_selected];
        // A no-op self-assignment (user-approved, like '(void)0;'): the
        // compiler drops the store, but the selection is then reloaded for
        // the next store as in the original.
        visualConfigText[m_selected] = c;
        currConfigText[m_selected] = c;
        return;
    }
    for (i = 0; i < 4; i++) {
        value = g_inputMgr.GetValue(i, 129);
        if (value)
            break;
    }
    if (value) {
        m_applied = true;
        g_Shell.Set_currentControllerSetting(m_config);
        return;
    }
    for (i = 0; i < 4; i++) {
        value = g_inputMgr.GetValue(i, 127);
        if (value)
            break;
    }
    if (value) {
        m_flag0e = true;
        m_applied = true;
        g_Shell.Set_currentControllerSetting(m_controllerSetting);
        m_config = m_controllerSetting;
        if (m_controllerSetting >= 6) {
            char buttons[13];
            g_Shell.RetrieveShellControllerButtons(buttons, m_controllerSetting - 6);
            for (i = 0; i < 13; i++) {
                currConfigText[i] = buttons[i];
                visualConfigText[i] = buttons[i];
            }
            currConfigText[13] = 0;
            visualConfigText[13] = 0;
        } else {
            SetUpDefaultConfig(m_controllerSetting);
        }
        return;
    }
    for (i = 0; i < 4; i++) {
        value = g_inputMgr.GetValue(i, 130);
        if (value)
            break;
    }
    if (value) {
        SetUpDefaultConfig(1);
        currConfigText[13] = 0;
        visualConfigText[13] = 0;
    }
}

void CDualConfig::HandleAnalogButtonPress() {
    float value;
    int i;

    // The dead 'value = 0.0f;' initialisations before the polls give the
    // original's constant load order.
    value = 0.0f;
    for (i = 0; i < 4; i++) {
        value = g_inputMgr.GetValue(i, 138);
        if (value)
            break;
    }
    m_vertAccum += value;
    if (value < -48.0f && value < -10.0f) {
        if (m_vertAccum < -775.0f) {
            visualConfigText[m_selected] = currConfigText[m_selected];
            m_selected--;
            m_vertAccum = 0.0f;
            g_Shell.PlayShellSound(0);
        }
    } else if (value > 48.0f && value > 10.0f) {
        if (m_vertAccum > 775.0f) {
            visualConfigText[m_selected] = currConfigText[m_selected];
            m_selected++;
            m_vertAccum = 0.0f;
            g_Shell.PlayShellSound(0);
        }
    } else {
        m_vertAccum = 0.0f;
    }
    value = 0.0f;
    for (i = 0; i < 4; i++) {
        value = g_inputMgr.GetValue(i, 137);
        if (value)
            break;
    }
    m_horzAccum += value;
    if (value < -48.0f && value < -10.0f) {
        if (m_horzAccum < -775.0f) {
            char v = visualConfigText[m_selected];
            v--;
            if (v < 0)
                v = 14;
            visualConfigText[m_selected] = v;
            if (m_selected != 13 && configArray[m_selected][v] == -1) {
                int sel;
                while (configArray[sel = m_selected][v] == -1) {
                    v--;
                    visualConfigText[sel]--;
                }
            }
            m_horzAccum = 0.0f;
            g_Shell.PlayShellSound(0);
        }
    } else if (value > 48.0f && value > 10.0f) {
        if (m_horzAccum > 775.0f) {
            char v = visualConfigText[m_selected];
            v++;
            if (v > 14)
                v = 0;
            visualConfigText[m_selected] = v;
            if (m_selected != 13 && configArray[m_selected][v] == -1) {
                int sel;
                while (configArray[sel = m_selected][v] == -1) {
                    v++;
                    visualConfigText[sel]++;
                }
            }
            m_horzAccum = 0.0f;
            g_Shell.PlayShellSound(0);
        }
    } else {
        m_horzAccum = 0.0f;
    }
}
