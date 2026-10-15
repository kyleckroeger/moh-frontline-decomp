// A fragment of UserInterface.cpp (0x800ba8c0): UserInterface::AddHitDirection shows a hit indicator: for directions 1 to 4
// its timer (+1956 by direction) is set to 3 (the view is inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class UserInterface {
public:
    void AddHitDirection(unsigned int);

    unsigned char unknown000[1956];
    float m_hitTimes[5];
};

void UserInterface::AddHitDirection(unsigned int direction) {
    if (direction >= 1 && direction <= 4)
        m_hitTimes[direction] = 3.0f;
}
