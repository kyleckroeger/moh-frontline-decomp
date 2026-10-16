/* A fragment of saems.c (0x8015385c), the start of the file: the AEMS
   system's restore hook, SNDAEMSI_addsystem (installs a loaded system,
   registers the 100 Hz module timer and the restore hook, turns the
   system's offsets into pointers, and allocates the per-global state and the
   16 bank slots), SNDAEMSI_removesystem (removes every bank and frees the
   state) and SNDAEMSI_resolvemodulebank (turns a loaded bank's offsets into
   pointers, allocates each module's instance slots and component offsets,
   resolves the system's parameter and trigger references to module and
   component indices in this bank, and records the bank's name and user
   value) and SNDAEMSI_removemodulebank (unresolves the references to the
   bank, destroys every instance, frees the module lists and removes the
   bank's sound banks). SNDAEMSI_beginevent takes the next instance id (the
   counter wraps to 0 when negative), stuffs the start trigger and the event's
   parameters and, when an instance was created, the globals; updateevent and
   endevent stuff the parameters and the update or end trigger. Each runs the
   modules once. SNDAEMSI_setglobal clamps a global's value to each
   referencing parameter's range and writes it into every instance of the
   parameter's module in a loaded bank (reusing the loop counter for the
   minimum and a separate field pointer reproduce the original's register use;
   found with decomp-permuter), then records it. SNDAEMSI_stuffparam does the
   same for one of a trigger's parameters, writing every instance of the
   module when the module belongs to another trigger and only the given
   instance otherwise (the bank-search counter is reused for both instance
   loops, also found with decomp-permuter). SNDAEMSI_stufftrigger sets the
   value of a trigger's start, update or end references to 1 in their
   modules' instances, creating the instance first for a start reference to
   a module's first component, and returns the created instance or the
   failure. The system layout, the trigger and parameter records and the
   sndaems and sndgs views are inferred. */
struct AEMSCOMPDEFVIEW;

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
struct AEMSLINKDEFVIEW {
    int from;
    int to;
};

struct AEMSCOMPDEFVIEW {
    unsigned int id;
    unsigned char kind;
    unsigned char unknown05;
    unsigned char inputs;
    unsigned char unknown07;
    AEMSLINKDEFVIEW* links;
    int* data0;
    void* data1;
    void* data2;
};

struct AEMSCOMPINFOVIEW {
    void (*update)();
    unsigned char definitionSize;
    unsigned char componentSize;
};

extern AEMSCOMPINFOVIEW aemscompinfo[];

struct MODULEINSTANCE {
    int id;
    unsigned char active;
    unsigned char unknown05[3];
    char* data;
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
struct AEMSMODULEVIEW {
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

struct SNDAEMSVIEW {
    int unknown00;
    int rate;
    AEMSSYSTEMVIEW* system;
    MODULEBANK** banks;
    unsigned char unknown10[4];
    int* globalState;
    unsigned char unknown18[19];
    unsigned char handlerCount;
};

struct SNDGSVIEW {
    unsigned char unknown000[440];
    void (*aemsRestore)();
};

extern SNDAEMSVIEW sndaems;
extern SNDGSVIEW sndgs;
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
        AEMSMODULEVIEW* module = bank->version >= 8 ? (AEMSMODULEVIEW*)(bank->modules + i * 28)
                                                    : (AEMSMODULEVIEW*)(bank->modules + i * 20);
        AEMSCOMPDEFVIEW* def;
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
        def = (AEMSCOMPDEFVIEW*)module->definitions;
        for (c = 0; c < module->components; c++) {
            int k;

            def->links = (AEMSLINKDEFVIEW*)((int)def->links + (int)bank);
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
            def = (AEMSCOMPDEFVIEW*)((char*)def + aemscompinfo[def->kind].definitionSize);
        }
    }
    for (i = 0; i < sndaems.system->paramCount; i++) {
        AEMSPARAMREFVIEW* ref = &sndaems.system->params[i];

        if (ref->bank == bank->id) {
            for (m = 0; m < bank->moduleCount; m++) {
                AEMSMODULEVIEW* module = bank->version >= 8 ? (AEMSMODULEVIEW*)(bank->modules + m * 28)
                                                            : (AEMSMODULEVIEW*)(bank->modules + m * 20);

                if (module->id == ref->module) {
                    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)module->definitions;
                    int c;

                    for (c = 0; c < module->components; c++) {
                        if (def->id == ref->component) {
                            ref->resolvedBank = ref->bank;
                            ref->resolvedModule = m;
                            ref->resolvedComponent = c;
                            m = bank->moduleCount;
                            break;
                        }
                        def = (AEMSCOMPDEFVIEW*)((char*)def + aemscompinfo[def->kind].definitionSize);
                    }
                }
            }
        }
    }
    for (i = 0; i < sndaems.system->refCount; i++) {
        AEMSTRIGREFVIEW* ref = &sndaems.system->refs[i];

        if (ref->bank == bank->id) {
            for (m = 0; m < bank->moduleCount; m++) {
                AEMSMODULEVIEW* module = bank->version >= 8 ? (AEMSMODULEVIEW*)(bank->modules + m * 28)
                                                            : (AEMSMODULEVIEW*)(bank->modules + m * 20);

                if (module->id == ref->module) {
                    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)module->definitions;
                    int c;

                    for (c = 0; c < module->components; c++) {
                        if (def->id == ref->component) {
                            ref->resolvedBank = ref->bank;
                            ref->resolvedModule = m;
                            ref->resolvedComponent = c;
                            m = bank->moduleCount;
                            break;
                        }
                        def = (AEMSCOMPDEFVIEW*)((char*)def + aemscompinfo[def->kind].definitionSize);
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
        AEMSMODULEVIEW* module = bank->version >= 8 ? (AEMSMODULEVIEW*)(bank->modules + i * 28)
                                                    : (AEMSMODULEVIEW*)(bank->modules + i * 20);
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
    AEMSMODULEVIEW* module;
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
            module = bank->version >= 8 ? (AEMSMODULEVIEW*)(bank->modules + ref->resolvedModule * 28)
                                                        : (AEMSMODULEVIEW*)(bank->modules + ref->resolvedModule * 20);
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
    AEMSMODULEVIEW* module;
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
            module = bank->version >= 8 ? (AEMSMODULEVIEW*)(bank->modules + ref->resolvedModule * 28)
                                        : (AEMSMODULEVIEW*)(bank->modules + ref->resolvedModule * 20);
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
    AEMSMODULEVIEW* module;
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
            module = bank->version >= 8 ? (AEMSMODULEVIEW*)(bank->modules + ref->resolvedModule * 28)
                                        : (AEMSMODULEVIEW*)(bank->modules + ref->resolvedModule * 20);
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
