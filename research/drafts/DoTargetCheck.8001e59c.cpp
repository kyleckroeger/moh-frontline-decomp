// A fragment of bsbifunc.cpp (0x8001e7b8): the scheduled target-search check:
// by mode (2 field of view, 3 line of sight) the script object's AI checks its
// match list; on a hit it stores the target's script object and a random 1 to 9
// times the trigger's search scale in the two outputs and returns 1, otherwise
// 0. The file name is this project's; the original record is bsbifunc.cpp and
// the functions around it are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (GetAIDoodad at +204)
// and BSGO_Basic in the order of __vt__10BSGO_Basic; BSObject, the AI object,
// filter, doodad, owner and trigger views and the script helper are inferred
// (members at their offsets, names not original).
class CCollision;
class CDrawContext;
class CMatrix;


#include "game/CVector3.h"

class CBullet;
class CLight;
class CPlayerObject;
class CAIDoodad;

#include "game/ISceneNode.h"



class BSObject;

struct AIOwnerView {
    unsigned char unknown00[12];
    BSObject* script;
};

class CAIObject {
public:
    CAIObject* CheckFieldOfViewCorpse();
    CAIObject* CheckFieldOfViewModeObject();
    CAIObject* CheckLineOfSightModeObject();
    CAIObject* CheckVisionSeekingMode();
    CAIObject* CheckFieldOfViewModeList();
    CAIObject* CheckLineOfSightModeList();

    unsigned char unknown00[8];
    AIOwnerView* m_owner;
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

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct TriggerCoreView {
    unsigned char unknown00[121];
    unsigned char searchScale;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

class BSObject {
public:
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
    BSGO_Basic* user;
};

inline BSObject* GetScript(CAIObject* ai) {
    if (ai)
        return ai->m_owner->script;
    return 0;
}

int DoTargetCheck(BSObject* object, int mode, int distance, int target, int*, int* extra) {
    int result = 1;
    ISceneNode* node = object->user->GetSceneNode();
    CAIFilterView* filter = node->GetAIDoodad()->filter;
    switch (mode) {
    case 1: {
        ISceneNode* other = ((BSObject*)target)->user->GetSceneNode();
        if (other) {
            CVector3 otherPosition;
            other->GetPosition(otherPosition);
            CVector3 copy(otherPosition);
            CVector3 position;
            node->GetPosition(position);
            float dx = position.x - otherPosition.x;
            float dy = position.y - otherPosition.y;
            result = dx * dx + dy * dy > distance * distance;
        }
        break;
    }
    case 2:
        if (GetScript(filter->object->CheckFieldOfViewModeObject()))
            result = 0;
        break;
    case 3:
        if (GetScript(filter->object->CheckLineOfSightModeObject()))
            result = 0;
        break;
    case 4:
        result = GetScript(filter->object->CheckVisionSeekingMode()) != 0;
        break;
    case 5:
        if (!GetScript(filter->object->CheckLineOfSightModeObject()))
            result = 0;
        break;
    }
    *extra = 0;
    return result;
}

long long MathFunRandomI64(long long, long long);

int DoTargetSearch(BSObject* object, int mode, int, int, int* found, int* extra) {
    BSObject* script = 0;
    CAIFilterView* filter = object->user->GetSceneNode()->GetAIDoodad()->filter;
    switch (mode) {
    case 2:
        script = GetScript(filter->object->CheckFieldOfViewModeList());
        break;
    case 3:
        script = GetScript(filter->object->CheckLineOfSightModeList());
        break;
    }
    if (!script)
        return 0;
    *extra = MathFunRandomI64(1, 9) * object->trigger->core->searchScale;
    *found = (int)script;
    return 1;
}
