/* PATH_createtrack and PATH_addmapfile (path.c, 0x8010cb8c): createtrack
   passes its arguments to PATH_createtrackfrombuf with no buffer; addmapfile
   initialises the four player states on first use, then (unless the player
   is busy, -18) takes the first free state for a map whose header carries the
   PFDx magic and version 3.4, gives it an id from the map's project number
   and the count of states already holding that project, selects it, sets up
   the pointers into the map (node index, records, maps and the two tables
   after them) and the state defaults, and gives every node without a repeat
   count a count of 1. It returns the project number, or -1 when the map is
   not taken. The functions are named by their symbols; the map, node and
   state layouts, the first-use initialisation helper and the state
   selection inline (here storing the active id before the state pointer)
   are inferred. */
struct PATHMAPVIEW {
    unsigned int magic;
    unsigned char version;
    unsigned char subversion;
    unsigned char unknown06[6];
    unsigned char project;
    unsigned char count0d;
    unsigned char count0e;
    unsigned char count0f;
    unsigned char count10;
    unsigned char index11;
    unsigned short nodes;
};

struct PATHNODEVIEW {
    unsigned char unknown00[5];
    unsigned char branches : 5;
    unsigned char mode : 3;
    unsigned char repeat : 4;
    unsigned char unknown06 : 4;
    unsigned char unknown07[5];
    int branch[1];
};

struct PATHEVENTVIEW {
    unsigned char unknown00[12];
    unsigned int mask;
};

struct PATHSTATEVIEW {
    unsigned char unknown000[12];
    int value0c;
    int value10;
    int value14;
    int value18;
    unsigned char value1c;
    unsigned char unknown01d[3];
    unsigned int id;
    PATHMAPVIEW* active;
    short* nodemap;
    short* nodemapEnd;
    unsigned char* recordmap;
    unsigned char* records;
    int* maps;
    int* table3c;
    int* table40;
    int* table44;
    PATHEVENTVIEW events[16];
    void* tracks[24];
};

extern signed char inited;
extern signed char paused;
extern unsigned int pfactive;
extern PATHSTATEVIEW* pfstate;
extern PATHSTATEVIEW pfstates[4];

extern "C" int PATH_createtrackfrombuf(int, int, int, int, int, int, int, int, int, int);

static inline int selectstate(int index) {
    if (!pfactive)
        pfactive = -1;
    if (index >= 0 && index < 4 && pfstates[index].active) {
        pfactive = pfstates[index].id;
        pfstate = &pfstates[index];
        return pfactive;
    } else if (index != -1) {
        pfactive = 0;
    }
    return pfactive;
}

static inline void selectid(unsigned int id) {
    int i;

    for (i = 0; i < 4; i++) {
        if (pfstates[i].active && id == pfstates[i].id) {
            selectstate(i);
            break;
        }
    }
}

extern "C" int PATH_createtrack(int a, int b, int c, int d, int e, int f, int g, int h, int i) {
    return PATH_createtrackfrombuf(a, b, c, d, e, f, g, h, i, 0);
}

static inline void pfinit() {
    int i;
    int j;
    for (i = 0; i < 4; i++) {
        pfstates[i].active = 0;
        for (j = 0; j < 24; j++)
            pfstates[i].tracks[j] = 0;
    }
    pfstate = 0;
    inited = 1;
    paused = 0;
}
extern "C" int PATH_addmapfile(PATHMAPVIEW* map) {
    int count;
    int j;
    int slot;
    PATHNODEVIEW* node;
    int project;
    int result;
    int cells;
    int i;
    slot = 4;
    result = -1;

    if (!inited && !pfactive)
        pfinit();
    if (pfactive)
        return -18;
    project = map->project;
    count = 0;
    for (i = 0; i < 4; i++) {
        if (!pfstates[i].active) {
            if (slot == 4)
                slot = i;
        } else if (project == pfstates[i].active->project) {
            count++;
        }
    }
    if (slot < 4 && map->magic == 0x50464478 && map->version == 3 && map->subversion == 4) {
        // A no-op self-assignment (user-approved, like '(void)0;'): the
        // compiler drops it, but it gives the original's order for the
        // state's active and id stores.
        pfstates[slot].id = pfstates[slot].id;
        pfstates[slot].active = map;
        pfstates[slot].id = (0x1000000 << project) | (0x10000000 << count);
        selectid(pfstates[slot].id);
        result = project;
        pfstate->nodemap = (short*)((char*)pfstate->active + 36);
        pfstate->nodemapEnd = pfstate->nodemap + pfstate->active->nodes;
        node = (PATHNODEVIEW*)((int*)pfstate->active + pfstate->nodemap[pfstate->active->nodes - 1]);
        cells = pfstate->active->count0f * pfstate->active->count0e * pfstate->active->count0d;
        node = (PATHNODEVIEW*)((int*)node + node->branches);
        pfstate->recordmap = (unsigned char*)node + 12;
        cells = (cells + 3) & ~3;
        pfstate->records = pfstate->recordmap + cells;
        pfstate->maps = (int*)(pfstate->records + pfstate->active->count10 * 4);
        pfstate->table3c = (int*)pfstate->active + pfstate->maps[pfstate->active->index11];
        pfstate->table40 = pfstate->table3c + 1 + pfstate->active->count0d;
        pfstate->table44 = (int*)pfstate->active + pfstate->table3c[pfstate->active->count0d];
        pfstate->events[0].mask = 0;
        pfstate->events[1].mask = 0;
        pfstate->value0c = 50;
        pfstate->value10 = 10;
        pfstate->value14 = 1000;
        pfstate->value18 = 1;
        pfstate->value1c = 0;
        for (i = 0; i < pfstate->active->nodes; i++) {
            node = (PATHNODEVIEW*)((int*)pfstate->active + pfstate->nodemap[i]);
            if (!node->repeat)
                node->repeat = 1;
        }
    }
    pfactive = 0;
    pfstate = 0;
    return result;
}
