
void DebugMsg(const char*, ...);

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
    friend CoverPointTypes operator|(const CoverPointTypes& a, const CoverPointTypes& o) { CoverPointTypes t; t.m_bits = a.m_bits | o.m_bits; return t; }
};
inline CoverPointTypes Mask(unsigned int v) { CoverPointTypes t; t.m_bits = v; return t; }

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
    void GetFront(CAIFilterRealVector3& v) const {
        CMatrix m;
        orientation.GetMatrix(m);
        m.GetFront(v);
    }
    int m_flags;
};

enum COVERPOINT_TYPE {};

class CCoverPoint {
public:
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;
    CAIFilterRealVector3 GetPeekLocation();
    void GetObjectFront(CAIFilterRealVector3& v) const {
        CoverPointObjectView* object = m_object;
        CMatrix matrix;
        object->orientation.GetMatrix(matrix);
        matrix.GetFront(v);
    }

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

CAIFilterRealVector3 CCoverPoint::GetPeekLocation() {
    CAIFilterRealVector3 front;
    m_object->GetFront(front);
    CoverPointTypes types = GetTypes();
    if ((types & 0x51).m_bits) {
        float t = front.x;
        front.x = -front.y;
        front.y = t;
    } else if ((types & 0xa2).m_bits) {
        float t = -front.x;
        front.x = front.y;
        front.y = t;
    } else if ((types & 4).m_bits) {
        front.Set(0.0f, 0.0f, 1.0f);
    }
    front *= 0.8f;
    CAIFilterRealVector3 peek;
    CAIFilterRealVector3 position;
    GetModifiedPosition(position, (COVERPOINT_TYPE)types.m_bits);
    peek.Set(position.x + front.x, position.y + front.y, position.z + front.z);
    peek.z += 1.0;
    return peek;
}
