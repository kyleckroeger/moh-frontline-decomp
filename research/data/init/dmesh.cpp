// dmesh.cpp: its initialised data (.data, .sdata): the GX fog type for each of the
// game's fog modes and the two finish-draw switches.
// Checked by research/tools/check_data.py.
// settings: config/GMFE69/compartment_render.json

#include <dolphin/gx.h>

// File static in compartment.cpp, staticmesh.cpp and dmesh.cpp alike.
static GXFogType g_GCFogType[4] = {GX_FOG_NONE, GX_FOG_PERSP_LIN, GX_FOG_PERSP_EXP, GX_FOG_PERSP_EXP2}; // address: g_GCFogType 0x8018b580

bool _finish_draw = true; // file-local in the original
bool _finish_draw_weapon = true; // file-local in the original
