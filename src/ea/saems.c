/* The AEMS module updaters from the multiplexer to the event sender: each
   reads the component's inputs and writes its output (arithmetic with
   clamping, table lookups, delays, extremes), destroys the instance or sends
   an event when the inputs change; the ramp moves its output toward a target
   over a duration in 36-bit fixed point (restarting when the target or
   duration changes, clamping at the target) and the merge pulses once when
   any input is set. SNDAEMSI_createinstance takes a free slot, sizes one
   allocation for the instance header, every component and their buffers
   (inputs, outputs, delay lines and tables per component kind, kept 4-byte
   aligned), lays the components out and reorders the module;
   SNDAEMSI_destroyinstance frees the slot after stopping any sound a player
   component started (stream, player or bank hook by the sound id).
   SNDAEMSI_reordermodule orders the instance's components for updating:
   it counts the links (resetting each component's link count, connected
   inputs and placed flag; the kind-28 component is remembered), allocates
   the order and link arrays together, places every component without inputs
   (constants, kind 20, copy their outputs straight into the targets' inputs
   and are not ordered; others link each output to its targets) and then
   repeatedly places the components whose inputs are all connected. The
   link, link-definition and order-definition views and the output lookup
   (an inline helper by the component kind's output mode) are inferred.
   AEMSCOMPDYNAMIC and MODULE are named by
   the mangled symbols; the component layout, the per-component static
   definitions it points to, the sndaems view and the event buffer's size
   (from the stack frame) are inferred. */
struct MODULEINSTANCE;

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

struct MODULEBANK {
    unsigned char unknown00[2];
    unsigned char version;
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

struct AEMSCOMPINFOVIEW {
    void (*update)(struct AEMSCOMPDYNAMIC*, MODULE*);
    unsigned char definitionSize;
    unsigned char componentSize;
    unsigned short outputMode;
};

extern AEMSCOMPINFOVIEW aemscompinfo[];

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
};

struct AEMSCOMPDYNAMIC {
    void* def;
    signed char connected;
    signed char placed;
    unsigned short linkCount;
    struct AEMSLINKVIEW* links;
    int* in;
    AEMSOUTPUTVIEW out;
    AEMSSTATEVIEW state0;
    union {
        int value;
        short* samples;
        int* values;
    } state1;
    union {
        short* read;
        int target;
    };
    unsigned char unknown20[20];
    long long* step;
    long long* current;
};

struct AEMSMERGEDEFVIEW {
    unsigned char unknown00[5];
    unsigned char inputs;
};

struct SNDAEMSVIEW {
    int unknown00;
    int rate;
    unsigned char unknown08[20];
    void (*playerStop)(struct AEMSCOMPDYNAMICPLAYER*);
    unsigned char unknown20[4];
    void (*streamStop)(struct AEMSCOMPDYNAMICPLAYER*);
    unsigned char unknown28[3];
    unsigned char handlerCount;
    void (*handlers[6])(int, int*);
};

extern SNDAEMSVIEW sndaems;
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

/* inferred: a link from a component output to another component's input */
struct AEMSLINKVIEW {
    int* source;
    int* destination;
};

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
