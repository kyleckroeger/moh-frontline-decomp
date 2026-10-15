// A fragment of pumhandler.cpp (0x800b92c0): the two PromptUserMessage
// overloads. The first adds a pop-up with the given text, taking its
// placement, screen position, property, on-screen time and colour from a
// pop-up message (the getters are called in reverse argument order); the
// second looks the message up in the user-message table and, when there is
// one, adds it centred (320, 240) in grey at half opacity, returning whether
// it did. The classes, functions and umTable are named by the symbols; the
// meaning of the fixed arguments (22, 35, 4, 1, 5) and the result types are
// not known, and the table's record view follows pumhandler_umt_unload.cpp.
// The rest of the file is not part of this unit.
struct SUserMessage {
    char* m_text;
    unsigned char m_length;
    unsigned char m_extra;
};

class CUserMessageTable {
public:
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

extern CUserMessageTable umTable;

class PopUpMessage {
public:
    unsigned char Get_b();
    unsigned char Get_g();
    unsigned char Get_r();
    float Get_onScreenTime();
    unsigned char Get_property();
    unsigned short Get_screenLocY();
    unsigned short Get_screenLocX();
    unsigned char Get_placement();
};

class PopUpMessageHandler {
public:
    void AddPopUpMessage(char*, unsigned char, unsigned short, unsigned short, unsigned char, unsigned char,
                         unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
};

void PromptUserMessage(char* text, PopUpMessageHandler* handler, PopUpMessage* message) {
    handler->AddPopUpMessage(text, message->Get_placement(), message->Get_screenLocX(), message->Get_screenLocY(),
                             message->Get_property(), message->Get_onScreenTime(), 22, message->Get_r(),
                             message->Get_g(), message->Get_b(), 35);
}

bool PromptUserMessage(unsigned int id, PopUpMessageHandler* handler) {
    char* text = umTable.GetMessage(id);
    if (text) {
        handler->AddPopUpMessage(text, 4, 320, 240, 1, 5, 22, 63, 63, 63, 128);
        return true;
    }
    return false;
}
