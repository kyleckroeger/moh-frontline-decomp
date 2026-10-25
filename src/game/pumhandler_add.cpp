// A fragment of pumhandler.cpp (0x800b9420): PopUpMessageHandler::AddPopUpMessage
// queues a pop-up message in one of the handler's placement sections (four
// messages of 76 bytes each, POPUPS_IN_SECTION of them in use): it takes the
// first free slot, or (except in section 3, which then drops the message)
// recycles the highest (sections 0 and 4) or lowest (1 and 2) message by its
// top and resets it. For section 4 it remembers the slot and the text (cut
// to 49 characters) in function statics. Then it advances the properties of
// the section's busy messages (1->4, 2->5, 3->6, 7->8) and moves them up,
// and fills the slot: text, placement, colour, the screen location, top and
// bottom of the section (scaled by the screen's scale; section 3 uses the
// caller's location), the property (mapped the same way), the on-screen time
// and the text size, and marks it busy. The classes, functions and globals
// are named by the mangled symbols; the section layout, the slot search
// helper (FindSlot) and the screen view are inferred.
extern "C" unsigned long strlen(const char*);
extern "C" char* strncpy(char*, const char*, unsigned long);

class PopUpMessage {
public:
    void MoveTop();
    unsigned short Get_top();
    unsigned char Get_property();
    unsigned char Available();
    void SetString(const char*);
    void ChangeAvailability(unsigned char);
    void Set_bottom(unsigned short);
    void Set_top(unsigned short);
    void Set_rgba(unsigned char, unsigned char, unsigned char, unsigned char);
    void Set_textSize(unsigned char);
    void Set_onScreenTime(float);
    void Set_property(unsigned char);
    void Set_screenLocs(unsigned short, unsigned short);
    void Set_placement(unsigned char);
    void Reset();

    unsigned char unknown00[76];
};

class CScreen {
public:
    unsigned char unknown00[40];
    float m_scaleX;
    float m_scaleY;
};

extern CScreen g_screen;
extern int POPUPS_IN_SECTION;

class PopUpMessageHandler {
public:
    void AddPopUpMessage(char*, unsigned char, unsigned short, unsigned short, unsigned char, unsigned char,
                         unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);

    PopUpMessage m_messages[5][4];
};

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
            message->Set_screenLocs(320, 144.0f * g_screen.m_scaleY);
            message->Set_top(144.0f * g_screen.m_scaleY);
            message->Set_bottom(120.0f * g_screen.m_scaleY);
            message->Set_placement(0);
            break;
        case 1:
            message->Set_screenLocs(140.0f * g_screen.m_scaleX, 402.0f * g_screen.m_scaleY);
            message->Set_top(402.0f * g_screen.m_scaleY);
            message->Set_bottom(426.0f * g_screen.m_scaleY);
            message->Set_placement(1);
            break;
        case 2:
            message->Set_screenLocs(128, 296.0f * g_screen.m_scaleY);
            message->Set_top(296.0f * g_screen.m_scaleY);
            message->Set_bottom(320.0f * g_screen.m_scaleY);
            message->Set_placement(2);
            break;
        case 3:
            message->Set_screenLocs(x, y);
            break;
        case 4:
            message->Set_screenLocs(320, 12.0f * g_screen.m_scaleY);
            message->Set_top(12.0f * g_screen.m_scaleY);
            message->Set_bottom(88.0f * g_screen.m_scaleY);
            message->Set_placement(4);
            break;
        default:
            message->Set_screenLocs(320, 12.0f * g_screen.m_scaleY);
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
