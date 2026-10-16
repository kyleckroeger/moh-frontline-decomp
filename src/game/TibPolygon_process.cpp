// A fragment of TibPolygon.cpp (0x8008c630): TibPolyProcessFnc, the polygon
// plugin's handler: -1 initialises the polygon, -2 draws it, -3 deletes its
// library object (and clears the library slot); 0 and 1 set a vertex's two
// coordinate pairs, 2 and 3 set a vertex's colour bytes and alpha (vertex -1:
// all four vertices, from the last), 4 sets the value at +6, and 5 sets the
// polygon's id and library (from one packed parameter or from two) and
// initialises it.
// TibPolyData_t and UISParam_t are named by the mangled symbols and
// LibraryInfo by its symbol; the polygon, vertex, parameter and library views,
// the library object class (its table pointer after 28 bytes of members),
// the hoisted declarations and the action meanings are inferred. The rest of
// the file is in other units.
struct UISParam_t {
    long value;
};

struct TibVertex_t {
    float x0;
    float y0;
    float x1;
    float y1;
    unsigned char unknown10[4];
    unsigned char r;
    unsigned char b;
    unsigned char g;
    unsigned char a;
};

struct TibPolyData_t {
    short id;
    short library;
    unsigned char unknown04[2];
    short value06;
    TibVertex_t vertices[4];
};

class TibLibraryObject {
public:
    unsigned char unknown00[28];

    virtual ~TibLibraryObject();
};

struct LibraryInfoView {
    unsigned char unknown00[8];
    long* objects;
};

extern LibraryInfoView* LibraryInfo;

void TibPolyInit(TibPolyData_t*);
void TibPolyDraw(TibPolyData_t*);

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
