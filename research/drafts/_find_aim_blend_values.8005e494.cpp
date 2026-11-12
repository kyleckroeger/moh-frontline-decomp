// EndianSwapAnimPlugIn, the first function of the file: converts an animation
// file's plug-in data in place when its version shows the other byte order,
// choosing the layout by the state's selection callback. The names come from
// the mangled symbols; the per-callback layouts and the state-selection entry
// view are inferred from offsets. The conversion helpers are the inlined ones
// described in propdat.cpp (inferred): the aim fields go straight through the
// int conversion and the others through the template, which places their
// temporaries in separate stack groups. The callbacks themselves and the rest
// of the file are not part of this unit.
inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

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

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

struct AnimFileStateAnimList_t;
struct AnimObjHdr_t;
struct AnimChannel_t;

class CAnimCallbacks {
public:
    static int pathing_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int random_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int turn_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int death_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int weapon_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int aim_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int face_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
    static int floco_cb(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);
};

typedef int (*AnimStateSelFunc)(AnimFileStateAnimList_t*, unsigned short, AnimObjHdr_t*, AnimChannel_t*, void*, unsigned long, void*, float);

struct AnimStateSelView {
    unsigned char unknown00[4];
    AnimStateSelFunc callback;
};

extern AnimStateSelView _AnimObj_StateSelFuncArray[];

struct AnimFilePluginData_t {
    unsigned short version;
    unsigned short length;
    float words[9];
};

struct AimPluginView {
    unsigned short version;
    unsigned short length;
    unsigned char unknown04[4];
    int words[2];
};

struct FlocoEntryView {
    float a;
    float b;
    float c;
};

struct FlocoPluginView {
    unsigned short version;
    unsigned short length;
    unsigned int count;
    float words[3];
    FlocoEntryView entries[1];
};

void EndianSwapAnimPlugIn(AnimFilePluginData_t& data, int state) {
    if (data.version < 256)
        return;
    ChangeEndian(data.version);
    ChangeEndian(data.length);
    if (data.length == 0)
        return;
    AnimStateSelFunc callback = _AnimObj_StateSelFuncArray[state].callback;
    if (callback == CAnimCallbacks::pathing_cb)
        return;
    if (callback == CAnimCallbacks::random_cb) {
        ChangeEndian(data.words[0]);
        ChangeEndian(data.words[1]);
    } else if (callback == CAnimCallbacks::turn_cb) {
        ChangeEndian(data.words[2]);
        ChangeEndian(data.words[3]);
        ChangeEndian(data.words[4]);
        ChangeEndian(data.words[5]);
        ChangeEndian(data.words[6]);
        ChangeEndian(data.words[7]);
    } else if (callback == CAnimCallbacks::death_cb) {
        ChangeEndian(data.words[0]);
        ChangeEndian(data.words[1]);
        ChangeEndian(data.words[2]);
        ChangeEndian(data.words[3]);
        ChangeEndian(data.words[4]);
        ChangeEndian(data.words[5]);
        ChangeEndian(data.words[6]);
        ChangeEndian(data.words[7]);
        ChangeEndian(data.words[8]);
    } else if (callback == CAnimCallbacks::weapon_cb) {
        ChangeEndian(data.words[0]);
    } else if (callback == CAnimCallbacks::aim_cb) {
        AimPluginView& aim = (AimPluginView&)data;
        ChangeEndian(aim.words[0]);
        ChangeEndian(aim.words[1]);
    } else if (callback == CAnimCallbacks::face_cb) {
        return;
    } else if (callback == CAnimCallbacks::floco_cb) {
        FlocoPluginView& floco = (FlocoPluginView&)data;
        ChangeEndian(floco.count);
        ChangeEndian(floco.words[0]);
        ChangeEndian(floco.words[1]);
        ChangeEndian(floco.words[2]);
        for (unsigned int i = 0; i < floco.count; i++) {
            ChangeEndian(floco.entries[i].a);
            ChangeEndian(floco.entries[i].b);
            ChangeEndian(floco.entries[i].c);
        }
    }
}

// Inferred inlines: the 2D cross product, and the barycentric weights of
// (x, y) in the triangle of the origin and the corners (bx, by) and (cx, cy)
// (the doubled area is -2). Written with the differences as call arguments.
inline float Cross2(float ax, float ay, float bx, float by) { return ax * by - bx * ay; }
static inline void TriangleBlend(float x, float y, float bx, float by, float cx, float cy,
                                 float& wa, float& wb, float& wc) {
    float k = -0.5f;
    wa = k * Cross2(bx - x, by - y, cx - x, cy - y);
    wb = k * Cross2(cx - x, cy - y, -x, -y);
    wc = k * Cross2(-x, -y, bx - x, by - y);
}
void _find_aim_blend_values(float x, float y, float* blend) {
    blend[0] = blend[1] = blend[2] = blend[3] = blend[4] = 0.0f;
    if (x == 0.0f && y == 0.0f) {
        blend[4] = 1.0f;
        return;
    }
    if (x > y) {
        if (x > -y) {
            if (x > 1.0f) {
                blend[1] = 0.5f * (1.0f + y / x);
                blend[2] = 1.0f - blend[1];
                return;
            }
            TriangleBlend(x, y, 1.0f, 1.0f, 1.0f, -1.0f, blend[4], blend[1], blend[2]);
        } else {
            if (y < -1.0f) {
                blend[2] = 0.5f * (1.0f + x / -y);
                blend[3] = 1.0f - blend[2];
                return;
            }
            TriangleBlend(x, y, 1.0f, -1.0f, -1.0f, -1.0f, blend[4], blend[2], blend[3]);
        }
    } else {
        if (-x < y) {
            if (y > 1.0f) {
                blend[1] = 0.5f * (1.0f + x / y);
                blend[0] = 1.0f - blend[1];
                return;
            }
            TriangleBlend(x, y, -1.0f, 1.0f, 1.0f, 1.0f, blend[4], blend[0], blend[1]);
        } else {
            if (x < -1.0f) {
                blend[0] = 0.5f * (1.0f + -y / x);
                blend[3] = 1.0f - blend[0];
                return;
            }
            TriangleBlend(x, y, -1.0f, -1.0f, -1.0f, 1.0f, blend[4], blend[3], blend[0]);
        }
    }
}
