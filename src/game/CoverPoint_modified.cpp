// A fragment of CoverPoint.cpp (0x8005314c): CCoverPoint::GetModifiedPosition
// starts from the cover point object's position, takes the front row of its
// orientation matrix and, for peek types 1 and 2, steps 0.275 back along it
// and then 0.25 to one side (the scaled front turned a quarter turn about z
// one way or the other); type 4 steps a full unit back; type 3 and the others
// keep the position. GetAvailablePeekPosition maps the object's byte-swapped
// flag word (+112) through a table of {flag, peek type} pairs to the peek
// types it offers (1, 2 and 4), keeps those not flagged occupied in the cover
// point's low byte and returns them, reduced to 'avail & (1 - avail)' when
// more than one bit is left. The class, the type and the functions are named
// by the symbols; the object view, the vector and matrix views (as in
// AIObject_coverwalkto.cpp), the type meanings, the type-set helper (its
// operands bound to 'const int&', so the original's small-data literals are
// emitted) and the inline helpers are inferred. The rest of the file is in
// other units.
union CVector3Pair {
    double pair[2];
    float v[4];
};

class CAIFilterRealPosition {
public:
    void Set(float nx, float ny, float nz) { x = nx; y = ny; z = nz; }
    float x;
    float y;
    float z;
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
public:
    CAIFilterRealVector3() {}
    CAIFilterRealVector3(const CAIFilterRealVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
    }
    CAIFilterRealVector3& operator=(const CAIFilterRealVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    CAIFilterRealVector3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    CAIFilterRealVector3& operator-=(const CAIFilterRealVector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    CAIFilterRealVector3& operator+=(const CAIFilterRealVector3& o) { x += o.x; y += o.y; z += o.z; return *this; }

    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    CAIFilterRealVector3 GetRow(int i) const { return *(const CAIFilterRealVector3*)m[i]; }
    void GetFront(CAIFilterRealVector3& v) const { v = GetRow(1); }

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void GetMatrix(CMatrix&) const;
    float x;
    float y;
    float z;
    float w;
};

struct CoverPointObjectView {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    CQuaternion orientation;
    unsigned char unknown2c[68];
    int m_flags;
};

enum COVERPOINT_TYPE {};

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

struct CoverPointTypes {
    int m_bits;
    CoverPointTypes() {}
    CoverPointTypes(const int& bits) : m_bits(bits) {}
    CoverPointTypes& operator&=(const int& bits) { m_bits &= bits; return *this; }
    CoverPointTypes& operator&=(const CoverPointTypes& o) { m_bits &= o.m_bits; return *this; }
    CoverPointTypes operator&(const int& bits) const { return CoverPointTypes(m_bits & bits); }
    CoverPointTypes operator&(const CoverPointTypes& o) const { return CoverPointTypes(m_bits & o.m_bits); }
    CoverPointTypes& operator|=(const int& bits) { m_bits |= bits; return *this; }
    CoverPointTypes& operator=(const CoverPointTypes& o) { m_bits = o.m_bits; return *this; }
    CoverPointTypes operator|(const int& bits) const { CoverPointTypes t; t.m_bits = m_bits | bits; return t; }
};

class CCoverPoint {
public:
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;
    COVERPOINT_TYPE GetAvailablePeekPosition() const;

    CoverPointTypes GetTypes() const {
        CoverPointTypes types;
        types.m_bits = m_object->m_flags;
        ChangeEndian(*(int*)&types.m_bits);
        struct { unsigned int mask; unsigned int type; } table[8] = {
            {0x01, 1}, {0x02, 2}, {0x04, 4}, {0x08, 4}, {0x10, 1}, {0x20, 2}, {0x40, 1}, {0x80, 2}};
        unsigned int bits = 0;
        for (int i = 0; i < 8; i++) {
            if (types.m_bits & table[i].mask)
                bits |= table[i].type;
        }
        types.m_bits = bits;
        types &= 0xff;
        CoverPointTypes mask = CoverPointTypes(1) | 2 | 4;
        types &= mask;
        return types;
    }

    unsigned int m_occupied;
    CoverPointObjectView* m_object;
};
void CCoverPoint::GetModifiedPosition(CAIFilterRealVector3& position, COVERPOINT_TYPE type) const {
    position.Set(m_object->x, m_object->y, m_object->z);
    CoverPointObjectView* object = m_object;
    CMatrix matrix;
    object->orientation.GetMatrix(matrix);
    CAIFilterRealVector3 front;
    matrix.GetFront(front);
    switch (type) {
    case 1: {
        front *= 0.275f;
        position -= front;
        front *= 0.9090909f;
        float t = -front.x;
        front.x = front.y;
        front.y = t;
        position += front;
        break;
    }
    case 2: {
        front *= 0.275f;
        position -= front;
        front *= 0.9090909f;
        float t = front.x;
        front.x = -front.y;
        front.y = t;
        position += front;
        break;
    }
    case 3:
        break;
    case 4:
        front *= 1.0f;
        position -= front;
        break;
    }
}

COVERPOINT_TYPE CCoverPoint::GetAvailablePeekPosition() const {
    CoverPointTypes types = GetTypes();
    int avail = ~(m_occupied & 0xff);
    avail &= types.m_bits;
    if (avail <= 1)
        return (COVERPOINT_TYPE)avail;
    avail &= 1 - avail;
    return (COVERPOINT_TYPE)avail;
}
