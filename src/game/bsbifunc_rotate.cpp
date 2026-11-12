// A fragment of bsbifunc.cpp (0x8001fde0): the script built-in that applies
// damage when a hit's location (+16) matches the given location: for a
// hierarchical object the hit's damage (+4) is first scaled by
// CHierObject::GetBulletDamageMultiplierByClass for the location and a class
// chosen by a switch over the hit's body class (+12); the given amount less
// the damage, at least 0, is returned (unchanged when the location differs).
// Then the built-in that constrains a rotation: given a rotation, a current
// angle and a minimum and maximum, it returns the rotation reduced so that the
// current angle plus it stays within the range (through an integer/float
// union). Each reads its arguments below the script stack top and pops the
// built-in's arguments. The file name is this project's; the original record
// is bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols;
// ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (AsHierObject at +124); the hit view and its fields'
// meanings, the class table, and the value union and built-in record views
// are inferred.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CSoldierObject;
class CHierObject;
struct AIDoodadView;

#include "game/ISceneNode.h"


class CHierObject {
public:
    float GetBulletDamageMultiplierByClass(int, int);
};

struct HitInfoView {
    unsigned char unknown00[4];
    int damage;
    unsigned char unknown08[4];
    int bodyClass;
    int location;
};

union BSValueView {
    int i;
    float f;
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

void BIFunc_DoDamageIfLocationMatches(int** stack, void* object) {
    BSValueView value;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    HitInfoView* hit = *(HitInfoView**)(*stack - (count - 1));
    int location = *(*stack - (count - 2));
    value.i = *(*stack - (count - 3));
    if (hit->location == location) {
        CHierObject* hier = ((ISceneNode*)object)->AsHierObject();
        if (hier) {
            int cls;
            switch (hit->bodyClass) {
            case 26:
            case 27:
                cls = 0;
                break;
            case 10:
            case 11:
            case 17:
            case 18:
            case 25:
                cls = 1;
                break;
            case 1:
            case 4:
            case 6:
            case 7:
            case 8:
            case 9:
                cls = 2;
                break;
            case 0:
            case 14:
            case 15:
            case 19:
            case 20:
                cls = 3;
                break;
            case 12:
                cls = 4;
                break;
            case 5:
            case 13:
            case 16:
                cls = 5;
                break;
            case 2:
                cls = 6;
                break;
            case 3:
            case 21:
            case 22:
            case 23:
            case 24:
                cls = 7;
                break;
            default:
                cls = 0;
                break;
            }
            hit->damage = (float)hit->damage * hier->GetBulletDamageMultiplierByClass(location, cls);
        }
        value.i -= hit->damage;
        if (value.i < 0)
            value.i = 0;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_GetConstrainedRotateAngle(int** stack, void*) {
    BSValueView value;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    value.f = *(float*)(*stack - (count - 1));
    float current = *(float*)(*stack - (count - 2));
    float minimum = *(float*)(*stack - (count - 3));
    float maximum = *(float*)(*stack - (count - 4));
    if (current + value.f > maximum)
        value.f = maximum - current;
    else if (current + value.f < minimum)
        value.f = minimum - current;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}
