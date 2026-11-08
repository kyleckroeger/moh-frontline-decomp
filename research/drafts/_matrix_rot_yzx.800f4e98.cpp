union CVector3Data {
    double pair[2];
    struct { float x, y, z, w; } c;
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float x, float y, float z) {
        d.c.x = x;
        d.c.y = y;
        d.c.z = z;
    }
    CVector3(const CVector3& o) : d(o.d) {}
    CVector3 operator=(const CVector3& o) {
        d.pair[0] = o.d.pair[0];
        d.pair[1] = o.d.pair[1];
        return *this;
    }
    CVector3 operator*(float s) const { return CVector3(s * d.c.x, s * d.c.y, s * d.c.z); }
    CVector3 operator+(const CVector3& o) const { return CVector3(d.c.x + o.d.c.x, d.c.y + o.d.c.y, d.c.z + o.d.c.z); }
    CVector3 operator-(const CVector3& o) const { return CVector3(d.c.x - o.d.c.x, d.c.y - o.d.c.y, d.c.z - o.d.c.z); }

    CVector3& operator+=(const CVector3& o) {
        d.c.x += o.d.c.x;
        d.c.y += o.d.c.y;
        d.c.z += o.d.c.z;
        return *this;
    }
    CVector3& operator-=(const CVector3& o) {
        d.c.x -= o.d.c.x;
        d.c.y -= o.d.c.y;
        d.c.z -= o.d.c.z;
        return *this;
    }

    CVector3& operator*=(float s) {
        d.c.x = s * d.c.x;
        d.c.y = s * d.c.y;
        d.c.z = s * d.c.z;
        return *this;
    }

    void Set(float x, float y, float z) {
        d.c.x = x;
        d.c.y = y;
        d.c.z = z;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CVector3 GetRight() const { return *(const CVector3*)m[0]; }
    CVector3 GetFront() const { return *(const CVector3*)m[1]; }
    CVector3 GetUp() const { return *(const CVector3*)m[2]; }
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);

    float m[4][4];
} __attribute__((aligned(16)));

float MathSinf(float);
float MathCosf(float);

void _matrix_rot_yzx(CMatrix& m, long ay, long az, long ax) {
    float y = 6.2831855f * (float)ay / 65536.0f;
    float sy = MathSinf(y);
    float z = 6.2831855f * (float)az / 65536.0f;
    float sz = MathSinf(z);
    float x = 6.2831855f * (float)ax / 65536.0f;
    float sx = MathSinf(x);
    float cy = MathCosf(y);
    float cz = MathCosf(z);
    float cx = MathCosf(x);
    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 newRight;
    CVector3 newFront;
    CVector3 newUp;
    right = m.GetRight();
    front = m.GetFront();
    up = m.GetUp();
    newRight.d.c.x = cz * right.d.c.x;
    newRight.d.c.y = cz * right.d.c.y;
    newRight.d.c.z = cz * right.d.c.z;
    newUp.d.c.x = cz * up.d.c.x;
    CVector3 t1;
    t1.d.c.x = sz * up.d.c.x;
    t1.d.c.y = sz * up.d.c.y;
    t1.d.c.z = sz * up.d.c.z;
    newRight -= t1;
    newUp.d.c.y = cz * up.d.c.y;
    newUp.d.c.z = cz * up.d.c.z;
    CVector3 t2;
    t2.d.c.x = sz * right.d.c.x;
    t2.d.c.y = sz * right.d.c.y;
    t2.d.c.z = sz * right.d.c.z;
    newUp += t2;
    CVector3 c;
    c.d.c.x = cx * front.d.c.x;
    c.d.c.y = cx * front.d.c.y;
    c.d.c.z = cx * front.d.c.z;
    CVector3 t3;
    t3.d.c.x = sx * newRight.d.c.x;
    t3.d.c.y = sx * newRight.d.c.y;
    t3.d.c.z = sx * newRight.d.c.z;
    c -= t3;
    newRight *= cx;
    CVector3 t4;
    t4.d.c.x = sx * front.d.c.x;
    t4.d.c.y = sx * front.d.c.y;
    t4.d.c.z = sx * front.d.c.z;
    newRight += t4;
    newFront.d.c.x = cy * c.d.c.x;
    newFront.d.c.y = cy * c.d.c.y;
    newFront.d.c.z = cy * c.d.c.z;
    CVector3 t5;
    t5.d.c.x = sy * newUp.d.c.x;
    t5.d.c.y = sy * newUp.d.c.y;
    t5.d.c.z = sy * newUp.d.c.z;
    newFront += t5;
    newUp *= cy;
    CVector3 t6;
    t6.d.c.x = sy * c.d.c.x;
    t6.d.c.y = sy * c.d.c.y;
    t6.d.c.z = sy * c.d.c.z;
    newUp -= t6;
    m.SetRight(newRight);
    m.SetFront(newFront);
    m.SetUp(newUp);
}
