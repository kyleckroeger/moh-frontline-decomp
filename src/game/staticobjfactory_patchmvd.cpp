/* CStaticObjectFactory::PatchUpMVDFile (a fragment of staticobjfactory.cpp):
   byte-swap a loaded motion data file in place (the header's count, a word
   at +4 and the relative flag, then each frame's position and rotation); a
   relative file then turns its per-frame position deltas into running sums.
   MotionData, CVector3, CQuaternion and CStaticObjectFactory are named by
   the mangled symbols; the layouts are inferred from offsets (frames of a
   16-byte position and a quaternion from +16, the count at +8, the relative
   flag at +12). ChangeEndian is the byte-swap helper as in AnimObjFactory.cpp.
   The relative pass copies each frame's rotation through a local and stores
   it back before the position (the original reloads and stores all four
   rotation words). */
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

inline void EndianSwap(float& value, bool) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}

/* inferred: a 16-byte vector copied as a double pair */
union CVector3Data {
    double pair[2];
    float v[4];
};

struct CVector3 {
    CVector3() {}
    CVector3(const CVector3& o) : d(o.d) {}
    CVector3& operator=(const CVector3& o) { d = o.d; return *this; }
    CVector3(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }
    void EndianSwap() {
        ::EndianSwap(d.v[0], true);
        ::EndianSwap(d.v[1], true);
        ::EndianSwap(d.v[2], true);
    }
    CVector3& operator+=(const CVector3& o) {
        d.v[0] += o.d.v[0];
        d.v[1] += o.d.v[1];
        d.v[2] += o.d.v[2];
        return *this;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

class CQuaternion {
public:
    void EndianSwap();
    CQuaternion() {}
    CQuaternion(const CQuaternion& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}
    CQuaternion& operator=(const CQuaternion& q) { x = q.x; y = q.y; z = q.z; w = q.w; return *this; }

    float x, y, z, w;
};

/* inferred views */
struct MotionFrameView {
    CVector3 position;
    CQuaternion rotation;
};

struct MotionData {
    int data00;
    int data04;
    int count;
    int relative;
    MotionFrameView frames[1];
};

class CStaticObjectFactory {
public:
    static void PatchUpMVDFile(MotionData*);
};

void CStaticObjectFactory::PatchUpMVDFile(MotionData* data) {
    ChangeEndian(data->count);
    ChangeEndian(data->data04);
    ChangeEndian(data->relative);
    for (int i = 0; i < data->count; i++) {
        data->frames[i].position.EndianSwap();
        data->frames[i].rotation.EndianSwap();
    }
    if (data->relative) {
        CVector3 sum(0.0f, 0.0f, 0.0f);
        for (int i = 0; i < data->count; i++) {
            sum += data->frames[i].position;
            CQuaternion q(data->frames[i].rotation);
            data->frames[i].rotation = q;
            data->frames[i].position = sum;
        }
    }
}
