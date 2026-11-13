// A fragment of bsbifunc.cpp (0x8003357c): AI built-ins that start a
// cover-point walk to the stored point (returning the result), continue an A*
// path walk (walk state 2) and stop one (clearing the path, moving the target
// back to the stored position, read z, y and x, and recomputing the squared
// distance to it, then cleaning the walk up). Each reaches the script object's
// AI object through the scene node's AI doodad and its filter, and pops the
// built-in's arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (GetAIDoodad at +204), and the doodad, filter and CAIObject views are
// inferred (members at their offsets, names not original).
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CAIDoodad;
class CAIDoodad;

#include "game/ISceneNode.h"


class CAIFilterRealPosition {
public:
    CAIFilterRealPosition(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIObject {
public:
    void CommitCoverPointWalkToStored();
    bool StartCoverPointWalkToStored(bool);
    bool StartAStarPathWalk();
    bool StartAStarPathWalk(CAIObject*);
    void CleanupAStarPathWalk();

    unsigned char unknown000[24];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[44];
    CAIFilterRealPosition m_moveTarget;
    unsigned char unknown05c[104];
    CAIObject* m_target;
    unsigned char unknown0c8[336];
    int m_walkState;
    unsigned char unknown21c[4];
    float m_moveTargetDistance;
    bool m_flag224;
    bool m_flag225;
    unsigned char unknown226[62];
    int m_path264;
    int m_path268;
    int m_path26c;
    unsigned char unknown270[80];
    bool m_coverFlag;
    unsigned char unknown2c1[7];
    float m_storedX;
    float m_storedY;
    float m_storedZ;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObject* object;
};

class CAIDoodad {
public:
    void* unknown00;
    CAIFilterView* filter;
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline CAIObject* GetAIObject(void* object) {
    return ((ISceneNode*)object)->GetAIDoodad()->filter->object;
}

void BIFunc_AIStopCoverPointWalk(int** stack, void* object);

void BIFunc_AICommitCoverPointWalkToStored(int** stack, void* object);

inline int BSTopBool(int** stack) {
    return **stack != 0;
}

inline void BSRet(int** s, int v) {
    *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **s = v;
}
inline void BSRetB(int** s, bool v) {
    *s -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **s = v;
}
inline bool BSArgB(int** s) { return **s; }
void BIFunc_AIStartCoverPointWalkToStored(int** stack, void* object) {
    bool flag = **stack;
    bool result = GetAIObject(object)->StartCoverPointWalkToStored(flag);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}

void BIFunc_AIStartCoverPointWalk(int** stack, void* object) {
    bool result = GetAIObject(object)->StartCoverPointWalkToStored(true);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = result;
}

void BIFunc_AIContinueAStarPathWalk(int** stack, void* object) {
    GetAIObject(object)->m_walkState = 2;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIStopAStarPathWalk(int** stack, void* object) {
    CAIObject* ai = GetAIObject(object);
    ai->m_path264 = 0;
    ai->m_path268 = 0;
    ai->m_path26c = 0;
    float z = ai->m_storedZ;
    float y = ai->m_storedY;
    float x = ai->m_storedX;
    ai->m_moveTarget.x = x;
    ai->m_moveTarget.y = y;
    ai->m_moveTarget.z = z;
    ai->m_moveTargetDistance = ai->m_position.GetDistanceSquaredXYZReal(ai->m_moveTarget);
    ai->m_flag224 = false;
    ai->m_flag225 = false;
    ai->m_walkState = 0;
    ai->CleanupAStarPathWalk();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIStartAStarPathWalkToMovePoint(int** stack, void* object);

void BIFunc_AIStartAStarPathWalkWithCurrentTarget(int** stack, void* object);

