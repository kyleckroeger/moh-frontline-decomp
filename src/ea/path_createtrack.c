/* PATH_createtrackfrombuf, PATH_createtrack and PATH_addmapfile (path.c,
   0x8010c384): createtrackfrombuf (unless the player is busy, -18) selects
   the state of the handle's song and, when its track slot is free (else
   -13), builds the track in the caller's buffer: the track record, then the
   node queue (every entry cleared with node -1), then the rest of the buffer
   from the next 16-byte boundary for PATHX_createtrack; the record gets the
   slot, the state's control byte, the global volume scale, the latency (and
   whether it is under 50), three fade records {-1, 0, -1} (the first at full
   volume) and the queue size, the file name and a short label (the
   project's number and the file's base name, lower-cased up to eight
   letters or digits); the service starts with the first track. It returns
   the handle with the slot's bit, or PATHX_createtrack's error after
   freeing the slot. createtrack passes its arguments to
   PATH_createtrackfrombuf with no buffer; addmapfile
   initialises the four player states on first use, then (unless the player
   is busy, -18) takes the first free state for a map whose header carries the
   PFDx magic and version 3.4, gives it an id from the map's project number
   and the count of states already holding that project, selects it, sets up
   the pointers into the map (node index, records, maps and the two tables
   after them) and the state defaults, and gives every node without a repeat
   count a count of 1. It returns the project number, or -1 when the map is
   not taken. The functions are named by their symbols; the map, node,
   state and track layouts, the first-use initialisation helper, the state
   selection, track count, project index and base-name inlines (the state
   selection here storing the active id before the state pointer) and the
   parameter names are inferred. */
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

struct PATHFADEVIEW {
    int volume : 8;
    unsigned int unknown1 : 8;
    int target : 16;
};

struct PATHQUEUEENTRY {
    int unknown0;
    int unknown4;
    int unknown8;
    int node;
};

struct PATHTRACKBUF {
    unsigned char slot;
    unsigned char control;
    signed char paused;
    signed char volume;
    int unknown04;
    signed char unknown08;
    signed char unknown09;
    signed char unknown0a;
    unsigned char unknown0b;
    unsigned int unknown0c : 4;
    signed int direction : 5;
    unsigned int unknown0c_9 : 5;
    unsigned int queuesize : 5;
    unsigned int queueindex : 5;
    unsigned int unknown0c_24 : 8;
    unsigned int latency;
    int early;
    short unknown18;
    short unknown1a;
    int unknown1c;
    PATHFADEVIEW fade20;
    PATHFADEVIEW fade24;
    PATHFADEVIEW fade28;
    int unknown2c;
    int unknown30;
    int unknown34;
    int unknown38;
    int handle;
    int unknown40;
    char name[128];
    char label[32];
    PATHQUEUEENTRY* queue;
};

extern signed char volscale;

extern "C" {
char* strcpy(char*, const char*);
unsigned long strlen(const char*);
int sprintf(char*, const char*, ...);
void PATHX_initservice(void (*)(), void (*)());
int PATHX_createtrack(char*, char*, int, int, int, char*, int, int);
}
void PATHI_servicetask();
void PATHI_servicetimer();


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

static inline int counttracks(unsigned int songs) {
    int i;
    int j;
    int count = 0;
    for (i = 0; i < 4; i++) {
        if ((songs & (0x01000000 << i)) && pfstates[i].active) {
            for (j = 0; j < 24; j++) {
                if (pfstates[i].tracks[j])
                    count++;
            }
        }
    }
    return count;
}

static inline char* basename(char* name) {
    char* p;
    for (p = name + strlen(name); p >= name && *p != '\\' && *p != '/' && *p != ':'; p--)
        ;
    return p;
}

static inline int projectindex(unsigned int id) {
    int i;
    for (i = 0; i < 4; i++) {
        if ((1 << (i + 28)) & id)
            break;
    }
    return i;
}

extern "C" int PATH_createtrackfrombuf(unsigned int handle, int slot, char* name, unsigned int latency, int p5, int nodes, int p7, char* buffer, int size, int p10) {
    char* start = buffer;
    int result;

    if (!inited || pfactive || !selectstate(-1))
        return -18;
    selectid(handle);
    if (pfstate->tracks[slot]) {
        result = -13;
    } else {
        PATHTRACKBUF* track = (PATHTRACKBUF*)buffer;
        PATHFADEVIEW fade;
        char* p;
        char* q;
        int n;
        int i;
        char c;

        buffer += sizeof(PATHTRACKBUF);
        track->queue = (PATHQUEUEENTRY*)buffer;
        buffer += nodes * sizeof(PATHQUEUEENTRY);
        buffer += 16 - (buffer - start) % 16;
        size -= buffer - start;
        for (i = 0; i < nodes; i++) {
            track->queue[i].node = -1;
            track->queue[i].unknown0 = 0;
            track->queue[i].unknown4 = 0;
            track->queue[i].unknown8 = 0;
        }
        fade.volume = -1;
        fade.unknown1 = 0;
        fade.target = -1;
        track->slot = slot;
        track->control = pfstate->value1c;
        track->paused = 0;
        track->volume = volscale;
        track->unknown04 = 0;
        track->unknown0a = -1;
        track->unknown08 = -1;
        track->unknown09 = -1;
        track->unknown1c = 0;
        track->latency = latency & 0xFFFF;
        track->queuesize = (unsigned char)nodes;
        track->fade20 = fade;
        track->fade20.volume = 127;
        track->fade24 = fade;
        track->fade28 = fade;
        track->unknown1a = -1;
        track->early = (int)latency < 50;
        track->direction = track->early ? 0 : -1;
        track->unknown0c_9 = 0;
        track->unknown18 = -1;
        track->unknown0c = 0;
        track->unknown34 = 0;
        track->unknown2c = 0;
        track->unknown30 = 0;
        track->unknown38 = 0;
        track->unknown40 = 0;
        track->queueindex = 0;
        strcpy(track->name, name);
        p = basename(name);
        n = projectindex(handle);
        sprintf(track->label, "%x%s", n + 1, p + 1);
        c = 'a';
        for (q = track->label + 1; q < track->label + 32; q++) {
            bool more = false;
            if (c && *q)
                more = true;
            char ch;
            if (more)
                ch = *q | 0x20;
            else
                ch = 0;
            c = ch;
            if (!(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9')) {
                c = 0;
                if (q >= track->label + 8) {
                    *q = 0;
                    break;
                }
                *q = ' ';
            }
        }
        pfstate->tracks[slot] = track;
        if (counttracks(0x0F000000) == 1)
            PATHX_initservice(PATHI_servicetask, PATHI_servicetimer);
        track->label[8] = 0;
        track->handle = PATHX_createtrack(name, track->label, p5, nodes, p7, buffer, size, p10);
        if (track->handle < 0) {
            pfstate->tracks[slot] = 0;
            result = track->handle;
        } else {
            track->unknown40 = -1;
            result = handle | (1 << slot);
        }
    }
    pfactive = 0;
    pfstate = 0;
    return result;
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
extern "C" int PATH_createtrack(int a, int b, int c, int d, int e, int f, int g, int h, int i) {
    return PATH_createtrackfrombuf(a, b, (char*)c, d, e, f, g, (char*)h, i, 0);
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
