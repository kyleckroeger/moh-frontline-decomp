// A fragment of animskinned.cpp (0x80066a08): CAnimSkinned's
// UpdateSkeletonMatrices traverses the skeleton (+3252) for the frame
// (+3280) under a unit matrix pushed on g_matStack, then widens the bounds
// it reports (when asked for) by 0.1 on each side. The classes, functions and
// globals are named by the symbols; the members and the bounds' meaning are
// inferred (the frame is embedded at +3280; m_frame only marks its start, its
// size unknown). The rest of the file is in other units.
class CMatrix;

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
};

struct CharFrame_t;
struct CharSkel_t;

void DMClusterSkelTraverse(CharFrame_t*, CharSkel_t*, CMatrix*, CVector3*, CVector3*);

class CMatrixStack {
public:
    void PushUnit();
    void Pop();
};

extern CMatrixStack* g_matStack;

class CAnimSkinned {
public:
    void UpdateSkeletonMatrices(CMatrix*, CVector3*, CVector3*);

    unsigned char unknown000[3252];
    CharSkel_t* m_skeleton;
    unsigned char unknown0cb8[24];
    unsigned char m_frame[4];
};

void CAnimSkinned::UpdateSkeletonMatrices(CMatrix* matrices, CVector3* min, CVector3* max) {
    g_matStack->PushUnit();
    DMClusterSkelTraverse((CharFrame_t*)m_frame, m_skeleton, matrices, min, max);
    g_matStack->Pop();
    if (min) {
        min->x -= 0.1f;
        min->y -= 0.1f;
        min->z -= 0.1f;
    }
    if (max) {
        max->x += 0.1f;
        max->y += 0.1f;
        max->z += 0.1f;
    }
}
