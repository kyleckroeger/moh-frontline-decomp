// lightvolman.cpp: its initialised data (.data): the blend light set, the same four
// lights as compartment.cpp's default set, and the light volume that holds it. Both are
// file statics in the original, defined global here because the file's fragments reach
// them with extern. Checked by research/tools/check_data.py.

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

BPDLight g_BlendLights[4] = {
    {1, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {16.0f, 16.0f, 16.0f}, 1.0f, 0},
    {2, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1.0f, 0},
    {2, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {64.0f, 64.0f, 64.0f}, 1.0f, 0},
    {2, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {128.0f, 128.0f, 128.0f}, 1.0f, 0},
}; // file-local in the original

BPDLightVolume g_BlendLightVolume = {{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0, 0, 4, g_BlendLights, 0, 0}; // file-local in the original
