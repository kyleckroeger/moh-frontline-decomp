// A fragment of pumhandler.cpp (0x800ba304): PopUpMessageHandler's
// destructor. It deletes the three objects it holds and then its array of
// pop-up messages is destroyed. PopUpMessageHandler and
// PopUpMessage are named by the mangled symbols; the members are inferred
// from offsets. The held pointers are deleted through a virtual destructor at
// the slot CRenderBin's sprites and fonts use, so they are viewed as
// CRenderBin pointers (their exact classes are not known). CRenderBin is an
// inferred view: 28 bytes of members, then its virtual table pointer, with
// the virtual destructor in the first slot.
class CRenderBin {
public:
    unsigned char unknown00[28];

    virtual ~CRenderBin();
};

class PopUpMessage {
public:
    ~PopUpMessage();

    unsigned char unknown00[76];
};

class PopUpMessageHandler {
public:
    ~PopUpMessageHandler();

    PopUpMessage m_messages[20];
    unsigned char unknown5f0[4];
    CRenderBin* m_bin5f4;
    unsigned char unknown5f8[4];
    CRenderBin* m_bin5fc;
    CRenderBin* m_bin600;
};

PopUpMessageHandler::~PopUpMessageHandler() {
    delete m_bin600;
    delete m_bin5fc;
    delete m_bin5f4;
    m_bin5f4 = 0;
}
