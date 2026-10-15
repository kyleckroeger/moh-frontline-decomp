// A fragment of popup.cpp (0x800bf4e8): PopUpMessage::UpdateDisplayTime counts the remaining display time (+8)
// down by the elapsed time (the member is inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class PopUpMessage {
public:
    void UpdateDisplayTime(float);

    unsigned char unknown00[8];
    float m_displayTime;
};

void PopUpMessage::UpdateDisplayTime(float delta) {
    m_displayTime -= delta;
}
