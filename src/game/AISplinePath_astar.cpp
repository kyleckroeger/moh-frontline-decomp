// A fragment of AISplinePath.cpp (0x8005039c):
// CAISplinePathTraversal::ContinueTraversalForwardAStar, when the AI object is
// more than 1.5 from the traversal's target in XY, sets up an A* patrol walk
// to it (state 2 when that worked, else 1) and clears the value at +8. The
// classes and functions are named by the mangled symbols; the members are
// inferred from offsets. The rest of the file is in other units.
class CAIFilterRealVector3 {
public:
    float x;
    float y;
    float z;
};

class CAIFilterRealPosition : public CAIFilterRealVector3 {
public:
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;
};

class CAIObject {
public:
    bool SetupAStarPathWalkForPatrol(const CAIFilterRealVector3&);

    unsigned char unknown00[24];
    CAIFilterRealPosition m_position;
};

class CAIFilterObject {
public:
    unsigned char unknown00[8];
    CAIObject* m_ai;
};

class CAISplinePathTraversal {
public:
    void ContinueTraversalForwardAStar(CAIFilterObject*);

    int m_state;
    unsigned char unknown04[4];
    float m_value08;
    unsigned char unknown0c[12];
    CAIFilterRealPosition m_target;
};

void CAISplinePathTraversal::ContinueTraversalForwardAStar(CAIFilterObject* object) {
    if (m_target.GetDistanceXYReal(object->m_ai->m_position) > 1.5f) {
        if (object->m_ai->SetupAStarPathWalkForPatrol(m_target))
            m_state = 2;
        else
            m_state = 1;
        m_value08 = 0.0f;
    }
}
