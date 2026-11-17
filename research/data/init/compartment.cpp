// compartment.cpp: its initialised data (.data): the GX fog type for each of the game's
// fog modes. (The default light set and its volume, once here, belong to light.cpp's .data
// and are defined by src/game/light.cpp since 2026-11-16.) Checked by
// research/tools/check_data.py.
// settings: config/GMFE69/compartment_render.json

#include <dolphin/gx.h>

// File static in compartment.cpp, staticmesh.cpp and dmesh.cpp alike.
static GXFogType g_GCFogType[4] = {GX_FOG_NONE, GX_FOG_PERSP_LIN, GX_FOG_PERSP_EXP, GX_FOG_PERSP_EXP2}; // address: g_GCFogType 0x80183a60
