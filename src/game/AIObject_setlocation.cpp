// A fragment of AIObject.cpp (0x8004d75c): CAIObject::SetLocation from a position and from three coordinates (the
// position at +24; the copy loads z, y, x as the class's inline assignment
// does, inferred). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
class CAIFilterRealPosition {
public:
    CAIFilterRealPosition& operator=(const CAIFilterRealPosition& o) {
        float z = o.z;
        float y = o.y;
        float x = o.x;
        this->x = x;
        this->y = y;
        this->z = z;
        return *this;
    }

    float x;
    float y;
    float z;
};

class CAIObject {
public:
    void SetLocation(CAIFilterRealPosition&);
    void SetLocation(float, float, float);

    unsigned char unknown00[24];
    CAIFilterRealPosition m_position;
};

void CAIObject::SetLocation(CAIFilterRealPosition& position) {
    m_position = position;
}

void CAIObject::SetLocation(float x, float y, float z) {
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}
