// shadow.cpp: its initialised globals (.sdata). All six are file statics in the original,
// defined global here because the file's fragments reach them with extern (as in
// research/data/bss/shadow.cpp). Checked by research/tools/check_data.py.
// settings: config/GMFE69/shadow.json

float g_FadeDistance = 30.0f; // file-local in the original
float g_ShadowAlpha = 0.25f; // file-local in the original
float g_ProjectionDistance = 3.0f; // file-local in the original
float g_ProjectionHeight = 1.5f; // file-local in the original
float g_ShadowScale = 1.5f; // file-local in the original
float g_zDeltaMax = 1.0f; // file-local in the original
