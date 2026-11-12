// compartment.cpp: its initialised data (.data): the GX fog type for each of the game's
// fog modes, and the default light set (an ambient light and three directional lights)
// with the light volume that holds it, used where a compartment has no light volume of
// its own. Checked by research/tools/check_data.py.
// settings: config/GMFE69/compartment_render.json

#include <dolphin/gx.h>

// Views from src/game/propdat_core.cpp (the property-data light records). field28 holds a
// float (1.0) in every record here; light_dtor.cpp reads it as the light's intensity.
struct PropVector3 {
    float x, y, z;
};

struct PropPlane4;

struct BPDLight {
    int field00; // 1 ambient, 2 directional (inferred from the values)
    PropVector3 field04; // position
    PropVector3 field10; // direction (light_dtor.cpp: m_direction)
    PropVector3 field1c; // colour (light_dtor.cpp: m_color)
    float field28; // intensity (light_dtor.cpp: m_intensity); propdat_core.cpp types it unsigned long
    int field2c;
};

struct BPDLightVolume {
    PropVector3 field00;
    float field0c;
    float field10;
    float field14;
    int planeCount;
    PropPlane4* planes;
    int lightCount;
    BPDLight* lights;
    int field28;
    int field2c;
};

// File static in compartment.cpp, staticmesh.cpp and dmesh.cpp alike.
static GXFogType g_GCFogType[4] = {GX_FOG_NONE, GX_FOG_PERSP_LIN, GX_FOG_PERSP_EXP, GX_FOG_PERSP_EXP2}; // address: g_GCFogType 0x80183a60

BPDLight g_DefaultLights[4] = {
    {1, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {16.0f, 16.0f, 16.0f}, 1.0f, 0},
    {2, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1.0f, 0},
    {2, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {64.0f, 64.0f, 64.0f}, 1.0f, 0},
    {2, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {128.0f, 128.0f, 128.0f}, 1.0f, 0},
};

BPDLightVolume g_DefaultLightVolume = {{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0, 0, 4, g_DefaultLights, 0, 0};
