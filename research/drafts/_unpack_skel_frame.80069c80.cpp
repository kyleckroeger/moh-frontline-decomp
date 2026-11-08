// Character skeletons: joint lookup by name, CharRelocateSkeleton (converts
// the byte order once, relocates the joint, hierarchy and bone pointers,
// converts each bone's matrix and walks the joint hierarchy to relocate its
// nodes), the weak EndianSwap(CharSkel_t&), CharTraverseSkeleton (recursive;
// the compiler inlines it into itself) and _SkelRelocateHierarchyNode.
// CharSkel_t and CharJointHierarchy_t are named by the mangled symbols; the
// record members and the per-record conversion helpers are inferred from
// offsets and the order of the conversions' stack temporaries, and are not
// original. The byte-order helpers are the inlined ones described in
// propdat.cpp (inferred).
extern "C" int strcmp(const char*, const char*);

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


struct CharJoint_t {
    int offset;
    char name[16];
};

struct CharJointHierarchy_t {
    unsigned char childcount;
    unsigned char limit;
    unsigned char field02[2];
    CharJointHierarchy_t* parent;
    CharJointHierarchy_t* children[1];
};

class CMatrix {
public:
    void EndianSwap();
};

struct CharBone_t {
    unsigned char field00[16];
    int field10;
    CMatrix* matrix;
};

struct CharSkelEntry_t {
    float a;
    float b;
    float c;
    int d;
};

struct CharSkelShorts_t {
    short v[3];

    void EndianSwap() {
        ChangeEndian(v[0]);
        ChangeEndian(v[1]);
        ChangeEndian(v[2]);
    }
};

struct CharSkel_t {
    int field00;
    unsigned short flags;
    unsigned short jointcount;
    CharJointHierarchy_t* hierarchy;
    CharJoint_t* joints;
    unsigned int bonecount;
    CharBone_t* bones;
    unsigned char field18[8];
    CharSkelEntry_t entries[80];
    CharSkelShorts_t shorts[80];
};

inline void EndianSwap(CharJoint_t& joint) {
    ChangeEndian(joint.offset);
}

inline void EndianSwap(CharBone_t& bone) {
    ChangeEndian(bone.field10);
    ChangeEndian(*reinterpret_cast<int*>(&bone.matrix));
}

int CharTraverseSkeleton(CharJointHierarchy_t*, void (*)(CharJointHierarchy_t*, void*), void*);
static void _SkelRelocateHierarchyNode(CharJointHierarchy_t*, void*);

void EndianSwap(CharSkel_t&);

int CharSkelGetJointByName(CharSkel_t* skel, char* name) {
    CharJoint_t* joint = skel->joints;
    int i;

    for (i = 0; i < skel->jointcount; joint++, i++) {
        if (strcmp(name, joint->name) == 0)
            break;
    }
    if (i == skel->jointcount)
        i = -1;
    return i;
}

void CharRelocateSkeleton(CharSkel_t* skel) {
    EndianSwap(*skel);
    if (skel->flags & 0x8000)
        return;
    skel->joints = (CharJoint_t*)((int)skel + (int)skel->joints);
    skel->hierarchy = (CharJointHierarchy_t*)((int)skel + (int)skel->hierarchy);
    skel->bones = (CharBone_t*)((int)skel + (int)skel->bones);
    for (unsigned int i = 0; i < skel->bonecount; i++) {
        EndianSwap(skel->bones[i]);
        skel->bones[i].matrix = (CMatrix*)((int)skel + (int)skel->bones[i].matrix);
        skel->bones[i].matrix->EndianSwap();
    }
    CharTraverseSkeleton(skel->hierarchy, _SkelRelocateHierarchyNode, skel);
    skel->hierarchy->parent = skel->hierarchy;
    for (unsigned int i = 0; i < skel->jointcount; i++) {
        EndianSwap(skel->joints[i]);
        skel->joints[i].offset = (int)skel + skel->joints[i].offset;
    }
    skel->flags |= 0x8000;
}

__declspec(weak) void EndianSwap(CharSkel_t& skel) {
    ChangeEndian(skel.field00);
    ChangeEndian(skel.flags);
    ChangeEndian(skel.jointcount);
    ChangeEndian(*reinterpret_cast<int*>(&skel.hierarchy));
    ChangeEndian(*reinterpret_cast<int*>(&skel.joints));
    ChangeEndian(*reinterpret_cast<int*>(&skel.bonecount));
    ChangeEndian(*reinterpret_cast<int*>(&skel.bones));
    for (int i = 0; i < 80; i++) {
        ChangeEndian(skel.entries[i].a);
        ChangeEndian(skel.entries[i].b);
        ChangeEndian(skel.entries[i].c);
        ChangeEndian(skel.entries[i].d);
    }
    for (int i = 0; i < 80; i++)
        skel.shorts[i].EndianSwap();
}

int CharTraverseSkeleton(CharJointHierarchy_t* node, void (*callback)(CharJointHierarchy_t*, void*), void* data) {
    int i;
    int result = -1;
    while (result < node->limit) {
        callback(node, data);
        if (node->childcount > 1) {
            for (i = 0; i < node->childcount; i++)
                result = CharTraverseSkeleton(node->children[i], callback, data);
        } else {
            result = node->limit;
        }
        node = node->children[0];
    }
    return result;
}

static inline void EndianSwap(CharJointHierarchy_t& node) {
    ChangeEndian(node.parent);
    for (int i = 0; i < node.childcount; i++)
        ChangeEndian(node.children[i]);
}

static void _SkelRelocateHierarchyNode(CharJointHierarchy_t* node, void* base) {
    EndianSwap(*node);
    node->parent = (CharJointHierarchy_t*)((int)base + (int)node->parent);
    for (int i = 0; i < node->childcount; i++)
        node->children[i] = (CharJointHierarchy_t*)((int)base + (int)node->children[i]);
}

/* inferred: three packed shorts (a position, angle or bone delta) */
struct SkelShorts_t {
    short v[3];

    void EndianSwap() {
        ChangeEndian(v[0]);
        ChangeEndian(v[1]);
        ChangeEndian(v[2]);
    }
};

/* inferred: the packed frame header in the motion data */
struct SkelFrameHeader_t {
    unsigned char count;
    unsigned char unknown01;
    short angle;
    SkelShorts_t position;
    short height;
    unsigned char unknown0c[2];
    SkelShorts_t bend;
    SkelShorts_t rotation;
    unsigned char data[1];
};

/* inferred: the base pose (triples after a short) and the delta groups
   (a count, then start/length byte pairs) */
struct SkelFrameRef_t {
    int unknown00;
    short* base;
    unsigned short* groups;
};

struct CharFrame_t {
    int m_count;
    int unknown04;
    float m_position[3];
    int unknown14;
    float m_rotation[3];
    int unknown24;
    int m_angle;
    int m_bend[3];
    short* m_values;
};

struct AnimMotWeightFrame_t {
    unsigned char unknown00[8];
    unsigned char m_flags;
    unsigned char unknown09[3];
    int m_angle;
    float m_position[3];
    int unknown1c;
    float m_rotation[3];
    int unknown2c;
    int m_bend[3];
    unsigned char unknown3c[8];
    SkelFrameRef_t* m_ref;
    SkelFrameHeader_t* m_header;
    int unknown4c;
};

static inline int SkelMirror(unsigned int flags) {
    int sign = 1;
    if (flags & 1)
        sign = -1;
    return sign;
}

static inline int SkelMirror2(unsigned int flags) {
    return flags ? -1 : 1;
}

static inline float SkelMirror3(unsigned int flags) {
    return flags ? -1 : 1;
}

static inline float SkelMirror4(unsigned int flags) {
    int sign = 1;
    if (flags)
        sign = -1;
    return sign;
}

/* inferred: a short read from the motion data, byte-swapped in place */
union SkelSwapShort_t {
    short value;
    unsigned char bytes[2];

    void Swap() {
        unsigned char c = bytes[0];
        bytes[0] = bytes[1];
        bytes[1] = c;
    }
};

static void _unpack_skel_frame(CharFrame_t* frame, AnimMotWeightFrame_t* weight) {
    SkelShorts_t position;
    SkelShorts_t bend;
    SkelShorts_t rotation;
    SkelShorts_t value;
    SkelShorts_t delta;
    unsigned int n;
    unsigned int noposition;
    SkelFrameHeader_t* header;
    unsigned int mirrored;
    SkelSwapShort_t height;
    unsigned int noangle;
    SkelSwapShort_t angle;
    unsigned int noheight;
    unsigned int i;
    unsigned int count;
    unsigned int hasbend;
    SkelFrameRef_t* ref;
    unsigned char flags;
    unsigned int g;
    unsigned char* data;

    header = weight->m_header;
    flags = weight->m_flags;
    count = header->count;
    ref = weight->m_ref;
    n = 0;
    noangle = flags & 2;
    noposition = flags & 4;
    noheight = flags & 8;
    hasbend = flags & 16;
    mirrored = flags & 1;
    frame->m_count = count;
    bend = header->bend;
    position = header->position;
    rotation = header->rotation;
    position.EndianSwap();
    height.value = header->height;
    height.Swap();
    bend.EndianSwap();
    angle.value = header->angle;
    angle.Swap();
    rotation.EndianSwap();
    if (count > 80)
        count = 80;
    data = header->data;
    if (noposition) {
        frame->m_position[0] = 0.0f;
        frame->m_position[1] = 0.0f;
        frame->m_position[2] = 0.0f;
        weight->m_position[0] = position.v[0] / 4096.0f * (mirrored ? -1 : 1);
        weight->m_position[1] = position.v[1] / 4096.0f;
    } else {
        float y = position.v[1] / 4096.0f;
        frame->m_position[0] = position.v[0] / 4096.0f * (mirrored ? -1 : 1);
        frame->m_position[1] = y;
        frame->m_position[2] = 0.0f;
    }
    if (noheight) {
        weight->m_position[2] = position.v[2] / 4096.0f;
        frame->m_position[2] = height.value / 4096.0f;
    } else {
        frame->m_position[2] = (position.v[2] + height.value) / 4096.0f;
    }
    if (hasbend) {
        weight->m_bend[0] = bend.v[0] << 8;
        weight->m_bend[1] = bend.v[1] << 8;
        weight->m_bend[2] = bend.v[2] << 8;
        weight->m_rotation[0] = rotation.v[0] / 4096.0f;
        weight->m_rotation[1] = rotation.v[1] / 4096.0f;
        weight->m_rotation[2] = rotation.v[2] / 4096.0f;
    } else {
        weight->m_bend[2] = 0;
        weight->m_bend[1] = 0;
        weight->m_bend[0] = 0;
        weight->m_rotation[0] = 0.0f;
        weight->m_rotation[1] = 0.0f;
        weight->m_rotation[2] = 0.0f;
    }
    frame->m_values[0] = 0;
    frame->m_values[1] = 0;
    frame->m_values[2] = 0;
    for (i = 1; i < count; i++) {
        value = *(SkelShorts_t*)(ref->base + i * 3 + 1);
        value.EndianSwap();
        frame->m_values[i * 3] = value.v[0];
        frame->m_values[i * 3 + 1] = value.v[1];
        frame->m_values[i * 3 + 2] = value.v[2];
    }
    for (g = 0; g < ref->groups[0]; g++) {
        unsigned int j = ((unsigned char*)ref->groups)[g * 2 + 2];
        unsigned int end = j + ((unsigned char*)ref->groups)[g * 2 + 3];


        for (; j < end; j++) {
            delta = ((SkelShorts_t*)data)[n];
            n++;
            delta.EndianSwap();
            frame->m_values[j * 3] += delta.v[0];
            frame->m_values[j * 3 + 1] += delta.v[1];
            frame->m_values[j * 3 + 2] += delta.v[2];
        }
    }
    if (noangle) {
        weight->m_angle = angle.value << 8;
        frame->m_angle = 0;
    } else {
        frame->m_angle = -angle.value << 8;
    }
    frame->m_rotation[0] = rotation.v[0] / 4096.0f;
    frame->m_rotation[1] = rotation.v[1] / 4096.0f;
    frame->m_rotation[2] = rotation.v[2] / 4096.0f;
    {
        int sign = mirrored ? -1 : 1;
        frame->m_rotation[0] = sign * frame->m_rotation[0];
    }
    frame->m_bend[0] = bend.v[0] << 8;
    frame->m_bend[1] = (bend.v[1] * (mirrored ? -1 : 1)) << 8;
    frame->m_bend[2] = (bend.v[2] * (mirrored ? -1 : 1)) << 8;
}

void _unpack_skel_frame_ref(CharFrame_t* f, AnimMotWeightFrame_t* w) { _unpack_skel_frame(f, w); }
