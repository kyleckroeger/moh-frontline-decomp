/* saems.c: the AEMS (audio event module system) runtime, the whole file in one unit, compiled
   as C++ ('-lang c++', the symbols are mangled), '-inline auto', in image order: the system's
   install and removal, module bank resolution and removal, events, globals, parameters and
   triggers; the 100 Hz timer and the module updaters (counter to random); the updaters from
   the multiplexer to the merge; instance creation and destruction and the module ordering.
   MODULEBANK, MODULE, MODULEINSTANCE, AEMSCOMPDYNAMIC and AEMSCOMPDYNAMICPLAYER are named by
   the mangled symbols; their members and every other view are inferred. Where two parts of the
   file read the same record differently the views stay separate (the system's component and
   module records, the timer's module view) or share one struct with anonymous unions
   (AEMSCOMPDYNAMIC's mode/connected byte and state words, the link's ends, the state's hooks).
   SNDAEMSI_destroyinstance returns int: it ends with li r3,0 (its callers ignore the value). */
struct AEMSCOMPDYNAMIC;
struct AEMSCOMPDYNAMICPLAYER;
struct MODULE;
struct MODULEINSTANCE;

/* inferred: a component kind's update function, definition and component sizes, and output
   mode */
struct AEMSCOMPINFOVIEW {
    void (*update)(AEMSCOMPDYNAMIC*, MODULE*);
    unsigned char definitionSize;
    unsigned char componentSize;
    unsigned short outputMode;
};

extern AEMSCOMPINFOVIEW aemscompinfo[];


/* inferred: a parameter or trigger reference, by ids as loaded, then
   resolved to the bank id and module and component indices */
struct AEMSPARAMREFVIEW {
    int min;
    int max;
    unsigned short bank;
    unsigned short module;
    unsigned int component;
    unsigned short resolvedBank;
    unsigned char unknown12[2];
    unsigned short resolvedModule;
    unsigned short resolvedComponent;
};

struct AEMSTRIGREFVIEW {
    unsigned short bank;
    unsigned short module;
    unsigned int component;
    unsigned short resolvedBank;
    unsigned char unknown0a[2];
    unsigned short resolvedModule;
    unsigned short resolvedComponent;
};

/* inferred: a component definition's id, kind, inputs (8-byte links) and
   kind-specific offsets */
struct AEMSLINKIDVIEW {
    int from;
    int to;
};

struct AEMSCOMPRECVIEW {
    unsigned int id;
    unsigned char kind;
    unsigned char unknown05;
    unsigned char inputs;
    unsigned char unknown07;
    AEMSLINKIDVIEW* links;
    int* data0;
    void* data1;
    void* data2;
};

/* inferred: a module's runtime lists (instance slots, live count, component
   data offsets) */
struct AEMSMODULELISTVIEW {
    MODULEINSTANCE** instances;
    short live;
    int* offsets;
};

/* inferred: a module record (id, slots, component count, trigger and
   definitions) */
struct AEMSMODULERECVIEW {
    unsigned short id;
    short slots;
    short components;
    unsigned char unknown06[2];
    int trigger;
    char* definitions;
    AEMSMODULELISTVIEW* list;
};

/* inferred: a loaded bank's name and user value */
struct AEMSBANKINFOVIEW {
    int soundBank;
    int soundBank2;
    char* name;
    int user;
};

/* inferred: a global link (cleared, then an offset made a pointer) */
struct AEMSGLOBALLINKVIEW {
    int value;
    unsigned char unknown04[4];
    int target;
};

/* MODULEBANK is named by the mangled symbols; its members are inferred */
struct MODULEBANK {
    unsigned char unknown00[2];
    unsigned char version;
    unsigned char unknown03[9];
    int moduleCount;
    char* modules;
    unsigned short id;
    unsigned char unknown16[2];
    short globalLinkCount;
    unsigned char unknown1a[18];
    AEMSGLOBALLINKVIEW* globalLinks;
    AEMSBANKINFOVIEW* info;
    void* extra;
};

/* inferred: a reference list (offset, then count) */
struct AEMSREFLISTVIEW {
    int* refs;
    short count;
};

/* inferred: a trigger record (parameter lists, then three trigger lists) */
struct AEMSTRIGGERVIEW {
    short paramCount;
    unsigned char unknown02[2];
    AEMSREFLISTVIEW* params;
    AEMSREFLISTVIEW lists[3];
};

/* inferred: a loaded AEMS system (counts, then tables stored as offsets) */
struct AEMSSYSTEMVIEW {
    unsigned char unknown00[8];
    short paramCount;
    short refCount;
    short triggerCount;
    short globalCount;
    AEMSPARAMREFVIEW* params;
    AEMSTRIGREFVIEW* refs;
    AEMSTRIGGERVIEW* triggers;
    AEMSREFLISTVIEW* globals;
};

struct SNDGSVIEW {
    unsigned char unknown000[440];
    void (*aemsRestore)();
};

extern SNDGSVIEW sndgs;

/* inferred: the AEMS state (rate, system, banks, global state, the player, bank and stream
   hooks, and the event handlers) */
struct SNDAEMSVIEW {
    int unknown00;
    int rate;
    AEMSSYSTEMVIEW* system;
    MODULEBANK** banks;
    unsigned char unknown10[4];
    int* globalState;
    void (*playerUpdate)(AEMSCOMPDYNAMIC*);
    void (*playerStop)(AEMSCOMPDYNAMICPLAYER*);
    void (*bankUpdate)(AEMSCOMPDYNAMIC*);
    union {
        void (*streamUpdate)(AEMSCOMPDYNAMIC*);
        void (*streamStop)(AEMSCOMPDYNAMICPLAYER*);
    };
    unsigned char unknown28[3];
    unsigned char handlerCount;
    void (*handlers[6])(int, int*);
};

extern SNDAEMSVIEW sndaems;

extern void (*SNDAEMS_removesystem)();
extern int (*SNDAEMS_removemodulebank)(int);

extern "C" {
void* memset(void*, int, unsigned long);
unsigned long strlen(const char*);
char* strcpy(char*, const char*);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
void SNDSYS_add100hzclient(void (*)());
void SNDSYS_remove100hzclient(void (*)());
}
void SNDAEMS_linkbank();
void AEMSI_timerupdate();
void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);
int SNDAEMSI_destroyinstance(struct MODULE*, int);
extern "C" void SNDbankremove(int);
int SNDAEMSI_createinstance(MODULEBANK*, struct MODULE*, int);
int SNDAEMSI_stufftrigger(int, int, int);
int SNDAEMSI_stuffparam(int, int, int, int);
int SNDAEMSI_setglobal(int, int);
void SNDAEMSI_updatemodules(int);


/* inferred: a component definition's kind (an index into aemscompinfo) */
struct AEMSCOMPDEFHEADERVIEW {
    unsigned char unknown00[4];
    unsigned char kind;
};

/* inferred: a module instance (its id, active flag and components; the
   component at +24 ends the instance) */
struct AEMSINSTANCEVIEW {
    int id;
    unsigned char active;
    unsigned char unknown05[7];
    int componentCount;
    AEMSCOMPDYNAMIC** components;
    unsigned char unknown14[4];
    AEMSCOMPDYNAMIC* end;
};

struct AEMSINSTANCELISTVIEW {
    AEMSINSTANCEVIEW** instances;
};

/* inferred: a module (instance count, update timer and instances) */
struct AEMSMODULETIMERVIEW {
    unsigned char unknown00[2];
    short instanceCount;
    unsigned char unknown04[2];
    short timer;
    unsigned char unknown08[8];
    AEMSINSTANCELISTVIEW* instances;
};

/* inferred: a loaded module bank (version, module count and records, which
   are 28 bytes from version 8 and 20 bytes before) */
struct AEMSCOMPDEFVIEW {
    unsigned char unknown00[12];
    int param0;
    int param1;
};

struct AEMSTABLEDEFVIEW {
    unsigned char unknown00[12];
    short* table;
    short count;
    short min;
    short max;
};

struct AEMSTABLE2DEFVIEW {
    unsigned char unknown00[12];
    void* table;
    int count;
    int min;
    int max;
    int width;
};

struct AEMSCOUNTERDEFVIEW {
    unsigned char unknown00[12];
    int min;
    int max;
    unsigned char up;
};

struct AEMSOSCDEFVIEW {
    unsigned char unknown00[12];
    short waveform;
    short polarity;
};

struct AEMSPLAYERTABLEVIEW {
    int* sounds;
    short count;
};

struct AEMSPLAYERDEFVIEW {
    unsigned char unknown00[12];
    AEMSPLAYERTABLEVIEW* table;
};

struct AEMSRANGETRIGDEFVIEW {
    unsigned char unknown00[12];
    int onMin;
    int onMax;
    int offMin;
    int offMax;
};

struct AEMSENVPOINTVIEW {
    int duration;
    int level;
};

struct AEMSENVDEFVIEW {
    unsigned char unknown00[12];
    AEMSENVPOINTVIEW* points;
    short count;
    short release;
    int initial;
};

struct AEMSRANDOMDEFVIEW {
    unsigned char unknown00[12];
    int min;
    int max;
    unsigned char* weights;
    unsigned char mode;
};

struct AEMSSTATEGENDEFVIEW {
    unsigned char unknown00[12];
    int count;
    short* values;
};

union AEMSOUTPUTVIEW {
    int value;
    int* values;
};

union AEMSSTATEVIEW {
    int value;
    short* samples;
    unsigned short* counts;
    unsigned short step;
};

/* inferred: a connection copying one component's output (from, source) to another's input
   (to, destination) */
struct AEMSLINKVIEW {
    union {
        int* from;
        int* source;
    };
    union {
        int* to;
        int* destination;
    };
};

struct AEMSCOMPDYNAMIC {
    void* def;
    union {
        signed char mode;
        signed char connected;
    };
    signed char placed;
    unsigned short linkCount;
    AEMSLINKVIEW* links;
    int* in;
    AEMSOUTPUTVIEW out;
    AEMSSTATEVIEW state0;
    union {
        int value;
        short* samples;
        int* values;
        short round;
        unsigned int phase;
        struct {
            short stage;
            short trigger;
        } env;
    } state1;
    union {
        union {
            int value;
            short* samples;
        } state2;
        short* read;
        int target;
    };
    unsigned char unknown20[12];
    unsigned short sound;
    unsigned char playing;
    unsigned char unknown2f[5];
    long long* step;
    long long* current;
};

extern void (*SNDAEMS_beginevent)(int*);

int SNDAEMSI_destroyinstance(MODULE*, int);
void SNDAEMSI_updatemodules(int);
unsigned int iSNDrandom();
int iSNDsin(int);

void SNDAEMSI_updatemodule(int, int, int);


/* AEMSCOMPDYNAMICPLAYER is named by the mangled symbols; its members (the
   player's inputs, voice handle, stream and sound id) are inferred. */
struct AEMSCOMPDYNAMICPLAYER {
    void* def;
    unsigned char unknown04[8];
    int* in;
    int out;
    unsigned char unknown14[8];
    int handle;
    unsigned char unknown20[4];
    int stream;
    unsigned char unknown28[4];
    unsigned short sound;
    unsigned char playing;
};

/* inferred: a module's instance slots and the number in use */
struct MODULEINSTANCELISTVIEW {
    MODULEINSTANCE** instances;
    short live;
    int* offsets;
};

/* MODULE and MODULEINSTANCE are named by the mangled symbols; their
   members (instance slots, component count, packed component definitions;
   an instance's id, active flag and component data) are inferred. */
struct MODULE {
    unsigned char unknown00[2];
    short slots;
    short components;
    unsigned char unknown06[6];
    char* definitions;
    MODULEINSTANCELISTVIEW* list;
    int linkTotal;
    int orderCount;
};

struct MODULEINSTANCE {
    int id;
    unsigned char active;
    unsigned char unknown05[3];
    char* data;
    int count;
    void* order;
    struct AEMSLINKVIEW* links;
    void* special;
};

/* inferred: the fields of a component definition read when sizing it */
struct AEMSCOMPDEFSIZEVIEW {
    unsigned char unknown00[4];
    unsigned char kind;
    unsigned char inputs;
    unsigned char unknown06[6];
    int param0;
    int param1;
    union {
        unsigned int value;
        unsigned char flag;
    } param2;
    unsigned char mode;
};

struct AEMSMERGEDEFVIEW {
    unsigned char unknown00[5];
    unsigned char inputs;
};

extern "C" void* memset(void*, int, unsigned long);
void* SNDMEMI_allocz(int);
void SNDAEMSI_reordermodule(MODULE*, MODULEINSTANCE*);
void SNDAEMSI_reordermodule2(MODULE*, MODULEINSTANCE*);
void SNDMEMI_free(void*);
void SNDAEMSI_streamupdatestatus(AEMSCOMPDYNAMICPLAYER*);
extern "C" void SNDSTRM_purge(int);
extern "C" void SNDstop(int);
extern void (*SNDAEMS_beginevent)(int*);

int SNDAEMSI_destroyinstance(MODULE*, int);

void SNDAEMSI_restore() {
    SNDAEMS_removesystem();
}

int SNDAEMSI_addsystem(void* data) {
    AEMSSYSTEMVIEW* system = (AEMSSYSTEMVIEW*)data;
    int i;

    SNDAEMS_linkbank();
    SNDSYS_entercritical();
    sndaems.system = system;
    sndgs.aemsRestore = SNDAEMSI_restore;
    sndaems.unknown00 = 0;
    sndaems.rate = 10;
    SNDSYS_add100hzclient(AEMSI_timerupdate);
    sndaems.system = system;
    sndaems.handlerCount = 0;
    system->params = (AEMSPARAMREFVIEW*)((int)system + (int)system->params);
    sndaems.system->refs = (AEMSTRIGREFVIEW*)((int)system + (int)sndaems.system->refs);
    sndaems.system->triggers = (AEMSTRIGGERVIEW*)((int)system + (int)sndaems.system->triggers);
    sndaems.system->globals = (AEMSREFLISTVIEW*)((int)system + (int)sndaems.system->globals);
    for (i = 0; i < sndaems.system->triggerCount; i++) {
        AEMSTRIGGERVIEW* trigger = &sndaems.system->triggers[i];
        int j;

        trigger->params = (AEMSREFLISTVIEW*)((int)system + (int)trigger->params);
        for (j = 0; j < trigger->paramCount; j++)
            trigger->params[j].refs = (int*)((int)system + (int)trigger->params[j].refs);
        trigger->lists[0].refs = (int*)((int)system + (int)trigger->lists[0].refs);
        trigger->lists[1].refs = (int*)((int)system + (int)trigger->lists[1].refs);
        trigger->lists[2].refs = (int*)((int)system + (int)trigger->lists[2].refs);
    }
    for (i = 0; i < sndaems.system->globalCount; i++)
        sndaems.system->globals[i].refs = (int*)((int)system + (int)sndaems.system->globals[i].refs);
    if (sndaems.system->globalCount > 0) {
        sndaems.globalState = (int*)SNDMEMI_allocz(sndaems.system->globalCount * 4);
        memset(sndaems.globalState, 0, sndaems.system->globalCount * 4);
    }
    sndaems.banks = (MODULEBANK**)SNDMEMI_allocz(64);
    memset(sndaems.banks, 0, 64);
    SNDSYS_leavecritical();
    return 0;
}

int SNDAEMSI_removesystem() {
    int i;

    SNDSYS_entercritical();
    sndaems.handlerCount = 0;
    if (sndaems.system) {
        SNDSYS_remove100hzclient(AEMSI_timerupdate);
        for (i = 0; i < 16; i++) {
            if (sndaems.banks[i])
                SNDAEMS_removemodulebank(i);
        }
        SNDMEMI_free(sndaems.banks);
        sndaems.banks = 0;
        if (sndaems.system->globalCount > 0)
            SNDMEMI_free(sndaems.globalState);
        sndaems.system = 0;
        sndaems.globalState = 0;
    }
    SNDSYS_leavecritical();
    return 0;
}

void SNDAEMSI_resolvemodulebank(MODULEBANK* bank, char* name, int user) {
    int m;
    int i;

    SNDSYS_entercritical();
    bank->info = (AEMSBANKINFOVIEW*)SNDMEMI_allocz(16);
    memset(bank->info, 0, 16);
    if (bank->globalLinks) {
        bank->globalLinks = (AEMSGLOBALLINKVIEW*)((int)bank->globalLinks + (int)bank);
        for (i = 0; i < bank->globalLinkCount; i++) {
            bank->globalLinks[i].target += (int)bank;
            bank->globalLinks[i].value = 0;
        }
    }
    if (bank->version >= 5 && bank->extra)
        bank->extra = (void*)((int)bank->extra + (int)bank);
    bank->modules = (char*)((int)bank->modules + (int)bank);
    for (i = 0; i < bank->moduleCount; i++) {
        AEMSMODULERECVIEW* module = bank->version >= 8 ? (AEMSMODULERECVIEW*)(bank->modules + i * 28)
                                                    : (AEMSMODULERECVIEW*)(bank->modules + i * 20);
        AEMSCOMPRECVIEW* def;
        int offset;
        int c;

        module->list = (AEMSMODULELISTVIEW*)SNDMEMI_allocz(12);
        module->list->live = 0;
        module->list->instances = (MODULEINSTANCE**)SNDMEMI_allocz(module->slots * 4);
        for (c = 0; c < module->slots; c++)
            module->list->instances[c] = 0;
        module->list->offsets = (int*)SNDMEMI_allocz(module->components * 4);
        offset = 0;
        module->definitions = (char*)((int)module->definitions + (int)bank);
        def = (AEMSCOMPRECVIEW*)module->definitions;
        for (c = 0; c < module->components; c++) {
            int k;

            def->links = (AEMSLINKIDVIEW*)((int)def->links + (int)bank);
            for (k = 0; k < def->inputs; k++)
                def->links[k].from = (int)bank + def->links[k].from;
            if (def->kind == 4) {
                def->data0 = (int*)((int)def->data0 + (int)bank);
                if ((unsigned int)*def->data0 < (unsigned int)bank)
                    *def->data0 = *def->data0 + (int)bank;
                def->data1 = bank;
            } else if (def->kind == 6) {
                def->data0 = (int*)((int)def->data0 + (int)bank);
            } else if (def->kind == 8 && def->data2) {
                def->data2 = (void*)((int)def->data2 + (int)bank);
            } else if (def->kind == 17) {
                def->data0 = (int*)((int)def->data0 + (int)bank);
            } else if (def->kind == 30) {
                def->data0 = (int*)((int)def->data0 + (int)bank);
            } else if (def->kind == 25) {
                def->data1 = (void*)((int)def->data1 + (int)bank);
            }
            module->list->offsets[c] = offset;
            offset += aemscompinfo[def->kind].componentSize;
            def = (AEMSCOMPRECVIEW*)((char*)def + aemscompinfo[def->kind].definitionSize);
        }
    }
    for (i = 0; i < sndaems.system->paramCount; i++) {
        AEMSPARAMREFVIEW* ref = &sndaems.system->params[i];

        if (ref->bank == bank->id) {
            for (m = 0; m < bank->moduleCount; m++) {
                AEMSMODULERECVIEW* module = bank->version >= 8 ? (AEMSMODULERECVIEW*)(bank->modules + m * 28)
                                                            : (AEMSMODULERECVIEW*)(bank->modules + m * 20);

                if (module->id == ref->module) {
                    AEMSCOMPRECVIEW* def = (AEMSCOMPRECVIEW*)module->definitions;
                    int c;

                    for (c = 0; c < module->components; c++) {
                        if (def->id == ref->component) {
                            ref->resolvedBank = ref->bank;
                            ref->resolvedModule = m;
                            ref->resolvedComponent = c;
                            m = bank->moduleCount;
                            break;
                        }
                        def = (AEMSCOMPRECVIEW*)((char*)def + aemscompinfo[def->kind].definitionSize);
                    }
                }
            }
        }
    }
    for (i = 0; i < sndaems.system->refCount; i++) {
        AEMSTRIGREFVIEW* ref = &sndaems.system->refs[i];

        if (ref->bank == bank->id) {
            for (m = 0; m < bank->moduleCount; m++) {
                AEMSMODULERECVIEW* module = bank->version >= 8 ? (AEMSMODULERECVIEW*)(bank->modules + m * 28)
                                                            : (AEMSMODULERECVIEW*)(bank->modules + m * 20);

                if (module->id == ref->module) {
                    AEMSCOMPRECVIEW* def = (AEMSCOMPRECVIEW*)module->definitions;
                    int c;

                    for (c = 0; c < module->components; c++) {
                        if (def->id == ref->component) {
                            ref->resolvedBank = ref->bank;
                            ref->resolvedModule = m;
                            ref->resolvedComponent = c;
                            m = bank->moduleCount;
                            break;
                        }
                        def = (AEMSCOMPRECVIEW*)((char*)def + aemscompinfo[def->kind].definitionSize);
                    }
                }
            }
        }
    }
    if (name && *(unsigned char*)name) {
        bank->info->name = (char*)SNDMEMI_allocz(strlen(name) + 1);
        strcpy(bank->info->name, name);
    } else {
        bank->info->name = 0;
    }
    bank->info->user = user;
    SNDSYS_leavecritical();
}

int SNDAEMSI_removemodulebank(int index) {
    MODULEBANK* bank;
    int i;

    SNDSYS_entercritical();
    bank = sndaems.banks[index];
    if (bank->info->name)
        SNDMEMI_free(bank->info->name);
    for (i = 0; i < sndaems.system->paramCount; i++) {
        if (bank->id == sndaems.system->params[i].bank)
            sndaems.system->params[i].resolvedBank = 0xFFFF;
    }
    for (i = 0; i < sndaems.system->refCount; i++) {
        AEMSTRIGREFVIEW* ref = &sndaems.system->refs[i];

        if (ref->bank == bank->id)
            ref->resolvedBank = 0xFFFF;
    }
    for (i = 0; i < bank->moduleCount; i++) {
        AEMSMODULERECVIEW* module = bank->version >= 8 ? (AEMSMODULERECVIEW*)(bank->modules + i * 28)
                                                    : (AEMSMODULERECVIEW*)(bank->modules + i * 20);
        int j;

        for (j = 0; j < module->slots; j++) {
            MODULEINSTANCE* instance = module->list->instances[j];

            if (instance)
                SNDAEMSI_destroyinstance((struct MODULE*)module, instance->id);
        }
        SNDMEMI_free(module->list->offsets);
        SNDMEMI_free(module->list->instances);
        SNDMEMI_free(module->list);
    }
    if (bank->info->soundBank >= 0)
        SNDbankremove(bank->info->soundBank);
    if (bank->info->soundBank2 >= 0)
        SNDbankremove(bank->info->soundBank2);
    SNDMEMI_free(bank->info);
    sndaems.banks[index] = 0;
    SNDSYS_leavecritical();
    return 0;
}

int SNDAEMSI_beginevent(void* data) {
    int* event = (int*)data;
    int id = sndaems.unknown00;
    int i;
    int trigger = event[0];
    int* values = event + 1;
    int created;
    int result = -8;

    if ((sndaems.unknown00 = id + 1) < 0)
        sndaems.unknown00 = 0;
    created = SNDAEMSI_stufftrigger(trigger, 0, id);
    for (i = 0; i < sndaems.system->triggers[trigger].paramCount; i++) {
        if (SNDAEMSI_stuffparam(*values, trigger, i, id) == 0)
            result = 0;
        values++;
    }
    if (created >= 0) {
        for (i = 0; i < sndaems.system->globalCount; i++)
            SNDAEMSI_setglobal(i, sndaems.globalState[i]);
    } else if (result < 0) {
        id = created;
    }
    SNDAEMSI_updatemodules(1);
    return id;
}

int SNDAEMSI_updateevent(int id, void* data) {
    int* event = (int*)data;
    int i;
    int trigger = event[0];
    int* values = event + 1;
    int result = -8;

    for (i = 0; i < sndaems.system->triggers[trigger].paramCount; i++) {
        if (SNDAEMSI_stuffparam(*values, trigger, i, id) >= 0)
            result = 0;
        values++;
    }
    if (SNDAEMSI_stufftrigger(trigger, 1, id) >= 0)
        result = 0;
    SNDAEMSI_updatemodules(1);
    return result;
}

int SNDAEMSI_endevent(int id, void* data) {
    int* event = (int*)data;
    int i;
    int trigger = event[0];
    int* values = event + 1;

    for (i = 0; i < sndaems.system->triggers[trigger].paramCount; i++) {
        SNDAEMSI_stuffparam(*values, trigger, i, id);
        values++;
    }
    SNDAEMSI_stufftrigger(trigger, 2, id);
    SNDAEMSI_updatemodules(1);
    return 0;
}

int SNDAEMSI_setglobal(int index, int value) {
    char* field;
    int j;
    MODULEINSTANCE* instance;
    int b;
    AEMSREFLISTVIEW* list;
    AEMSPARAMREFVIEW* ref;
    AEMSMODULERECVIEW* module;
    int i;
    MODULEBANK* bank;

    if (index >= sndaems.system->globalCount)
        return -8;
    list = &sndaems.system->globals[index];
    for (i = 0; i < list->count; i++) {
        ref = &sndaems.system->params[list->refs[i]];

        if (value > ref->max)
            value = ref->max;
        else {
            j = ref->min;
            if (value < j)
                value = j;
        }
        for (b = 0; b < 16; b++) {
            if (sndaems.banks[b] && ref->resolvedBank == sndaems.banks[b]->id)
                break;
        }
        if (b < 16) {
            bank = sndaems.banks[b];
            module = bank->version >= 8 ? (AEMSMODULERECVIEW*)(bank->modules + ref->resolvedModule * 28)
                                                        : (AEMSMODULERECVIEW*)(bank->modules + ref->resolvedModule * 20);
            for (j = 0; j < module->slots; j++) {
                instance = module->list->instances[j];

                if (instance) {
                    field = instance->data + module->list->offsets[ref->resolvedComponent];
                    *(int*)(field + 16) = value;
                    instance->active = 1;
                }
            }
        }
    }
    sndaems.globalState[index] = value;
    return 0;
}

int SNDAEMSI_stuffparam(int value, int trigger, int index, int instanceId) {
    MODULEBANK* bank;
    AEMSMODULERECVIEW* module;
    MODULEINSTANCE* instance;
    int b;
    AEMSTRIGGERVIEW* entry;
    AEMSPARAMREFVIEW* ref;
    AEMSREFLISTVIEW* list;
    int i;
    int result;

    entry = &sndaems.system->triggers[trigger];
    list = &entry->params[index];
    result = -8;
    for (i = 0; i < list->count; i++) {
        ref = &sndaems.system->params[list->refs[i]];

        if (value > ref->max)
            value = ref->max;
        else if (value < ref->min)
            value = ref->min;
        for (b = 0; b < 16; b++) {
            if (sndaems.banks[b] && ref->resolvedBank == sndaems.banks[b]->id)
                break;
        }
        if (b < 16) {
            bank = sndaems.banks[b];
            module = bank->version >= 8 ? (AEMSMODULERECVIEW*)(bank->modules + ref->resolvedModule * 28)
                                        : (AEMSMODULERECVIEW*)(bank->modules + ref->resolvedModule * 20);
            if (module->trigger == trigger) {
                for (b = 0; b < module->slots; b++) {
                    instance = module->list->instances[b];

                    if (instance && instance->id == instanceId) {
                        result = 0;
                        *(int*)(instance->data + module->list->offsets[ref->resolvedComponent] + 16) = value;
                        instance->active = 1;
                    }
                }
            } else {
                for (b = 0; b < module->slots; b++) {
                    instance = module->list->instances[b];

                    if (instance) {
                        result = 0;
                        *(int*)(instance->data + module->list->offsets[ref->resolvedComponent] + 16) = value;
                        instance->active = 1;
                    }
                }
            }
        }
    }
    return result;
}

int SNDAEMSI_stufftrigger(int trigger, int kind, int instanceId) {
    int b;
    MODULEBANK* bank;
    MODULEINSTANCE* instance;
    AEMSMODULERECVIEW* module;
    AEMSTRIGGERVIEW* entry;
    AEMSTRIGREFVIEW* ref;
    AEMSREFLISTVIEW* list;
    int i;
    int result;
    int created;

    entry = &sndaems.system->triggers[trigger];
    result = -8;
    created = 0;
    if (kind == 0)
        list = &entry->lists[0];
    else if (kind == 1)
        list = &entry->lists[1];
    else
        list = &entry->lists[2];
    if (!list)
        return -8;
    for (i = 0; i < list->count; i++) {
        ref = &sndaems.system->refs[list->refs[i]];

        for (b = 0; b < 16; b++) {
            if (sndaems.banks[b] && ref->resolvedBank == sndaems.banks[b]->id)
                break;
        }
        if (b < 16) {
            bank = sndaems.banks[b];
            module = bank->version >= 8 ? (AEMSMODULERECVIEW*)(bank->modules + ref->resolvedModule * 28)
                                        : (AEMSMODULERECVIEW*)(bank->modules + ref->resolvedModule * 20);
            if (module->trigger == trigger) {
                if (ref->resolvedComponent == 0) {
                    if (kind == 0) {
                        created = SNDAEMSI_createinstance(bank, (struct MODULE*)module, instanceId);
                        if (created >= 0) {
                            result = created;
                            for (b = 0; b < module->slots; b++) {
                                instance = module->list->instances[b];

                                if (instance && instance->id == instanceId) {
                                    *(int*)(instance->data + module->list->offsets[ref->resolvedComponent] + 16) = 1;
                                    instance->active = 1;
                                }
                            }
                        }
                    }
                } else {
                    for (b = 0; b < module->slots; b++) {
                        instance = module->list->instances[b];

                        if (instance && instance->id == instanceId) {
                            result = 0;
                            *(int*)(instance->data + module->list->offsets[ref->resolvedComponent] + 16) = 1;
                            instance->active = 1;
                        }
                    }
                }
            } else {
                for (b = 0; b < module->slots; b++) {
                    instance = module->list->instances[b];

                    if (instance) {
                        result = 0;
                        *(int*)(instance->data + module->list->offsets[ref->resolvedComponent] + 16) = 1;
                        instance->active = 1;
                    }
                }
            }
        }
    }
    if (result < 0 && created < 0)
        result = created;
    return result;
}

void AEMSI_timerupdate() {
    SNDAEMSI_updatemodules(0);
}

void SNDAEMSI_updatemodules(int timer) {
    int bank;
    int module;

    for (bank = 0; bank < 16; bank++) {
        if (sndaems.banks[bank]) {
            for (module = 0; module < sndaems.banks[bank]->moduleCount; module++)
                SNDAEMSI_updatemodule(bank, module, timer);
        }
    }
}

void SNDAEMSI_updatemodule(int bank, int index, int timer) {
    MODULEBANK* modules = sndaems.banks[bank];
    AEMSMODULETIMERVIEW* module = modules->version >= 8 ? (AEMSMODULETIMERVIEW*)(modules->modules + index * 28)
                                                   : (AEMSMODULETIMERVIEW*)(modules->modules + index * 20);
    AEMSINSTANCEVIEW* instance;
    int i;

    if (timer == module->timer) {
        for (i = 0; i < module->instanceCount; i++) {
            instance = module->instances->instances[i];

            if (instance && instance->active) {
                int c;

                for (c = 0; c < instance->componentCount; c++) {
                    AEMSCOMPDYNAMIC* comp = instance->components[c];
                    int k;

                    aemscompinfo[((AEMSCOMPDEFHEADERVIEW*)comp->def)->kind].update(comp, (MODULE*)module);
                    for (k = 0; k < comp->linkCount; k++)
                        *comp->links[k].to = *comp->links[k].from;
                }
                if (instance->end->in[0] > 0)
                    SNDAEMSI_destroyinstance((MODULE*)module, instance->id);
            }
        }
    }
}

void AEMSI_updateinparam(AEMSCOMPDYNAMIC*, MODULE*) {
}

void AEMSI_updatetrigger(AEMSCOMPDYNAMIC* comp, MODULE*) {
    if (comp->out.value >= 1) {
        if (comp->state0.value == 0)
            comp->state0.value = 1;
        else
            comp->out.value = comp->state0.value = 0;
    }
}

void AEMSI_updatestatic(AEMSCOMPDYNAMIC*, MODULE*) {
}

void AEMSI_updateoscillator(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSOSCDEFVIEW* def = (AEMSOSCDEFVIEW*)comp->def;

    if (comp->mode == 2) {
        int period = comp->in[0];
        int step;

        if (period <= 0)
            period = 1;
        step = (sndaems.rate << 16) / period;
        if (step > 0xFFFF)
            step = 0xFFFF;
        comp->state0.step = step;
    }
    comp->state1.phase += comp->state0.step;
    if (comp->state1.phase >= 0x10000)
        comp->state1.phase -= 0x10000;
    if (def->waveform == 0) {
        comp->out.value = (comp->in[1] * iSNDsin(comp->state1.phase >> 6)) >> 16;
        if (def->polarity == 1) {
            comp->out.value += comp->in[1];
            comp->out.value >>= 1;
        }
    } else if (def->waveform == 1) {
        if (comp->state1.phase >= 0x8000)
            comp->out.value = comp->in[1];
        else if (def->polarity == 1)
            comp->out.value = 0;
        else
            comp->out.value = -comp->in[1];
    } else if (def->waveform == 2 && def->polarity == 1) {
        comp->out.value = (comp->state1.phase * comp->in[1]) >> 16;
    } else if (def->waveform == 2) {
        comp->out.value = (comp->state1.phase * comp->in[1]) >> 15;
        comp->out.value -= comp->in[1];
    } else if (def->waveform == 3 && def->polarity == 0) {
        if (comp->state1.phase < 0x8000)
            comp->out.value = (comp->state1.phase * comp->in[1]) >> 14;
        else
            comp->out.value = ((0x10000 - comp->state1.phase) * comp->in[1]) >> 14;
        comp->out.value -= comp->in[1];
    } else if (def->waveform == 3) {
        if (comp->state1.phase < 0x8000)
            comp->out.value = (comp->state1.phase * comp->in[1]) >> 15;
        else
            comp->out.value = ((0x10000 - comp->state1.phase) * comp->in[1]) >> 15;
    }
}

void SNDAEMSI_updateplayer(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSPLAYERDEFVIEW* def = (AEMSPLAYERDEFVIEW*)comp->def;

    if (comp->in[0] > 0xFFFF)
        comp->in[0] = 0xFFFF;
    else if (comp->in[0] < 0)
        comp->in[0] = 0;
    if (comp->in[2] > 0x7FFF)
        comp->in[2] = 0x7FFF;
    else if (comp->in[2] < 0)
        comp->in[2] = 0;
    if (comp->playing != comp->in[7] && comp->in[7] == 1 && comp->playing == 0) {
        AEMSPLAYERTABLEVIEW* table = def->table;
        int index = comp->in[6];

        if (index >= table->count)
            index = table->count - 1;
        comp->sound = table->sounds[index];
    }
    if (comp->sound >= 0xC000)
        sndaems.streamUpdate(comp);
    else if (comp->sound >= 0x8000)
        sndaems.playerUpdate(comp);
    else if (comp->sound >= 0x4000)
        sndaems.bankUpdate(comp);
}

void AEMSI_updaterangetrig(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int in;
    AEMSRANGETRIGDEFVIEW* def;

    def = (AEMSRANGETRIGDEFVIEW*)comp->def;
    in = comp->in[0];

    if (in >= def->onMin && in <= def->onMax && comp->state1.value == 0) {
        comp->out.value = comp->state0.value = comp->state1.value = 1;
        return;
    }
    if (comp->state0.value > 0) {
        comp->state0.value--;
        return;
    }
    if (in >= def->offMin && in <= def->offMax && comp->state1.value == 1) {
        comp->state1.value = 0;
        return;
    }
    comp->out.value = 0;
}

void AEMSI_updateenvelope(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSENVDEFVIEW* def = (AEMSENVDEFVIEW*)comp->def;
    int trigger = comp->in[0];

    if (trigger == 1 && comp->state1.env.trigger == 0) {
        comp->state0.value = 0;
        comp->state1.env.stage = 0;
        comp->out.value = def->initial;
        comp->state2.value = def->initial;
    } else if (trigger == 3 && comp->state1.env.trigger != 3 && def->release - 1 > comp->state1.env.stage) {
        comp->state0.value = 0;
        comp->state1.env.stage = def->release - 1;
        comp->out.value = comp->state2.value = def->points[comp->state1.env.stage - 1].level;
    } else if ((trigger == 3 || trigger == 1) && comp->state1.env.stage < def->count) {
        comp->state0.value += sndaems.rate;
        if (comp->state0.value >= def->points[comp->state1.env.stage].duration) {
            comp->state0.value = 0;
            comp->state2.value = def->points[comp->state1.env.stage].level;
            comp->state1.env.stage++;
            if (comp->state1.env.stage >= def->count)
                comp->out.value = 0;
            else
                comp->out.value = comp->state2.value;
        } else {
            long long fraction = ((long long)comp->state0.value << 32) / def->points[comp->state1.env.stage].duration;
            comp->out.value = (fraction * (def->points[comp->state1.env.stage].level - comp->state2.value)) >> 32;
            comp->out.value += comp->state2.value;
        }
    } else if (trigger != 2) {
        comp->out.value = 0;
    }
    comp->state1.env.trigger = comp->in[0];
}

void AEMSI_updatecounter(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOUNTERDEFVIEW* def = (AEMSCOUNTERDEFVIEW*)comp->def;
    if (comp->in[1] >= def->min && comp->in[1] <= def->max) {
        comp->out.value = comp->in[1];
    } else if (comp->in[0] > 0 && comp->state0.value == 0) {
        if (def->up) {
            comp->out.value++;
            if (comp->out.value > def->max)
                comp->out.value = def->min;
        } else {
            comp->out.value--;
            if (comp->out.value < def->min)
                comp->out.value = def->max;
        }
    }
    comp->state0.value = comp->in[0];
}

void AEMSI_updaterandom(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSRANDOMDEFVIEW* def = (AEMSRANDOMDEFVIEW*)comp->def;
    int found = 0;
    int start;
    int i;
    int sum;
    int r;
    if (comp->in[0] > 0 && comp->state2.value == 0) {
        if (def->mode == 2) {
            start = iSNDrandom() % (def->max + 1 - def->min);
            while (!found) {
                for (i = start; i <= def->max - def->min && !found; i++) {
                    if (comp->state0.counts[i] != comp->state1.round) {
                        comp->state0.counts[i]++;
                        found = 1;
                        comp->out.value = i + def->min;
                    }
                }
                for (i = 0; i < start && !found; i++) {
                    if (comp->state0.counts[i] != comp->state1.round) {
                        comp->state0.counts[i]++;
                        found = 1;
                        comp->out.value = i + def->min;
                    }
                }
                if (!found) {
                    if (comp->out.value == start + def->min)
                        start++;
                    if (start > def->max)
                        start = def->min;
                    comp->state1.round++;
                }
            }
        } else if (def->mode == 0) {
            r = iSNDrandom() % 100;
            sum = 0;
            for (i = 0; i <= def->max - def->min; i++) {
                sum += def->weights[i];
                if (sum > r) {
                    comp->out.value = i + def->min;
                    break;
                }
            }
        } else if (def->mode == 1) {
            comp->out.value = def->min + iSNDrandom() % (def->max + 1 - def->min);
        }
    }
    comp->state2.value = comp->in[0];
}

void AEMSI_updatemux(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int* in = comp->in;
    if (in[0] > 0)
        comp->out.value = in[in[0]];
    else
        comp->out.value = 0;
}

void AEMSI_updatedemux(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    int i;
    for (i = 0; i < def->param0; i++) {
        if (i == comp->in[0] - 1)
            comp->out.values[i] = comp->in[1];
        else
            comp->out.values[i] = 0;
    }
}

void AEMSI_updateadd(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    int sum = 0;
    int i;
    for (i = 0; i < def->param0; i++)
        sum += comp->in[i];
    if (sum > def->param1)
        comp->out.value = def->param1;
    else
        comp->out.value = sum;
}

void AEMSI_updatesubtract(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int result = comp->in[0] - comp->in[1];
    if (result < ((AEMSCOMPDEFVIEW*)comp->def)->param0)
        comp->out.value = ((AEMSCOMPDEFVIEW*)comp->def)->param0;
    else
        comp->out.value = result;
}

void AEMSI_updatescale(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    int i;
    comp->out.value = comp->in[0];
    for (i = 1; i < def->param1; i++)
        comp->out.value = ((long long)comp->out.value * comp->in[i]) / def->param0;
}

void AEMSI_updatemultiply(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int result = comp->in[0] * comp->in[1];
    if (result > ((AEMSCOMPDEFVIEW*)comp->def)->param0)
        comp->out.value = ((AEMSCOMPDEFVIEW*)comp->def)->param0;
    else
        comp->out.value = result;
}

void AEMSI_updatedivide(AEMSCOMPDYNAMIC* comp, MODULE*) {
    if (comp->in[1] > 0)
        comp->out.value = comp->in[0] / comp->in[1];
    else
        comp->out.value = 0;
}

void AEMSI_updatemodulo(AEMSCOMPDYNAMIC* comp, MODULE*) {
    comp->out.value = comp->in[0] % comp->in[1];
}

void AEMSI_updatetable(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSTABLEDEFVIEW* def = (AEMSTABLEDEFVIEW*)comp->def;
    comp->out.value = comp->in[0];
    if (comp->out.value < def->min)
        comp->out.value = def->min;
    else if (comp->out.value > def->max)
        comp->out.value = def->max;
    comp->out.value = ((def->count - 1) * (comp->out.value - def->min)) / (def->max - def->min);
    comp->out.value = def->table[comp->out.value];
}

void AEMSI_updatetable2(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSTABLE2DEFVIEW* def = (AEMSTABLE2DEFVIEW*)comp->def;
    comp->out.value = comp->in[0];
    if (comp->out.value < def->min)
        comp->out.value = def->min;
    else if (comp->out.value > def->max)
        comp->out.value = def->max;
    comp->out.value = (def->count * (comp->out.value - def->min)) / (def->max - def->min + 1);
    if (def->width == 4)
        comp->out.value = ((int*)def->table)[comp->out.value];
    else if (def->width == 2)
        comp->out.value = ((short*)def->table)[comp->out.value];
    else {
        signed char* bytes = (signed char*)def->table;
        comp->out.value = bytes[comp->out.value];
    }
}

void AEMSI_updatedelaytrig(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    if (comp->in[0] > 0 && comp->state1.value == 0)
        comp->state0.value = 0;
    if ((comp->state0.value >= comp->in[1] || comp->state0.value > def->param0) && comp->out.value == 0) {
        comp->out.value = 1;
    } else if (comp->state0.value > comp->in[1] || comp->state0.value > def->param0) {
        comp->out.value = 0;
        comp->state0.value = -1;
    } else {
        comp->out.value = 0;
    }
    comp->state1.value = comp->in[0];
    if (comp->state0.value >= 0)
        comp->state0.value += sndaems.rate;
}

void AEMSI_updatedelayline(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    comp->state1.samples++;
    if (comp->state1.samples >= comp->state0.samples + def->param0 / sndaems.rate)
        comp->state1.samples = comp->state0.samples;
    *comp->state1.samples = comp->in[0];
    if (comp->in[1] / sndaems.rate > def->param0 / sndaems.rate - 1)
        comp->read = comp->state1.samples - def->param0 / sndaems.rate + 1;
    else
        comp->read = comp->state1.samples - comp->in[1] / sndaems.rate;
    if (comp->read < comp->state0.samples)
        comp->read += def->param0 / sndaems.rate;
    comp->out.value = *comp->read;
}

void AEMSI_updatewarp(AEMSCOMPDYNAMIC* comp, MODULE*) {
    comp->out.value = comp->in[0];
}

void AEMSI_updatemax(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int result = 0;
    int i;
    for (i = 0; i < ((AEMSCOMPDEFVIEW*)comp->def)->param0; i++) {
        if (comp->in[i] > result)
            result = comp->in[i];
    }
    comp->out.value = result;
}

void AEMSI_updatemin(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int result = 0x7fffffff;
    int i;
    for (i = 0; i < ((AEMSCOMPDEFVIEW*)comp->def)->param0; i++) {
        if (comp->in[i] < result)
            result = comp->in[i];
    }
    comp->out.value = result;
}

void AEMSI_updatestategen(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSSTATEGENDEFVIEW* def = (AEMSSTATEGENDEFVIEW*)comp->def;
    int i;
    for (i = 0; i < def->count; i++) {
        if (comp->state0.samples[i] <= 0 && comp->in[i] > 0)
            comp->out.value = def->values[i];
        comp->state0.samples[i] = comp->in[i];
    }
}

void SNDAEMSI_updatedestroy(AEMSCOMPDYNAMIC* comp, MODULE* module) {
    if (comp->in[0] > 0)
        SNDAEMSI_destroyinstance(module, comp->state0.value);
}

void SNDAEMSI_updatesend(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    int event[18];
    int i;
    int changed = 0;
    for (i = 0; i < def->param0; i++) {
        if (comp->state1.values[i] != comp->in[i])
            changed = 1;
        comp->state1.values[i] = comp->in[i];
    }
    if (changed == 1) {
        event[0] = def->param1;
        for (i = 0; i < def->param0; i++)
            event[i + 1] = comp->in[i];
        SNDAEMS_beginevent(event);
        for (i = 0; i < sndaems.handlerCount; i++)
            sndaems.handlers[i](comp->state0.value, event);
    }
}

void SNDAEMSI_updateramp(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int out = comp->out.value;

    if (comp->in[2] == out)
        return;
    if (comp->in[2] != comp->target || comp->in[0] != comp->state0.value) {
        comp->state1.value = out;
        *comp->current = (long long)comp->out.value << 36;
        comp->target = comp->in[2];
        comp->state0.value = comp->in[0];
        if (comp->state0.value <= 0) {
            comp->out.value = comp->target;
            return;
        }
        *comp->step = ((long long)sndaems.rate * (comp->target - comp->state1.value)) << 24;
        *comp->step /= comp->state0.value;
    } else {
        int rising = out < comp->target;

        *comp->current += *comp->step * comp->in[1];
        comp->out.value = *comp->current >> 36;
        if ((rising && comp->out.value > comp->target) || (!rising && comp->out.value < comp->target))
            comp->out.value = comp->target;
    }
}

void SNDAEMSI_updatemerge(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSMERGEDEFVIEW* def = (AEMSMERGEDEFVIEW*)comp->def;
    int i;

    if (comp->out.value == 1) {
        comp->out.value = 0;
        return;
    }
    for (i = 0; i < def->inputs; i++) {
        if (comp->in[i] > 0) {
            comp->out.value = 1;
            return;
        }
    }
}

int SNDAEMSI_createinstance(MODULEBANK* bank, MODULE* module, int id) {
    int slot;
    int dataSize = 0;
    char* memory;
    MODULEINSTANCE* instance;
    char* comp;
    int c;
    int size;
    char* buffer;
    AEMSCOMPDEFSIZEVIEW* def;

    if (module->list->live >= module->slots)
        return -9;
    for (slot = 0; slot < module->slots; slot++) {
        if (!module->list->instances[slot])
            break;
    }
    size = 28;
    def = (AEMSCOMPDEFSIZEVIEW*)module->definitions;
    for (c = 0; c < module->components; c++) {
        size += aemscompinfo[def->kind].componentSize;
        dataSize += aemscompinfo[def->kind].componentSize;
        size += def->inputs * 4;
        if (def->kind == 10)
            size += def->param0 * 4;
        else if (def->kind == 19)
            size += (def->param0 / sndaems.rate + 1) * 2 + 2;
        else if (def->kind == 8 && def->param2.value == 0 && def->mode == 2)
            size += (def->param1 - def->param0 + 1) * 2 + 2;
        else if (def->kind == 25)
            size += def->param0 * 2 + 2;
        else if (def->kind == 29)
            size += def->param0 * 4;
        def = (AEMSCOMPDEFSIZEVIEW*)((char*)def + aemscompinfo[def->kind].definitionSize);
    }
    memory = (char*)SNDMEMI_allocz(size);
    module->list->instances[slot] = (MODULEINSTANCE*)memory;
    instance = module->list->instances[slot];
    memset(memory, 0, size);
    instance->active = 0;
    instance->id = id;
    buffer = memory + 28;
    instance->data = buffer;
    buffer += dataSize;
    def = (AEMSCOMPDEFSIZEVIEW*)module->definitions;
    comp = instance->data;
    for (c = 0; c < module->components; c++) {
        AEMSCOMPDYNAMIC* dyn = (AEMSCOMPDYNAMIC*)comp;

        dyn->def = def;
        dyn->in = (int*)buffer;
        buffer += def->inputs * 4;
        if (def->kind == 10) {
            dyn->out.values = (int*)buffer;
            buffer += def->param0 * 4;
        } else if (def->kind == 19) {
            dyn->state0.samples = (short*)buffer;
            buffer += (def->param0 / sndaems.rate + 1) * 2;
            if ((int)buffer & 2)
                buffer += 2;
            dyn->state1.samples = dyn->state0.samples;
            dyn->read = dyn->state0.samples;
        } else if (def->kind == 8 && def->param2.value == 0 && def->mode == 2) {
            int count = def->param1 - def->param0 + 1;

            dyn->state0.samples = (short*)buffer;
            buffer += count * 2;
            if ((int)buffer & 2)
                buffer += 2;
        } else if (def->kind == 25) {
            dyn->state0.samples = (short*)buffer;
            buffer += def->param0 * 2;
            if ((int)buffer & 2)
                buffer += 2;
        } else if (def->kind == 18) {
            dyn->state0.value = -1;
        } else if (def->kind == 28) {
            dyn->state0.value = id;
        } else if (def->kind == 29) {
            dyn->state0.value = id;
            dyn->state1.values = (int*)buffer;
            buffer += def->param0 * 4;
        } else if (def->kind == 31) {
            dyn->step = (long long*)(((int)dyn + 39) & ~7);
            dyn->current = dyn->step + 1;
        } else if (def->kind == 7) {
            if (def->param2.flag)
                dyn->out.value = def->param0;
            else
                dyn->out.value = def->param1;
        }
        comp += aemscompinfo[def->kind].componentSize;
        def = (AEMSCOMPDEFSIZEVIEW*)((char*)def + aemscompinfo[def->kind].definitionSize);
    }
    instance->active = 1;
    module->list->live++;
    if (bank->version > 7)
        SNDAEMSI_reordermodule2(module, instance);
    else
        SNDAEMSI_reordermodule(module, instance);
    return 0;
}

int SNDAEMSI_destroyinstance(MODULE* module, int id) {
    MODULEINSTANCE* instance = 0;
    int slot;
    AEMSCOMPDYNAMICPLAYER* comp;
    int c;

    if (module->list->live == 0)
        return -1;
    for (slot = 0; slot < module->slots; slot++) {
        instance = module->list->instances[slot];
        if (instance && instance->id == id)
            break;
    }
    if (slot >= module->slots)
        return -13;
    module->list->instances[slot] = 0;
    module->list->live--;
    comp = (AEMSCOMPDYNAMICPLAYER*)instance->data;
    for (c = 0; c < module->components; c++) {
        AEMSCOMPDEFSIZEVIEW* def = (AEMSCOMPDEFSIZEVIEW*)comp->def;

        if (def->kind == 4 && comp->out != 0) {
            if (comp->sound >= 0xC000) {
                comp->in[7] = 0;
                if (comp->stream > -1) {
                    SNDSTRM_purge(comp->stream);
                    comp->handle = 0x7FFFFFFF;
                    SNDAEMSI_streamupdatestatus(comp);
                }
                comp->playing = 0;
                sndaems.streamStop(comp);
            } else if (comp->sound >= 0x8000) {
                sndaems.playerStop(comp);
            } else if (comp->sound >= 0x4000) {
                SNDstop(comp->handle);
            }
        }
        comp = (AEMSCOMPDYNAMICPLAYER*)((char*)comp + aemscompinfo[def->kind].componentSize);
    }
    SNDMEMI_free(instance->order);
    SNDMEMI_free(instance);
    return 0;
}

/* inferred: an output's link targets (component index, input index) */
struct AEMSLINKDEFVIEW {
    unsigned short* targets;
    short count;
};

/* inferred: the definition fields read when ordering the module */
struct AEMSCOMPORDERDEFVIEW {
    unsigned char unknown00[4];
    unsigned char kind;
    unsigned char inputs;
    unsigned char outputs;
    unsigned char unknown07;
    AEMSLINKDEFVIEW* links;
    int value;
};

/* inferred: where a component's output i lives (its own outputs, the
   definition's constant or its output buffer) */
static inline int* GetOutput(AEMSCOMPDYNAMIC* comp, int i) {
    unsigned short mode;
    AEMSCOMPORDERDEFVIEW* def;
    int* output = 0;
    def = (AEMSCOMPORDERDEFVIEW*)comp->def;
    mode = aemscompinfo[def->kind].outputMode;
    if (mode == 1) {
        output = &comp->out.value;
        if (i > 0)
            output += i;
    } else if (mode == 0) {
        output = &def->value;
    } else if (mode == 2) {
        output = comp->out.values;
        output += i;
    }
    return output;
}

void SNDAEMSI_reordermodule(MODULE* module, MODULEINSTANCE* instance) {
    int done;
    AEMSCOMPDYNAMIC* comp;
    AEMSCOMPORDERDEFVIEW* def;
    AEMSCOMPDYNAMIC* target;
    unsigned short* link;
    int total;
    int c;
    int o;
    int l;

    comp = (AEMSCOMPDYNAMIC*)instance->data;
    instance->count = 0;
    total = 0;
    for (c = 0; c < module->components; c++) {
        def = (AEMSCOMPORDERDEFVIEW*)comp->def;
        comp->linkCount = 0;
        if (def->kind == 22)
            comp->connected = 1;
        else
            comp->connected = 0;
        if (def->kind == 28)
            instance->special = comp;
        if (def->kind != 20) {
            for (o = 0; o < def->outputs; o++)
                total += def->links[o].count;
        }
        comp->placed = 0;
        comp = (AEMSCOMPDYNAMIC*)((char*)comp + aemscompinfo[def->kind].componentSize);
    }
    instance->order = SNDMEMI_allocz(module->components * 4 + total * 8);
    if (total != 0)
        instance->links = (AEMSLINKVIEW*)((char*)instance->order + module->components * 4);
    else
        instance->links = 0;
    total = 0;
    comp = (AEMSCOMPDYNAMIC*)instance->data;
    for (c = 0; c < module->components; c++) {
        def = (AEMSCOMPORDERDEFVIEW*)comp->def;
        if (def->inputs == 0) {
            if (def->kind != 20) {
                ((AEMSCOMPDYNAMIC**)instance->order)[instance->count] = comp;
                instance->count++;
            }
            comp->links = instance->links + total;
            for (o = 0; o < def->outputs; o++) {
                for (l = 0; l < def->links[o].count; l++) {
                    target = (AEMSCOMPDYNAMIC*)(instance->data + module->list->offsets[def->links[o].targets[l * 2]]);
                    target->connected++;
                    if (def->kind == 20) {
                        target->in[def->links[o].targets[l * 2 + 1]] = *GetOutput(comp, o);
                    } else {
                        total++;
                        comp->links[comp->linkCount].destination = &target->in[def->links[o].targets[l * 2 + 1]];
                        comp->links[comp->linkCount].source = GetOutput(comp, o);
                        comp->linkCount++;
                    }
                }
            }
            comp->placed = 1;
        }
        comp = (AEMSCOMPDYNAMIC*)((char*)comp + aemscompinfo[def->kind].componentSize);
    }
    done = 0;
    while (!done) {
        comp = (AEMSCOMPDYNAMIC*)instance->data;
        done = 1;
        for (c = 0; c < module->components; c++) {
            def = (AEMSCOMPORDERDEFVIEW*)comp->def;
            if (def->inputs != 0 && comp->connected >= def->inputs && !comp->placed && def->kind != 28) {
                ((AEMSCOMPDYNAMIC**)instance->order)[instance->count] = comp;
                done = 0;
                instance->count++;
                comp->links = instance->links + total;
                for (o = 0; o < def->outputs; o++) {
                    for (l = 0; l < def->links[o].count; l++) {
                        total++;
                        link = &def->links[o].targets[l * 2];
                        target = (AEMSCOMPDYNAMIC*)(instance->data + module->list->offsets[link[0]]);
                        comp->links[comp->linkCount].destination = &target->in[link[1]];
                        comp->links[comp->linkCount].source = GetOutput(comp, o);
                        comp->linkCount++;
                        target->connected++;
                        comp->placed = 1;
                    }
                }
                comp->placed = 1;
            }
            comp = (AEMSCOMPDYNAMIC*)((char*)comp + aemscompinfo[def->kind].componentSize);
        }
    }
}

void SNDAEMSI_reordermodule2(MODULE* module, MODULEINSTANCE* instance) {
    AEMSCOMPDYNAMIC** order;
    int total;
    AEMSCOMPDYNAMIC* target;
    AEMSCOMPORDERDEFVIEW* def;
    AEMSCOMPDYNAMIC* comp;
    int c;
    AEMSLINKDEFVIEW* links;
    int o;
    int l;

    instance->count = module->orderCount;
    order = (AEMSCOMPDYNAMIC**)SNDMEMI_allocz(module->orderCount * 4 + module->linkTotal * 8);
    instance->order = order;
    if (module->linkTotal != 0)
        instance->links = (AEMSLINKVIEW*)((char*)instance->order + module->orderCount * 4);
    else
        instance->links = 0;
    total = 0;
    instance->special = instance->data + module->list->offsets[module->components - 1];
    comp = (AEMSCOMPDYNAMIC*)instance->data;
    for (c = 0; c < module->components; c++) {
        def = (AEMSCOMPORDERDEFVIEW*)comp->def;
        comp->linkCount = 0;
        if (def->inputs == 0) {
            if (def->kind != 20)
                *order++ = comp;
            comp->links = instance->links + total;
            for (o = 0; o < def->outputs; o++) {
                links = &def->links[o];
                for (l = 0; l < links->count; l++) {
                    target = (AEMSCOMPDYNAMIC*)(instance->data + module->list->offsets[links->targets[l * 2]]);
                    if (def->kind == 20) {
                        target->in[links->targets[l * 2 + 1]] = *GetOutput(comp, o);
                    } else {
                        total++;
                        comp->links[comp->linkCount].destination = &target->in[links->targets[l * 2 + 1]];
                        comp->links[comp->linkCount].source = GetOutput(comp, o);
                        comp->linkCount++;
                    }
                }
            }
        } else {
            *order++ = comp;
            comp->links = instance->links + total;
            for (o = 0; o < def->outputs; o++) {
                links = &def->links[o];
                for (l = 0; l < links->count; l++) {
                    total++;
                    target = (AEMSCOMPDYNAMIC*)(instance->data + module->list->offsets[links->targets[l * 2]]);
                    comp->links[comp->linkCount].destination = &target->in[links->targets[l * 2 + 1]];
                    comp->links[comp->linkCount].source = GetOutput(comp, o);
                    comp->linkCount++;
                }
            }
        }
        comp->placed = 1;
        comp->connected = def->inputs;
        comp = (AEMSCOMPDYNAMIC*)((char*)comp + aemscompinfo[def->kind].componentSize);
    }
}
