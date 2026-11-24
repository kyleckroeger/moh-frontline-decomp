// A fragment of CoverPoint.cpp (0x80052abc): CCoverPoint::CheckLineCollision returns 1 when the
// line from 'from' to 'to' is clear of the environment. Otherwise, for a point with neither left
// (0x51) nor right (0xa2) peek types it tries 'from' moved 1.5 units to each side of the cover
// object's front (a quarter turn about z each way) and returns 2 with the first clear position in
// 'out'; a peek point tries one position 0.8 units to its peek side. 0 when nothing is clear.
// GetTypes is the body CoverPoint_peek and CoverPoint_modified share; its final mask is a
// temporary in the and (a named 'mask' local numbers the masked types before the mask and
// swaps two volatile registers here, research/notes/matching-rules.md).
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

    void GetFront(CAIFilterRealVector3& v) const {
        CMatrix matrix;
        orientation.GetMatrix(matrix);
        matrix.GetFront(v);
    }
};

class CAIFilterObject;

class CAIFilterGlobal {
public:
    static bool CheckCollisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&, CAIFilterObject*);
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
    CoverPointTypes operator|(const CoverPointTypes& o) const { CoverPointTypes t; t.m_bits = m_bits | o.m_bits; return t; }
};
inline CoverPointTypes Mask(unsigned int v) { CoverPointTypes t; t.m_bits = v; return t; }



class CCoverPoint {
public:
    int CheckLineCollision(const CAIFilterRealPosition&, const CAIFilterRealPosition&, CAIFilterRealPosition*) const;
    void GetTypes(CoverPointTypes& out) const {
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
        out.m_bits = types.m_bits & (CoverPointTypes(1) | 2 | 4).m_bits;
        types.m_bits = out.m_bits;
    }


    void GetFront(CAIFilterRealVector3& v) const { m_object->GetFront(v); }
    unsigned char unknown00[4];
    CoverPointObjectView* m_object;
};

int CCoverPoint::CheckLineCollision(const CAIFilterRealPosition& from, const CAIFilterRealPosition& to, CAIFilterRealPosition* out) const {
    if (CAIFilterGlobal::CheckCollisionLineToEnvironment(from, to, 0)) {
        CAIFilterRealVector3 test;
        CoverPointTypes types;
        GetTypes(types);
        if (!(types & (CoverPointTypes(0x51) | 0xa2)).m_bits) {
            CAIFilterRealVector3 side;
            {
                GetFront(side);
                float t = side.x;
                side.x = -side.y;
                side.y = t;
                side *= 1.5f;
                test.x = from.x + side.x;
                test.y = from.y + side.y;
                test.z = from.z + side.z;
                if (!CAIFilterGlobal::CheckCollisionLineToEnvironment(test, to, 0)) {
                    float z = test.z; float y = test.y; float x = test.x; out->x = x; out->y = y; out->z = z;
                    return 2;
                }
            }
            {
                GetFront(side);
                float t = -side.x;
                side.x = side.y;
                side.y = t;
                side *= 1.5f;
                test.x = from.x + side.x;
                test.y = from.y + side.y;
                test.z = from.z + side.z;
                if (!CAIFilterGlobal::CheckCollisionLineToEnvironment(test, to, 0)) {
                    float z = test.z; float y = test.y; float x = test.x; out->x = x; out->y = y; out->z = z;
                    return 2;
                }
            }
            return 0;
        }
        CAIFilterRealVector3 side;
        GetFront(side);
        if ((types & 0x51).m_bits) {
            float t = side.x;
            side.x = -side.y;
            side.y = t;
        } else if ((types & 0xa2).m_bits) {
            float t = -side.x;
            side.x = side.y;
            side.y = t;
        }
        side *= 0.8f;
        test.x = from.x + side.x;
        test.y = from.y + side.y;
        test.z = from.z + side.z;
        if (!CAIFilterGlobal::CheckCollisionLineToEnvironment(test, to, 0)) {
            float z = test.z; float y = test.y; float x = test.x; out->x = x; out->y = y; out->z = z;
            return 2;
        }
        return 0;
    }
    return 1;
}
