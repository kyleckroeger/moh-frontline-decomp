// A fragment of bsbifunc.cpp (0x80024054): script built-ins that set the
// motion type of the calling node's soldier's jaw controller, set the
// soldier's blink parameters from a state argument (five floats chosen by a
// switch over the state, then a float before them cleared) and set its facial
// expression (an expression id and a float time: replacing a pending
// expression, starting the first one, or queueing a different one, with rates
// of a quarter of the times), returning -1. Each reads its arguments below the
// script stack top and pops the built-in's arguments. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around these
// are not reconstructed. The functions, classes and globals are named by the
// mangled symbols; ISceneNode is declared with its virtual functions in the
// order of __vt__10ISceneNode (AsSoldierObject at +148); the soldier view (the
// expression fields from +17108, the blink fields from +17140, the jaw
// controller at +17472; the fields' meanings are not known), the result union
// and the built-in record view are inferred.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CSoldierObject;
struct AIDoodadView;

#include "game/ISceneNode.h"



enum EJawType {};

class CJawController {
public:
    void SetMotionType(EJawType);
};

class CSoldierObject {
public:
    unsigned char unknown0000[17108];
    int m_unknown42d4;
    int m_unknown42d8;
    float m_unknown42dc;
    float m_unknown42e0;
    float m_unknown42e4;
    float m_unknown42e8;
    float m_unknown42ec;
    float m_unknown42f0;
    float m_unknown42f4;
    float m_unknown42f8;
    float m_unknown42fc;
    float m_unknown4300;
    float m_unknown4304;
    float m_unknown4308;
    unsigned char unknown430c[308];
    CJawController m_jaw;
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

union BSValueView {
    int i;
    float f;
};

void BIFunc_SetJawMotion(int** stack, void* object) {
    EJawType type = (EJawType)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    ((ISceneNode*)object)->AsSoldierObject()->m_jaw.SetMotionType(type);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetBlinkState(int** stack, void* object) {
    int state = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CSoldierObject* soldier = ((ISceneNode*)object)->AsSoldierObject();
    switch (state) {
    case 0:
        soldier->m_unknown42f8 = 0.0f;
        soldier->m_unknown42fc = 0.0f;
        soldier->m_unknown4300 = 0.0f;
        soldier->m_unknown4308 = 0.0f;
        soldier->m_unknown4304 = 86400.0f;
        break;
    case 1:
        soldier->m_unknown42f8 = 2.0f;
        soldier->m_unknown42fc = 4.0f;
        soldier->m_unknown4300 = 0.1f;
        soldier->m_unknown4308 = 0.1f;
        soldier->m_unknown4304 = 0.1f;
        break;
    case 6:
        soldier->m_unknown42f8 = 2.0f;
        soldier->m_unknown42fc = 4.0f;
        soldier->m_unknown4300 = 1.5f;
        soldier->m_unknown4308 = 0.5f;
        soldier->m_unknown4304 = 0.5f;
        break;
    case 2:
    case 3:
        soldier->m_unknown42f8 = 1.0f;
        soldier->m_unknown42fc = 2.0f;
        soldier->m_unknown4300 = 0.017f;
        soldier->m_unknown4308 = 0.017f;
        soldier->m_unknown4304 = 0.017f;
        break;
    case 4:
        soldier->m_unknown42f8 = 0.5f;
        soldier->m_unknown42fc = 2.0f;
        soldier->m_unknown4300 = 0.017f;
        soldier->m_unknown4308 = 0.017f;
        soldier->m_unknown4304 = 0.017f;
        break;
    case 5:
        soldier->m_unknown42f8 = 2.0f;
        soldier->m_unknown42fc = 6.0f;
        soldier->m_unknown4300 = 0.017f;
        soldier->m_unknown4308 = 0.017f;
        soldier->m_unknown4304 = 0.017f;
        break;
    case 8:
        soldier->m_unknown42f8 = 0.0f;
        soldier->m_unknown42fc = 0.0f;
        soldier->m_unknown4300 = 0.0f;
        soldier->m_unknown4308 = 0.0f;
        soldier->m_unknown4304 = 86400.0f;
        break;
    case 7:
        soldier->m_unknown42f8 = 0.0f;
        soldier->m_unknown42fc = 0.0f;
        soldier->m_unknown4300 = 86400.0f;
        soldier->m_unknown4308 = 0.0f;
        soldier->m_unknown4304 = 0.5f;
        break;
    default:
        soldier->m_unknown42f8 = 2.0f;
        soldier->m_unknown42fc = 5.0f;
        soldier->m_unknown4300 = 0.017f;
        soldier->m_unknown4308 = 0.017f;
        soldier->m_unknown4304 = 0.017f;
        break;
    }
    soldier->m_unknown42f4 = 0.0f;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetExpression(int** stack, void* object) {
    BSValueView value;
    int expression = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    float time = *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 2));
    value.i = -1;
    CSoldierObject* soldier = ((ISceneNode*)object)->AsSoldierObject();
    if (soldier->m_unknown42d8 >= 0) {
        soldier->m_unknown42e4 = time;
        soldier->m_unknown42ec = 0.0f;
        soldier->m_unknown42dc = time;
        soldier->m_unknown42d4 = expression;
        soldier->m_unknown42e8 = 0.0f;
        soldier->m_unknown42f0 = 0.0f;
        soldier->m_unknown42e0 = 0.0f;
        soldier->m_unknown42d8 = -1;
    } else if (soldier->m_unknown42d4 == -1) {
        soldier->m_unknown42e4 = time;
        soldier->m_unknown42ec = 0.25f * soldier->m_unknown42e4;
        soldier->m_unknown42dc = 0.0f;
        soldier->m_unknown42d4 = expression;
    } else if (soldier->m_unknown42d4 != expression) {
        soldier->m_unknown42ec = 0.25f * -soldier->m_unknown42dc;
        soldier->m_unknown42e8 = time;
        soldier->m_unknown42f0 = 0.25f * soldier->m_unknown42e8;
        soldier->m_unknown42e0 = 0.0f;
        soldier->m_unknown42d8 = expression;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}
