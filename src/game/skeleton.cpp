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

#include "game/CMatrix.h"


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
