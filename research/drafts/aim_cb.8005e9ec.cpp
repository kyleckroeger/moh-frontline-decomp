union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
    CVector3 operator*(float k) const { return CVector3(x * k, y * k, z * k); }
    CVector3 operator+(CVector3 o) const { return CVector3(x + o.x, y + o.y, z + o.z); }
    CVector3 operator-(CVector3 o) const { return CVector3(x - o.x, y - o.y, z - o.z); }
    void operator*=(float k) {
        x *= k;
        y *= k;
        z *= k;
    }
    void Set(const CVector3& o) {
        x = o.x;
        y = o.y;
        z = o.z;
    }
    CVector3& operator=(const CVector3& other) {
        *(CVector3Pair*)this = *(const CVector3Pair*)&other;
        return *this;
    }
    void operator-=(const CVector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

inline CVector3 operator*(float k, const CVector3& v) {
    CVector3 r;
    r.x = k * v.x;
    r.y = k * v.y;
    r.z = k * v.z;
    return r;
}

#define X(a) (a).x
#define Y(a) (a).y
#define Z(a) (a).z

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    CMatrix& operator=(const CMatrix&);
    static void InitClass();
    const CVector3& GetRight() const { return *(CVector3*)m[0]; }
    const CVector3& GetForward() const { return *(CVector3*)m[1]; }
    const CVector3& GetUp() const { return *(CVector3*)m[2]; }
    CVector3 GetPos() const {
        CVector3 v;
        v = *(CVector3*)m[3];
        return v;
    }

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

extern "C" double tan(double);
extern "C" double sin(double);
extern "C" double cos(double);

class CSoldierObject;

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual CSoldierObject* AsSoldierObject();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual struct AIDoodadView* GetAIDoodad();
};

class CSoldierObject : public ISceneNode {
};

struct CAIFilterRealPosition {
    float x;
    float y;
    float z;
};

class CAIObject {
public:
    unsigned char unknown000[196];
    CAIObject* m_target;
    unsigned char unknown0c8[40];
    CAIFilterRealPosition m_unknown0f0;
    unsigned char unknown0fc[84];
    CAIFilterRealPosition m_unknown150;
    unsigned char unknown15c[84];
    unsigned int m_flags;
};

struct CAIFilterObject {
    void* unknown00;
    unsigned char unknown04[4];
    CAIObject* m_object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterObject* filter;
};

struct AimOwnerView : ISceneNode {
    unsigned char unknown0004[9020];
    CMatrix m_matrix;
};

struct AnimStateMachineView {
    unsigned char unknown00[4];
};

struct AimUserView {
    unsigned char unknown000[2192];
    AnimStateMachineView m_machine;
    unsigned char unknown894[420];
    unsigned int m_flags;
    unsigned char unknowna3c[636];
    AimOwnerView* m_owner;
};

struct AnimAimInfoView {
    unsigned char unknown00[8];
    long pitch;
    long yaw;
};

struct AnimListEntryView {
    int anim;
    unsigned short flags;
    AnimAimInfoView* info;
};

struct AnimFileStateAnimList_t {
    unsigned short count;
    AnimListEntryView entries[1];
};

struct AnimObjHdr_t;
struct AnimChannel_t {
    unsigned char unknown00[52];
    unsigned char m_weight[84];
};

struct AimSlotView {
    float weight;
    int channel;
};

struct AimStateView {
    unsigned char unknown00[4];
    CAIFilterRealPosition target;
    AimSlotView slots[5];
};

class CAnimCallbacks {
public:
    static int aim_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
};

extern "C" int AnimChanStartAnim(AnimObjHdr_t*, AnimChannel_t*, int, unsigned short, unsigned short, void*, float);
extern "C" void* AnimStGetStateUserMemory(AnimStateMachineView*, unsigned short);
extern "C" void AnimStSetStateFlags(AnimStateMachineView*, unsigned short, int);
extern "C" unsigned short AnimStGetCurrentState(AnimStateMachineView*);
extern "C" void AnimWgtSet(void*, int, int, float, float);
void _find_aim_blend_values(float, float, float*);

static inline unsigned short FindAim(AnimFileStateAnimList_t* list, long yaw, long pitch) {
    for (unsigned int i = 0; i < list->count; i++) {
        AnimAimInfoView* info = list->entries[i].info;
        if (info->yaw == yaw && info->pitch == pitch)
            return i;
    }
    return 0;
}

static inline unsigned short FindCentre(AnimFileStateAnimList_t* list) {
    for (unsigned int i = 0; i < list->count; i++) {
        if (list->entries[i].info->yaw == 0)
            return i;
    }
    return 0;
}

int CAnimCallbacks::aim_cb(AnimFileStateAnimList_t* list, unsigned short state, AnimObjHdr_t* header,
                           AnimChannel_t* channel, void* user, unsigned long event, void* extra, float time) {
    CAIFilterObject* filter;
    AimUserView* animated = (AimUserView*)user;
    AimOwnerView* owner = animated->m_owner;
    CSoldierObject* soldier = owner->AsSoldierObject();
    if (!soldier)
        return 0;
    filter = soldier->GetAIDoodad()->filter;
    filter->m_object->m_flags |= 0x10;
    CVector3 target;
    if (!filter->m_object->m_target) {
        float x = filter->m_object->m_unknown150.x;
        float z = filter->m_object->m_unknown150.z;
        float y = filter->m_object->m_unknown150.y;
        X(target) = x;
        Y(target) = y;
        Z(target) = z;
        Z(target) += 1.0f;
    } else {
        float x = filter->m_object->m_unknown0f0.x;
        float z = filter->m_object->m_unknown0f0.z;
        float y = filter->m_object->m_unknown0f0.y;
        X(target) = x;
        Y(target) = y;
        Z(target) = z;
    }
    float tanHalf = tan(0.5235987901687622);
    AimStateView* memory = (AimStateView*)AnimStGetStateUserMemory(&animated->m_machine, state);
    float weights[5];
    int i;
    switch (event) {
    case 0: {
        AnimStSetStateFlags(&animated->m_machine, state, 1);
        CMatrix m;
        m = owner->m_matrix;
        unsigned short index[5];
        memory->target.x = X(target);
        memory->target.y = Y(target);
        memory->target.z = Z(target);
        CVector3 diff;
        diff = target;
        diff -= m.GetPos();
        CVector3 forward;
        forward = m.GetForward();
        CVector3 right;
        right = m.GetRight();
        CVector3 up;
        up = m.GetUp();
        index[0] = FindAim(list, -45, 45);
        index[1] = FindAim(list, 45, 45);
        index[2] = FindAim(list, 45, -45);
        index[3] = FindAim(list, -45, -45);
        index[4] = FindCentre(list);
        long pitch = list->entries[index[4]].info->pitch;
        if (pitch != 0) {
            float c;
            float s;
            float a = 0.017453 * pitch;
            c = cos(a);
            s = sin(a);
            float t = tan(a);
            CVector3 cf = c * forward;
            CVector3 su = s * up;
            Y(forward) = Y(cf) + Y(su);
            X(forward) = X(cf) + X(su);
            Z(forward) = Z(cf) + Z(su);
            up *= c;
            float bz = Z(forward) - Z(su);
            float by = Y(forward) - Y(su);
            float bx = X(forward) - X(su);
            bx *= t;
            by *= t;
            bz *= t;
            X(up) -= bx;
            Y(up) -= by;
            Z(up) -= bz;
        }
        float distance = X(forward) * X(diff) + Y(forward) * Y(diff) + Z(forward) * Z(diff);
        float vertical = X(up) * X(diff) + Y(up) * Y(diff) + Z(up) * Z(diff);
        float horizontal = X(right) * X(diff) + Y(right) * Y(diff) + Z(right) * Z(diff);
        float d = distance;
        if (distance < 1.0f)
            d = 1.0f;
        float scale = 1.0f / (tanHalf * d);
        _find_aim_blend_values(scale * (horizontal - 0.2f), scale * (vertical - 1.15f), weights);
        for (i = 0; i < 5; i++) {
            unsigned short k = index[i];
            AimSlotView* slot = &memory->slots[i];
            slot->channel = AnimChanStartAnim(header, channel, list->entries[k].anim, list->entries[k].flags, state, user, weights[i]);
            slot->weight = weights[i];
        }
        animated->m_flags &= ~0x10;
        break;
    }
    case 2:
        if (state == AnimStGetCurrentState(&animated->m_machine)) {
            if (memory->target.x != X(target) || memory->target.y != Y(target) || memory->target.z != Z(target)) {
                CMatrix m;
                m = owner->m_matrix;
                CVector3 diff;
                diff = target;
                diff -= m.GetPos();
                CVector3 forward;
                forward = m.GetForward();
                CVector3 right;
                right = m.GetRight();
                CVector3 up;
                up = m.GetUp();
                unsigned short centre = FindCentre(list);
                long pitch = list->entries[centre].info->pitch;
                if (pitch != 0) {
                    float a = 0.017453 * pitch;
                    float c = cos(a);
                    float s = sin(a);
                    float t = tan(a);
                    CVector3 cf = c * forward;
                    CVector3 su = s * up;
                    Y(forward) = Y(cf) + Y(su);
                    X(forward) = X(cf) + X(su);
                    Z(forward) = Z(cf) + Z(su);
                    up *= c;
                    float bz = Z(forward) - Z(su);
                    float by = Y(forward) - Y(su);
                    float bx = X(forward) - X(su);
                    bx *= t;
                    by *= t;
                    bz *= t;
                    X(up) -= bx;
                    Y(up) -= by;
                    Z(up) -= bz;
                }
                float distance = X(forward) * X(diff) + Y(forward) * Y(diff) + Z(forward) * Z(diff);
                float vertical = X(up) * X(diff) + Y(up) * Y(diff) + Z(up) * Z(diff);
                float horizontal = X(right) * X(diff) + Y(right) * Y(diff) + Z(right) * Z(diff);
                float d = distance;
                if (distance < 1.0f)
                    d = 1.0f;
                float scale = 1.0f / (tanHalf * d);
                _find_aim_blend_values(scale * (horizontal - 0.2f), scale * (vertical - 1.15f), weights);
                for (i = 0; i < 5; i++) {
                    AnimWgtSet(&channel[memory->slots[i].channel].m_weight, 0, 0, weights[i], weights[i]);
                    memory->slots[i].weight = weights[i];
                }
            }
        }
        break;
    case 1:
        filter->m_object->m_flags &= ~0x10;
        break;
    }
    return 0;
}
