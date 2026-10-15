// A fragment of AIObject.cpp (0x800480a4): CAIObject::IsObjectReachable
// reports whether the line from the AI object's position to the target, both
// raised by 0.4, is clear of the environment. The classes and functions are
// named by the mangled symbols; the members are inferred from offsets. The
// object's position is the game's CVector3 (copied as a doubleword pair) and
// is converted through an inferred inline constructor (as in
// AIFilter_queries.cpp); the position class's member-wise copy constructor
// and its 8-byte alignment (16-byte stack slots) are inferred. The rest of
// the file is in other units.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition() {}
    CAIFilterRealPosition(CVector3 v) : x(v.d.v[0]), y(v.d.v[1]), z(v.d.v[2]) {}
    CAIFilterRealPosition(const CAIFilterRealPosition& o) {
        float x = o.x;
        float y = o.y;
        float z = o.z;
        this->x = x;
        this->y = y;
        this->z = z;
    }

    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CAIFilterObject;

class CAIFilterGlobal {
public:
    static bool CheckCollisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&,
                                                CAIFilterObject*);
};

class CAIObject {
public:
    bool IsObjectReachable(const CAIFilterRealPosition&);

    unsigned char unknown00[8];
    CAIFilterObject* m_filter;
    unsigned char unknown0c[12];
    CVector3 m_position;
};

bool CAIObject::IsObjectReachable(const CAIFilterRealPosition& target) {
    CAIFilterRealPosition from(m_position);
    from.z += 0.4f;
    CAIFilterRealPosition to = target;
    to.z += 0.4f;
    return !CAIFilterGlobal::CheckCollisionLineToEnvironment(from, to, m_filter);
}
