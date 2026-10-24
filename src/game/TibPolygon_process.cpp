// A fragment of TibPolygon.cpp (0x8008bcd4): the polygon plugin of the UI
// studio. TibPolyDraw (skipped for library 12 while the shell's flag at
// +0x19a7 is set and +0x199b is not) transforms the four vertices by the
// current matrix and the screen scale, colours them from the vertex bytes
// through the UI colour multiplier and additive (halved and clamped to 127;
// flat 127 grey for a textured polygon without the flag 1), sets the unit
// texture coordinates and draws a strip (textured through the library's
// sprite when the polygon has one). TibPolyInit creates the textured
// polygon's sprite once: render priority by library (11: load type 3,
// priority 11) and, for library 13, by the texture name ("Logo", "HL",
// "pistol", "TXT", "can", "case", "folder", "smoke_"; names containing
// "master" get 11). TibPolyProcessFnc, the polygon plugin's handler: -1
// initialises the polygon, -2 draws it, -3 deletes its library object (and
// clears the library slot); 0 and 1 set a vertex's two coordinate pairs, 2 and
// 3 set a vertex's colour bytes and alpha (vertex -1: all four vertices, from
// the last), 4 sets the value at +6, and 5 sets the polygon's id and library
// (from one packed parameter or from two) and initialises it.
// TibPolyData_t, UISParam_t, _spritepolyvert and CSprite's functions are
// named by the mangled symbols and LibraryInfo, g_Shell, g_screen and
// g_matStack by their symbols; the polygon, vertex, parameter, shell,
// screen and library views, the library object class (its table pointer
// after 28 bytes of members), the class operator new, the name macros, the
// hoisted declarations and the action and priority meanings are inferred.
// The "master" test is made twice, as compiled. The rest of the file is in
// other units.
enum ERenderPriority {};

class CVector3 {
public:
    union {
        double pair[2];
        float v[4];
    } d;
} __attribute__((aligned(8)));

class CMatrix {
public:
    void TransformPoint(CVector3&, CVector3) const;

    float m[4][4];
};

class CMatrixStack {
public:
    CMatrix& Current() { return m_stack[m_index]; }

    unsigned long m_size;
    int m_index;
    CMatrix* m_stack;
};

extern CMatrixStack* g_matStack;

class CScreen {
public:
    unsigned char unknown000[40];
    float m_scaleX;
    float m_scaleY;
    unsigned char unknown030[224];
};

extern CScreen g_screen;

class CShellMenu {
public:
    unsigned char unknown0000[6555];
    unsigned char m_value199b;
    unsigned char unknown199c[11];
    unsigned char m_value19a7;
};

extern CShellMenu g_Shell;

void* DWI_alloc(const char*, int, int);

struct _spritepolyvert {
    float u;
    float v;
    float x;
    float y;
    float z;
    unsigned char color[4];
};

class CSprite {
public:
    enum ESpriteLoadType {};
    enum EPolyPrimType {};
    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }
    CSprite(const char*, ESpriteLoadType, ERenderPriority);
    void DrawPoly(_spritepolyvert*, int, EPolyPrimType);
    static void DrawPolyNoTex(_spritepolyvert*, int, EPolyPrimType);

    unsigned char unknown00[100];
};

extern "C" {
float* UISGetColorMultipler(void);
float* UISGetColorAdditive(void);
char* strstr(const char*, const char*);
}

struct UISParam_t {
    long value;
};

struct TibVertex_t {
    float x0;
    float y0;
    float x1;
    float y1;
    float z;
    unsigned char r;
    unsigned char b;
    unsigned char g;
    unsigned char a;
};

struct TibPolyData_t {
    short id;
    short library;
    short flags;
    short value06;
    TibVertex_t vertices[4];
};

class TibLibraryObject {
public:
    unsigned char unknown00[28];

    virtual ~TibLibraryObject();
};

struct LibraryInfoView {
    unsigned char unknown00[4];
    const char** names;
    long* objects;
};

extern LibraryInfoView* LibraryInfo;



static inline unsigned char Channel(unsigned char c, float mul, float add) {
    unsigned char value = (int)(0.5f * mul * ((float)c + add));
    if (value > 127)
        value = 127;
    return value;
}

void TibPolyDraw(TibPolyData_t* poly) {
    _spritepolyvert verts[4];
    if (g_Shell.m_value19a7 && !g_Shell.m_value199b && poly->library == 12)
        return;
    float* add = UISGetColorAdditive();
    float* mul = UISGetColorMultipler();
    _spritepolyvert* out = verts;
    for (int i = 0; i < 4; i++) {
        CVector3 screen;
        CVector3 point;
        point.d.v[0] = poly->vertices[i].x1;
        point.d.v[1] = poly->vertices[i].y1;
        point.d.v[2] = poly->vertices[i].z;
        g_matStack->Current().TransformPoint(screen, point);
        out->x = screen.d.v[0] * g_screen.m_scaleX;
        out->y = screen.d.v[1] * g_screen.m_scaleY;
        out->z = screen.d.v[2];
        if ((poly->flags & 1) || !(poly->flags & 2)) {
            out->color[0] = (int)(0.5f * (mul[0] * ((float)poly->vertices[i].r + add[0])));
            if (out->color[0] > 127)
                out->color[0] = 127;
            out->color[1] = (int)(0.5f * (mul[1] * ((float)poly->vertices[i].b + add[1])));
            if (out->color[1] > 127)
                out->color[1] = 127;
            out->color[2] = (int)(0.5f * (mul[2] * ((float)poly->vertices[i].g + add[2])));
            if (out->color[2] > 127)
                out->color[2] = 127;
            out->color[3] = (int)(0.5f * (mul[3] * ((float)poly->vertices[i].a + add[3])));
            if (out->color[3] > 127)
                out->color[3] = 127;
        } else {
            out->color[0] = 127;
            out->color[1] = 127;
            out->color[2] = 127;
            out->color[3] = 127;
        }
        out->u = poly->vertices[i].x0;
        out->v = poly->vertices[i].y0;
        out++;
    }
    verts[0].u = 0.0f;
    verts[1].u = 1.0f;
    verts[2].u = 1.0f;
    verts[3].u = 0.0f;
    verts[0].v = 0.0f;
    verts[1].v = 0.0f;
    verts[2].v = 1.0f;
    verts[3].v = 1.0f;
    if (poly->flags & 2) {
        if (poly->library >= 0)
            ((CSprite*)LibraryInfo[poly->library].objects[poly->id])->DrawPoly(verts, 4, (CSprite::EPolyPrimType)5);
    } else {
        CSprite::DrawPolyNoTex(verts, 4, (CSprite::EPolyPrimType)5);
    }
}

#define NAME LibraryInfo[poly->library].names[poly->id]
#define SPRITE(type, priority) new CSprite(NAME, (CSprite::ESpriteLoadType)type, (ERenderPriority)priority)

void TibPolyInit(TibPolyData_t* poly) {
    if ((poly->flags & 2) && poly->library >= 0 && !LibraryInfo[poly->library].objects[poly->id]) {
        CSprite* sprite;
        if (poly->library == 11)
            sprite = SPRITE(3, 11);
        else if (strstr(NAME, "master") || strstr(NAME, "master"))
            sprite = SPRITE(2, 11);
        else if (poly->library == 12)
            sprite = SPRITE(2, 92);
        else if (poly->library == 13) {
            if (strstr(NAME, "Logo"))
                sprite = SPRITE(2, 82);
            else if (strstr(NAME, "HL"))
                sprite = SPRITE(2, 72);
            else if (strstr(NAME, "pistol"))
                sprite = SPRITE(2, 72);
            else if (strstr(NAME, "TXT"))
                sprite = SPRITE(2, 62);
            else if (strstr(NAME, "can"))
                sprite = SPRITE(2, 52);
            else if (strstr(NAME, "case"))
                sprite = SPRITE(2, 42);
            else if (strstr(NAME, "folder"))
                sprite = SPRITE(2, 32);
            else if (strstr(NAME, "smoke_"))
                sprite = SPRITE(2, 37);
            else
                sprite = SPRITE(2, 12);
        } else
            sprite = SPRITE(2, 12);
        LibraryInfo[poly->library].objects[poly->id] = (long)sprite;
    }
}

void TibPolyProcessFnc(void* object, unsigned long action, unsigned long count, UISParam_t* params, UISParam_t*) {
    long alpha;
    long index;
    int i;
    long r;
    long c;
    long a;
    TibPolyData_t* poly;
    poly = (TibPolyData_t*)object;
    switch (action) {
    case (unsigned long)-1:
        TibPolyInit((TibPolyData_t*)object);
        break;
    case (unsigned long)-2:
        TibPolyDraw((TibPolyData_t*)object);
        break;
    case (unsigned long)-3: {
        TibPolyData_t* data = (TibPolyData_t*)object;
        if (data->library >= 0) {
            long library = LibraryInfo[data->library].objects[data->id];
            if (library) {
                delete (TibLibraryObject*)library;
                LibraryInfo[data->library].objects[data->id] = 0;
            }
        }
        break;
    }
    case 0: {
        long index = params[0].value;
        float y = *(float*)&params[2].value;
        float x = *(float*)&params[1].value;
        poly->vertices[index].x1 = x;
        poly->vertices[index].y1 = y;
        break;
    }
    case 1: {
        long index = params[0].value;
        float y = *(float*)&params[2].value;
        float x = *(float*)&params[1].value;
        poly->vertices[index].x0 = x;
        poly->vertices[index].y0 = y;
        break;
    }
    case 2: {
        index = params[0].value;
        a = params[3].value;
        c = params[2].value;
        r = params[1].value;
        if (index == -1) {
            i = 4;
            while (i-- != 0) {
                poly->vertices[i].r = r;
                poly->vertices[i].g = a;
                poly->vertices[i].b = c;
            }
        } else {
            poly->vertices[index].r = r;
            poly->vertices[index].g = a;
            poly->vertices[index].b = c;
        }
        break;
    }
    case 3: {
        index = params[0].value;
        alpha = params[1].value;
        if (index == -1) {
            i = 4;
            while (i-- != 0)
                poly->vertices[i].a = alpha;
        } else {
            poly->vertices[index].a = alpha;
        }
        break;
    }
    case 4:
        poly->value06 = params[0].value;
        break;
    case 5:
        if (count == 1) {
            poly->id = params[0].value;
            poly->library = (unsigned long)params[0].value >> 16;
        } else {
            poly->id = params[1].value;
            poly->library = params[0].value;
        }
        TibPolyInit(poly);
        break;
    }
}
