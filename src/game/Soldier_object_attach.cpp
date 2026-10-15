// A fragment of Soldier_object.cpp (0x800a9f1c): CSoldierObject::
// SetHelmetOffset moves the "head" node's hand matrix by the offset turned
// into the matrix's frame. The class and functions are named by the mangled
// symbols; the members and the skeleton view are inferred (the node search
// is the inline helper of Soldier_object_cleanup.cpp), and the CVector3 view
// copies through a double pair as in player_weapon.cpp. The rest of the file
// is not part of this unit.
extern "C" int stricmp(const char*, const char*);

union CVector3Pair {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& other) { *(CVector3Pair*)this = *(const CVector3Pair*)&other; }
    CVector3& operator+=(const CVector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    void TransformVector(CVector3&, CVector3) const;
    void SetPos(CVector3);
    CVector3 GetPos() const { return pos; }

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
} __attribute__((aligned(16)));

/* inferred: a skeleton node (24 bytes): name, bone, own matrix */
struct SkeletonNodeView {
    char m_name[16];
    int m_bone;
    CMatrix* m_matrix;
};

struct SkeletonView {
    unsigned char unknown00[16];
    unsigned int m_count;
    SkeletonNodeView* m_nodes;

    /* inferred: the index of the node with the given name, or -1 */
    unsigned int Find(const char* name) const {
        for (unsigned int i = 0; i < m_count; i++) {
            if (stricmp(m_nodes[i].m_name, name) == 0)
                return i;
        }
        return (unsigned int)-1;
    }
};

class CSoldierObject {
public:
    void SetHelmetOffset(CVector3&);

    unsigned char unknown0000[3300];
    SkeletonView* m_skeleton;
    unsigned char unknown0ce8[13132];
    unsigned char unknown4034[76];
    CMatrix m_handMatrices[8];
};

void CSoldierObject::SetHelmetOffset(CVector3& offset) {
    unsigned int head = m_skeleton->Find("head");
    CMatrix& matrix = m_handMatrices[head];
    CVector3 pos = matrix.GetPos();
    CVector3 move;
    matrix.TransformVector(move, offset);
    pos += move;
    matrix.SetPos(pos);
}
