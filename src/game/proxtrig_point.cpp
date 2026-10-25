// A fragment of proxtrig.cpp (0x80044d3c): IsPointInTrigger tests a point
// against a trigger's shape, chosen by the type at +44 of its core: 0 a
// sphere (radius), 1 an upright cylinder (radius, top and bottom heights), 2
// a box oriented by the core's rotation quaternion (the point is taken into
// the box frame through the matrix rows and compared with the half sizes;
// the third axis reuses the second half size as its upper bound, as in the
// original); other types are never inside. Sizes are 12.4 fixed-point shorts
// (+46, +48, +50). The function and the classes are named by the symbols; the
// members, the transform and setter helpers, the row accessor and the
// CVector3 copy forms (a member-initialiser copy constructor and an
// assignment returning by value) are inferred. The rest of the file is in
// other units.
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
    float Dot(float x, float y, float z) const { return d.c.x * x + d.c.y * y + d.c.z * z; }

    CVector3Data d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetPos(CVector3);
    CVector3 GetRow(int i) const { return *(const CVector3*)m[i]; }

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

struct MOH_core_Struct {
    unsigned char unknown00[16];
    float x;
    float y;
    float z;
    CQuaternion rotation;
    short type;
    short size0;
    short size1;
    short size2;
};

struct BSObject;

struct TriggerObject_struct {
    unsigned int flags;
    MOH_core_Struct* core;
    BSObject* bsObject;
};

static inline void Set3(float& a, float& b, float& c, float x, float y, float z) { a = x; b = y; c = z; }

static inline void Dots(float& d0, float& d1, float& d2, const CVector3& r0, const CVector3& r1, const CVector3& r2, float dx, float dy, float dz) {
    Set3(d0, d1, d2, r0.Dot(dx, dy, dz), r1.Dot(dx, dy, dz), r2.Dot(dx, dy, dz));
}

static inline void ToLocal(const CMatrix& m, const CVector3& point, float& d0, float& d1, float& d2) {
    CVector3 pos;
    pos = m.GetRow(3);
    float dy = point.d.c.y - pos.d.c.y;
    float dx = point.d.c.x - pos.d.c.x;
    float dz = point.d.c.z - pos.d.c.z;
    Dots(d0, d1, d2, m.GetRow(0), m.GetRow(1), m.GetRow(2), dx, dy, dz);
}

bool IsPointInTrigger(const CVector3& point, const TriggerObject_struct* trigger) {
    MOH_core_Struct* core = trigger->core;
    CVector3 center(core->x, core->y, core->z);
    bool result;
    float dy;
    float dx;
    float dz;
    float radius;
    float bottom;
    float top;
    switch (core->type) {
    case 0: {
        dx = point.d.c.x - center.d.c.x;
        dy = point.d.c.y - center.d.c.y;
        dz = point.d.c.z - center.d.c.z;
        radius = core->size0 / 16.0f;
        if (dx * dx + dy * dy + dz * dz > radius * radius)
            result = false;
        else
            result = true;
        break;
    }
    case 1: {
        radius = core->size0 / 16.0f;
        dy = point.d.c.y - center.d.c.y;
        dx = point.d.c.x - center.d.c.x;
        bottom = core->size2 / 16.0f;
        dz = point.d.c.z - center.d.c.z;
        top = core->size1 / 16.0f;
        if (dx * dx + dy * dy > radius * radius)
            result = false;
        else if (dz <= top && dz >= bottom)
            result = true;
        else
            result = false;
        break;
    }
    case 2: {
        CMatrix matrix;
        trigger->core->rotation.GetMatrix(matrix);
        matrix.SetPos(CVector3(trigger->core->x, trigger->core->y, trigger->core->z));
        core = trigger->core;
        float hx;
        float hy;
        float hz;
        Set3(hz, hy, hx, core->size1 / 16.0f / 2.0f, -(core->size2 / 16.0f) / 2.0f, core->size0 / 16.0f / 2.0f);
        float d0;
        float d1;
        float d2;
        ToLocal(matrix, point, d0, d1, d2);
        if (d0 > hx || d0 < -hx)
            result = false;
        else if (d1 > hy || d1 < -hy)
            result = false;
        else if (d2 <= hy && d2 >= -hz)
            result = true;
        else
            result = false;
        break;
    }
    default:
        result = false;
        break;
    }
    return result;
}
