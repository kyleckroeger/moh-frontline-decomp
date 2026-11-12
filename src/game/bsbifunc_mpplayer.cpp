// A fragment of bsbifunc.cpp (0x8001eae4): multiplayer built-ins that return
// the script object's player's floco velocity and animated-player body (through
// the script object's scene node), popping the built-in's arguments. The file
// name is this project's; the original record is bsbifunc.cpp; DoMMGPointCheck
// before these is 8 instructions off. The functions, classes and globals are
// named by the mangled symbols; ISceneNode is declared with its virtual
// functions in the order of __vt__10ISceneNode, BSGO_Basic in the order of
// __vt__10BSGO_Basic and CAIFilterObject in the order of
// __vt__15CAIFilterObject; the player and built-in record views are inferred.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
struct BSObject;
class CPlayerObject;
struct AIDoodadView;
struct AIDoodadView;

#include "game/ISceneNode.h"


// BSGO_Basic, the object at BSObject+12: 12 bytes of members, then its
// virtual table pointer; the virtual functions it needs, in the order of
// __vt__10BSGO_Basic.
class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObject {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};
enum aifilter_target_mode {};
struct aistatus_match;

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual int ChooseTarget(float);
    virtual void SetTargetMode(aifilter_target_mode);
    virtual void ResetTargetMatchList(aifilter_target_mode);
    virtual void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    int ShouldILeaveMMG(BSObject*);
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterObject* filter;
};
union BSValueView {
    int i;
    float f;
};

class CAnimatedPlayer {
public:
    void GetFlocoParams(float&, long&);
};

class CPlayerObject {
public:
    unsigned char unknown000[1992];
    CAnimatedPlayer* m_animatedPlayer;
};

extern BSObject* g_pBSObject;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

int DoMMGPointCheck(BSObject* checker, int other, int, int, int*, int* result);

void BIFunc_MPGetPlayerVelocity(int** stack, void*) {
    long frame;
    float velocity;
    CPlayerObject* player = g_pBSObject->user->GetSceneNode()->AsPlayerObject();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    player->m_animatedPlayer->GetFlocoParams(velocity, frame);
    **stack = *(int*)&velocity;
}

void BIFunc_MPGetPlayerBody(int** stack, void*) {
    BSValueView value;
    CPlayerObject* player = g_pBSObject->user->GetSceneNode()->AsPlayerObject();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    value.i = (int)player->m_animatedPlayer;
    *(BSValueView*)*stack = value;
}

