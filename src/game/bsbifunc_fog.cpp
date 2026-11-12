// A fragment of bsbifunc.cpp (0x800206bc): the weak BSGO_Basic::GetScriptData
// (0), then script built-ins that stop a corpse search (unregistering the given
// registration record from the corpse-search schedule, or the calling node's
// script object when there is none) and restore the default fog (the colour and
// distances of the level's fog resource, type 13, alpha 128) or set it (the
// given colour components times 255.5 as bytes, or the fog resource's colour when
// any is negative, magenta without one; the 0 and 255.5 constants are pooled
// .sdata2 entries). Each reads its
// argument below the script stack top and pops the built-in's arguments. The
// file name is this project's; the original record is bsbifunc.cpp and the
// built-ins around these are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (GetScriptObject at +96)
// and BSGO_Basic in the order of __vt__10BSGO_Basic; the schedule (BSSchedule's
// size is not known), colour (built by an inline constructor), resource and
// fog-resource views and the built-in record view are inferred.
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
class BSObject;

#include "game/ISceneNode.h"



class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
};

struct BSScheduleRegistrationRecord_struct;

class BSSchedule {
public:
    void Unregister(BSScheduleRegistrationRecord_struct*);
    void Unregister(BSObject*);

    unsigned char unknown00[40];
};

extern BSSchedule g_CorpseSearchSchedule;

class CColor {
public:
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

enum TLTResourceID {};
struct LevelFileContentsStruct_;

struct FogResourceView {
    unsigned char unknown00[148];
    int red;
    unsigned char unknown98[4];
    int green;
    unsigned char unknowna0[4];
    int blue;
    unsigned char unknowna8[4];
    float nearDistance;
    unsigned char unknownb0[4];
    float farDistance;
};

struct TLTResourceView {
    unsigned char unknown00[8];
    FogResourceView* data;
};

TLTResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void SetFog(CColor&, float, float);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

__declspec(weak) int BSGO_Basic::GetScriptData() {
    return 0;
}

void BIFunc_StopCorpseSearch(int** stack, void* object) {
    BSScheduleRegistrationRecord_struct* record = *(BSScheduleRegistrationRecord_struct**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_CorpseSearchSchedule.Unregister(record);
    else
        g_CorpseSearchSchedule.Unregister(((ISceneNode*)object)->GetScriptObject());
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetDefaultFogParams(int** stack, void*) {
    TLTResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)13, 0);
    if (resource) {
        CColor color(0, 0, 0, 128);
        FogResourceView* fog = resource->data;
        color.r = fog->red;
        color.g = fog->green;
        color.b = fog->blue;
        SetFog(color, fog->nearDistance, fog->farDistance);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetFogParams(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float nearDistance = *(float*)(*stack - (count - 1));
    float farDistance = *(float*)(*stack - (count - 2));
    float red = *(float*)(*stack - (count - 3));
    float green = *(float*)(*stack - (count - 4));
    float blue = *(float*)(*stack - (count - 5));
    CColor color(0, 0, 0, 128);
    if (red < 0.0f || green < 0.0f || blue < 0.0f) {
        TLTResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)13, 0);
        if (resource) {
            FogResourceView* fog = resource->data;
            color.r = fog->red;
            color.g = fog->green;
            color.b = fog->blue;
        } else {
            color.r = 255;
            color.g = 0;
            color.b = 255;
        }
    } else {
        color.r = red * 255.5f;
        color.g = green * 255.5f;
        color.b = blue * 255.5f;
    }
    SetFog(color, nearDistance, farDistance);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
