// A fragment of CoverPoint.cpp (0x8005314c): CCoverPoint::GetModifiedPosition
// starts from the cover point object's position, takes the front row of its
// orientation matrix and, for peek types 1 and 2, steps 0.275 back along it
// and then 0.25 to one side (the scaled front turned a quarter turn about z
// one way or the other); type 4 steps a full unit back; type 3 and the others
// keep the position. The class, the type and the functions are named by the
// symbols; the object view, the vector and matrix views (as in
// AIObject_coverwalkto.cpp), the type meanings and the inline helpers are
// inferred. The rest of the file is in other units.
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
};

enum COVERPOINT_TYPE {};

class CCoverPoint {
public:
    void GetModifiedPosition(CAIFilterRealVector3&, COVERPOINT_TYPE) const;
    unsigned char unknown00[4];
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
