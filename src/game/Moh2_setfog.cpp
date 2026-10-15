// A fragment of Moh2.cpp (0x80017df8): SetFog stores the fog colour (copied byte by byte), its near and far
// distances in g_fog (+64, +68, +72; the view is inferred, its size is the
// symbol's). The classes
// and functions are named by the symbols. The rest of the file is in other
// units.
struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

/* inferred: the fog record's colour and distances */
struct FogView {
    unsigned char unknown00[64];
    CColor m_color;
    float m_near;
    float m_far;
    unsigned char unknown4c[4];
};

extern FogView g_fog;

void SetFog(CColor& color, float fogNear, float fogFar) {
    g_fog.m_color = color;
    g_fog.m_near = fogNear;
    g_fog.m_far = fogFar;
}
