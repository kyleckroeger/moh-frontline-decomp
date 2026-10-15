// A fragment of CoverPoint.cpp (0x80052f5c): CCoverPoint::FlagAsUnoccupied
// clears the type's bits in the occupancy word (+0). CCoverPoint and
// COVERPOINT_TYPE are named by the mangled symbol; the member is inferred.
// The rest of the file is in other units.
enum COVERPOINT_TYPE {};

class CCoverPoint {
public:
    void FlagAsUnoccupied(COVERPOINT_TYPE);

    unsigned int m_occupied;
};

void CCoverPoint::FlagAsUnoccupied(COVERPOINT_TYPE type) {
    m_occupied &= ~type;
}
